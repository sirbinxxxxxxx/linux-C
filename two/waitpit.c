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

        sleep(2);
         return 7;
    }else 
    {
        value-=10;
    printf("fujinc  value:%d父进程id:%ld,他的子进程id:%ld,fork返回值:%ld\n",value, (long)getpid(),(long)pid,(long)pid);


    printf("父进程开始等待子进程\n");


    int status ;
    pid_t pid_wait  = waitpid(pid,&status,0);
        if (pid_wait == -1)
        {
            perror("waitpid");
            return 1;
        }
        
        if(WIFEXITED(status)){
            printf("子进程正常退出  退出码%d\n",WEXITSTATUS(status));
        }else if(WIFSIGNALED(status)){
             printf("子进程信号打断退出 退出码%d\n",WTERMSIG(status));
        }
    


    }
    

   


    return 0;
}