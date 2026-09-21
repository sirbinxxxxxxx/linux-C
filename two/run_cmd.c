#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>   
#include <unistd.h>
#include <errno.h>
   int main(int argc,char* argv[])
   {
       //无法暂定参数数量
    if (argc < 2) {
    fprintf(stderr,
            "用法：%s 命令 [参数...]\n",
            argv[0]);
    return 1;
    }
       pid_t pid = fork();
       if(pid<0){
        perror("fork");
        return 1;
       }else if(pid == 0){
       
        execvp(argv[1],argv+1);
        perror("execvp");
        _exit(127);
        
       }else{

        int status;
        pid_t ret_w;
        while (1)
        {
            ret_w = waitpid(pid,&status,0);
            if(ret_w == -1){
                if(errno == EINTR){
                    continue;
                }
                perror("waitpid");
                return 1;
            }
            break;
        }
        if(WIFEXITED(status)){
            int exit_code = WEXITSTATUS(status);
            printf("子进程成功退出，退出码为%d\n",WEXITSTATUS(status));
            return exit_code;
        }else if(WIFSIGNALED(status)){
            int signal_number = WTERMSIG(status);
            printf("子进程被信号终止，信号编号为：%d\n",WTERMSIG(status));
            return signal_number + 128;
        }
                
       }
       return 1;
   }