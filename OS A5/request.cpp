/*!************************************************************************
 \file request.cpp
 \author Seetoh Wei Tung
 \par Email: seetoh.w@digipen.edu
 \par Course: CSD2182
 \par Assignment #2.2
 \date 8-11-2023
 \brief Contains the Request class, which encapsulates the data and 
 methods needed for performing block matrix multiplication, including 
 initialization, processing of blocks, and merging the results into a 
 final matrix, while ensuring thread-safe operations with the help of a 
 dispatcher.
**************************************************************************/
#include "request.h"
#include "dispatcher.h"
#include "mm.h"
using namespace std;

static int check = false;

/**
 * @brief Sets the matrix dimension for the input matrices to compute the matrix multiplication. 
 * The first matrix size is m × n and the second n × l. C is the pointer to the final results. 
 * rowABlockIdx and colABlockIdx are respectively the row and column index for the block from 
 * the first input matrix. colBBlockIdx is the column index for the block from the second 
 * input matrix. Please note that colABlockIdx is the row index for the block from the second 
 * input matrix. blkSz is the number of the rows(columns) of the block matrix.
 *
 * @param m number of rows in the first input matrix for the matrix multiplication.
 * @param n number of columns in the first input matrix and the number of rows in the second input matrix.
 * @param l number of columns in the second input matrix for the matrix multiplication.
 * @param C pointer to the memory space where the result of the matrix multiplication will be stored.
 * @param blkSz size of the block matrix, which is the number of rows (or columns) of each subdivided block for computation.
 * @param rowABlockIdx row index of the current block from the first input matrix being processed.
 * @param colABlockIdx column index for the block from the first input matrix and also represents the row index for the block from the second input matrix.
 * @param colBBlockIdx column index for the block from the second input matrix being processed.
 */
Request::Request(int m, int n, int l,
                 float *C, int blkSz,
                 int rowABlockIdx, int colABlockIdx,
                 int colBBlockIdx)
    : numARows(m), numAColumns(n), numBColumns(l),
      rowABlkIdx(rowABlockIdx), colABlkIdx(colABlockIdx),
      colBBlkIdx(colBBlockIdx), blockSz(blkSz),
      out(C), initialized(false) {}

/**
 * @brief Allocate memory for the block matrices a, b and c. 
 * It copies the data from the input matrices A and B to the allocated memory of block matrices.
 * 
 * @param A Pointer to the array of float values for A.
 * @param B Pointer to the array of float values for B.
 */
void Request::init(float *A, float *B)
{
    if (!check)
    {
        // Allocating memory for the local copies of matrices A, B, and the resultant matrix C
        a = new float[numARows * numAColumns];
        b = new float[numAColumns * numBColumns];
        c = new float[numARows * numBColumns](); // Resulting block C with actual sizes

        // Copy the relevant block from A, considering the actual number of rows
        for (int i = 0; i < numARows; ++i)
        {
            for (int j = 0; j < numAColumns; ++j)
            {
                a[i * numAColumns + j] = A[i * numAColumns + j];
            }
        }

        // Copy the relevant block from B, considering the actual number of columns
        for (int i = 0; i < numAColumns; ++i)
        {
            for (int j = 0; j < numBColumns; ++j)
            {
                b[i * numBColumns + j] = B[i * numBColumns + j];
            }
        }

        check = true; // Marking the request as initialized
        process();    // Processing the matrix multiplication
        finish();     // Merging the result and cleaning up
    }
}

/**
 * @brief computes the block matrix multiplication using the provided compute() from mm.h.
 */
void Request::process()
{
    // c = a * b, where c is the result, a and b are blocks of matrices A and B
    compute(c, a, b, numARows, numAColumns, numBColumns);
}

/**
 * @brief function merges the partial results at c from block matrix multiplication into 
 * the final result by using outputMutex to synchronize the access to out. It decrements 
 * the job counter nJobs calling Dispatcher::decreaseJobs().
 */
void Request::finish()
{
    Dispatcher::lockOutput(); // Locking output for thread safety

    // Merge partial result c into the output matrix out, considering actual block sizes
    for (int i = 0; i < numARows; ++i)
    {
        for (int j = 0; j < numBColumns; ++j)
        {
            out[i * numBColumns + j] = c[i * numBColumns + j];
        }
    }

    Dispatcher::unlockOutput(); // Unlocking output
    Dispatcher::decreaseJobs(); // Notify the dispatcher that a job has been completed
    delete[] a; delete[] b; delete[] c; // Deallocating memory
    a = b = c = nullptr;                // Resetting pointers to nullz
    initialized = false;                // Resetting initialization flag
}