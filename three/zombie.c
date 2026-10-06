#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    pid_t pid = fork();

    if(pid == 0)
    {
        printf("子进程 pid=%d\n", getpid());

        sleep(2);

        printf("子进程退出\n");

        exit(10);
    }
    else
    {
        printf("父进程 pid=%d\n", getpid());

        while(1)
        {
            sleep(5);
            break;
        }
    }

    wait(NULL);
    return 0;
}