#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;

    /* fork another process */
    pid = fork();

    if (pid < 0)
    {
        /* error occurred */
        fprintf(stderr, "Fork failed.\n");
        return 1;
    }
    else if (pid == 0)
    {
        /* child process */
        printf("I am the child, PID = %d\n", getpid());

        pid_t pid2 = fork();
        if (pid2 < 0)
        {
            /* error occurred */
            fprintf(stderr, "Fork failed.\n");
            return 1;
        }
        else if (pid2 == 0)
        {
            /* child process */
            printf("I am the grand-child, PID = %d\n", getpid());
        }
        else
        {
            /* parent process */
            printf("2nd Child complete. PID = %d\n", getpid());
        }
    }
    else
    {
        /* parent process */
        /* parent will wait for the child to complete */
        wait(NULL);
        printf("Child complete.\n");
    }
    return 0;
}