#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>

int main()
{
    int seconds = 3;
    int return_code =10;

    for(int i = 0;i < 3 ; i++)
    {
        pid_t pid = fork();
        if(pid < 0){
            perror("fork");
            return 1;
        }else if(pid == 0){
            printf("创建子进程%d  pid:%ld\n", i ,(long)getpid());
            sleep(seconds-i);
            return  return_code+i;
        }

        printf("子进程%d对应的父进程pid:%ld\n", i ,(long)getpid());

    }

    int status ;
    pid_t wait_ret;
    for(int i = 0; i < 3 ; i++){

    while(1){
    wait_ret = wait(&status);
    if(wait_ret==-1){
        if(errno == EINTR){
            continue;
        }

        perror("wait");
        return 1 ;
    }
    break;
    }
    printf("回收子进程PID:%ld\n",(long)wait_ret);
    if(WIFEXITED(status)){
        printf("子进程正常退出 退出码为：%d\n",WEXITSTATUS(status));
    }else if (WIFSIGNALED(status)){
        printf("子进程信号打断退出  退出码为%d\n",WTERMSIG(status));
    }
    }


    return 0 ;
}