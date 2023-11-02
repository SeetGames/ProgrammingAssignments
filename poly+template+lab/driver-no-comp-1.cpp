#include "polynomial.h"
#include <iostream>

// the purpose of this driver is to test [through unsuccessful compilation] that
// polynomials with different types [say, int and float] cannot be multiplied!!!
int main() {   
  HLP3::Polynomial<int, 3> p3;
  p3[0] = 1;
  p3[1] = 1;
  p3[2] = 1;
  p3[3] = 1; // defines 1+x+x^2+x^3

  HLP3::Polynomial<float, 2> pf_1;
  pf_1[0] = 1.1;
  pf_1[1] = -2.2;
  pf_1[2] = 1.1;
  std::cout << pf_1 << std::endl;

  // multiplcation of 2 different types of coefficients 
  // should NOT compile
  pf_1 * p3;
}
