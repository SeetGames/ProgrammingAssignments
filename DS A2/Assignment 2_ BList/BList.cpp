/*!*************************************************************************
\file BList.cpp
\author Seetoh Wei Tung
\par DP email: seetoh.w@digipen.edu
\par Course: Data Structures
\par Assignment 2
\date 12-02-2024
\brief
This file contains the implementation for BList
***************************************************************************/
/*!
\brief
  This function returns the memory size of a node in bytes.


\return size of node.
*/
/******************************************************************************/
template <typename T, unsigned Size>
size_t BList<T, Size>::nodesize(void)
{
  return sizeof(BNode);
}

/******************************************************************************/
/*!
\brief
  This function returns the head of list.

\return The head node.
*/
/******************************************************************************/
template <typename T, unsigned Size>
const typename BList<T, Size>::BNode *BList<T, Size>::GetHead() const
{
  return head_;
}

/******************************************************************************/
/*!
\brief
  Default Constructor
*/
/******************************************************************************/
template <typename T, unsigned Size>
BList<T, Size>::BList() : head_{nullptr}, tail_{nullptr}
{
  listStats_.NodeSize = nodesize();
  listStats_.ArraySize = static_cast<int>(Size);
}

/******************************************************************************/
/*!
\brief
  Copy Constructor
\par rhs the BList to copy.
*/
/******************************************************************************/
template <typename T, unsigned Size>
BList<T, Size>::BList(const BList &rhs) : listStats_{rhs.listStats_}
{
  auto *sourceCurrent = rhs.GetHead();
  BNode *newCurrent = nullptr;
  BNode *newPrev = nullptr;

  while (sourceCurrent)
  {
    newCurrent = AllocateNewNode(sourceCurrent);

    if (newPrev) // Link the new node with the previous one if it's not the first node
    {
      newPrev->next = newCurrent;
      newCurrent->prev = newPrev;
    }
    else // For the first node, set it as the head of the new list
    {
      head_ = newCurrent;
    }

    newPrev = newCurrent; // Move forward in the list
    sourceCurrent = sourceCurrent->next; // Move to the next node in the source list
  }

  tail_ = newCurrent; // The last created node is the tail of the new list
}

/******************************************************************************/
/*!
\brief
  Destructor
*/
/******************************************************************************/
template <typename T, unsigned Size>
BList<T, Size>::~BList()
{
  clear();
}

/******************************************************************************/
/*!
\brief
  Copy assignment operator
\par rhs the BList to copy.
*/
/******************************************************************************/
template <typename T, unsigned Size>
BList<T, Size> &BList<T, Size>::operator=(const BList &rhs)
{
  // Check for self-assignment
  if (this == &rhs) {
    return *this; // Return the current object if the same
  }

  clear(); // Clear the current list to prepare for the copy

  // Initialize pointers for iteration and linking
  auto *sourceNode = rhs.GetHead();  // Node in the source list
  BNode *newNode = nullptr;          // New node for the current list
  BNode *lastNewNode = nullptr;      // Last node added to the current list

  // Iterate over the source list and copy its nodes
  while (sourceNode)
  {
    // Create a new node based on the current node in the source list
    newNode = AllocateNewNode(sourceNode);

    if (lastNewNode) // If not the first node, link it with the previous one
    {
      lastNewNode->next = newNode; // Link the previous node's next to the new node
      newNode->prev = lastNewNode; // Link the new node's prev to the last node
    }
    else // For the first node, set it as the head of the new list
    {
      head_ = newNode;
    }

    // Prepare for the next iteration
    lastNewNode = newNode;       // Update the last node to the new node
    sourceNode = sourceNode->next; // Move to the next node in the source list
  }

  tail_ = lastNewNode; // Set the tail of the new list to the last new node created
  listStats_ = rhs.GetStats(); // Copy the statistics from the source list

  return *this; // Return a reference to the current list
}


/******************************************************************************/
/*!
\brief
  This function pushes a value to the back of the list.
\par value to push.
*/
/******************************************************************************/
template <typename T, unsigned Size>
void BList<T, Size>::push_back(const T &value)
{
  // Check if the tail exists and has space for the new value
  if (tail_ && tail_->count < listStats_.ArraySize)
  {
    // Add the value to the tail node and increment the count within that node
    tail_->values[tail_->count] = value;
    IncreaseNodeItemCount(tail_);
  }
  else
  {
    // Create a new node since there's no space in the tail node or no tail exists
    BNode *newTail = AllocateNewNode();
    newTail->values[0] = value; // Add the value as the first element in the new node
    IncreaseNodeItemCount(newTail);

    // Link the new node to the list
    if (!head_) // If the list is empty (no head exists)
    {
      head_ = tail_ = newTail; // The new node becomes both head and tail
    }
    else // If the list is not empty
    {
      tail_->next = newTail; // Link the old tail to the new node
      newTail->prev = tail_; // Set the new node's previous to the old tail
      tail_ = newTail; // Update the tail to the new node
    }

    // Update list statistics
    ++listStats_.NodeCount; // Increment the node count
  }

  ++listStats_.ItemCount; // Increment the total item count in the list
}


/******************************************************************************/
/*!
\brief
  This function pushes a value to the front of the list.
\par value to push.
*/
/******************************************************************************/
template <typename T, unsigned Size>
void BList<T, Size>::push_front(const T &value)
{
  // Check if the head exists and has space to insert the new value at the front
  if (head_ && head_->count < listStats_.ArraySize)
  {
    // Shift existing values in the head node to the right to make space for the new value
    for (int index = head_->count; index > 0; --index)
    {
      head_->values[index] = head_->values[index - 1];
    }

    // Insert the new value at the beginning of the head node
    head_->values[0] = value;
    IncreaseNodeItemCount(head_); // Update the count of values in the head node
  }
  else
  {
    // Create a new node as no space is available at the front of the head node or the head does not exist
    BNode *newHead = AllocateNewNode();
    newHead->values[0] = value; // Set the first value in the new node to the new value
    IncreaseNodeItemCount(newHead); // Update the count of values in the new node

    // Link the new node into the list
    if (!head_) // If the list is empty (head does not exist)
    {
      head_ = tail_ = newHead; // The new node becomes both head and tail
    }
    else // If the list is not empty
    {
      newHead->next = head_; // Link the new node to the old head
      head_->prev = newHead; // Set the old head's previous to the new node
      head_ = newHead; // Update the head to the new node
    }

    // Update list statistics
    ++listStats_.NodeCount; // Increment the node count as a new node has been added
  }

  ++listStats_.ItemCount; // Increment the total item count in the list
}

/******************************************************************************/
/*!
\brief
  This function insert a value into the list while maintaining order.
\par value to push.
*/
/******************************************************************************/
template <typename T, unsigned Size>
void BList<T, Size>::insert(const T &value)
{
  // If the list is empty, add the value at the front and return
  if (!head_)
  {
    push_front(value);
    return;
  }

  // Initialize pointers to traverse the list and find the insert position
  BNode *nodeToInsert = head_;
  int insertPosition = 0;

  // Traverse the list to find the right node and position for the new value
  while (nodeToInsert)
  {
    // Find the insert position within the current node
    while (insertPosition < listStats_.ArraySize && nodeToInsert->values[insertPosition] < value)
    {
      ++insertPosition;
    }

    // Check if we've found the correct position within the current node
    if (insertPosition < nodeToInsert->count)
    {
      break; // Found the right node and position
    }

    // Reset position and move to the next node
    insertPosition = 0;
    nodeToInsert = nodeToInsert->next;
  }

  // Insert the value in the found position
  if (nodeToInsert) // If a suitable node was found within the list
  {
    if (insertPosition == 0) // If inserting at the beginning of a node
    {
      // Try inserting in the previous node if it has space
      if (nodeToInsert->prev && nodeToInsert->prev->count < listStats_.ArraySize)
      {
        InsertValueAtIndex(nodeToInsert->prev, nodeToInsert->prev->count, value);
      }
      // Otherwise, insert at the current position, split the current or previous node if needed
      else
      {
        if (nodeToInsert->count < listStats_.ArraySize)
        {
          InsertValueAtIndex(nodeToInsert, insertPosition, value);
        }
        else if (nodeToInsert->prev)
        {
          SplitNode(nodeToInsert->prev, listStats_.ArraySize, value);
        }
        else
        {
          SplitNode(nodeToInsert, insertPosition, value);
        }
      }
    }
    else // If inserting at a position other than the beginning
    {
      // Insert directly if there's space, or split the node if it's full
      if (nodeToInsert->count < listStats_.ArraySize)
      {
        InsertValueAtIndex(nodeToInsert, insertPosition, value);
      }
      else
      {
        SplitNode(nodeToInsert, insertPosition, value);
      }
    }
  }
  else // If no suitable node was found (we're at the tail)
  {
    // Insert in the tail if there's space, or split the tail node if it's full
    if (tail_->count < listStats_.ArraySize)
    {
      InsertValueAtIndex(tail_, tail_->count, value);
    }
    else
    {
      SplitNode(tail_, tail_->count, value);
    }
  }
}


/******************************************************************************/
/*!
\brief
  This function removes a value at the given index.
\par index of the list to remove the value from.
*/
/******************************************************************************/
template <typename T, unsigned Size>
void BList<T, Size>::remove(int index)
{
  // Find the node that contains the value at 'index'
  BNode *targetNode = FindNodeByIndex(index);

  // Calculate the position within 'targetNode' where the value is located
  int positionInNode = index % Size;

  // Remove the value at 'positionInNode' from 'targetNode'
  RemoveValueAtIndex(targetNode, positionInNode);

  // If 'targetNode' is now empty after removal, free it to maintain list integrity
  if (targetNode->count == 0)
  {
    DeleteNode(targetNode);
  }
}

/******************************************************************************/
/*!
\brief
  This function removes a value from the list.
\par value to remove.
*/
/******************************************************************************/
template <typename T, unsigned Size>
void BList<T, Size>::remove_by_value(const T &value)
{
  BNode *searchNode = head_;
  int valuePosition = 0;
  bool valueFound = false;

  // Iterate through each node in the list
  while (searchNode)
  {
    // Search for the value within the current node
    for (int i = 0; i < searchNode->count; ++i)
    {
      if (searchNode->values[i] == value)
      {
        valuePosition = i; // Record the position of the value
        valueFound = true; // Indicate that the value has been found
        break; // Stop searching once the value is found
      }
    }

    if (valueFound)
    {
      break; // Exit the loop if the value has been found
    }

    searchNode = searchNode->next; // Move to the next node if the value hasn't been found
  }

  // If the value was found in a node
  if (searchNode)
  {
    // Remove the value from the node
    RemoveValueAtIndex(searchNode, valuePosition);

    // If the node is empty after removal, free it
    if (searchNode->count == 0)
    {
      DeleteNode(searchNode);
    }
  }
}

/******************************************************************************/
/*!
\brief
  This function finds the index of the element containing \p value.
\par value to find.
\return -1 if index is not found.
*/
/******************************************************************************/
template <typename T, unsigned Size>
int BList<T, Size>::find(const T &value) const
{
  BNode *searchNode = head_;
  int absoluteIndex = 0;

  // Iterate through each node in the list
  while (searchNode)
  {
    // Search for the value within the current node
    for (int i = 0; i < searchNode->count; ++i)
    {
      // Check if the current value matches the target value
      if (searchNode->values[i] == value)
      {
        // Return the absolute index of the found value within the list
        return absoluteIndex + i;
      }
    }

    // Update the running total of indices to include the current node's count
    absoluteIndex += searchNode->count;

    // Move to the next node in the list
    searchNode = searchNode->next;
  }

  // Return -1 if the value is not found in the list
  return -1;
}

/******************************************************************************/
/*!
\brief
  Subscript operator of the list allows array like access. No bounds check.
\par index position to access.
*/
/******************************************************************************/
template <typename T, unsigned Size>
T &BList<T, Size>::operator[](int index)
{
  return RetrieveValueByIndex(index);
}

/******************************************************************************/
/*!
\brief
  Subscript operator of the list allows array like access. No bounds check.
\par index position to access.
*/
/******************************************************************************/
template <typename T, unsigned Size>
const T &BList<T, Size>::operator[](int index) const
{
  return RetrieveValueByIndex(index);
}

/******************************************************************************/
/*!
\brief
  This function returns the current size of the list.
\return number of items currently in the list.
*/
/******************************************************************************/
template <typename T, unsigned Size>
size_t BList<T, Size>::size() const
{
  return listStats_.ItemCount;
}

/******************************************************************************/
/*!
\brief
  This function removes all items in the list.
*/
/******************************************************************************/
template <typename T, unsigned Size>
void BList<T, Size>::clear()
{
  while (listStats_.ItemCount > 0)
    remove(0);
}


template <typename T, unsigned Size>
BListStats BList<T, Size>::GetStats() const
{
  return listStats_;
}


template <typename T, unsigned Size>
typename BList<T, Size>::BNode *BList<T, Size>::AllocateNewNode(const BNode *sourceNode)
{
  BNode *newNode = nullptr;

  try
  {
    // Attempt to allocate a new node
    newNode = new BNode();

    // If a source node is provided, copy its data to the new node
    if (sourceNode)
    {
      // Copy the count of values from the source node to the new node
      newNode->count = sourceNode->count;

      // Copy each value from the source node to the new node
      for (int i = 0; i < sourceNode->count; ++i)
      {
        newNode->values[i] = sourceNode->values[i];
      }
    }
  }
  catch (const std::exception &e) // Catch any exceptions that occur during allocation
  {
    // Rethrow as a BList-specific exception if memory allocation fails
    throw BListException(BListException::E_NO_MEMORY, e.what());
  }

  // Return the newly created node, which may be a blank node or a copy of 'sourceNode'
  return newNode;
}


template <typename T, unsigned Size>
typename BList<T, Size>::BNode *BList<T, Size>::FindNodeByIndex(int targetIndex) const
{
  // Validate the index to ensure it's within the bounds of the list
  if (targetIndex < 0 || targetIndex >= listStats_.ItemCount)
  {
    throw BListException(BListException::E_BAD_INDEX, "Index out of range!");
  }

  BNode *node = head_;
  int accumulatedCount = 0;

  // Traverse the list to find the node that contains the value at 'targetIndex'
  while (node)
  {
    accumulatedCount += node->count;

    // Check if the accumulated count has reached or surpassed 'targetIndex'
    if (accumulatedCount > targetIndex)
    {
      break; // The current node is the one or is past the one containing the target index
    }

    node = node->next; // Move to the next node in the list
  }

  // Determine the correct node to return based on the traversal outcome
  if (node == nullptr)
  {
    // If we've traversed past the last node, return the tail
    return tail_;
  }
  else if (accumulatedCount > targetIndex)
  {
    // If we're within a node (not past it), return that node
    return node;
  }
  else
  {
    // The index is within the head node
    return head_;
  }
}


template <typename T, unsigned Size>
void BList<T, Size>::DeleteNode(BNode *node)
{
  // If the node to be freed has a previous node, update the previous node's 'next' pointer
  if (node->prev)
    node->prev->next = node->next; // Link the previous node directly to the next node, bypassing the current node
  else
    head_ = node->next; // If there's no previous node, this node is the head, so update the head pointer to the next node

  // If the node to be freed has a next node, update the next node's 'prev' pointer
  if (node->next)
    node->next->prev = node->prev; // Link the next node back to the previous node, bypassing the current node
  else
    tail_ = node->prev; // If there's no next node, this node is the tail, so update the tail pointer to the previous node

  // Explicitly call the destructor for the node object before freeing the memory
  // This is necessary to properly clean up the node's resources, especially if it contains non-POD types
  node->~BNode();

  // Free the memory allocated for the node
  delete node;

  // Decrement the node count to reflect the removal of a node from the list
  --listStats_.NodeCount;
}


template <typename T, unsigned Size>
void BList<T, Size>::IncreaseNodeItemCount(BNode *node)
{
  ++node->count;
  if (node->count > listStats_.ArraySize)
    node->count = listStats_.ArraySize;
}


template <typename T, unsigned Size>
void BList<T, Size>::SplitNode(BNode *targetNode, int insertIndex, const T &insertValue)
{
  // Create a new node and set its previous link to the target node
  BNode *newNode = AllocateNewNode();
  newNode->prev = targetNode;

  // If the target node has a next node, adjust the links accordingly
  if (targetNode->next)
  {
    newNode->next = targetNode->next;
    targetNode->next->prev = newNode;
  }
  targetNode->next = newNode;

  // Special handling when each node contains only one value
  if (listStats_.ArraySize == 1)
  {
    // Splitting and inserting for nodes that can hold only 1 value
    if (insertIndex == 0)
    {
      newNode->values[0] = targetNode->values[0];
      targetNode->values[0] = insertValue;
    }
    else
    {
      newNode->values[0] = insertValue;
    }
    IncreaseNodeItemCount(newNode);
  }
  else
  {
    // Calculate the middle index for splitting the node
    int middleIndex = listStats_.ArraySize / 2;

    // Move the upper half of the values from the target node to the new node
    for (int i = middleIndex, j = 0; i < listStats_.ArraySize; ++i, ++j)
    {
      newNode->values[j] = targetNode->values[i];
      IncreaseNodeItemCount(newNode);
    }
    // Adjust the count of values in the target node
    targetNode->count = middleIndex;

    // Insert the new value in the appropriate node
    if (insertIndex <= middleIndex)
    {
      // Insert in the target node if the index is within the lower half
      for (int i = targetNode->count; i > insertIndex; --i)
      {
        targetNode->values[i] = targetNode->values[i - 1];
      }
      targetNode->values[insertIndex] = insertValue;
      IncreaseNodeItemCount(targetNode);
    }
    else
    {
      // Adjust the insert index for the new node and insert
      insertIndex -= middleIndex;
      for (int i = newNode->count; i > insertIndex; --i)
      {
        newNode->values[i] = newNode->values[i - 1];
      }
      newNode->values[insertIndex] = insertValue;
      IncreaseNodeItemCount(newNode);
    }
  }

  // Update the tail pointer if necessary
  if (targetNode == tail_)
  {
    tail_ = newNode;
  }

  // Increment the overall item and node counts
  ++listStats_.ItemCount;
  ++listStats_.NodeCount;
}


template <typename T, unsigned Size>
T &BList<T, Size>::RetrieveValueByIndex(int targetIndex) const
{
  BNode *node = head_;
  int cumulativeIndex = node->count; // Tracks the cumulative index up to the current node
  int nodeIndex = targetIndex; // Adjusted index within the found node

  // Traverse the list to find the node containing the target index
  while (cumulativeIndex <= targetIndex)
  {
    nodeIndex -= node->count; // Adjust the index relative to the start of the next node
    node = node->next; // Move to the next node

    if (node)
    {
      cumulativeIndex += node->count; // Update the cumulative index to include the next node
    }
  }

  // Return a reference to the value at the adjusted index within the found node
  return node->values[nodeIndex];
}


template <typename T, unsigned Size>
void BList<T, Size>::InsertValueAtIndex(BNode *targetNode, int targetIndex, const T &insertValue)
{
  // Start from the last value in the node and shift values to the right until reaching the target index
  for (int i = targetNode->count; i > targetIndex; --i)
  {
    targetNode->values[i] = targetNode->values[i - 1]; // Shift each value one position to the right
  }

  // Insert the new value at the target index
  targetNode->values[targetIndex] = insertValue;

  // Increment the count of values in the node to account for the newly inserted value
  IncreaseNodeItemCount(targetNode);

  // Increment the total item count in the list
  ++listStats_.ItemCount;
}


template <typename T, unsigned Size>
void BList<T, Size>::RemoveValueAtIndex(BNode *targetNode, int targetIndex)
{
  // Shift values to the left starting from the target index to fill the gap created by the removed value
  for (int i = targetIndex; i < targetNode->count - 1; ++i)
  {
    targetNode->values[i] = targetNode->values[i + 1]; // Move each subsequent value one position to the left
  }

  // Decrement the count of values in the node to reflect the removal of a value
  --targetNode->count;

  // Decrement the total item count in the list
  --listStats_.ItemCount;
}
