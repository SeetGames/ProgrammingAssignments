#pragma once
#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <mutex>
using namespace std;

typedef void (*logFptr)(string text);

class Request  
{
	int numARows; //# of rows for 1st input matrix 
	int numAColumns;//# of columns for 1st input matrix
	int numBColumns;//# of rows for 2nd input matrix
	int rowABlkIdx;//Block row index for the block from 1st input matrix
	int colABlkIdx;//Block column index  for the block from 1st input matrix
	int colBBlkIdx;//Block row index for the block from 2nd input matrix
	int blockSz;//block size for block matrix

	float *a; // 1st input for block matrix mulitiplication
	float *b; // 2nd input for block matrix mulitiplication
	float *c; // partial result for block matrix mulitiplication
	float *out; //final result for block matrix mulitiplication

	logFptr outFnc;	//print function ptr.
	bool initialized;//whether the request has been initialized

	public:
	Request(int m, int n, int l, 
			float *C, int blkSz,
			int rowABlockIdx, int colABlockIdx, 
			int colBBlockIdx); //constructor
	void init(float *A, float *B); //allocate memory/copy data
	void setOutput(logFptr fnc) { outFnc = fnc; }
	void process();
	void finish();
};

#endif