#pragma once
#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "worker.h"

#include <queue>
#include <mutex>
#include <thread>
#include <vector>

using namespace std;

class Dispatcher 
{
	static queue<Request*> reqs;//job requests
	static queue<Worker*> workers;
	static mutex requestsMutex;
	static mutex workersMutex;
	static vector<Worker*> allWorkers;//workers 
	static vector<thread*> threads;//threads
	static mutex jobCountMutex; //mutex for job counter
	static mutex outputMutex; //mutex for final results
	static int nJobs; //counter for total job requests 		
	
public:
	static bool init(int workers, int jobs);
	static bool stop();
	static void addRequest(Request* req);
	static bool addWorker(Worker* worker);

	static void lockOutput();
	static void unlockOutput();
	static void decreaseJobs();
};

#endif