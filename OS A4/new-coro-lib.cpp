/*!************************************************************************
 \file new-coro-lib.cpp
 \author Seetoh Wei Tung
 \par Email: seetoh.w@digipen.edu
 \par Course: CSD2182
 \par Assignment #2.1
 \date 30-10-2023
 \brief A cooperatively scheduled user-level thread library under Linux.
**************************************************************************/

#include "new-coro-lib.h"
#include <algorithm>
#include <climits>
#include <ucontext.h>
#include <queue>
#include <deque>
#include <list>
#include <map>


#define ALLOC (1024*1024)
namespace CORO
{
    enum ThreadState : int
    {
        NEW = 0,
        WAITING,
        READY,
        RUNNING,
        TERMINATED
    };

    struct TCB
    {
        ThreadID threadID;
        ThreadID parentID;
        ThreadState state;
        ucontext_t savedContext;
        void* (*function)(void*) = nullptr;
        void* params = nullptr;
        void* returnVal = nullptr;

        TCB()
        {
            threadID = 0;
            parentID = 0;
            state = ThreadState::RUNNING;

        }
        TCB(ThreadID tid, ThreadID pid, ThreadState st, void* (*func)(void*), void* param)
            : threadID(tid),
            parentID(pid),
            state(st),
            function(func),
            params(param),
            returnVal(nullptr) {
        }

        void GetContext()
        {
            getcontext(&savedContext);
        }
        void SetContext()
        {
            setcontext(&savedContext);
        }
    };

    std::deque<TCB*> readyQueue;
    std::map<ThreadID, TCB*> waitingQueue;
    TCB* runningQueue;
    std::queue<TCB*> newQueue;

    std::map<ThreadID, void*> pushedVals;
    std::list<TCB*> pullers;

    std::list<TCB> allThreads;

    /*!*****************************************************************************
    * @brief Creates a thread control block (TCB) for a new thread.
    *
    * This function initializes a new TCB with a unique thread ID and the provided 
    * function and parameters. It sets the thread's initial state to NEW and captures 
    * the current context to be used when the thread starts executing.
    *
    * @param func A pointer to the function that the thread will execute.
    * @param param A pointer to the parameter that the thread's function will receive.
    * @return TCB The initialized thread control block for the new thread.
    *******************************************************************************/
    TCB create_thread_context(void* (*func)(void*), void* param)
    {
        TCB newThread(allThreads.size(), 0, NEW, func, param);

        getcontext(&newThread.savedContext);
        return newThread;
    }

    /*!*****************************************************************************
    * @brief The starting point for thread execution.
    *
    * This function is called when a new thread starts running. It executes the 
    * function associated with the current thread, captures the return value, and 
    * then calls thread_exit to cleanly exit the thread.
    *
    * This function should not be called directly; it is used internally by the 
    * threading library to handle the execution flow of threads.
    *******************************************************************************/
    void context_function()
    {
        TCB* currentThread = runningQueue;
        currentThread->returnVal = currentThread->function(currentThread->params);
        thread_exit(currentThread->returnVal);
    }

    /*!*****************************************************************************
     * @brief Initializes the thread library, setting up all necessary data 
     structures and states required for managing user-level threads. Must be called 
     before any other functions in the library are used.
     * 
    *******************************************************************************/
    void thd_init()
    {
        TCB primaryThread;
        primaryThread.threadID = 0;                       // I assume threadid should be id as per the TCB structure
        primaryThread.GetContext();                 // Refactored myget() to GetContext() to match earlier code
        primaryThread.state = ThreadState::RUNNING; // threadstate should be state

        // Initialize stack pointer
        primaryThread.savedContext.uc_stack.ss_sp = new char[ALLOC] + ALLOC; // Allocate and set stack pointer
        primaryThread.savedContext.uc_stack.ss_flags = 0;
        primaryThread.savedContext.uc_stack.ss_size = ALLOC;

        // Add to list of TCBs
        allThreads.emplace_back(primaryThread); // Refactored listoftcb to all_threads to match earlier code

        // Set to running thread
        runningQueue = &allThreads.front(); // Set the running_queue to the address of the primary thread
    }

    /*!*****************************************************************************
     * @brief Creates a new thread that starts execution by calling thd_function_t 
     with the argument param. Allocates a stack and TCB for the new thread and 
     assigns a unique thread ID.
     * 
     * @param thd_function_t A pointer to the function that the new thread will execute.
     * @param param A pointer to the argument that will be passed to thd_function_t.
     * @return ThreadID that uniquely identifies the newly created thread.
    *******************************************************************************/

    ThreadID new_thd(void* (*thd_function_t)(void*), void* param)
    {
        static ThreadID id = 0;

        // auto newthread = TCB();



        if (++id == std::numeric_limits<unsigned int>::max())
        {
            id = 1;
        }
        TCB newThread(id, runningQueue->threadID, NEW, thd_function_t, param);

        allThreads.push_back(newThread);

        newQueue.push(&allThreads.back());

        return newThread.threadID;
    }

    /*!*****************************************************************************
     * @brief Terminates the calling thread, setting its return value to ret_value. 
     If a thread is waiting on this thread, it is made schedulable. Also handles 
     the cleanup of the thread's resources.
     * 
     * @param value A pointer to the value that the thread returns upon exit.
    *******************************************************************************/
    void thread_exit(void* value)
    {
        TCB* currentThread = runningQueue;
        currentThread->returnVal = value;
        currentThread->state = ThreadState::TERMINATED;

        auto it = waitingQueue.find(currentThread->threadID);
        if (it != waitingQueue.end())
        {
            readyQueue.push_front(it->second);
            it->second->state = ThreadState::READY;

            waitingQueue.erase(it);
        }
        runningQueue = nullptr;
        currentThread->GetContext();
        thd_yield();
    }

    /*!*****************************************************************************
     * @brief Blocks the calling thread until the thread with the specified ID 
     terminates. If the thread has already terminated, retrieves its return value.
     * 
     * @param id The thread ID of the thread to wait for.
     * @param value A pointer to a pointer where the return value of the waited 
     * thread will be stored.
     * @return int indicating success (WAIT_SUCCESSFUL) or failure 
    *******************************************************************************/
    int wait_thread(ThreadID id, void** value)
    {
        const auto threadToWait = std::find_if(allThreads.begin(), allThreads.end(),
            [id](const TCB& thread)
            {
                return thread.threadID == id;
            });

        if (threadToWait == allThreads.end())
        {
            return NO_THREAD_FOUND;
        }

        TCB& thread = *threadToWait;

        if (thread.state != TERMINATED)
        {
            TCB* currentThread = runningQueue;
            currentThread->state = WAITING;
            waitingQueue.emplace(id, currentThread);

            currentThread->GetContext();
            runningQueue = nullptr;
            thd_yield();

            delete[](reinterpret_cast<char*>(thread.savedContext.uc_stack.ss_sp));
        }
        else
        {

            delete[](reinterpret_cast<char*>(thread.savedContext.uc_stack.ss_sp));
        }

        if (value != nullptr)
        {
            *value = thread.returnVal;
        }

        allThreads.erase(threadToWait);

        return WAIT_SUCCESSFUL;
    }


    /*!*****************************************************************************
     * @brief Causes the calling thread to yield execution to the next thread in the 
     ready queue. The current thread's context is saved, and the scheduler is 
     invoked to determine and switch to the next thread to run.
     * 
    *******************************************************************************/
    void thd_yield()
    {
        TCB* currentThread;
        //find next
        if (readyQueue.empty())
        {
            if (!(newQueue.empty()))
            {
                currentThread = newQueue.front();
                currentThread->GetContext();
                currentThread->savedContext.uc_stack.ss_flags = 0;
                currentThread->savedContext.uc_stack.ss_size = ALLOC;
                currentThread->savedContext.uc_stack.ss_sp = reinterpret_cast<void*>(new char[ALLOC]);

                makecontext(&currentThread->savedContext, context_function, 0);

                currentThread->state = ThreadState::READY;
                readyQueue.emplace_back(currentThread);
                newQueue.pop();
            }
            else
            {
                return;
            }
        }

        TCB* oldThread = runningQueue;

        if (runningQueue != nullptr)
        {
            readyQueue.push_back(oldThread);
            oldThread->state = ThreadState::READY;
            oldThread->GetContext();
        }
        currentThread = readyQueue.front();
        readyQueue.pop_front();
        currentThread->state = ThreadState::RUNNING;
        runningQueue = currentThread;

        runningQueue->SetContext();
    }

    /*!*****************************************************************************
     * @brief Used by a pusher thread to push a value to its puller thread. If the 
     puller is ready, it transfers the value; otherwise, it yields execution after 
     adding the value to the puller's queue.
     * 
     * @param pushed_value A pointer to the value to be pushed to the puller.
    *******************************************************************************/
    void push_value(void* pushed_value)
    {
        TCB* child = runningQueue;
        ThreadID parentID = child->parentID;


        pushedVals[parentID] = pushed_value;


        if (const auto it = std::find_if(pullers.begin(), pullers.end(),
            [parentID](const TCB* tcb) { return tcb->threadID == parentID; });
            it != pullers.end())
        {

            readyQueue.push_back(*it);
            child->state = READY;
            pullers.erase(it);
        }


        thd_yield();
    }
    
    /*!*****************************************************************************
     * @brief Used by a puller thread to retrieve a value pushed by its pusher 
     thread. If the value is not ready, the puller suspends and schedules the next 
     thread.
     * 
     * @param pulled_value A pointer to a pointer where the pulled value will be stored.
    *******************************************************************************/
    void pull_value(void** pulled_value)
    {
        TCB* parent = runningQueue;
        // Use if with initializer to check for the existence of the threadID in pushedVals
        if (auto i = pushedVals.find(parent->threadID); i == pushedVals.end())
        {
            pullers.push_back(parent);
            parent->GetContext();
            runningQueue = nullptr;
            thd_yield();

            // Check again after yielding
            if (auto j = pushedVals.find(parent->threadID); j != pushedVals.end())
            {
                *pulled_value = j->second;
                pushedVals.erase(j);
            }
        }
        else
        {
            *pulled_value = i->second;
            pushedVals.erase(i);
        }
    }
}