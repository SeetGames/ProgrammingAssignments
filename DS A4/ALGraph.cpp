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
void ALGraph::AddDEdge(unsigned _source, unsigned _destination, unsigned _weight)
{
    // Calculate zero-based index for source to match internal storage
    unsigned zeroBasedIndex = _source - 1;

    // Create adjacency info for the edge to be added
    AdjacencyInfo newEdgeInfo{_destination, _weight};

    // Reference to the adjacency list of the source node
    auto &sourceAdjList = m_AdjacencyList.at(zeroBasedIndex);

    // Iterate through the adjacency list to find the correct position
    // for the new edge based on weight (and id for tie-breaking)
    for (auto it = sourceAdjList.begin(); it != sourceAdjList.end(); ++it)
    {
        // If the new edge's weight is less than the current edge's weight,
        // or they are equal but the new edge's destination id is smaller,
        // insert the new edge before the current edge.
        if (newEdgeInfo.weight < it->weight || 
            (newEdgeInfo.weight == it->weight && newEdgeInfo.id < it->id))
        {
            sourceAdjList.insert(it, newEdgeInfo);
            return; // Exit after inserting to avoid adding the edge twice
        }
    }

    // If the new edge does not fit before any existing edge in the list,
    // add it to the end of the list.
    sourceAdjList.push_back(newEdgeInfo);
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

/**
 * Perform Dijkstra's algorithm on the graph
 * @param _startNode The starting node for the algorithm
 * @return A vector of DijkstraInfo structs, one for each node in the graph
 */
std::vector<DijkstraInfo> ALGraph::Dijkstra(unsigned _startNode) const 
{
    // Lambda function to access adjacency list of a given node, adjusting for 0-based indexing
    auto GetAdjList = [=](unsigned index) { return m_AdjacencyList.at(index - 1); };

    // Priority queue comparison function; assumes definition of GNode supports < operator
    auto comp = [](const GNode* lhs, const GNode* rhs) { return lhs->info.cost < rhs->info.cost; };

    // Priority queue to manage nodes by minimum cost, initialized with custom comparator
    std::priority_queue<GNode*, std::vector<GNode*>, decltype(comp)> pq(comp);
    
    // Preallocate memory for result vector to improve performance
    std::vector<DijkstraInfo> dijkstraResults;
    dijkstraResults.reserve(m_AdjacencyList.size());

    // Container to store node states during algorithm execution
    std::vector<GNode> nodes;
    nodes.reserve(m_AdjacencyList.size());

    // Initialize nodes with infinite cost and mark the start node
    for (unsigned i = 0; i < m_AdjacencyList.size(); ++i) 
    {
        GNode node;
        node.m_id = i + 1; // Adjust for 1-based indexing
        bool isStartNode = (node.m_id == _startNode);
        node.info.cost = isStartNode ? 0 : INF;
        node.m_Evaluated = isStartNode;
        if (isStartNode) node.info.path.push_back(_startNode);
        nodes.push_back(node);
    }

    //push all nodes into queue with updated edge costs.
    const auto &adj_nodes = GetAdjList(_startNode);
    
    for (unsigned i = 0; i < adj_nodes.size(); ++i)
    {
        const auto &info = adj_nodes.at(i);
        auto &node = nodes[info.id - 1];
        node.info.cost = info.weight;
        
        pq.push(&node);
        node.info.path.push_back(_startNode);
    }

    // Dijkstra's algorithm execution
    while (!pq.empty()) 
    {
        GNode* current = pq.top(); 
        pq.pop();
        
        current->m_Evaluated = true;
        current->info.path.push_back(current->m_id);
        
        const auto &adj_list = GetAdjList(current->m_id);

        for (const auto& info : GetAdjList(current->m_id)) 
        {
            auto &neighbour = nodes[info.id - 1];
            unsigned newCost = info.weight + current->info.cost;
            
            // Relaxation step: update neighbour's cost and path if a shorter path is found
            if (newCost < neighbour.info.cost)
            {
                neighbour.info.cost = newCost;
                neighbour.info.path = current->info.path; // Copy path from current to neighbour
                if (!current->m_Evaluated)
                  neighbour.info.path.push_back(current->m_id); // Append neighbour's own ID
                pq.push(&neighbour); // Reinsert neighbour for further consideration
            }
        }
    }

    // Construct result from nodes' Dijkstra information
    for (unsigned i = 0; i < nodes.size(); ++i)
    {
        dijkstraResults.push_back(nodes.at(i).info);
    }

    return dijkstraResults;
}

/**
 * Get the adjacency list representation of the graph
 * @return The adjacency list
 */
ALIST ALGraph::GetAList() const
{
  return m_AdjacencyList;
}

/**
 * Less than operator for GNode
 * @param rhs The right-hand side of the comparison
 * @return True if the left-hand side is less than the right-hand side
 */
bool ALGraph::GNode::operator<(const GNode &rhs) const
{
  return info.cost < rhs.info.cost;
}