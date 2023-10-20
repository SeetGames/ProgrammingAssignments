#include "polynomial.h"
#include <iostream>
#include <string>
#include <random>

namespace {
void test0();
void test1();
void test2();
void test3();
void test4();
void test5();
void test6();
}

int main(int argc, char *argv[] ) {
  void (*pTests[])() = {test0, test1, test2, test3, test4, test5, test6};

  if (argc > 1) {
    int test = std::stoi(std::string(argv[1]));
    if (test < 7) {
      pTests[test]();
    } else {
      for (size_t i{}; i < sizeof(pTests)/sizeof(pTests[0]); ++i) {
        pTests[i]();
      }
    }
  }
}

namespace {
// test0: default ctor, overloads of op[], and non-member op<<
void test0() {   
  std::cout << "-------- " << __func__ << " --------\n";
  
  HLP3::Polynomial<int, 3> p3;
  std::cout << p3 << std::endl; // all zeroes

  p3[0] = 1;
  p3[1] = 1;
  p3[2] = 1;
  p3[3] = 1; 
  std::cout << p3 << std::endl; // all ones

  HLP3::Polynomial<float, 1> p1;
  p1[0] = 2.2;
  p1[1] = 10.1; 
  std::cout << p1 << std::endl;
}

// test1: default and single argument conversion ctors, op[] overloads,
// op* overloads, and non-member op<< 
void test1() {
  std::cout << "-------- " << __func__ << " --------\n";
  
  HLP3::Polynomial<int,3> p3;
  p3[0] = 1;
  p3[1] = 1;
  p3[2] = 1;
  p3[3] = 1; // defines 1+x+x^2+x^3

  HLP3::Polynomial<int,1> p1;
  
  p1[0] = 1;
  p1[1] = -1; // defines 1-x
  std::cout << p1 << std::endl;

  HLP3::Polynomial<int,4> p4 = p3*p1;
  std::cout << p4 << std::endl; // produces 1+ -1 * x^4
                                // (1+x+x^2+x^3)*(1-x) = 1-x^4

  HLP3::Polynomial<float,2> pf_1;
  pf_1[0] = 1.1;
  pf_1[1] = -2.2;
  pf_1[2] = 1.1;
  std::cout << pf_1 << std::endl;

  // expression pf_1 * p3 should NOT compile because multiplication of two
  // different types of coefficients should NOT compile!!!
  // see driver-no-comp-1.cpp
  // run: make gcc1_NC 
}

// test2: default and single-argument conversion ctors, op[] overloads,
// and non-member op<<
void test2() {   // conversion
  std::cout << "-------- " << __func__ << " --------\n";
  
  HLP3::Polynomial<float,2> pf_1;
  pf_1[0] = 1.1;
  pf_1[1] = -2.2;
  pf_1[2] = 1.1;
  std::cout << pf_1 << std::endl;

  HLP3::Polynomial<int,2> p2( pf_1 ); // convert
  std::cout << p2 << std::endl;

  // Following definition should NOT compile: HLP3::Polynomial<int,3> p2(pf_1);
  // since conversion to a different degree should NOT compile!!!
  // run: make gcc2_NC
  // see driver-no-comp-2.cpp
} 

// test3: default and implicit copy ctors, op[] overloads,
// implicit copy assignment op= overload, and non-member op<<
void test3() {   // copy 
  std::cout << "-------- " << __func__ << " --------\n";
  
  HLP3::Polynomial<int,2> p2;
  p2[0] = 3;
  p2[1] = 2;
  p2[2] = 1;
  std::cout << p2 << std::endl;
  
  // The compiler will implicitly generate a copy ctor to copy construct an
  // object of type HLP3::Polynomial<T,N> from another object of type HLP3::Polynomial<T,N>.
  // This is ok since the class is not dynamically allocating memory on the free store.
  // The templated conversion ctor will NOT be used since the implicitly generated
  // member function is a better match.
  HLP3::Polynomial<int,2> p2_copy(p2);
  std::cout << p2_copy << std::endl;

  p2[0] = 5;
  p2[1] = 5;
  p2[2] = 5;
  // using compiler generated assignment
  p2_copy = p2;
  std::cout << p2_copy << std::endl;
}

// test4: default ctor, op[] overloads, op= overload, and non-member op<<
void test4() {   // templated assignment
  std::cout << "-------- " << __func__ << " --------\n";
  
  HLP3::Polynomial<float,2> pf_2;
  pf_2[0] = 1.1;
  pf_2[1] = -2.2;
  pf_2[2] = 1.1;
  std::cout << pf_2 << std::endl;

  HLP3::Polynomial<int,2> p2;
  p2[0] = 3;
  p2[1] = 2;
  p2[2] = 1;
  std::cout << p2 << std::endl;

  p2 = pf_2;
  std::cout << p2 << std::endl;
}

// test5: default and single argument conversion ctor, op[] overloads, op* overload,
// op() to evaluate the polynomial, and non-member op<<
void test5() {   // evaluate
  std::cout << "-------- " << __func__ << " --------\n";
  
  HLP3::Polynomial<int,2> p2;
  p2[0] = 1;
  p2[1] = -2;
  p2[2] = 1;
  std::cout << p2 << std::endl;

  HLP3::Polynomial<int,5> p5;
  p5[0] = 3;
  p5[1] = -2;
  p5[2] = 1;
  p5[3] = 1;
  p5[4] = -1;
  p5[5] = 1;
  std::cout << p5 << std::endl;
  
  HLP3::Polynomial<int,7> p7_1 = p2*p5;
  HLP3::Polynomial<int,7> p7_2 = p5*p2; // should be commutative

  int values[] = { 1,2,3,4,5 };
  for ( int i=0; i<5; ++i ) {
    int r1 = p2(values[i]);
    int r2 = p5(values[i]);
    int r3 = p7_1(values[i]);
    int r4 = p7_2(values[i]);
    std::cout << r3 << " " << r4 << std::endl;
    if ( r1*r2 != r3 or r1*r2 != r4 ) {
      std::cout << "Error\n";
    }
  }
}

// evaluate randomly generated polynomials using C++11 <random> library ...
// 
void test6() {   // evaluate randomly generated polynomials
  std::cout << "-------- " << __func__ << " --------\n";

  std::random_device          rd;
  std::mt19937                gen( rd() );
  std::uniform_int_distribution<>  rand_coeff( -10, 10 );
  
  HLP3::Polynomial<int, 5> p5;
  for ( int i=0; i<=5; ++i ) { 
    p5[i] = rand_coeff( gen ); 
  }

  HLP3::Polynomial<int, 4> p4;
  for (int i=0; i<=4; ++i) { 
    p4[i] = rand_coeff(gen); 
  }
  
  HLP3::Polynomial<int, 9> p9_1 = p4*p5;
  HLP3::Polynomial<int, 9> p9_2 = p5*p4; // should be commutative

  int values[] = {1, 2, 3, 4, 5};
  for (int i=0; i<5; ++i) {
    int r1 = p4(values[i]);
    int r2 = p5(values[i]);
    int r3 = p9_1(values[i]);
    int r4 = p9_2(values[i]);
    if (r1*r2 != r3 or r1*r2 != r4) {
      std::cout << "Error\n";
    } else {
      std::cout << "OK\n";
    }
  }
}

}
