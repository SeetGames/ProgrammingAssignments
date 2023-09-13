#include "tddaa.h"
#include <iostream> 
#include <string>
#include <ctime>   // time
#include <cstdlib> // rand/srand

namespace {
void run(int, int, int);
void test0();
void test1();
}

int main (int argc, char ** argv) {
  void (*pTests[])() = {test0, test1};

  if (argc > 1) {
    int test = std::stoi(std::string(argv[1]));
    if (test < 2) {
      pTests[test]();
    } else {
      for (size_t i{}; i < sizeof(pTests)/sizeof(pTests[0]); ++i) {
        pTests[i]();
      }
    }
  }
}

namespace {
void run(int d1, int d2, int d3) {
  int ***ppp = allocate( d1,d2,d3 );

  for (int i{}; i<d1; ++i ) {
    for (int j{}; j<d2; ++j ) {
      for (int k{}; k<d3; ++k ) {
        ppp[i][j][k] = 100*i + 10*j + k; // 3-digit number ijk if i,j,k are single digits
      }
    }
  }

  bool ok = true;
  for (int i{}; i<d1; ++i ) {
    for (int j{}; j<d2; ++j ) {
      for (int k{}; k<d3; ++k ) {
        if ( ppp[i][j][k] != 100*i + 10*j + k ) {
          std::cout << "E";
          ok = false;
        }
      }
    }
  }
  if ( ok ) { std::cout << "OK\n"; }
  deallocate( ppp );
}

void test0() {
  run(2,3,4);
}

void test1() {
  srand(time(0));
  run(rand()%100 +1, rand()%100 +1, rand()%100 +1);
}
}
