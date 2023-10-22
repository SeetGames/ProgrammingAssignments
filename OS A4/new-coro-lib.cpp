#include "new-coro-lib.h"
#include <ucontext.h>
#include <list>
#include <map>
#include <queue>

namespace CORO
{

    struct TCB
    {
        ThreadID id;
        ucontext_t context;
        void *returnValue;
        bool finished;
        std::queue<void*> valueQueue; // Queue to hold pushed values
    };

    std::map<ThreadID, TCB> threads;
    std::list<ThreadID> readyQueue;
    ThreadID currentThreadID = 0; // 0 is reserved for the main thread
    ThreadID nextThreadID = 1;    // Start assigning IDs from 1

    void scheduler()
    {
        // Simple round-robin scheduler
        if (!readyQueue.empty())
        {
            ThreadID next = readyQueue.front();
            readyQueue.pop_front();
            ThreadID prev = currentThreadID;
            currentThreadID = next;
            swapcontext(&threads[prev].context, &threads[next].context);
        }
    }

    void thd_init()
    {
        // Initialize the main thread TCB
        TCB mainThread;
        mainThread.id = currentThreadID;
        mainThread.finished = false;
        getcontext(&mainThread.context);
        threads[currentThreadID] = mainThread;
    }

    ThreadID new_thd(void *(*start_routine)(void *), void *arg)
    {
        TCB newThread;
        newThread.id = nextThreadID++;
        newThread.finished = false;
        getcontext(&newThread.context);

        newThread.context.uc_stack.ss_sp = malloc(16384); // Allocate stack
        newThread.context.uc_stack.ss_size = 16384;
        newThread.context.uc_link = &threads[currentThreadID].context;
        makecontext(&newThread.context, (void (*)())start_routine, 1, arg);

        threads[newThread.id] = newThread;
        readyQueue.push_back(newThread.id);

        return newThread.id;
    }

    void thread_exit(void *value)
    {
        threads[currentThreadID].returnValue = value;
        threads[currentThreadID].finished = true;
        scheduler(); // Switch to the next thread
    }

    int wait_thread(ThreadID id, void **value)
    {
        // Simple wait implementation, might block the entire application
        while (!threads[id].finished)
        {
            scheduler();
        }

        if (value != nullptr)
        {
            *value = threads[id].returnValue;
        }

        // Clean up
        free(threads[id].context.uc_stack.ss_sp);
        threads.erase(id);

        return WAIT_SUCCESSFUL;
    }

    void thd_yield()
    {
        readyQueue.push_back(currentThreadID);
        scheduler(); // Switch to the next thread
    }

    void pull_value(void **pulled_value)
    {
        TCB& currentTCB = threads[currentThreadID];
        
        if (!currentTCB.valueQueue.empty()) 
        {
            *pulled_value = currentTCB.valueQueue.front();
            currentTCB.valueQueue.pop();
        } 
        else 
        {
            // Suspend the Puller until a value is pushed
            scheduler();
            pull_value(pulled_value); // Retry pulling the value
        }
    }

    void push_value(void *pushed_value)
    {
        TCB& currentTCB = threads[currentThreadID];
        
        currentTCB.valueQueue.push(pushed_value);
        
        // If the Puller is waiting, it should be scheduled to run next
        if (/* condition to check if Puller is waiting */) 
        {
            // Add logic to schedule the Puller next
        }
        
        thd_yield(); // Yield to the next thread (possibly the Puller)
    }

} // namespace CORO
