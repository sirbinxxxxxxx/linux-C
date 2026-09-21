#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <errno.h>
 #include <sys/wait.h>
 int main()
 {
    int last_result = 1;
    int pipefd[2];
    if(pipe(pipefd)==-1){
        perror("pipe");
        return 1 ;
    }
    pid_t pid[2];
    for( int i = 0; i < 2; i++){
        pid[i] = fork();
        if(pid[i] == -1){
            perror("fork");
            close(pipefd[1]);
            close(pipefd[0]);
            return 1 ;
            }

            else if(pid[i] == 0){
            if(i==0){
                close(pipefd[0]);
                    if((dup2(pipefd[1],STDOUT_FILENO))==-1){
                        perror("dup2");
                        close(pipefd[1]);
                        _exit(2);
                    }
                    close(pipefd[1]);
                    execlp(
                        "ls",
                        "ls",
                        "-l",
                        (char*)NULL
                    );
                    perror("execlp");
                    _exit(127);
                }

                if(i==1){
                close(pipefd[1]);
                 if((dup2(pipefd[0],STDIN_FILENO))==-1){
                        perror("dup2");
                        close(pipefd[0]);
                        _exit(3);
                    }
                    
                close(pipefd[0]);
                    execlp(
                        "wc",
                        "wc",
                        "-l",
                        (char *)NULL
                    );
                    perror("execlp");
                    _exit(127);
                }

        }
        else{
            
        }
    }
            close(pipefd[0]);
            close(pipefd[1]);
    for(int i= 0;i<2;i++){
            int status ;
            pid_t ret ;
            while (1)
            {
                ret = waitpid(pid[i],&status,0);
                if(ret == -1){
                    if(errno == EINTR){
                        continue;
                    }
                    perror("waitpid");
                    return 1;
                }   
                break;
                }
    
                if(WIFEXITED(status)){
                last_result = WEXITSTATUS(status);
                printf("程序正常退出 退出码：%d\n",WEXITSTATUS(status));
            }else if(WIFSIGNALED(status)){
                last_result =WTERMSIG(status) + 128;
                printf("子程序被信号打断，信号码为：%d\n",WTERMSIG(status));
                }
            }

   return last_result;

}