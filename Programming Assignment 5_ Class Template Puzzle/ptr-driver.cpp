// ptr-driver.cpp: test driver for class template Ptr [that encapsulates
// a pointer that points to a dynamically allocated object of type T on
// the free store] ...

#include "ptr.h"

// dummy types for testing type HLP3::Ptr<T>
struct A {
  int a=10;
  A(int x=11) : a{x} {}
};

struct B {
  // implicit conversion operator ...
  operator A () const { return A(); }
};

#include <iostream>

int main() {
using namespace HLP3;

  Ptr<int> ip1{new int {11}}, ip2{new int {22}};
  Ptr<int> ip3{ip2}; // ip3 encapsulates a pointer that points to an int
                     // object dynamically allocated on free store and that
                     // is initialized with int value stored in dynamically
                     // allocated int object that ip2.p points to ...

  // prints to standard output: 11|22|22
  std::cout << *ip1.get() << '|' << *ip2.get() << '|' << *ip3.get();
  std::cout << '\n';

  *ip2 = *ip3.get() * 2; // modify value in int object pointed to by ip2.p
  ip1 = ip2; // overwrite value in int object pointed to by ip1.p with
             // value in int object pointed to by ip2.p
  
  // prints to standard output: 44|44|22
  std::cout << *ip1 << '|' << *ip2 << '|' << *ip3;
  std::cout << '\n';

  Ptr<float> fp1{new float {1.23f}}, fp2{new float {12.3f}};
  Ptr<double> dp1(fp1); // dp1 encapsulates a pointer to points to a
                        // double object dynamically allocated on free
                        // store and that is initialized with float value
                        // [cast to double] in dynamically allocated float
                        // object that fp1.p points to ...
  Ptr<double> dp2(ip2); // dp2 encapsulates a pointer to points to a
                        // double object dynamically allocated on free
                        // store and that is initialized with int value
                        // [cast to double] in dynamically allocated int
                        // object that ip2.p points to ...

  // prints to standard output: 1.23|12.3|1.23|44
  std::cout << *fp1 << '|' << *fp2 << '|' << *dp1.get() << '|' << *dp2.get();
  std::cout << '\n';

  *ip1 /= 6;
  fp1 = ip1; // assign int value [cast to float] pointed to by ip1.p
             // to float object pointed to by fp1.p ...

  // prints to standard output: 7|12.3|1.23|44
  std::cout << *fp1 << '|' << *fp2 << '|' << *dp1.get() << '|' << *dp2.get();
  std::cout << '\n';

  Ptr<B> pb{new B};
  Ptr<A> pa{pb};
  pa->a += (*pa).a;

  // prints to standard output: 22|22
  std::cout << (*pa).a << '|' << pa->a;
  std::cout << "\n";
}
