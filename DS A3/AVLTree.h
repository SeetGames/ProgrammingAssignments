/*!*************************************************************************
\file AVLTree.h
\author Seetoh Wei Tung
\par DP email: seetoh.w@digipen.edu
\par Course: Data Structures
\par Assignment 3
\date 02-23-2024
\brief
This file contains the declaration for the AVLTree
***************************************************************************/
//---------------------------------------------------------------------------
#ifndef AVLTREE_H
#define AVLTREE_H
//---------------------------------------------------------------------------
#include <stack>
#include "BSTree.h"

/*!
  Definition for the AVL Tree
*/
template <typename T>
class AVLTree : public BSTree<T>
{
  public:
    AVLTree(ObjectAllocator *oa = 0, bool ShareOA = false);
    virtual ~AVLTree() = default; // DO NOT IMPLEMENT
    virtual void insert(const T& value) override;
    virtual void remove(const T& value) override;

      // Returns true if efficiency implemented
    static bool ImplementedBalanceFactor(void);

  private:
    // private stuff
    using BinTree = typename BSTree<T>::BinTree;
    using Stack = std::stack<BinTree *>;
    void InsertAVL(BinTree& tree, const T& value, Stack& visited);
    void RemoveAVL(BinTree& tree, const T& value, Stack& visited);
    void BalanceAVL(Stack& visited);
    
    void LeftRotation(BinTree& tree);
    void RightRotation(BinTree& tree);
    unsigned int CountTree(BinTree& tree);
    void RecountAVL(BinTree& tree);
};

#include "AVLTree.cpp"

#endif
//---------------------------------------------------------------------------

s