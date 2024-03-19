#include "ALGraph.h"
#include <algorithm>
#include <queue>

ALGraph::ALGraph(unsigned size)
{
  list_.reserve(size);
  for (size_t i = 0; i < size; ++i)
    list_.push_back(ALIST::value_type());
}

ALGraph::~ALGraph(void)
{
}

void ALGraph::AddDEdge(unsigned source, unsigned destination, unsigned weight)
{
  unsigned id = source - 1;
  AdjacencyInfo info{destination, weight};
  auto &adj_list = list_.at(id);

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

void ALGraph::AddUEdge(unsigned node1, unsigned node2, unsigned weight)
{
  AddDEdge(node1, node2, weight);
  AddDEdge(node2, node1, weight);
}

std::vector<DijkstraInfo> ALGraph::Dijkstra(unsigned start_node) const
{
  auto GetAdjList = [=](unsigned i) { return list_.at(i - 1); };
  auto comp = [](const GNode *lhs, const GNode *rhs) -> bool { return *lhs < *rhs; };

  std::priority_queue<GNode *, std::vector<GNode *>, decltype(comp)> pq(comp);
  std::vector<DijkstraInfo> result;
  std::vector<GNode> nodes;

  result.reserve(list_.size());
  nodes.reserve(list_.size());

  //initialize all nodes in graph
  for (unsigned i = 1; i < list_.size() + 1; ++i)
  {
    GNode node;
    if (i != start_node)
    {
      node.evaluated = false;
      node.info.cost = INFINITY_;
    }
    else
    {
      node.evaluated = true;
      node.info.cost = 0;
      node.info.path.push_back(start_node);
    }

    node.id = i;
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

    v->evaluated = true;
    v->info.path.push_back(v->id);

    const auto &adj_list = GetAdjList(v->id);

    for (unsigned i = 0; i < adj_list.size(); ++i)
    {
      const auto &info = adj_list.at(i);
      auto &u = nodes.at(info.id - 1);

      unsigned new_cost = info.weight + v->info.cost;

      if (new_cost < u.info.cost)
      {
        u.info.cost = new_cost;
        u.info.path = v->info.path;

        if (!v->evaluated)
          u.info.path.push_back(v->id);

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

ALIST ALGraph::GetAList(void) const
{
  return list_;
}

bool ALGraph::GNode::operator<(const GNode &rhs) const
{
  return info.cost < rhs.info.cost;
}