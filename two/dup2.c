#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
 #include <sys/wait.h>

int main()
{
    int last_result;
    int pipefd[2];
    if(pipe(pipefd) == -1){
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if(pid < 0){
        perror("fork");
        close(pipefd[1]);
        close(pipefd[0]);
        return 1;
    }else if(pid==0){
        close(pipefd[0]);
         if(dup2(pipefd[1],STDOUT_FILENO)==-1){
            perror("dup2");
            _exit(1) ;
         }
        close(pipefd[1]);

        execlp("ls",
                "ls",
                "-l",
                NULL
        );
        perror("execvp");
        _exit(127);
    }else{
        close(pipefd[1]);
        char buf[100];

       
        
        ssize_t n;
        while ((n=read(pipefd[0],buf,sizeof(buf)))>0)
        {
        size_t total = 0;
        while((size_t)(n)>total){

         ssize_t n_w = write(STDOUT_FILENO,buf+total,n-total);
            if(n_w == -1){
                if(errno == EINTR){
                    continue;
                }
                perror("write");
                close(pipefd[0]);
                return 1;
            }else if(n_w == 0){
                fprintf(stderr,
                    "写入失败"
                );
                return 1;
            }
            total += (size_t)n_w;

        }
        }
        
        close(pipefd[0]);
       
    }

    int status ;
    pid_t ret ;
    while (1)
    {
        ret = waitpid(pid,&status,0);
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
        last_result =WTERMSIG(status);
        printf("子程序被信号打断，信号码为：%d\n",WTERMSIG(status));
    }
    
    return last_result;
}