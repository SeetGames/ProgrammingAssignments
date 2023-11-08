#pragma once
#ifndef WORKER_H
#define WORKER_H

#include "request.h"

#include <condition_variable>
#include <mutex>

using namespace std;

class Worker 
{
	condition_variable cv;
	mutex mtx;
	unique_lock<mutex> ulock;
	Request* req;
	bool running;
	bool ready;
	
public:
	Worker();
	void run();
	void stop();
	void setRequest(Request* req);
	void getCondition(condition_variable* &cv);
};

#endif