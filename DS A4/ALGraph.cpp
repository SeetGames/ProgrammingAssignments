/*!*************************************************************************
\file ALGraph.cpp
\author Seetoh Wei Tung
\par DP email: seetoh.w@digipen.edu
\par Course: Data Structures
\par Assignment 4
\date 18-03-2024
\brief
This file contains the implementation for the ALGraph
***************************************************************************/

#include "ALGraph.h"
#include <algorithm>
#include <queue>


/**
 * Constructor for the ALGraph class
 * @param size The number of nodes in the graph
 */
ALGraph::ALGraph(unsigned size)
{
  m_AdjacencyList.reserve(size);
  for (size_t i = 0; i < size; ++i)
    m_AdjacencyList.push_back(ALIST::value_type());
}

/**
 * Destructor for the ALGraph class
 */
ALGraph::~ALGraph(void)
{
}

/**
 * Add a directed edge to the graph
 * @param source The source node
 * @param destination The destination node
 * @param weight The weight of the edge
 */
void ALGraph::AddDEdge(unsigned source, unsigned destination, unsigned weight)
{
  unsigned id = source - 1;
  AdjacencyInfo info{destination, weight};
  auto &adj_list = m_AdjacencyList.at(id);

  for (auto i = adj_list.begin(); i != adj_list.end(); ++i)
  {
    if (info.weight < i->weight)
    {
      adj_list.insert(i, info);
      return;
    }
    else if (info.weight == i->weight && info.id < i->id)
    {
      adj_list.insert(i, info);
      return;
    }
  }
  adj_list.push_back(info);
}

/**
 * Add an undirected edge to the graph
 * @param node1 One node of the edge
 * @param node2 The other node of the edge
 * @param weight The weight of the edge
 */
void ALGraph::AddUEdge(unsigned node1, unsigned node2, unsigned weight)
{
  AddDEdge(node1, node2, weight);
  AddDEdge(node2, node1, weight);
}


std::vector<DijkstraInfo> ALGraph::Dijkstra(unsigned start_node) const
{
  auto GetAdjList = [=](unsigned i) { return m_AdjacencyList.at(i - 1); };
  auto comp = [](const GNode *lhs, const GNode *rhs) -> bool { return *lhs < *rhs; };

  std::priority_queue<GNode *, std::vector<GNode *>, decltype(comp)> pq(comp);
  std::vector<DijkstraInfo> result;
  std::vector<GNode> nodes;

  result.reserve(m_AdjacencyList.size());
  nodes.reserve(m_AdjacencyList.size());

  //initialize all nodes in graph
  for (unsigned i = 1; i < m_AdjacencyList.size() + 1; ++i)
  {
    GNode node;
    if (i != start_node)
    {
      node.m_Evaluated = false;
      node.info.cost = INF;
    }
    else
    {
      node.m_Evaluated = true;
      node.info.cost = 0;
      node.info.path.push_back(start_node);
    }

    node.m_id = i;
    nodes.push_back(node);
  }

  //push all nodes into queue with updated edge costs.
  const auto &adj_nodes = GetAdjList(start_node);

  for (unsigned i = 0; i < adj_nodes.size(); ++i)
  {
    const auto &info = adj_nodes.at(i);
    auto &node = nodes[info.id - 1];
    node.info.cost = info.weight;

    pq.push(&node);
    node.info.path.push_back(start_node);
  }

  //go through the priority queue and evaluate all nodes.
  while (!pq.empty())
  {
    GNode *v = pq.top();
    pq.pop();

    v->m_Evaluated = true;
    v->info.path.push_back(v->m_id);

    const auto &adj_list = GetAdjList(v->m_id);

    for (unsigned i = 0; i < adj_list.size(); ++i)
    {
      const auto &info = adj_list.at(i);
      auto &u = nodes.at(info.id - 1);

      unsigned new_cost = info.weight + v->info.cost;

      if (new_cost < u.info.cost)
      {
        u.info.cost = new_cost;
        u.info.path = v->info.path;

        if (!v->m_Evaluated)
          u.info.path.push_back(v->m_id);

        pq.push(&u);
      }
    }
  }

  for (unsigned i = 0; i < nodes.size(); ++i)
  {
    result.push_back(nodes.at(i).info);
  }
  return result;
}

/**
 * Get the adjacency list representation of the graph
 * @return The adjacency list
 */
ALIST ALGraph::GetAList(void) const
{
  return m_AdjacencyList;
}


bool ALGraph::GNode::operator<(const GNode &rhs) const
{
  return info.cost < rhs.info.cost;
}