/*!*************************************************************************
\file ALGraph.h
\author Seetoh Wei Tung
\par DP email: seetoh.w@digipen.edu
\par Course: Data Structures
\par Assignment 4
\date 18-03-2024
\brief
This file contains the declaration for the ALGraph
***************************************************************************/
//---------------------------------------------------------------------------
#ifndef ALGRAPH_H
#define ALGRAPH_H
//---------------------------------------------------------------------------
#include <vector>
#include <limits>

struct DijkstraInfo
{
  unsigned cost;
  std::vector<unsigned> path;
};

struct AdjacencyInfo
{
  unsigned id;
  unsigned weight;
};

typedef std::vector<std::vector<AdjacencyInfo>> ALIST;

class ALGraph
{
public:
  ALGraph(unsigned size);
  ~ALGraph(void);
  void AddDEdge(unsigned source, unsigned destination, unsigned weight);
  void AddUEdge(unsigned node1, unsigned node2, unsigned weight);

  std::vector<DijkstraInfo> Dijkstra(unsigned start_node) const;
  ALIST GetAList(void) const;

private:
  // An EXAMPLE of some other classes you may want to create and
  // implement in ALGraph.cpp
  struct GNode
  {
    unsigned m_id;
    bool m_Evaluated;
    DijkstraInfo info;
    bool operator<(const GNode &rhs) const;
  };
  class GEdge;
  struct AdjInfo
  {
    //unsigned node;
    unsigned weight;
    unsigned cost;
    //AdjInfo();
    //bool operator<(const AdjInfo &rhs) const;
    //bool operator>(const AdjInfo &rhs) const;
  };

  // Other private fields and methods
  ALIST m_AdjacencyList;
  const unsigned INF = static_cast<unsigned>(-1);
};

#endif
