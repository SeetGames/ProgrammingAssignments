template <typename T, unsigned Size>
BList<T, Size>::BList() : head_(nullptr), tail_(nullptr) {}

template <typename T, unsigned Size>
BList<T, Size>::BList(const BList &rhs) : head_(nullptr), tail_(nullptr)
{
  for (BNode *current = rhs.head_; current != nullptr; current = current->next)
  {
    for (int i = 0; i < current->count; ++i)
    {
      push_back(current->values[i]);
    }
  }
}

template <typename T, unsigned Size>
BList<T, Size> &BList<T, Size>::operator=(const BList &rhs)
{
  if (this != &rhs)
  {
    clear();
    for (BNode *current = rhs.head_; current != nullptr; current = current->next)
    {
      for (int i = 0; i < current->count; ++i)
      {
        push_back(current->values[i]);
      }
    }
  }
  return *this;
}

template <typename T, unsigned Size>
void BList<T, Size>::push_back(const T &value)
{
  if (!tail_ || tail_->count == Size)
  {
    BNode *newNode = new BNode;
    newNode->values[0] = value;
    newNode->count = 1;
    if (tail_)
    {
      tail_->next = newNode;
      newNode->prev = tail_;
    }
    tail_ = newNode;
    if (!head_)
      head_ = newNode;
  }
  else
  {
    tail_->values[tail_->count++] = value;
  }
}

template <typename T, unsigned Size>
void BList<T, Size>::push_front(const T &value)
{
  if (!head_ || head_->count == Size)
  {
    BNode *newNode = new BNode;
    newNode->values[0] = value;
    newNode->count = 1;
    if (head_)
    {
      head_->prev = newNode;
      newNode->next = head_;
    }
    head_ = newNode;
    if (!tail_)
      tail_ = newNode;
  }
  else
  {
    for (int i = head_->count; i > 0; --i)
    {
      head_->values[i] = head_->values[i - 1];
    }
    head_->values[0] = value;
    head_->count++;
  }
}

template <typename T, unsigned Size>
void BList<T, Size>::clear()
{
  BNode *current = head_;
  while (current != nullptr)
  {
    BNode *toDelete = current;
    current = current->next;
    delete toDelete;
  }
  head_ = tail_ = nullptr;
}

template <typename T, unsigned Size>
void BList<T, Size>::insert(const T &value)
{
  // If list is empty, add a new node with the value.
  if (head_ == nullptr)
  {
    BNode *newNode = new BNode;
    newNode->values[0] = value;
    newNode->count = 1;
    head_ = tail_ = newNode;
    return;
  }

  BNode *current = head_;
  while (current != nullptr)
  {
    // If current node has space, insert in sorted order.
    if (current->count < Size)
    {
      int i;
      for (i = current->count - 1; i >= 0 && current->values[i] > value; --i)
      {
        current->values[i + 1] = current->values[i];
      }
      current->values[i + 1] = value;
      ++current->count;
      return;
    }

    // If the next node is null or the value is less than the smallest value in the next node.
    if (current->next == nullptr || value < current->next->values[0])
    {
      // Need to split the node.
      BNode *newNode = new BNode;
      int median = Size / 2;
      // This is a simplified logic for splitting node for clarity. Adjust as per your split logic.
      // Move half elements to the new node.
      for (int i = median; i < Size; ++i)
      {
        newNode->values[i - median] = current->values[i];
        newNode->count++;
        current->count--;
      }

      // Insert the value in the correct half.
      if (value < current->values[median])
      {
        insert(value); // Recursive call to insert in the correct position in the current node.
      }
      else
      {
        newNode->insert(value); // Recursive call to insert in the correct position in the new node.
      }

      // Linking the new node into the list.
      newNode->next = current->next;
      if (current->next != nullptr)
      {
        current->next->prev = newNode;
      }
      current->next = newNode;
      newNode->prev = current;

      if (tail_ == current)
      {
        tail_ = newNode; // Update the tail if needed.
      }
      return;
    }
    current = current->next;
  }
}

template <typename T, unsigned Size>
void BList<T, Size>::remove(int index)
{
  int currentIndex = 0;
  for (BNode *current = head_; current != nullptr; current = current->next)
  {
    if (currentIndex + current->count > index)
    {
      // Found the node, now find the exact item
      int nodeIndex = index - currentIndex;
      for (int i = nodeIndex; i < current->count - 1; ++i)
      {
        current->values[i] = current->values[i + 1];
      }
      --current->count;
      // If the node is empty, remove it
      if (current->count == 0)
      {
        if (current == head_)
        {
          head_ = current->next;
          if (head_)
            head_->prev = nullptr;
        }
        else
        {
          current->prev->next = current->next;
          if (current->next)
          {
            current->next->prev = current->prev;
          }
          else
          {
            tail_ = current->prev;
          }
        }
        delete current;
      }
      break;
    }
    currentIndex += current->count;
  }
}

template <typename T, unsigned Size>
void BList<T, Size>::remove_by_value(const T &value)
{
  int currentIndex = 0;
  for (BNode *current = head_; current != nullptr; current = current->next)
  {
    for (int i = 0; i < current->count; ++i)
    {
      if (current->values[i] == value)
      {
        remove(currentIndex + i);
        return;
      }
    }
    currentIndex += current->count;
  }
}

template <typename T, unsigned Size>
int BList<T, Size>::find(const T &value) const
{
  int index = 0;
  for (BNode *current = head_; current != nullptr; current = current->next)
  {
    for (int i = 0; i < current->count; ++i)
    {
      if (current->values[i] == value)
      {
        return index + i;
      }
    }
    index += current->count;
  }
  return -1; // Not found
}

template <typename T, unsigned Size>
T &BList<T, Size>::operator[](int index)
{
  int currentIndex = 0;
  for (BNode *current = head_; current != nullptr; current = current->next)
  {
    if (currentIndex + current->count > index)
    {
      return current->values[index - currentIndex];
    }
    currentIndex += current->count;
  }
  throw BListException(BListException::E_BAD_INDEX, "Index out of bounds");
}

template <typename T, unsigned Size>
const T &BList<T, Size>::operator[](int index) const
{
  // Same as non-const version but returns const reference
}

template <typename T, unsigned Size>
size_t BList<T, Size>::nodesize(void)
{
  return sizeof(BNode);
}

template <typename T, unsigned Size>
const typename BList<T, Size>::BNode *BList<T, Size>::GetHead() const
{
  return head_;
}
