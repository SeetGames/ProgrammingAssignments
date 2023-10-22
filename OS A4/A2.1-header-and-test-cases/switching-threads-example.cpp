#include <stdio.h>
#include "new-coro-lib.h"
#pragma GCC diagnostic ignored "-Wunused-parameter"
void *spin1(void *a)
{
    int i;
    for(i=0; i< 20; i++)
    {
        printf("SPIN1\n");
        if((i+1)%4==0)
            CORO::thd_yield();
    }
    return NULL;
}

void* spin2(void *a)
{
    int i;
    for(i=0; i< 20; i++)
    {
        printf("SPIN2\n");
        if((i+1)%4==0)
            CORO::thd_yield();
    }
    return NULL;
}


int main()
{
    CORO::thd_init();
    CORO::ThreadID id = CORO::new_thd(spin2, NULL);
    printf("Thread ID = %d\n", id);
    spin1(NULL);
    printf("switching threads test DONE\n");
}
