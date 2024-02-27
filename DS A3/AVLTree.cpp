/*!*************************************************************************
\file AVLTree.cpp
\author Seetoh Wei Tung
\par DP email: seetoh.w@digipen.edu
\par Course: Data Structures
\par Assignment 3
\date 02-23-2024
\brief
This file contains the implementation for the AVLTree
***************************************************************************/
#include "AVLTree.h"

/*!*****************************************************************************
 * @brief Constructs an AVLTree with an optional ObjectAllocator.
 * 
 * Initializes an AVL tree, optionally using an external memory allocator. If no
 * allocator is provided, the tree will manage its own memory. The constructor also
 * sets whether the allocator is shared based on the provided flag.
 * 
 * @param _OA Pointer to an external ObjectAllocator, or nullptr to use default allocation.
 * @param _shareOA Flag indicating whether the provided allocator is shared with other data structures.
 *******************************************************************************/
template <typename T>
AVLTree<T>::AVLTree(ObjectAllocator *_OA, bool _shareOA)
    : BSTree<T>{_OA, _shareOA}
{
}

/*!*****************************************************************************
 * @brief Inserts a new value into the AVL tree.
 * 
 * This function inserts a value into the AVL tree and ensures that the tree
 * remains balanced according to AVL tree rules. It utilizes a stack to track the
 * path of insertion for potential rebalancing.
 * 
 * @param _value The value to be inserted into the AVL tree.
 *******************************************************************************/
template <typename T>
void AVLTree<T>::insert(const T &_value)
{
    Stack visitedNodes;
    InsertAVL(BSTree<T>::get_root(), _value, visitedNodes);
}

/*!*****************************************************************************
 * @brief Removes a value from the AVL tree.
 * 
 * This function searches for and removes a value from the AVL tree. If the value
 * exists in the tree, it is removed, and the tree is then rebalanced to maintain
 * AVL tree properties. A stack is used to track the path of removal for rebalancing.
 * 
 * @param _value The value to be removed from the AVL tree.
 *******************************************************************************/
template <typename T>
void AVLTree<T>::remove(const T &_value)
{
    Stack visitedNodes;
    RemoveAVL(BSTree<T>::get_root(), _value, visitedNodes);
}

/*!*****************************************************************************
 * @brief Indicates whether the balance factor is implemented in the AVL tree.
 * 
 * This function returns a boolean value indicating whether the AVL tree has
 * implemented the balance factor optimization. Returning false suggests that
 * the balance factor is not explicitly stored or used in the current implementation.
 * 
 * @return A boolean value indicating the implementation status of the balance factor.
 *******************************************************************************/
template <typename T>
bool AVLTree<T>::ImplementedBalanceFactor(void)
{
    return false;
}

/*!*****************************************************************************
 * @brief Inserts a value into the AVL tree and balances the tree.
 *
 * This function inserts a new value into the AVL tree, maintaining the BST properties,
 * and then ensures the tree remains balanced according to AVL tree rules. It uses a stack
 * to keep track of the visited nodes for potential backtracking during the balancing phase.
 *
 * @param _tree Reference to the current subtree's root where the new value might be inserted.
 * @param _value The value to be inserted into the AVL tree.
 * @param _visited Stack data structure to keep track of visited nodes for balancing.
 *******************************************************************************/
template <typename T>
void AVLTree<T>::InsertAVL(BinTree &_tree, const T &_value, Stack &_visited)
{
    // Base case: If the current subtree is empty, insert the new value here
    if (_tree == nullptr)
    {
        _tree = BSTree<T>::make_node(_value); // Create a new node with the value
        ++this->m_Size;                       // Increment the size of the AVL tree
        BalanceAVL(_visited);                 // Balance the tree starting from the most recently visited node
    }
    // If the value is less than the current node's data, explore the left subtree
    else if (_value < _tree->data)
    {
        _visited.push(&_tree);                    // Push the current node's address onto the stack before going left
        ++_tree->count;                           // Increment the count of nodes in the subtree rooted at _tree
        InsertAVL(_tree->left, _value, _visited); // Recursive call to insert into the left subtree
    }
    // If the value is greater than the current node's data, explore the right subtree
    else if (_value > _tree->data)
    {
        _visited.push(&_tree);                     // Push the current node's address onto the stack before going right
        ++_tree->count;                            // Increment the count of nodes in the subtree rooted at _tree
        InsertAVL(_tree->right, _value, _visited); // Recursive call to insert into the right subtree
    }
}

/*!*****************************************************************************
 * @brief Removes a value from the AVL tree and rebalances the tree.
 * 
 * This function searches for and removes a node containing the specified value
 * from the AVL tree. It maintains the AVL tree properties by rebalancing the tree
 * after removal. A stack is used to keep track of visited nodes for potential
 * backtracking during the balancing phase.
 * 
 * @param _tree Reference to the current subtree's root from which the value might be removed.
 * @param _value The value to be removed from the AVL tree.
 * @param _visited Stack data structure to keep track of visited nodes for balancing.
 *******************************************************************************/
template <typename T>
void AVLTree<T>::RemoveAVL(BinTree &_tree, const T &_value, Stack &_visited)
{
    if (_tree == nullptr) // Base case: if the current subtree is empty, return
        return;

    // If the value to be removed is less than the current node's data, explore the left subtree
    if (_value < _tree->data)
    {
        _visited.push(&_tree); // Push the current node's address onto the stack before going left
        RemoveAVL(_tree->left, _value, _visited); // Recursive call to remove from the left subtree
        --_tree->count; // Decrement the count after recursion
    }
    // If the value to be removed is greater than the current node's data, explore the right subtree
    else if (_value > _tree->data)
    {
        _visited.push(&_tree); // Push the current node's address onto the stack before going right
        RemoveAVL(_tree->right, _value, _visited); // Recursive call to remove from the right subtree
        --_tree->count; // Decrement the count after recursion
    }
    // If the current node contains the value to be removed
    else
    {
        --_tree->count; // Decrement the count for the current node

        // If the node to be removed has no left child
        if (_tree->left == nullptr)
        {
            BinTree temp = _tree; // Temporary pointer to hold the current node
            _tree = _tree->right; // Replace the current node with its right child
            this->free_node(temp); // Free the memory of the deleted node
            --this->m_Size; // Decrement the size of the AVL tree
        }
        // If the node to be removed has no right child
        else if (_tree->right == nullptr)
        {
            BinTree temp = _tree; // Temporary pointer to hold the current node
            _tree = _tree->left; // Replace the current node with its left child
            this->free_node(temp); // Free the memory of the deleted node
            --this->m_Size; // Decrement the size of the AVL tree
        }
        // If the node to be removed has two children
        else
        {
            BinTree pred = nullptr; // Pointer to hold the predecessor node
            this->find_predecessor(_tree, pred); // Find the predecessor of the current node
            _tree->data = pred->data; // Copy the data from the predecessor to the current node
            RemoveAVL(_tree->left, pred->data, _visited); // Recursively delete the predecessor node
        }

        BalanceAVL(_visited); // Rebalance the tree after removal
    }
}

/*!*****************************************************************************
 * @brief Balances the AVL tree using rotations.
 * 
 * This function iteratively examines each node in the path that was visited during
 * the insertion or removal operation (stored in a stack) and performs necessary rotations
 * to maintain the AVL tree balance. It checks the balance factor (difference in heights
 * between left and right subtrees) and applies single or double rotations as needed.
 * 
 * @param _visited Stack of pointers to nodes that were visited during the insert or remove operation.
 *******************************************************************************/
template <typename T>
void AVLTree<T>::BalanceAVL(Stack &_visited)
{
    while (!_visited.empty()) // Loop until all visited nodes have been processed
    {
        BinTree *currentNode = _visited.top(); // Get the top node from the stack
        _visited.pop(); // Remove the top node from the stack

        int heightLeft = this->tree_height((*currentNode)->left); // Calculate the height of the left subtree
        int heightRight = this->tree_height((*currentNode)->right); // Calculate the height of the right subtree

        // Check if the current node is balanced (difference in heights is less than 2)
        if (std::abs(heightLeft - heightRight) < 2)
            continue; // Move to the next node in the stack if balanced

        // Right-heavy subtree case
        if (heightRight > heightLeft)
        {
            // Check for the need for a double rotation (right-left case)
            if ((*currentNode)->right && this->tree_height((*currentNode)->right->left) > this->tree_height((*currentNode)->right->right))
            {
                RightRotation((*currentNode)->right); // Right rotation on the right child
            }
            LeftRotation(*currentNode); // Left rotation on the current node
        }
        // Left-heavy subtree case
        else if (heightLeft > heightRight)
        {
            // Check for the need for a double rotation (left-right case)
            if ((*currentNode)->left && this->tree_height((*currentNode)->left->right) > this->tree_height((*currentNode)->left->left))
            {
                LeftRotation((*currentNode)->left); // Left rotation on the left child
            }
            RightRotation(*currentNode); // Right rotation on the current node
        }

        RecountAVL(*currentNode); // Recalculate the node counts after rotations
    }
}

/*!*****************************************************************************
 * @brief Performs a left rotation on the given subtree.
 * 
 * This function performs a left rotation around the root of the specified subtree
 * to restore the AVL tree balance. It adjusts the pointers accordingly to rotate
 * the subtree and maintains the binary search tree properties.
 * 
 * @param _tree Reference to the root of the subtree to be rotated.
 *******************************************************************************/
template <typename T>
void AVLTree<T>::LeftRotation(BinTree &_tree)
{
    BinTree newRoot = _tree->right; // The right child becomes the new root of the rotated subtree
    _tree->right = newRoot->left; // The left child of the new root becomes the right child of the old root
    newRoot->left = _tree; // The old root becomes the left child of the new root
    _tree = newRoot; // Update the reference to point to the new root of the subtree
}

/*!*****************************************************************************
 * @brief Performs a right rotation on the given subtree.
 * 
 * This function performs a right rotation around the root of the specified subtree
 * to restore the AVL tree balance. It adjusts the pointers accordingly to rotate
 * the subtree and maintains the binary search tree properties.
 * 
 * @param _tree Reference to the root of the subtree to be rotated.
 *******************************************************************************/
template <typename T>
void AVLTree<T>::RightRotation(BinTree &_tree)
{
    BinTree newRoot = _tree->left; // The left child becomes the new root of the rotated subtree
    _tree->left = newRoot->right; // The right child of the new root becomes the left child of the old root
    newRoot->right = _tree; // The old root becomes the right child of the new root
    _tree = newRoot; // Update the reference to point to the new root of the subtree
}

/*!*****************************************************************************
 * @brief Counts the number of nodes in a subtree.
 * 
 * This function recursively calculates the total number of nodes in the subtree
 * rooted at the given node, including the root node itself. It's a utility function
 * often used to update node counts in AVL tree operations.
 * 
 * @param _tree Reference to the root of the subtree to be counted.
 * @return The total number of nodes in the subtree.
 *******************************************************************************/
template <typename T>
unsigned int AVLTree<T>::CountTree(BinTree &_tree)
{
    if (_tree == nullptr) // Base case: if the subtree is empty, count is 0
        return 0;

    // Count the current node plus the counts from the left and right subtrees
    return 1 + CountTree(_tree->left) + CountTree(_tree->right);
}

/*!*****************************************************************************
 * @brief Recalculates the 'count' field for each node in a subtree.
 * 
 * This function traverses the subtree rooted at the given node and updates the
 * 'count' field of each node to reflect the current number of nodes in its subtree.
 * It's used to ensure the 'count' field is accurate after modifications to the tree structure.
 * 
 * @param _tree Reference to the root of the subtree whose node counts are to be updated.
 *******************************************************************************/
template <typename T>
void AVLTree<T>::RecountAVL(BinTree &_tree)
{
    if (_tree == nullptr) // Base case: if the subtree is empty, there's nothing to recount
        return;

    _tree->count = CountTree(_tree); // Update the count of the current node

    // Recursively update the counts for the left and right subtrees
    RecountAVL(_tree->left);
    RecountAVL(_tree->right);
}
