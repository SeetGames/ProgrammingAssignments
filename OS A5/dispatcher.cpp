/*!************************************************************************
 \file dispatcher.cpp
 \author Seetoh Wei Tung
 \par Email: seetoh.w@digipen.edu
 \par Course: CSD2182
 \par Assignment #2.2
 \date 8-11-2023
 \brief Defines a Dispatcher class that manages a pool of worker threads 
 and job requests for concurrent processing, ensuring thread safety using 
 mutexes and condition variables.
**************************************************************************/
#include "dispatcher.h"
#include "worker.h"
#include "request.h"
#include <condition_variable>
#include <memory>
using namespace std;

mutex Dispatcher::jobCountMutex;
int Dispatcher::nJobs = 0;
mutex Dispatcher::workersMutex;
queue<Worker *> Dispatcher::workers;
vector<thread *> Dispatcher::threads;
vector<Worker *> Dispatcher::allWorkers;
mutex Dispatcher::requestsMutex;
queue<Request *> Dispatcher::reqs;
mutex Dispatcher::outputMutex;


/**
 * @brief Adds the workers into allWorkers. It creates the threads 
 * based on std::thread (with total number of threads workers), each 
 * associated with a worker (i.e., running Worker::run() on each 
 * object Worker), and adds into threads. It also sets the expected 
 * number of jobs to nJobs.
 * 
 * @param workers The number of workers to be initialized.
 * @param jobs The number of jobs to be initialized.
 * @return True if initialization is successful, false otherwise.
 */
bool Dispatcher::init(int workers, int jobs)
{
    {
        unique_lock<mutex> lock(jobCountMutex);
        nJobs = jobs;
    }

    for (int i = 0; i < workers; ++i)
    {
        Worker *worker = new Worker();
        thread *workerThread = new thread(&Worker::run, worker);

        {
            unique_lock<mutex> lockWorkers(workersMutex);
            Dispatcher::workers.push(worker);
        }

        threads.push_back(workerThread);
        allWorkers.push_back(move(worker)); // Ownership transferred to allWorkers
    }

    return true;
}

/**
 * @brief Terminates the worker threads calling stop() for the workers 
 * and joins all the threads if the job requests are all serviced by 
 * checking nJobs.
 * 
 * @return true if the dispatcher was successfully stopped, false otherwise.
 */
bool Dispatcher::stop()
{
    {
        lock_guard<mutex> lock(jobCountMutex);
        if (nJobs > 0)
        {
            return false; // Not all jobs have been serviced
        }
    }

    for (auto &worker : allWorkers)
    {
        worker->stop();
    }

    for (auto &th : threads)
    {
        if (th->joinable())
        {
            th->join();
        }
    }

    threads.clear();
    allWorkers.clear();
    // Empty the workers queue safely
    {
        unique_lock<mutex> lockWorkers(workersMutex);
        queue<Worker *> empty;
        swap(workers, empty);
    }

    return true;
}

/**
 * @brief Checks whether there is a worker available in the workers 
 * queue. If so, it uses workersMutex to synchronize the access to 
 * workers queue, gets one worker from the queue, assigns the job 
 * request and notifies the worker using conditional variable cv. 
 * Otherwise it adds the request to the requests queue. It uses 
 * requestsMutex to synchronize the access to requests queue.
 *
 * @param req A pointer to the request to be added.
 */
void Dispatcher::addRequest(Request *req)
{
    unique_lock<mutex> lockRequests(requestsMutex);

    if (!workers.empty())
    {
        unique_lock<mutex> lockWorkers(workersMutex, defer_lock);
        lockRequests.unlock();
        lockWorkers.lock();

        Worker *worker = workers.front();
        workers.pop();
        lockWorkers.unlock();

        worker->setRequest(req);
    }
    else
    {
        reqs.push(req);
        lockRequests.unlock();
    }
}

/**
 * @brief If a request is waiting in the requests queue, assign it to 
 * the worker. It uses requestsMutex to synchronize the access to 
 * requests queue and returns false. Otherwise the function adds the 
 * worker to the workers queue. It returns true if the worker was 
 * added to the queue and has to wait for its condition variable. Use 
 * workersMutex to synchronize the access to workers queue.
 * 
 * @param worker A pointer to the worker to be added.
 * @return true if the worker was added successfully, false otherwise.
 */
bool Dispatcher::addWorker(Worker *worker)
{
    unique_lock<mutex> lockReq(requestsMutex);

    if (!reqs.empty())
    {
        Request *req = reqs.front();
        reqs.pop();
        lockReq.unlock();

        worker->setRequest(req);
        return false; // Worker is not available since it's now processing a request
    }
    else
    {
        unique_lock<mutex> lockWorkers(workersMutex);
        workers.push(worker);
        return true; // Worker is available and added back to the queue
    }
}

/**
 * @brief Locks the mutex of outputMutex, which is used for the access 
 * to the final results of matrix multiplication
 */
void Dispatcher::lockOutput()
{
    outputMutex.lock();
}

/**
 * @brief Unlocks the mutex of outputMutex, which is used for the 
 * access to the final results of matrix multiplication.
 * 
 */
void Dispatcher::unlockOutput()
{
    outputMutex.unlock();
}

/**
 * @brief This function decreases the job counter nJobs using 
 * jobCountMutex to synchronize the access to nJobs.
 * 
 */
void Dispatcher::decreaseJobs()
{
    lock_guard<mutex> lock(jobCountMutex);
    if (nJobs > 0)
    {
        nJobs--;
    }
}
