#pragma once
#ifndef MM_H
#define MM_H
//create input and output matrices 
//write into the input and output files
//the size of 1st matrix for multiplication is 
//numARows * numACols;
//the size of 2nd matrix for multiplication is 
//numACols * numBCols;
//compute the results and store at output fil
//the size of resulting matrix is numARows*numBCols
void createDataset(
	const char *input0_file_name,
	const char *input1_file_name,
	const char *output_file_name,
	int numARows,
	int numACols,
	int numBCols);

//read the input data from the file
//the size of the matrix is height*width
//return the pointer to the input data
float *readData(const char *file_name,
	int *height,
	int *width);

//write the output data to the file
//the size of the matrix is height*width
void writeData(const char *file_name,
	float *data,
	int height,
	int width);

//compute the matrix multiplication using 
//the given input matrices from the files
//input0 and input1. the result is saved
//into the file output. 
//the size of 1st matrix for multiplication is 
//numARows * numACols;
//the size of 2nd matrix for multiplication is 
//numACols * numBCols;
//the size of resulting matrix is numARows*numBCols
void compute(float *output, float *input0, float *input1,
	int numARows, int numAColumns, int numBColumns);

//create the random data for the matrix 
//return the pointer to the data of the matrix
float *createData(int height, int width);
#endif
