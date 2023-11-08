#include "dispatcher.h"
#include "request.h"

#include <iostream>
#include <string>
#include <csignal>
#include <thread>
#include <chrono>
#include <cstring>

#include "mm.h"
constexpr float epsilon = 0.0001;
using namespace std;

// Globals
mutex logMutex;
	
void logFnc(string text) 
{
	logMutex.lock();
	cout << text << "\n";
	logMutex.unlock();
}

int main() 
{
	int numThreads;
	int numARows;
	int numACols;
	int numBCols;
	int blockSize;

	std::cin >> numThreads; 
	std::cin >> numARows; 
	std::cin >> numACols; 
	std::cin >> numBCols; 
	std::cin >> blockSize;

	// Generate job requests.
	int nARowBlocks = (numARows - 1) / blockSize + 1;
	int nAColBlocks = (numACols - 1) / blockSize + 1;
	int nBColBlocks = (numBCols - 1) / blockSize + 1;
	
	// Initialise the dispatcher with some worker threads.
	Dispatcher::init(numThreads, nARowBlocks*nAColBlocks*nBColBlocks);	
	cout << "Initialised.\n";
	
	const char *in_file0 = "input0.raw";
	const char *in_file1 = "input1.raw";
	const char *out_file = "output.raw";
	
	createDataset(in_file0, in_file1, out_file, numARows, numACols, numBCols);

	float *input0_data = readData(in_file0, &numARows,  &numACols);
	float *input1_data = readData(in_file1, &numACols, &numBCols);
	float *result = new float[numARows * numBCols];	
	std::memset(result, 0, numARows * numBCols *sizeof(float));
	
	Request* rq = 0;
	for (int i = 0; i < nARowBlocks; i++) 
	{
		for (int j = 0; j < nAColBlocks; j++) 
		{
			for (int k = 0; k < nBColBlocks; k++) 
			{		
				rq = new Request(numARows, numACols, numBCols,
						result, blockSize, i, j, k);
								
				//allocate memory and copy from input 						
				rq->init(input0_data, input1_data); 

				rq->setOutput(&logFnc);
				Dispatcher::addRequest(rq);
			}
		}
	}
	
	// Cleanup.
	Dispatcher::stop();
	cout << "Clean-up done.\n";

	cout << "Checking...\n";
	
	float *ref = readData(out_file, & numARows, & numBCols);
	for (int i = 0; i < numARows; i++)
	{
		for (int j = 0; j < numBCols; j++)
		{
			// cout<<"Result: "<<result[i*numBCols+j]<<endl; 
			// cout<<"Ref: "<<ref[i*numBCols+j]<<endl;
			if (abs(result[i*numBCols+j] - ref[i*numBCols+j]) 
				> epsilon)
			{
				i = numARows;
				cout << "Error in multi-threading MM ...\n";
				break;
			}
		}
	}		
	return 0;
}