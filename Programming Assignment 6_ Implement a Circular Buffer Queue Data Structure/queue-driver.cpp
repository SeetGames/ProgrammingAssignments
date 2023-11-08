#include "queue.hpp"
#include <iostream>
#include <iomanip>
#include <cctype>

namespace
{
  std::ostream &operator<<(std::ostream &os, HLP3::Queue const &rhs);
  void test0();
  void test1();
  void test2();
  void test3();
}

int main(int argc, char **argv)
{
  constexpr int max_tests{4};
  void (*pTests[max_tests])() = {test0, test1, test2, test3};

  if (argc > 1)
  {
    int test = std::stoi(std::string(argv[1]));
    test = test > 0 ? test : -test;
    if (test < max_tests)
    {
      pTests[test]();
    }
    else
    {
      for (int i{}; i < max_tests; ++i)
      {
        pTests[i]();
      }
    }
  }
}

namespace
{
  std::ostream &operator<<(std::ostream &os, HLP3::Queue const &rhs)
  {
    os << "head: " << std::setw(2) << rhs.front_index() << " | size: " << std::setw(2) << rhs.size()
       << " | tail: " << std::setw(2) << rhs.back_index()
       << " | cap : " << std::setw(2) << rhs.capacity() << " | {" << (rhs.size() ? "" : "}");
    HLP3::Queue::const_pointer buff = rhs.c_buff();
    for (HLP3::Queue::size_type i{}; i < rhs.size(); ++i)
    {
      os << buff[(rhs.front_index() + i) % rhs.capacity()] << (i == rhs.size() - 1 ? '}' : ' ');
    }
    return os;
  }

  void test0()
  {
    std::cout << "------------------ test0 ------------------\n";
    std::cout << "sizeof(Queue): " << sizeof(HLP3::Queue) << "\n";
    HLP3::Queue a(4);
    std::cout << "a: " << a << "\n";
    a.push('1');
    std::cout << "a: " << a << "\n";
    a.push('2');
    std::cout << "a: " << a << "\n";
    a.pop();
    std::cout << "a: " << a << "\n";
    a.push('3');
    std::cout << "a: " << a << "\n";
    a.push('4');
    std::cout << "a: " << a << "\n";
    a.push('5');
    std::cout << "a: " << a << "\n";
    a.push('6');
    std::cout << "a: " << a << "\n";
    a.pop();
    std::cout << "a: " << a << "\n";
  }
  // testing ctor, dtor, push, pop, empty, full, size, swap member functions of Queue
  void test1()
  {
    std::cout << "------------------ test1 ------------------\n";
    HLP3::Queue a(5);
    std::cout << "a: " << a << "\n";
    a.push('a');
    a.push('b');
    a.push('c');
    std::cout << "a: " << a << "\n";
    a.push('d');
    a.push('e');
    std::cout << "a: " << a << "\n";
    a.push('f');
    std::cout << "a: " << a << "\n";
    a.pop();
    std::cout << "a: " << a << "\n";
    a.push('z');
    std::cout << "a: " << a << "\n";
    a.pop();
    a.pop();
    std::cout << "a: " << a << "\n";
    a.pop();
    while (!a.empty())
    {
      std::cout << "a: " << a << "\n";
      a.pop();
    }
    for (HLP3::Queue::value_type ch{'!'};; ++ch)
    {
      if (!a.full())
      {
        a.push(ch);
        continue;
      }
      break;
    }
    std::cout << "a: " << a << "\n";

    HLP3::Queue b(6);
    for (HLP3::Queue::value_type ch{'a'}; ch <= 'f'; ++ch)
      b.push(ch);
    std::cout << "b: " << b << "\n";
    b.swap(a);
    std::cout << "a: " << a << "\n";
    std::cout << "b: " << b << "\n";

    // test non-member swap function
    HLP3::swap(a, b);
    std::cout << "a: " << a << "\n";
    std::cout << "b: " << b << "\n";
  }

  // testing copy ctor, dtor, swap, push, pop, front, back member functions of class Queue
  void test2()
  {
    std::cout << "------------------ test2 ------------------\n";
    HLP3::Queue a(6); // conversion ctor
    for (HLP3::Queue::value_type ch{'z'}; ch >= 'u'; --ch)
    {
      a.push(ch);
      a.back() = std::toupper(a.back());
    }
    std::cout << "a: " << a << "\n";

    std::cout << "testing copy ctor:\n";
    HLP3::Queue b{a};
    std::cout << "b: " << b << "\n";
    b.pop();
    b.pop();
    std::cout << "b: " << b << "\n";

    std::cout << "testing copy assignment operator:\n";
    a = b;
    std::cout << "a: " << a << "\n";
    std::cout << "pushing three values to a:\n";
    a.push('a');
    a.push('b');
    a.push('c');
    std::cout << "a: " << a << "\n";

    std::cout << "testing member function swap:\n";
    a.swap(b);
    std::cout << "a: " << a << "\n";
    std::cout << "b: " << b << "\n";

    std::cout << "testing full and push member functions to fill a:\n";
    for (char ch{':'};; ++ch)
    {
      if (!a.full())
      {
        a.push(ch);
        continue;
      }
      break;
    }
    std::cout << "a: " << a << "\n";

    std::cout << "testing non-member function swap:\n";
    HLP3::swap(a, b);
    std::cout << "a: " << a << "\n";
    std::cout << "b: " << b << "\n";

    std::cout << "testing const member functions:\n";
    HLP3::Queue const d(a);
    std::cout << "d: " << d << "\n";

    std::cout << "testing empty, front, size, pop member functions: ";
    while (!a.empty())
    {
      std::cout << a.front() << (a.size() == 1 ? '\n' : ' ');
      a.pop();
    }
    std::cout << "a: " << a << "\n";

    std::cout << "testing push, front, back member functions: ";
    HLP3::Queue e(5);
    for (HLP3::Queue::value_type ch{'A'}; ch <= 'Z'; ++ch)
    {
      e.push(ch);
      e.back() = std::tolower(e.back());
      std::cout << "e: " << e << "\n";
      std::cout << "e's front value: " << e.front() << "\n";
      std::cout << "e's back value:  " << e.back() << "\n";
    }
    std::cout << "testing empty, pop, size member functions: ";
    while (!e.empty())
    {
      e.pop();
      if (e.size())
      {
        std::cout << "e: " << e << "\n";
      }
    }
  }

  // testing to make sure only N values are stored ...
  void test3()
  {
    std::cout << "------------------ test3 ------------------\n";
    std::cout << "pushing more than 20 elements to queue with capacity 20:\n";
    HLP3::Queue a{20};
    for (HLP3::Queue::value_type ch{'a'}; ch <= 'z'; ++ch)
    {
      a.push(ch);
      std::cout << "a: " << a << "\n";
    }
    std::cout << "a's front value: " << a.front() << "\n";
    std::cout << "a's back value:  " << a.back() << "\n";

    std::cout << "flushing a's buffer:\n";
    while (!a.empty())
    {
      a.pop();
      std::cout << "a: " << a << "\n";
    }

    std::cout << "pushing 10 elements:\n";
    for (HLP3::Queue::value_type ch{'0'}; ch <= '9'; ++ch)
    {
      a.push(ch);
      std::cout << "a: " << a << "\n";
    }
    std::cout << "a's front value: " << a.front() << "\n";
    std::cout << "a's back value:  " << a.back() << "\n";
  }

}

// compile with queue.cpp and run:
// g++ -Wall -Wextra -pedantic -std=c++17 -o queue queue-driver.cpp queue.cpp && ./queue-driver

// g++ -Wall -Wextra -pedantic -std=c++17 -o queue-driver queue-driver.cpp && ./queue-driver