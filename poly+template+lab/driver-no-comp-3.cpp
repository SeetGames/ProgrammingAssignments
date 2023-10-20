#include "polynomial.h"
#include <iostream>

// the purpose of this driver is to test [through unsuccessful compilation] that
// polynomial of degree N cannot be assigned a polynomial of degree M!!!
int main() {   // assignment to different degree
  HLP3::Polynomial<float, 2> p2;
  p2[0] = 5;
  p2[1] = 5;
  p2[2] = 5;

  HLP3::Polynomial<float, 3> p3;
  p3 = p2;
}
