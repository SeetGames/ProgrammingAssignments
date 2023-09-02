/*!*****************************************************************************
 \file splitter.h
 \author Seetoh Wei Tung
 \par DP email: seetoh.w@digipen.edu
 \par Course:  Modern C++ Design Patterns
 \par Section: B
 \par Assignment 1
 \date 30-08-2023
 \brief
    Header file defining the interface for splitting and joining binary files.
    It provides the necessary data structures, enumerations, and function 
    prototypes to be used in conjunction with the splitter.c and splitter.cpp 
    implementations. Designed for compatibility with both C and C++ drivers.
*******************************************************************************/
#ifndef SPLITTER_H
#define SPLITTER_H
// this header file must be amended to make both splitter.cpp AND splitter.c be compatible with
// both driver.c AND driver.cpp
// in other words, there will be a single splitter.h that will be used with driver.c, driver.cpp, splitter.c, and splitter.cpp
#include <stddef.h> // size_t definition
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum {
  E_BAD_SOURCE = 1,
  E_BAD_DESTINATION,
  E_NO_MEMORY,
  E_SMALL_SIZE,
  E_NO_ACTION,
  E_SPLIT_SUCCESS,
  E_JOIN_SUCCESS
} SplitResult;

/*!*****************************************************************************
 * @brief Depending on command line arguments passed through, this function will
 * either:
 * - combine the chunks specified by the user
 * - split the file into chunks specified by the user
 * 
 * @param argc Argument Count
 * @param argv Argument Vector
 * @return SplitResult Result of the split or join process enumeration
*******************************************************************************/
#ifdef __cplusplus
extern "C" 
#endif
SplitResult split_join(int argc, char *argv[]);


#endif // end of #ifndef SPLITTER_H
