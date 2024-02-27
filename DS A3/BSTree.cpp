/*!*************************************************************************
\file BSTree.cpp
\author Seetoh Wei Tung
\par DP email: seetoh.w@digipen.edu
\par Course: Data Structures
\par Assignment 3
\date 02-23-2024
\brief
This file contains the implementation for the BSTree
***************************************************************************/
#include "BSTree.h"

/*!*****************************************************************************
 * @brief Constructs a binary search tree with optional custom memory allocation.
 * 
 * Initializes a binary search tree, optionally using an external memory allocator.
 * If no allocator is provided, it creates a default one. The constructor also sets
 * the sharing policy for the allocator.
 * 
 * @param _allocator A pointer to an ObjectAllocator to be used for node allocations.
 * @param _shareAllocator Boolean indicating whether the allocator should be shared.
 *******************************************************************************/
template <typename T>
BSTree<T>::BSTree(ObjectAllocator *_allocator, bool _shareAllocator)
    : m_RootNode{nullptr}, m_Size{0}, m_Height{-1}, m_ShareOA{_shareAllocator}
{
  // If an external allocator is provided, use it and set not to free on destruction.
  if (_allocator)
  {
    m_OA = _allocator; // Use provided allocator
    m_FreeOA = false; // Do not free OA as it's provided externally
  }
  else
  {
    // No external allocator provided; create a default one.
    OAConfig defaultConfig(true); // Create a default configuration for the allocator
    m_OA = new ObjectAllocator(sizeof(BinTreeNode), defaultConfig); // Allocate a new ObjectAllocator with the default configuration
    m_FreeOA = true; // Set to free m_OA on destruction since it was created here
  }
  m_ShareOA = _shareAllocator; // Set the sharing policy for the allocator
}

/*!*****************************************************************************
 * @brief Constructs a new BSTree as a copy of an existing tree.
 * 
 * This constructor creates a deep copy of an existing BSTree. It either shares
 * the ObjectAllocator with the source tree or creates a new one, depending on
 * the sharing policy of the source tree.
 * 
 * @param _rhs The source tree to copy from.
 *******************************************************************************/
template <typename T>
BSTree<T>::BSTree(const BSTree<T> &_rhs)
    : m_Size{_rhs.m_Size}, m_Height{_rhs.m_Height}, m_ShareOA{_rhs.m_ShareOA}
{
  // If the source tree is sharing its ObjectAllocator, share it and avoid freeing it.
  if (_rhs.m_ShareOA)
  {
    m_OA = _rhs.m_OA; // Share the ObjectAllocator from _rhs
    m_FreeOA = false; // Do not free m_OA since it's shared
  }
  else
  {
    // If the source tree is not sharing its ObjectAllocator, create a new one.
    OAConfig defaultConfig(true); // Default configuration for the new allocator
    m_OA = new ObjectAllocator(sizeof(BinTreeNode), defaultConfig); // Allocate a new ObjectAllocator
    m_FreeOA = true; // Set to free m_OA on destruction since it was created here
  }

  // Perform a deep copy of the tree nodes from _rhs to this tree.
  DeepCopyTree(_rhs.m_RootNode, m_RootNode);
}

/*!*****************************************************************************
 * @brief Destroys the BSTree, freeing all allocated resources.
 * 
 * This destructor cleans up the tree by clearing all nodes and, if responsible,
 * also deletes the custom ObjectAllocator used for node allocation.
 *******************************************************************************/
template <typename T>
BSTree<T>::~BSTree()
{
  clear(); // Clear all nodes in the tree

  // If this tree instance owns the ObjectAllocator, delete it to free memory
  if (m_FreeOA)
  {
    delete m_OA;
    m_OA = nullptr; // Ensure the pointer is set to nullptr after deletion
  }
}

/*!*****************************************************************************
 * @brief Assigns a new value to the BSTree, replacing its current content.
 * 
 * This operator ensures deep copying of the tree structure from the right-hand
 * side (rhs) tree to this tree. It also manages the ObjectAllocator based on
 * the sharing policy of the rhs tree.
 * 
 * @param rhs The source tree to copy from.
 * @return A reference to the current tree after copying.
 *******************************************************************************/
template <typename T>
BSTree<T> &BSTree<T>::operator=(const BSTree<T> &_rhs)
{
  // Check for self-assignment
  if (this == &_rhs)
    return *this;

  // If the source tree shares its ObjectAllocator, manage the current allocator accordingly
  if (_rhs.m_ShareOA)
  {
    // If the current tree owns its ObjectAllocator, clear the tree and delete the allocator
    if (m_FreeOA)
    {
      clear(); // Clear all nodes in the current tree
      delete m_OA; // Delete the current ObjectAllocator
    }

    // Use the ObjectAllocator from the rhs tree and update sharing and ownership flags
    m_OA = _rhs.m_OA;
    m_FreeOA = false; // Current tree should not delete the shared allocator
    m_ShareOA = true; // Current tree is now sharing the allocator
  }
  else
  {
    // If the current tree is not sharing its allocator, just clear the tree
    // A new allocator will be created if needed in DeepCopyTree
    clear();
  }

  // Deep copy the tree structure from rhs to this tree
  DeepCopyTree(_rhs.m_RootNode, m_RootNode);
  m_Size = _rhs.m_Size; // Copy the size
  m_Height = _rhs.m_Height; // Copy the height

  return *this; // Return a reference to the current tree
}

/*!*****************************************************************************
 * @brief Accesses the node at the specified index in the BSTree.
 * 
 * This operator provides read-only access to a node at a given index based on
 * an in-order traversal of the tree. If the index is out of bounds (either negative
 * or beyond the size of the tree), it returns nullptr.
 * 
 * @param index The zero-based index of the node to access.
 * @return A const pointer to the BinTreeNode at the specified index, or nullptr if
 *         the index is out of bounds.
 *******************************************************************************/
template <typename T>
const typename BSTree<T>::BinTreeNode *BSTree<T>::operator[](int _index) const
{
  // Check if the index is out of bounds; consider casting to unsigned to compare with m_Size
  if (static_cast<unsigned>(_index) >= m_Size)
    return nullptr; // Return nullptr if the index is invalid
  else
    return FindNodeAtIndex(m_RootNode, _index); // Find and return the node at the given index
}

/*!*****************************************************************************
 * @brief Inserts a new value into the BSTree.
 * 
 * This function inserts a new value into the binary search tree while maintaining
 * the BST properties. If the insertion process encounters any exceptions, such as
 * memory allocation failures, it rethrows the exception for the caller to handle.
 * 
 * @param value The value to be inserted into the tree.
 *******************************************************************************/
template <typename T>
void BSTree<T>::insert(const T &_value)
{
  try
  {
    // Attempt to insert the new value into the tree
    InsertNode(m_RootNode, _value, 0);
  }
  catch (const BSTException &except)
  {
    // Rethrow the exception to be handled by the caller
    throw; // For rethrowing the current exception without losing its original context
  }
}

/*!*****************************************************************************
 * @brief Removes a value from the BSTree, if it exists.
 * 
 * This function searches for a node containing the specified value and removes
 * it from the tree, maintaining the binary search tree properties. After removal,
 * it recalculates the height of the tree to ensure accurate tree metrics.
 * 
 * @param value The value to be removed from the tree.
 *******************************************************************************/
template <typename T>
void BSTree<T>::remove(const T &_value)
{
  // Remove the node with the specified value from the tree
  DeleteNode(m_RootNode, const_cast<T &>(_value)); // Using const_cast to modify a const value is generally not recommended; consider revising the design if possible.

  // Recalculate the height of the tree after the removal
  m_Height = tree_height(m_RootNode);
}

/*!*****************************************************************************
 * @brief Clears the BSTree, removing all nodes and resetting its properties.
 * 
 * This function removes all nodes from the tree, effectively resetting it to its
 * initial state. After clearing, the tree will be empty, with a size of 0 and a
 * height of -1, indicating that it contains no nodes.
 *******************************************************************************/
template <typename T>
void BSTree<T>::clear()
{
  // Check if the tree has any nodes to clear
  if (m_RootNode)
  {
    FreeTree(m_RootNode); // Recursively free all nodes starting from the root

    // Reset tree properties to represent an empty tree
    m_RootNode = nullptr; // Set the root node pointer to nullptr
    m_Size = 0;           // Reset the size of the tree to 0
    m_Height = -1;        // Reset the height of the tree to -1, indicating an empty tree
  }
}

/*!*****************************************************************************
 * @brief Searches for a value in the BSTree, tracking the number of comparisons.
 * 
 * This function attempts to find a node containing the specified value within
 * the binary search tree. It tracks the number of comparisons made during the
 * search process, allowing for efficiency analysis.
 * 
 * @param value The value to search for in the tree.
 * @param compares A reference to an unsigned variable where the function will
 *                 store the number of comparisons made during the search.
 * @return True if the value is found in the tree, false otherwise.
 *******************************************************************************/
template <typename T>
bool BSTree<T>::find(const T &_value, unsigned &_compares) const
{
  // Delegate the search to the FindNode helper function, passing in the root node, 
  // the value to find, and the compares variable to track comparisons
  return FindNode(m_RootNode, _value, _compares);
}

/*!*****************************************************************************
 * @brief Checks if the BSTree is empty.
 * 
 * This function determines whether the binary search tree contains any nodes.
 * It returns true if the tree is empty (i.e., contains no nodes) and false
 * otherwise.
 * 
 * @return True if the tree is empty, false otherwise.
 *******************************************************************************/
template <typename T>
bool BSTree<T>::empty() const
{
  // Return true if the size of the tree is 0, indicating it's empty; false otherwise
  return m_Size == 0;
}

/*!*****************************************************************************
 * @brief Returns the number of nodes in the BSTree.
 * 
 * This function provides the total count of nodes present in the binary search
 * tree, representing its size.
 * 
 * @return The number of nodes in the tree.
 *******************************************************************************/
template <typename T>
unsigned int BSTree<T>::size() const
{
  return m_Size; // Return the current size of the tree
}

/*!*****************************************************************************
 * @brief Returns the height of the BSTree.
 * 
 * This function calculates the height of the binary search tree from the root
 * node. The height is defined as the number of edges on the longest path from
 * the root node to a leaf node.
 * 
 * @return The height of the tree.
 *******************************************************************************/
template <typename T>
int BSTree<T>::height() const
{
  return tree_height(m_RootNode); // Calculate and return the height of the tree starting from the root node
}

/*!*****************************************************************************
 * @brief Provides read-only access to the root node of the BSTree.
 * 
 * This function allows access to the root node of the binary search tree, enabling
 * operations or inspections that start from the root of the tree.
 * 
 * @return A const pointer to the root node of the tree.
 *******************************************************************************/
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::root() const
{
  return m_RootNode; // Return a const pointer to the root node
}

/*!*****************************************************************************
 * @brief Provides modifiable access to the root node of the BSTree.
 * 
 * This function allows direct access to the root node of the binary search tree,
 * enabling modifications to the tree starting from the root.
 * 
 * @return A reference to the pointer to the root node, allowing modifications.
 *******************************************************************************/
template <typename T>
typename BSTree<T>::BinTree &BSTree<T>::get_root()
{
  return m_RootNode; // Return a reference to the pointer to the root node, allowing it to be modified
}

/*!*****************************************************************************
 * @brief Allocates and constructs a new tree node with the given value.
 * 
 * This function attempts to allocate memory for a new tree node using the custom
 * ObjectAllocator and then constructs the node in-place with the provided value.
 * If memory allocation fails, it throws a BSTException indicating a memory error.
 * 
 * @param _value The value to store in the newly created tree node.
 * @return A pointer to the newly created tree node.
 *******************************************************************************/
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::make_node(const T &_value) const
{
  try
  {
    // Allocate memory for the node using the ObjectAllocator
    BinTree allocatedMemory = reinterpret_cast<BinTree>(m_OA->Allocate());

    // Use placement new to construct the node in the allocated memory
    BinTree newNode = new (allocatedMemory) BinTreeNode(_value);

    // Return the pointer to the newly constructed node
    return newNode;
  }
  catch (const OAException &except)
  {
    // If memory allocation fails, throw a BSTException with a specific error code and message
    throw BSTException(BSTException::E_NO_MEMORY, except.what());
  }
}

/*!*****************************************************************************
 * @brief Frees a tree node, deallocating its memory.
 * 
 * This function explicitly calls the destructor for a given tree node and then
 * deallocates its memory using the custom ObjectAllocator. It is used to properly
 * clean up and free memory for nodes that are removed from the binary search tree.
 * 
 * @param node The tree node to be freed.
 *******************************************************************************/
template <typename T>
void BSTree<T>::free_node(BinTree _node)
{
  // Explicitly call the destructor for the node to clean up its resources
  _node->~BinTreeNode();

  // Use the custom ObjectAllocator to deallocate the memory for the node
  m_OA->Free(_node);
}

/*!*****************************************************************************
 * @brief Calculates the height of the binary search tree.
 * 
 * This function determines the height of the tree or subtree rooted at the given
 * node. The height of a tree is the number of edges on the longest path from the
 * root node to a leaf node. An empty tree has a height of -1.
 * 
 * @param tree The root node of the tree or subtree whose height is to be calculated.
 * @return The height of the tree or subtree.
 *******************************************************************************/
template <typename T>
int BSTree<T>::tree_height(BinTree _tree) const
{
  // Base case: if the current node is nullptr, the height is -1
  if (_tree == nullptr)
    return -1;
  else
  {
    // Recursively calculate the height of the left and right subtrees
    int height_left = tree_height(_tree->left);
    int height_right = tree_height(_tree->right);

    // The height of the current node is 1 plus the greater of the heights of its subtrees
    return std::max(height_left, height_right) + 1;
  }
}

/*!*****************************************************************************
 * @brief Finds the predecessor of a given node in the BSTree.
 * 
 * The predecessor of a node in a binary search tree is the maximum value node
 * in its left subtree. This function finds and sets the predecessor by traversing
 * the rightmost path starting from the given node's left child.
 * 
 * @param _tree The node for which to find the predecessor.
 * @param _predecessor Reference to a pointer where the found predecessor will be stored.
 *******************************************************************************/
template <typename T>
void BSTree<T>::find_predecessor(BinTree _tree, BinTree &_predecessor) const
{
  _predecessor = _tree->left; // Start with the left child of the given node

  // Traverse to the rightmost node in the left subtree
  while (_predecessor->right != nullptr)
  {
    _predecessor = _predecessor->right;
  }
}

/*!*****************************************************************************
 * @brief Recursively copies a tree or subtree from a source to a destination.
 * 
 * This function performs a deep copy of the binary search tree or subtree rooted
 * at the source node, creating a new tree structure identical to the source
 * and storing the root of the new tree in the destination pointer.
 * 
 * @param _source The root of the source tree or subtree to be copied.
 * @param _dest Reference to the pointer where the root of the new copied tree will be stored.
 *******************************************************************************/
template <typename T>
void BSTree<T>::DeepCopyTree(const BinTree &_source, BinTree &_dest)
{
  if (_source == nullptr) // Base case: if the source node is null, the destination is also set to null
  {
    _dest = nullptr;
  }
  else
  {
    _dest = make_node(_source->data); // Create a new node with the same data as the source node
    _dest->count = _source->count; // Copy the count from the source to the destination node
    _dest->balance_factor = _source->balance_factor; // Copy the balance factor

    // Recursively copy the left and right subtrees
    DeepCopyTree(_source->left, _dest->left);
    DeepCopyTree(_source->right, _dest->right);
  }
}

/*!*****************************************************************************
 * @brief Recursively frees all nodes in a tree or subtree.
 * 
 * This function traverses the tree or subtree rooted at the given node in
 * post-order and deallocates each node, effectively clearing the tree or subtree.
 * 
 * @param _tree The root node of the tree or subtree to be freed.
 *******************************************************************************/
template <typename T>
void BSTree<T>::FreeTree(BinTree _tree)
{
  if (_tree == nullptr) // Base case: if the node is null, there's nothing to free
    return;

  // Recursively free the left and right subtrees first
  FreeTree(_tree->left);
  FreeTree(_tree->right);

  // Then free the current node
  free_node(_tree);
}

/*!*****************************************************************************
 * @brief Recursively inserts a new value into the BSTree, updating tree metrics.
 * 
 * This function inserts a new value into the binary search tree at the correct
 * position to maintain BST properties. It also updates the size and height of the
 * tree as necessary, and increments the count of nodes in the path to the inserted node.
 * 
 * @param _node Reference to the node pointer where the new value might be inserted.
 * @param _value The value to insert into the tree.
 * @param _depth The current depth in the tree, used for height calculation.
 *******************************************************************************/
template <typename T>
void BSTree<T>::InsertNode(BinTree &_node, const T &_value, int _depth)
{
  try
  {
    // Base case: if the current node is null, insert the new value here
    if (_node == nullptr)
    {
      _node = make_node(_value); // Create a new node with the given value
      ++m_Size; // Increment the size of the tree

      // Update the tree's height if the current depth is greater
      if (_depth > m_Height)
        m_Height = _depth;

      return;
    }

    // If the value is less than the current node's data, insert into the left subtree
    if (_value < _node->data)
    {
      InsertNode(_node->left, _value, _depth + 1); // Recursive call for the left child
    }
    else // Otherwise, insert into the right subtree
    {
      InsertNode(_node->right, _value, _depth + 1); // Recursive call for the right child
    }

    // After insertion, increment the count for the current node
    ++_node->count;
  }
  catch (const BSTException &except)
  {
    // Rethrow the caught exception to be handled by the caller
    throw; // For rethrowing the current exception without losing its original context
  }
}

/*!*****************************************************************************
 * @brief Recursively deletes a node with the specified value from the BSTree.
 * 
 * This function searches for and deletes a node containing the given value.
 * If the node to be deleted has two children, it finds the predecessor, copies
 * its data to the node, and then recursively deletes the predecessor. It also
 * updates the node count and tree size as necessary.
 * 
 * @param _node Reference to the current node being examined or modified.
 * @param _value The value of the node to be deleted.
 *******************************************************************************/
template <typename T>
void BSTree<T>::DeleteNode(BinTree &_node, const T &_value)
{
  if (_node == nullptr) // Base case: if the node is null, return
    return;

  if (_value < _node->data) // If the value is in the left subtree
  {
    DeleteNode(_node->left, _value); // Recurse on the left child
    --_node->count; // Decrement the count after recursion
  }
  else if (_value > _node->data) // If the value is in the right subtree
  {
    DeleteNode(_node->right, _value); // Recurse on the right child
    --_node->count; // Decrement the count after recursion
  }
  else // If the current node contains the value to be deleted
  {
    --_node->count; // Decrement the count

    // Case 1: The node has no left child (also covers the case where the node has no children)
    if (_node->left == nullptr)
    {
      BinTree tmp = _node; // Temporary pointer to hold the current node
      _node = _node->right; // Replace the current node with its right child
      free_node(tmp); // Free the memory of the deleted node
      --m_Size; // Decrement the size of the tree
    }
    // Case 2: The node has no right child
    else if (_node->right == nullptr)
    {
      BinTree tmp = _node; // Temporary pointer to hold the current node
      _node = _node->left; // Replace the current node with its left child
      free_node(tmp); // Free the memory of the deleted node
      --m_Size; // Decrement the size of the tree
    }
    // Case 3: The node has two children
    else
    {
      BinTree pred = nullptr; // Pointer to hold the predecessor node
      find_predecessor(_node, pred); // Find the predecessor of the current node
      _node->data = pred->data; // Copy the data from the predecessor to the current node
      DeleteNode(_node->left, pred->data); // Recursively delete the predecessor node
    }
  }
}

/*!*****************************************************************************
 * @brief Recursively searches for a value in the tree, counting comparisons.
 * 
 * This function searches for a node containing the specified value within the
 * binary search tree. It increments a comparison counter at each step of the
 * recursion, providing insight into the number of comparisons made during the search.
 * 
 * @param _node The current node being examined.
 * @param _value The value to search for.
 * @param _compares A reference to the comparison counter.
 * @return True if the value is found, false otherwise.
 *******************************************************************************/
template <typename T>
bool BSTree<T>::FindNode(BinTree _node, const T &_value, unsigned &_compares) const
{
  ++_compares; // Increment the comparison counter
  
  if (_node == nullptr) // Base case: if the node is null, the value is not found
    return false;

  else if (_value == _node->data) // If the value matches the current node's data, return true
    return true;
    
  else if (_value < _node->data) // If the value is less than the current node's data, search in the left subtree
    return FindNode(_node->left, _value, _compares);
    
  else // If the value is greater than the current node's data, search in the right subtree
    return FindNode(_node->right, _value, _compares);
}

/*!*****************************************************************************
 * @brief Finds a node at a specified index based on an in-order traversal.
 * 
 * This function retrieves a node at the specified index, where the index is
 * determined by an in-order traversal of the tree. It uses the count of nodes
 * in each subtree to navigate directly to the indexed node without traversing
 * every node.
 * 
 * @param _tree The current subtree being examined.
 * @param _index The zero-based index of the node to find.
 * @return The node at the specified index, or nullptr if not found.
 *******************************************************************************/
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::FindNodeAtIndex(BinTree _tree, unsigned _index) const
{
  if (_tree == nullptr) // Base case: if the subtree is null, return nullptr
  {
    return nullptr;
  }

  unsigned int left_count = (_tree->left) ? _tree->left->count : 0; // Count of nodes in the left subtree

  if (_index < left_count) // If the index is within the left subtree
  {
    return FindNodeAtIndex(_tree->left, _index);
  }
  else if (_index > left_count) // If the index is beyond the left subtree
  {
    return FindNodeAtIndex(_tree->right, _index - left_count - 1); // Adjust the index when moving to the right subtree
  }
  else // If the index matches the count of the left subtree, the current node is the target
  {
    return _tree;
  }
}