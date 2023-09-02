/*!*************************************************************************
****
\file splitter.h
\author Phua Jian Kai
\par DP email: p.jiankai@digipen.edu
\par Course: Modern C++ Design Patterns
\par Section: A
\par Assignment 1
\date 09-08-2022
\brief
This program contains the declaration of the split and join function.
****************************************************************************
***/
#ifndef SPLITTER_H
#define SPLITTER_H

// this header file must be amended to make both splitter.cpp AND splitter.c be compatible with
// both driver.c AND driver.cpp
// in other words, there will be a single splitter.h that will be used with driver.c, driver.cpp, splitter.c, and splitter.cpp

#include <stddef.h> // size_t definition
#include <stdio.h>

typedef enum {
  E_BAD_SOURCE = 1,
  E_BAD_DESTINATION,
  E_NO_MEMORY,
  E_SMALL_SIZE,
  E_NO_ACTION,
  E_SPLIT_SUCCESS,
  E_JOIN_SUCCESS
} SplitResult;

// provide function header documentation using Doxygen tags to explain to
// your client how they must use this function to either split big file into
// smaller files or join [previously split] smaller files into big file ...

/*!*************************************************************************
****
\brief
Joins and splits files based on the command line arguments that are passed
through.
\param argc
Number of arguments
\param argv
Array of arguments
\return
Result of the split or join process
****************************************************************************
***/
#ifdef __cplusplus
extern "C" 
#endif
SplitResult split_join(int argc, char *argv[]);

#endif // end of #ifndef SPLITTER_H
