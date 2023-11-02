#include <stdio.h>
#include <cstdlib>
#include "new-coro-lib.h"
void *t(void *a)
{
	long  v_rbx, v_r12, v_r13, v_r14, v_r15;

	v_rbx = rand()%100;
	v_r12 = rand()%100;
	v_r13 = rand()%100;	
	v_r14 = rand()%100;
	v_r15 = rand()%100;
	asm volatile
	( 	  
      "movq %0, %%rbx\n\t"
      "movq %1, %%r12\n\t"	  
      "movq %2, %%r13\n\t"	  
      "movq %3, %%r14\n\t"	  
      "movq %4, %%r15\n\t"	  
	  : 
	  : "r" (v_rbx),"r" (v_r12), \
			"r" (v_r13),"r" (v_r14),"r" (v_r15)
	  : "rbx", "r11","r12", "r13", "r14"
	);
	
	CORO::thd_yield();
	
	long  rbx, r12, r13, r14, r15;
	
	asm volatile
	( 
      "movq %%rbx, %0\n\t"
      "movq %%r12, %1\n\t"	  
      "movq %%r13, %2\n\t"	  
      "movq %%r14, %3\n\t"	  
      "movq %%r15, %4\n\t"	  
      : "+m" (rbx), "+m" (r12),\
			"+m" (r13), "+m" (r14), "+m" (r15)
	  :
	  : 
	);
	
	if (rbx!=v_rbx|| \
		r12!=v_r12||r13!=v_r13||r14!=v_r14||r15!=v_r15)
		printf ("%s: ERROR \n", (char*)a);
	else
		printf("%s: %ld %ld %ld %ld %ld\n", (char*)a, \
				rbx, r12, r13, r14, r15);
		
	CORO::thd_yield();
	
	return NULL;
}

int main()
{
    CORO::ThreadID id;
    CORO::thd_init();
    char p1[] = "t1";
    char p2[] = "t2";
	
    id = CORO::new_thd(t, (void*)p2);
    printf("Thread ID = %d\n", id);
    t( (void*)p1);
}
