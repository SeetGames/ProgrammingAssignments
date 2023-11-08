/*!*****************************************************************************
 \file queue.hpp
 \author Seetoh Wei Tung
 \par DP email: seetoh.w\@digipen.edu
 \par Course: HLP3
 \par Section: B
 \par Programming Assignment #6
 \date 9-11-2023
 \brief
    Defines a class Queue which is a circular buffer that stores values of type
    char.
*******************************************************************************/

#ifndef QUEUE_HPP
#define QUEUE_HPP

namespace HLP3
{

  class Queue
  {
  public:
    using value_type = char;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using size_type = unsigned long;

    /*!*****************************************************************************
     * @brief Prohibit default construction.
    *******************************************************************************/
    Queue() = delete;

    /*!*****************************************************************************
     * @brief Construct the Queue with a capacity of _capacity which will dynamically
      *        allocate an array of value_type of size _capacity
     * 
     * @param _capacity 
    *******************************************************************************/
    Queue(size_type _capacity);

    /*!*****************************************************************************
     * @brief Construct a new Queue object by copying the contents of another Queue
     * 
     * @param other
    *******************************************************************************/
    Queue(const Queue& _copiedQueue);

    /*!*****************************************************************************
     * @brief Destroy the Queue object
    *******************************************************************************/
    ~Queue();

    /*!*****************************************************************************
     * @brief Copy the contents of another Queue into this Queue
     * 
     * @param other 
     * @return Queue&
    *******************************************************************************/
    Queue& operator=(const Queue& _copiedQueue);

    /*!*****************************************************************************
     * @brief Returns a pointer to the queue's buffer [such that clients can only 
     examine but not change the buffer's contents].
     * 
     * @return pointer 
    *******************************************************************************/
    pointer c_buff() const;

    /*!*****************************************************************************
     * @brief Returns true if the queue is empty, false otherwise.
     * 
     * @return true 
     * @return false
    *******************************************************************************/
    bool empty() const;

    /*!*****************************************************************************
     * @brief Returns true if the queue is full, false otherwise.
     * 
     * @return true 
     * @return false
    *******************************************************************************/
    bool full() const;

    /*!*****************************************************************************
     * @brief Returns the number of elements in the queue.
     * 
     * @return size_type
    *******************************************************************************/
    size_type size() const;

    /*!*****************************************************************************
     * @brief Returns the capacity of the queue.
     * 
     * @return size_type
    *******************************************************************************/
    size_type capacity() const; 

    /*!*****************************************************************************
     * @brief Inserts a new element [an operation that could potentially overwrite 
     the oldest element in the buffer]
     * 
     * @param value
    *******************************************************************************/
    void push(value_type _value);
     
    /*!*****************************************************************************
     * @brief Removes the oldest element from the buffer.
    *******************************************************************************/
    void pop();

    /*!*****************************************************************************
     * @brief [Accessor] Returns the value of the element at the front of the queue.
     * 
     * @return value_type
    *******************************************************************************/
    value_type front() const;

    /*!*****************************************************************************
     * @brief [Mutator] Returns the value of the element at the front of the queue.
     * 
     * @return value_type
    *******************************************************************************/
    value_type& front();

    /*!*****************************************************************************
     * @brief [Accessor] Returns the value of the element at the back of the queue.
     * 
     * @return value_type
    *******************************************************************************/
    value_type back() const;

    /*!*****************************************************************************
     * @brief [Mutator] Returns the value of the element at the back of the queue.
     * 
     * @return value_type
    *******************************************************************************/
    value_type& back();

    /*!*****************************************************************************
     * @brief returns the index of the front element in the queue.
     * 
     * @return size_type 
    *******************************************************************************/
    size_type front_index() const;

    /*!*****************************************************************************
     * @brief returns the index of the back element in the queue.
     * 
     * @return size_type
    *******************************************************************************/
    size_type back_index() const;

    /*!*****************************************************************************
     * @brief Swaps the contents of this queue with another queue.
     * 
     * @param other
    *******************************************************************************/
    void swap(Queue& _otherQueue);

  private:
    pointer m_Buffer;
    size_type m_Capacity;
    size_type m_Size;
    size_type m_Head;
  };

  // declare necessary non-member function(s) ...
  /*!*****************************************************************************
   * @brief Swaps 2 objects of type queue.
   * 
   * @param _firstQueue 
   * @param _secondQueue 
  *******************************************************************************/
  void swap(Queue& _firstQueue, Queue& _secondQueue);
}
#endif
