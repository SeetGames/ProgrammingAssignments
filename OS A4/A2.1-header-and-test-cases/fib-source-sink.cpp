#include <iostream>
#include "new-coro-lib.h"
//#define RANDOM

void *fib(void *a)
{
    int num = *reinterpret_cast<int *>(a);
    long long first=0;
    long long second=1;
    for(auto i=0; i<num; ++i)
    {
        CORO::push_value(reinterpret_cast<void*>(first));
        int tmp=second;
        second+=first;
        first=tmp;
    }
    return nullptr;
}

int main(int argc, char **argv)
{
    CORO::ThreadID id;
    CORO::thd_init();
    int num;

#ifdef RANDOM
    srand(time(NULL));
    num = rand()%10 + rand()%5;
#else	
    if(argc != 2)
    {
        printf("%s <number>", argv[0]);
        return 1;
    }
    num = atoi(argv[1]);
    if(num < 0)
    {
        printf("%s <number>", argv[0]);
        printf("number must be positive!");
        return 1;
    }
#endif	
    id = CORO::new_thd(fib, &num);
    printf("Thread ID = %d\n", id);
    for(long long i=0; i<num; ++i)
    {
        long long value;
        CORO::pull_value(reinterpret_cast<void**>(&value));
        std::cout << value << std::endl;
    }
	return 0;
}
