#include <stdio.h>
#include <unistd.h>
#include <signal.h>
 #include <string.h>

volatile sig_atomic_t stop = 0;
volatile sig_atomic_t count = 0;
void handler(int sig)
{   
    (void)sig;
    if(sig == SIGUSR1){
    count++ ;
    }
    if(sig == SIGTERM || sig == SIGINT){
    stop = 1;
    }
}

int main()
{   

    struct sigaction sa;
    memset(&sa,0,sizeof(sa) );

    sa.sa_handler = handler;

    sigemptyset(&sa.sa_mask);

    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        return 1;
    }

    if (sigaction(SIGTERM, &sa, NULL) == -1)
    {
        perror("sigaction");
        return 1;
    }
     if (sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        perror("sigaction");
        return 1;
    }

    while (!stop)
    {   
            if(count)
            {
            printf("收到用户通知信号\n");
            count=0;
            }
        printf("程序正在运行... 若ctrl+c就退出\n");
        printf("before sleep\n");

        sleep(5);
        printf("after sleep\n");
    }

    printf("程序正在退出....\n");
    return 0;
}