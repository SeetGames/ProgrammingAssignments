/*!************************************************************************
 \file worker.cpp
 \author Seetoh Wei Tung
 \par Email: seetoh.w@digipen.edu
 \par Course: CSD2182
 \par Assignment #2.2
 \date 8-11-2023
 \brief Defines a Worker class responsible for handling tasks in a 
 threaded environment; it can set a task, run the task's computation, 
 and signal its completion, while also allowing for the worker to be 
 stopped and managing its ready state with synchronization primitives 
 like mutexes and condition variables.
**************************************************************************/
#include "worker.h"
#include "dispatcher.h"
#include <chrono>
using namespace std;

/**
 * @brief It sets its running state to true and ready state to false for this worker. 
 * It also prepares a unique_lock ulock that will be used in the function of wait_for 
 * of condition variable cv.
 * 
 */
Worker::Worker() : ulock(mtx, defer_lock), req(nullptr), running(true), ready(false) {}

/**
 * @brief sets the associated request to req and turns the ready state to true (so that 
 * it allows the thread function run() to be woken up to serve the request).
 * 
 * @param request Pointer to the request object.
 */
void Worker::setRequest(Request *request)
{
    lock_guard<mutex> lock(mtx);
    req = request;
    ready = true;
    cv.notify_one();
}

/**
 * @brief It sets its running state to false (e.g. when the main thread terminates the
 * worker thread).
 * 
 */
void Worker::stop()
{
    lock_guard<mutex> lock(mtx);
    running = false;
    cv.notify_one();
}

/**
 * @brief Returns the condition variable cv.
 * 
 * @param out_cv Pointer to the condition variable to be set.
 */
void Worker::getCondition(condition_variable *&out_cv)
{
    out_cv = &cv;
}

/**
 * @brief checks whether the worker is allowed to run. If so, it will repeat the following. 
 * It checks whether it is ready to serve the request. If so, it turns its ready state to false, 
 * calls process() to complete the computation for the request and then finish() to merge the 
 * partial result into the final results. Otherwise, it should add itself to the queue with 
 * addWorker() and execute next request or wait. If it has to wait, it uses a loop to deal with 
 * spurious wake-ups. It checks whether its ready state is false (not serving any request now) 
 * and the thread is allowed to run. If so, it uses condition variable cv with ulock to wait for 
 * timeout (e.g. 1 sec). If it is timed out, the function should keep waiting unless the worker 
 * is stopped by the dispatcher.
 * 
 */
void Worker::run()
{
    while (running)
    { // Continue running while the worker is not stopped
        unique_lock<mutex> lock(mtx);
        // Wait for a request to become ready or for the worker to be stopped, handling spurious wake-ups
        cv.wait(lock, [this]()
                { return ready || !running; });

        if (ready && req)
        {
            // Got a request, process it
            lock.unlock(); // Unlock before long-running operations
            req->process();
            req->finish();
            lock.lock(); // Re-lock to modify the condition
            ready = false;
            req = nullptr;

            // After finishing the request, the worker should add itself back to the available queue
            lock.unlock();
            Dispatcher::addWorker(this);
        }
        else if (!ready)
        {
            // If not ready, add self back to the worker queue or wait for the next request
            lock.unlock(); // Unlock before calling addWorker
            bool addedToQueue = Dispatcher::addWorker(this);
            if (!addedToQueue)
            {
                // No request available, wait for some time or until a new request arrives
                unique_lock<mutex> waitLock(mtx);
                cv.wait_for(waitLock, chrono::seconds(1), [this]()
                            { return ready || !running; });
            }
        }
    }
}
