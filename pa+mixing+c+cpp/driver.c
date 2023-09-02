// driver.c
// -- simple test of splitter/joiner of binary files
// hlp3 06/19/2020

#include <stdio.h>		// printf
#include "splitter.h"	// splitter-joiner interface

int main(int argc, char *argv[]) {
  SplitResult rs = split_join(argc, argv);
  switch(rs) {
    case E_BAD_SOURCE:
      printf("Unable to open or read from input file\n");
      break;
    case E_BAD_DESTINATION:
      printf("Unable to open or write to output file\n");
      break;
    case E_NO_MEMORY:
      printf("Unable to allocate heap memory\n");
      break;
    case E_SMALL_SIZE:
      printf("Negative or zero buffer size\n");
      break;
    case E_NO_ACTION:
      printf("No action specified by user\n");
      break;
    case E_SPLIT_SUCCESS:
      printf("Split successfully completed\n");
      break;
    case E_JOIN_SUCCESS:
      printf("Join successfully completed\n");
      break;
  }
}
