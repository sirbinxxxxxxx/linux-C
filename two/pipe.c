#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>   
#include <unistd.h>
#include <errno.h>


int main()
{   
    char buf[100];
    int pipefd[2];
    if(pipe( pipefd) == -1){
        perror("pipe");
        return 1;
    }
    
    pid_t pid = fork();
    if(pid < 0){
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }else if(pid == 0){
        close(pipefd[1]);

        while (1)
        {
            ssize_t n_r = read(pipefd[0],buf,sizeof(buf));
            if(n_r == -1){
                if(errno==EINTR){
                    continue;
                }
                perror("read");
                close(pipefd[0]);
                _exit(1);
            }
            else if(n_r == 0){
                break;
            }
        
            size_t total = 0;
            while (total < (size_t)n_r)
            {
                ssize_t n_w = write(STDOUT_FILENO,buf+total,(size_t)n_r - total);
                if(n_w==-1){
                    if(errno==EINTR){
                        continue;
                    }
                    perror("write");
                    close(pipefd[0]);
                    _exit(2);
                }else if(n_w==0){
                    fprintf(stderr,
                        "写入失败"
                    );
                    _exit(3);
                }
                total+=(size_t)n_w;
            }            
        }
                              
        close(pipefd[0]);
        _exit(0);
    }
    else 
    {
        close(pipefd[0]);
        
        size_t total3 = 0;
        ssize_t n_in ;
        while (1)
        {
            n_in = read(STDIN_FILENO,buf,sizeof(buf));
            if(n_in == -1){
                if(errno==EINTR){
                    continue;
                }
                perror("read");
                close(pipefd[1]);
                return 1;
            }
            break;
        }
        
         

        while (total3<(size_t)n_in)
        {
          ssize_t n_w =  write(pipefd[1],buf+total3,(size_t)n_in - total3);
          if(n_w==-1){
                    if(errno == EINTR){
                        continue;
                    }
                    perror("write");
                    close(pipefd[1]);
                    return 1;
                }else if(n_w ==0){
                    fprintf(stderr,
                        "写入失败"
                    );
                    return 1;
                }else{
                    total3+=(size_t)n_w;
                }
        }
        
        close(pipefd[1]);
    }

    wait(NULL);

    return 0;
}