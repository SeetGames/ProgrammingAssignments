/*!************************************************************************
 \file new-coro-lib.cpp
 \author Seetoh Wei Tung
 \par Email: seetoh.w@digipen.edu
 \par Course: CSD2182
 \par Assignment #2.1
 \date 30-10-2023
 \brief A cooperatively scheduled user-level thread library under Linux.
**************************************************************************/
#include <ucontext.h>
#include <queue>
#include <map>
#include <functional>
#include <vector>
#include <iostream>
#include "new-coro-lib.h"

namespace CORO
{
    // Forward declaration for scheduler
    void scheduler();

    // Constants for the user-level thread library
    const int STACK_SIZE = 1024 * 1024; // 1MB stack for each thread
    using ThreadID = unsigned;

    enum ThreadState : int
    {
        NEW,
        READY,
        RUNNING,
        WAITING,
        DONE
    };

    // Thread Control Block (TCB)
    struct TCB
    {
        ThreadID id;
        void *return_value;
        ucontext_t context;
        char stack[STACK_SIZE];
        ThreadState state;
        ThreadID waiting_for; // used when one thread waits for another
        std::function<void *(void *)> func;
        void *func_arg;
        void *pushed_value; // used in push-pull mechanism
        ThreadID parent;    // used in push-pull mechanism
    };

    std::queue<TCB *> new_threads;   // Threads created but not ready to run
    std::queue<TCB *> ready_threads; // Threads ready to run
    TCB *running_thread = nullptr;
    std::map<ThreadID, TCB *> waiting_threads;   // Threads waiting for another thread to complete
    std::map<ThreadID, TCB *> completed_threads; // Threads that have completed but not yet cleaned up
    ThreadID next_thread_id = 1;                 // starting from 1, as main thread (the process) has ID 0

    // Initialize the thread library
    void thd_init()
    {
        // Creating a TCB for the main thread
        running_thread = new TCB;
        running_thread->id = 0;
        running_thread->state = RUNNING;
        // TODO: Further initialization if necessary
    }

    ThreadID new_thd(void *(*thd_function_t)(void *), void *param)
    {
        // 1. Allocate a new TCB
        TCB *tcb = new TCB;

        // 2. Assign a unique thread ID
        tcb->id = next_thread_id++;

        // 3. Set up the context for the new thread
        getcontext(&(tcb->context)); // initialize with the current context
        tcb->context.uc_stack.ss_sp = tcb->stack;
        tcb->context.uc_stack.ss_size = sizeof(tcb->stack);
        tcb->context.uc_link = 0; // we might modify this later
        makecontext(&(tcb->context), (void (*)())thd_function_t, 1, param);

        // 4. Add the new thread to the new_threads queue
        new_threads.push(tcb);

        return tcb->id;
    }

    void thread_exit(void *ret_value)
    {
        running_thread->return_value = ret_value;
        running_thread->state = DONE;

        // Check if any thread is waiting for this thread
        auto it = waiting_threads.find(running_thread->id);
        if (it != waiting_threads.end())
        {
            TCB *waiting_tcb = it->second;
            waiting_tcb->state = READY;
            ready_threads.push(waiting_tcb);
            waiting_threads.erase(it);
        }

        // Invoke the scheduler
        scheduler();
    }

    int wait_thread(ThreadID id, void **value)
    {
        // Check if the thread to be waited on is already done
        auto it = completed_threads.find(id);
        if (it != completed_threads.end())
        {
            *value = it->second->return_value;
            delete it->second;
            completed_threads.erase(it);
            return WAIT_SUCCESSFUL;
        }

        // If the thread is not done yet
        running_thread->state = WAITING;
        running_thread->waiting_for = id;
        waiting_threads[id] = running_thread;

        // Invoke the scheduler
        scheduler();

        // Once this thread is resumed
        *value = completed_threads[id]->return_value;
        delete completed_threads[id];
        completed_threads.erase(id);
        return WAIT_SUCCESSFUL;
    }

    void scheduler()
    {
        // Add the current running thread to the back of the ready queue
        if (running_thread && running_thread->state == RUNNING)
        {
            ready_threads.push(running_thread);
        }

        // If there are ready threads, pick one to run next
        if (!ready_threads.empty())
        {
            running_thread = ready_threads.front();
            ready_threads.pop();
        }
        else if (!new_threads.empty()) // Else, pick a new thread
        {
            running_thread = new_threads.front();
            new_threads.pop();
        }

        running_thread->state = RUNNING; // Set the state of the new running thread
    }

    void thd_yield()
    {
        // Save the current context
        if (getcontext(&(running_thread->context)) == 0)
        {
            // Move the new threads to the ready queue
            while (!new_threads.empty())
            {
                ready_threads.push(new_threads.front());
                new_threads.pop();
            }

            // If there are ready threads, schedule one to run
            if (!ready_threads.empty())
            {
                TCB *next_thread = ready_threads.front();
                ready_threads.pop();

                // If the next thread is the same as the current running thread, just return
                if (next_thread == running_thread)
                    return;

                TCB *prev_thread = running_thread;
                running_thread = next_thread; // Update the running thread
                running_thread->state = RUNNING;

                // If the previous thread is still runnable, put it back to the ready queue
                if (prev_thread->state == RUNNING)
                {
                    ready_threads.push(prev_thread);
                }

                // Swap context to the new running thread
                swapcontext(&(prev_thread->context), &(running_thread->context));
            }
        }
        // This part is executed when the thread is resumed
    }

    void push_value(void *pushed_value)
    {
        TCB *puller_tcb = waiting_threads[running_thread->parent];
        puller_tcb->pushed_value = pushed_value;

        if (puller_tcb->state == WAITING)
        {
            puller_tcb->state = READY;
            ready_threads.push(puller_tcb);
        }

        thd_yield(); // Yield the CPU to allow another thread to run
    }

    void pull_value(void **pulled_value)
    {
        TCB *pusher_tcb = running_thread; // assuming we have a way to get the pusher thread

        if (pusher_tcb->pushed_value != nullptr)
        {
            *pulled_value = pusher_tcb->pushed_value;
            pusher_tcb->pushed_value = nullptr;
        }
        else
        {
            running_thread->state = WAITING;
            thd_yield(); // Yield the CPU, waiting for the pusher to push a value

            // Once resumed, retrieve the pushed value
            *pulled_value = pusher_tcb->pushed_value;
            pusher_tcb->pushed_value = nullptr;
        }
    }
}

/*!*****************************************************************************
g++ -std=c++17 only-one-thread.cpp coro-lib.a
./a.out > output_only-one-thread.txt
g++ -std=c++17 switching-threads-example.cpp coro-lib.a
./a.out > output_switching-threads-example.txt
g++ -std=c++17 wait-thread-example.cpp coro-lib.a
./a.out > output_wait-thread-example.txt
g++ -std=c++17 double-wait-test.cpp coro-lib.a
./a.out > output_double-wait-test.txt
g++ -std=c++17 return-test1.cpp coro-lib.a
./a.out > output_return-test1.txt
g++ -std=c++17 schedule-test.cpp coro-lib.a
./a.out > output_schedule-test.txt
g++ -std=c++17 fib-threads.cpp coro-lib.a
./a.out 12 > output_fib-threads.txt
g++ -std=c++17 matrix-multiply-test-case.cpp coro-lib.a
./a.out 4 3 5 4 > output_matrix-multiply-test-case.txt
g++ -std=c++17 fib-source-sink.cpp coro-lib.a
./a.out 12 > output_fib-source-sink.txt
g++ -std=c++17 context-saving-test-case.cpp coro-lib.a
./a.out > output_context-saving-test-case.txt
*******************************************************************************/