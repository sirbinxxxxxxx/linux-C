#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
int  main(int argc,char* argv[])
{
    if(argc != 3){
        fprintf(stderr,
            "用法：%s 源文件 目标文件\n", argv[0]
        );
        return 1;
    }

    int fd_1 =  open(argv[1],O_RDONLY);
    if (fd_1 == -1)
    {
        perror("open");
        return 1;
    }
    
    int fd_2 = open(argv[2],O_WRONLY|O_CREAT,0644);
    if (fd_2 == -1)
    {
        perror("open");
        close(fd_1);
        return 1;
    }

    struct stat statbuf1;
    struct stat statbuf2;
    int ret_1 = fstat(fd_1,&statbuf1);
    if(ret_1 == -1){
        perror("fstat");
        close(fd_1);
        close(fd_2);
        return 1;
    }
    int ret_2 = fstat(fd_2,&statbuf2);
        if(ret_2 == -1){
        perror("fstat");
        close(fd_1);
        close(fd_2);
        return 1;
        }
    if(statbuf1.st_dev==statbuf2.st_dev 
        && statbuf1.st_ino==statbuf2.st_ino
        ){
            fprintf(stderr,
            "两个文件是同一个文件，不合法"
            );
            close(fd_1);
            close(fd_2);
            return 1;
        }

    if(ftruncate(fd_2, 0)== -1){    
        perror("ftruncate");
        close(fd_1);
        close(fd_2);
        return 1;
    }


    ssize_t n_r ;
    char buf_r[100];
    while (1)
    {
        n_r=read(fd_1,buf_r,sizeof(buf_r));
         
        if(n_r == -1){
        if(errno == EINTR){
            continue;
            }
            perror("read"); 
            close(fd_1);
            close(fd_2);
            return 1;
            }
        if (n_r == 0) {
            break;
        }

        size_t total = 0;
        while (total < (size_t)n_r)
        {

            ssize_t n_w = write(fd_2,buf_r+total,(size_t)n_r-total);
            if(n_w == -1){                 
                    if(errno == EINTR){
                     continue;
                    }
                perror("write");
                close(fd_1);
                close(fd_2);
                return 1;
            }
            if(n_w == 0){
                fprintf(stderr,
                "write没有写入任何数据\n"
                );
                close(fd_1);
                close(fd_2);
                return 1;
            }

            total+=(size_t)n_w;
        }
       
        }
    int exit_status = 0;

    if(close(fd_1)==-1){
        perror("close");
        exit_status =1;  
    }
    
    if(close(fd_2)==-1){
        perror("close");
        exit_status =1;
    }
    return exit_status ;
}