// driver.cpp
// -- simple test of splitter/joiner of binary files
// hlp3 06/19/2020

#include <iostream>		// std::cout
#include "splitter.h"	// splitter-joiner interface

int main(int argc, char *argv[]) {
  SplitResult rs = split_join(argc, argv);
  switch(rs) {
    case SplitResult::E_BAD_SOURCE:
      std::cout << "Unable to open or read from input file\n";
      break;
    case SplitResult::E_BAD_DESTINATION:
      std::cout << "Unable to open or write to output file\n";
      break;
    case SplitResult::E_NO_MEMORY:
      std::cout << "Unable to allocate heap memory\n";
      break;
    case SplitResult::E_SMALL_SIZE:
      std::cout << "Negative or zero buffer size\n";
      break;
    case SplitResult::E_NO_ACTION:
      std::cout << "No action specified by user\n";
      break;
    case SplitResult::E_SPLIT_SUCCESS:
      std::cout << "Split successfully completed\n";
      break;
    case SplitResult::E_JOIN_SUCCESS:
      std::cout << "Join successfully completed\n";
      break;
  }
}
