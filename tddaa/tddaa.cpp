/*!*****************************************************************************
 \file tddaa.cpp
 \author Seetoh Wei Tung
 \par DP email: seetoh.w@digipen.edu
 \par Course: Modern C++ Design Patterns
 \par Section: B
 \par Assignment 3
 \date 14-09-2023
 \brief
    This file contains the implementation of functions allocate and deallocate.
*******************************************************************************/

#include "tddaa.h"

/**
 * @brief Allocates heap memory for a simulated 3D array and returns the address of the starting byte of the block of heap memory allocated.
 * 
 * @param _F The number of frames in the simulated 3D array.
 * @param _R The number of rows in each frame of the simulated 3D array.
 * @param _C The number of columns in each row of the simulated 3D array.
 * @return int*** The address of the starting byte of the block of heap memory allocated.
 */
int*** allocate(int _F, int _R, int _C) 
{
  // Steps to perform:
  // a) You must allocate a block of heap memory for storing all the int elements in the
  // simulated 3D array.
  int* elements = new int[_F * _R * _C];
  // b) Next, you must allocate a block of heap memory that will store pointers to each row
  // in the 3D array [simulated by the previously allocated block of heap memory].
  int** rows = new int*[_F * _R];
  // It is your responsibility to assign to each element of this array [of pointers] the 
  // address of each row in the heap memory allocated in a).
  for (int i{0}; i < _F*_R; i++)
  {
    int currentRow = i * _C;
    rows[i] = &elements[currentRow];
  }
  // c) Finally, you must allocate a block of heap memory that will store pointers to each
  // of the F frames.
  int*** frames = new int**[_F];
  // It is your responsibility to assign to each element of this array
  // [of pointers] the address of each frame in the heap memory allocated in b).
  for (int i{0}; i<_F; i++)
  {
    int currentFrame = i * _R;
    frames[i] = &rows[currentFrame];
  }
  // d) Function allocate will return the address of the starting byte of the block of
  // heap memory allocated in c).
  return frames;
}

/**
 * @brief Deallocates memory for a 3D integer array.
 * 
 * This function takes a pointer to a 3D integer array and deallocates the memory allocated for it.
 * It first frees the memory for the elements, then the memory for the rows, and finally the memory for the frames.
 * 
 * @param ppp Pointer to a 3D integer array.
 */
void deallocate(int*** ppp) 
{
  delete[] ppp[0][0]; // free memory for elements
  delete[] ppp[0]; // free memory for rows
  delete[] ppp; // free memory for frames
}
