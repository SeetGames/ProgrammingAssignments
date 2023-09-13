/*
Read the following to figure out what functions allocate and deallocate must do:

Given three constant integer values F, R, and C, the definition
  int a[F][R][C]; 
specifies a static "array of arrays of arrays" [or, a three-dimensional array] where,
F specifies the number of frames,
R specifies the number of rows in each frame, and
C specifies the number of columns within each row.

A specific element in the 3D array a is accessed through expression: a[i][j][k] where
i chooses a specific frame [out of a total of F]
j chooses a specific row within the frame [out of a total of R rows per frame], and
k chooses a specific element within the row [out of a total of C columns per row].

You must define functions allocate and deallocate with the following specifications.

Function allocate takes three int parameters for simulating a 3D array:
the number of frames (F), number of rows within each frame (R), and
the number of columns (C) within each row. You must do the following:
a) You must allocate a block of heap memory for storing all the int elements in the
simulated 3D array.
b) Next, you must allocate a block of heap memory that will store pointers to each row
in the 3D array [simulated by the previously allocated block of heap memory]. It is
your responsibility to assign to each element of this array [of pointers] the address
of each row in the heap memory allocated in a).
c) Finally, you must allocate a block of heap memory that will store pointers to each
of the F frames. It is your responsibility to assign to each element of this array
[of pointers] the address of each frame in the heap memory allocated in b).
d) Function allocate will return the address of the starting byte of the block of
heap memory allocated in c).

Function deallocate will take the address returned by function allocate and return
to the free store the blocks of memory dynamically allocated in function allocate

Your code must be implemented using C++ memory allocation/deallocation operators.
Obviously, your submission should not have memory leaks or other memory related bugs.
*/

#include "tddaa.h"

int*** allocate(int F, int R, int C) {
  // complete definition of function allocate ...
  return nullptr;
}

void deallocate( int *** ppp ) {
  // complete definition of function deallocate ...
}
