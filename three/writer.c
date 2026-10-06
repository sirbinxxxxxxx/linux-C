#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
/////////////////////////////////////myfifo
int main(){
    signal(SIGPIPE, SIG_IGN);
    printf("等待读端连接...");
  
    int fd =open("/tmp/myfifo0927",O_WRONLY);
    if(fd == -1){
        perror("open");
        return 1 ;
    }
    printf("读端已连接，可以输入内容：\n");


    char buf[100];
    ssize_t n ;
    while (1)
    {
    n = read (STDIN_FILENO,buf,sizeof(buf));
    if(n == -1){
        if(errno == EINTR){
            continue;
        }
        perror("read");
        return 1 ;
    }
    if(n==0){
        printf("标准输入结束\n");
        continue;;
    }

    size_t total = 0;
    while (total < (size_t)(n))
    {
        ssize_t n_w = write(fd,buf+total,(size_t)n-total);
        if(n_w ==-1){
            if(errno == EINTR){
                continue;
            }
            if(errno == EPIPE){
                printf("内核SIGPIPE打断进程");
                return 1 ;
            }
            perror("write");
            return 1 ;
        }
        if(n_w == 0){
            fprintf(stderr,
                "写入失败"
            );
            return  1 ;
        }
        total += (size_t)n_w;
    }
    
    }
    
    



    unlink("/tmp/myfifo0927");
    close(fd);


    return 0;
}