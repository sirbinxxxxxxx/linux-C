 #include <unistd.h>
 #include <stdio.h>
#include <sys/types.h>

       #include <sys/wait.h>
#include <signal.h>
#include <errno.h>
 int main()
 {
    pid_t pid = fork();

    if(pid < 0){
        perror("fork");
        return 1;
    }else if(pid==0){

        printf("成功创建子进程  PID = %ld\n",(long)getpid());
        fflush(stdout);
        char* argv[]={
            "env",
            NULL
        };
        char* const envp[]={
            "name=zhaobin",
            "age=18",
            NULL
        };
  
        if(execve("/usr/bin/env",argv,envp) == -1){
        perror("execve");
        _exit(127);
        }

        
    }  else{
         signal(SIGINT, SIG_IGN);
        printf("父进程自己的PID = %ld\n",(long)getpid());
         printf("子进程  PID = %ld\n",(long)pid);



        int status;
        pid_t wait_ret ;
        while (1)
        {
            wait_ret = waitpid(pid,&status,0);
            if(wait_ret==-1){
                if(errno=EINTR){
                    continue;
                }
            perror("waitpid");
            return 1;
            }
            break;
        }
        
      
        if(WIFEXITED(status)){
            printf("子进程正常返回 返回码为：%d\n",WEXITSTATUS(status));
        }
        else if(WIFSIGNALED(status)){
            printf("导致子进程终止的信号编号为：%d\n",WTERMSIG(status));
        }
        }


    


    return 0;
 }