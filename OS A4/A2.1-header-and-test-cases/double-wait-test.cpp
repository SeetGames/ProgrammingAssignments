#include <iostream>
#include <cstdio>
#include "new-coro-lib.h"
#pragma GCC diagnostic ignored "-Wunused-parameter"

void *thd(void *param )
{
    std::cout << "Do nothing\n";
    CORO::thd_yield();
    printf("after yield() but before thread function returns\n");
    return nullptr;
}


int main()
{
    CORO::ThreadID thd1_id;

    CORO::thd_init();
    thd1_id =CORO::new_thd(thd, nullptr);
    printf("First wait is %d\n", CORO::wait_thread(thd1_id, nullptr)==CORO::WAIT_SUCCESSFUL);
    printf("back to main\n");
    printf("Second wait is %d\n", CORO::wait_thread(thd1_id, nullptr)==CORO::NO_THREAD_FOUND);
}
