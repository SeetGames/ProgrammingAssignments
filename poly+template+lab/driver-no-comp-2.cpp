#include "polynomial.h"
#include <iostream>

// the purpose of this driver is to test [through unsuccessful compilation] that
// polynomial of degree N cannot be constructed from a polynomial of degree M!!!
int main() {   // conversion
  HLP3::Polynomial<float, 2> pf_1;
  pf_1[0] = 1.1;
  pf_1[1] = -2.2;
  pf_1[2] = 1.1;
  std::cout << pf_1 << std::endl;

  // conversion to a different degree should NOT compile
  HLP3::Polynomial<float, 3> p2( pf_1 );
}
