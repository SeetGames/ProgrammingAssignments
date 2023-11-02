/*!*****************************************************************************
 \file ptr.h
 \author Seetoh Wei Tung
 \par DP email: seetoh.w\@digipen.edu
 \par Course: HLP3
 \par Section: B
 \par Programming Assignment #5
 \date 29-10-2023
 \brief
    Defines a class template Ptr that encapsulates a pointer to a dynamically
    allocated object of type T on the free store.
*******************************************************************************/

#include <iostream> // std::ostream

#ifndef PTR_H
#define PTR_H

namespace HLP3
{
  // partially defined class template Ptr
  template <typename T>
  class Ptr
  {
  private:
    T *p; // DO NOT AMEND!!!
  public:
    Ptr() = delete;                  // DO NOT AMEND!!!
    Ptr(Ptr &&) = delete;            // DO NOT AMEND!!!
    Ptr &operator=(Ptr &&) = delete; // DO NOT AMEND!!!
    explicit Ptr(T *_p) : p{_p} {}   // DO NOT AMEND!!!
    ~Ptr()
    {
      std::cout << __PRETTY_FUNCTION__ << std::endl;
      delete p;
    } // DO NOT AMEND!!!

    T *get() const { return p; } // DO NOT AMEND!!!

    /*!*****************************************************************************
     * @brief Copy constructor
     * 
     * @param rhs 
    *******************************************************************************/
    Ptr(const Ptr &rhs) : p{new T{*rhs}} {}

    /*!*****************************************************************************
     * @brief Copy conversion constructor
     * 
     * @tparam U Other Ptr type
     * @param rhs 
    *******************************************************************************/
    template <typename U>
    Ptr(const Ptr<U> &rhs) : p{new T{static_cast<T>(*rhs)}} {}

    /*!*****************************************************************************
     * @brief Copy assignment operator
     * 
     * @param rhs 
     * @return Ptr&
    *******************************************************************************/
    Ptr &operator=(const Ptr &rhs)
    {
      if (this != &rhs)
      {
        delete p;
        p = new T{*rhs};
      }
      return *this;
    }

    /*!*****************************************************************************
     * @brief Copy assignment conversion operator
     * 
     * @tparam U 
     * @param rhs 
     * @return Ptr& 
    *******************************************************************************/
    template <typename U>
    Ptr &operator=(const Ptr<U> &rhs)
    {
      if (reinterpret_cast<const void*>(this) != reinterpret_cast<const void*>(&rhs))
      {
        delete p;
        p = new T{static_cast<T>(*rhs.get())};
      }
      return *this;
    }

    /*!*****************************************************************************
     * @brief Dereference operator
     * 
     * @return T& 
    *******************************************************************************/
    T& operator*() const { return *p; }

    /*!*****************************************************************************
     * @brief Arrow operator
     * 
     * @return T* 
    *******************************************************************************/
    T* operator->() const { return p; }
  };

} // end namespace HLP3

#endif // #ifndef PTR_H
