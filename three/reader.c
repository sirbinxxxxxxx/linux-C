#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
int main(){
    
    
    if(mkfifo("/tmp/myfifo0927",0664)== -1){
        if(errno == EEXIST){
            fprintf(stderr,
            "文件已存在，已忽略\n"
            );
        }else{
            perror("mkfifo");
            return 1;
        }  
    }
    printf("等待写端连接...\n");


    int fd =open("/tmp/myfifo0927",O_RDONLY);
    if(fd == -1){
        perror("open");
        return 1 ;

    }
    
    
    ssize_t n;
    char buf[100];
    while (1)
    {
        n=read(fd,buf,sizeof(buf));
        if(n == -1){
            perror("read");
            return 1;
        }
        else if (n==0){
            printf("\n所有写段已关闭，收到EOF\n");
            break;
        }else{
        size_t total = 0;
        while (total<(size_t)n){
            ssize_t n_w = write(STDOUT_FILENO,buf+total,n-total);
            if(n_w == -1){
                perror("write");
                return 1;
            }
            else if(n_w ==0 ){
                fprintf(stderr,
                    "没有写入数据\n"
                );
                return 1;
            }else{
                total +=(size_t)n_w;
            }


        }
        }
    }
    

  
    close(fd);

    return 0;
}