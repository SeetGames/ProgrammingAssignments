/*!*****************************************************************************
 \file queue.cpp
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

#include "queue.hpp"

namespace HLP3
{
    Queue::Queue(size_type _capacity) 
    : m_Buffer(new value_type[_capacity]),
    m_Capacity(_capacity),
    m_Size(0),
    m_Head(0)
    {
    }

    Queue::Queue(const Queue &_copiedQueue)
    : m_Buffer(new value_type[_copiedQueue.m_Capacity]),
    m_Capacity(_copiedQueue.m_Capacity),
    m_Size(_copiedQueue.m_Size),
    m_Head(_copiedQueue.m_Head)
    {
        for (size_type i{0}; i < m_Size; ++i)
        {
            m_Buffer[(m_Head + i) % m_Capacity] = _copiedQueue.m_Buffer[(_copiedQueue.m_Head + i) % _copiedQueue.m_Capacity];
        }
    }
    Queue::~Queue()
    {
        delete[] m_Buffer;
    }
    Queue& Queue::operator=(const Queue& _copiedQueue)
    {
        // Protect against self-assignment
        if (this == &_copiedQueue)
            return *this;
        //Copy and swap idiom
        Queue temp(_copiedQueue);
        swap(temp);
        return *this;
    }
    Queue::pointer Queue::c_buff() const
    {
        return m_Buffer;
    }
    bool Queue::empty() const
    {
        return m_Size == 0;
    }
    bool Queue::full() const
    {
        return m_Size == m_Capacity;
    }
    Queue::size_type Queue::size() const
    {
        return m_Size;
    }
    Queue::size_type Queue::capacity() const
    {
        return m_Capacity;
    }

    void Queue::push(value_type _value)
    {
        //Writes a value in element m_Bufffer[tail] and increments m_Size
        //When m_Size reaches m_Capacity, push continues to store values in the same place
        //buffer[tail], but increments m_Head
        //Whenever m_Head reaches m_Capacity, it is reset to 0
        //m_Size is never incremented past m_Capacity nor decremented below 0
        m_Buffer[back_index()] = _value;
        if (full())
            m_Head = (m_Head + 1) % m_Capacity;
        else
            ++m_Size;
    }
    void Queue::pop()
    {
        if (empty())
            return;
        //Increments head and decrements size
        m_Head = (m_Head + 1) % m_Capacity;
        --m_Size;
    }
    Queue::value_type Queue::front() const
    {
        return m_Buffer[m_Head];
    }
    Queue::value_type &Queue::front()
    {
        return m_Buffer[m_Head];
    }
    Queue::value_type Queue::back() const
    {
        return m_Buffer[(m_Head + m_Size-1) % m_Capacity];
    }
    Queue::value_type &Queue::back()
    {
        return m_Buffer[(m_Head + m_Size-1) % m_Capacity];
    }
    Queue::size_type Queue::front_index() const
    {
        return m_Head;
    }
    Queue::size_type Queue::back_index() const
    {
        return (m_Head + m_Size) % m_Capacity;
    }
    void Queue::swap(Queue &_otherQueue)
    {
        //swap data without std::swap
        pointer tempBuffer = m_Buffer;
        size_type tempCapacity = m_Capacity;
        size_type tempSize = m_Size;
        size_type tempHead = m_Head;

        m_Buffer = _otherQueue.m_Buffer;
        m_Capacity = _otherQueue.m_Capacity;
        m_Size = _otherQueue.m_Size;
        m_Head = _otherQueue.m_Head;

        _otherQueue.m_Buffer = tempBuffer;
        _otherQueue.m_Capacity = tempCapacity;
        _otherQueue.m_Size = tempSize;
        _otherQueue.m_Head = tempHead;
    }

    void swap(Queue &_firstQueue, Queue &_secondQueue)
    {
        _firstQueue.swap(_secondQueue);
    }
}
