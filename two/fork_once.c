#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(){

    int  value = 100;


    printf("fork之前：PID=%ld value=%d\n",
       (long)getpid(), value);
    
    fflush(stdout);

    pid_t pid = fork();
    if(pid<0){
        perror("fork");
        return 1;
    }else if(pid == 0){
        value+=10;
        printf("zijinc  value:%d子进程id:%ld,他的父进程id:%ld,fork返回值:%ld\n",value, (long)getpid(),(long)getppid(),(long)pid);
    }else 
    {
        value-=10;
    printf("fujinc  value:%d父进程id:%ld,他的子进程id:%ld,fork返回值:%ld\n",value, (long)getpid(),(long)pid,(long)pid);
    }
    

   


    return 0;
}