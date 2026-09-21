#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>


int  main(int argc,char* argv[])
{
    if(argc != 3){
        fprintf(stderr,
            "用法：%s 源文件 目标文件", argv[0]
        );
        return 1;
    }

    int fd_1 =  open(argv[1],O_RDONLY);
    if (fd_1 == -1)
    {
        perror("open");
        return 1;
    }
    
    int fd_2 = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0644);
    if (fd_2 == -1)
    {
        perror("open");
        close(fd_1);
        return 1;
    }



    ssize_t n_r ;
    char buf_r[100];
    while ((n_r=read(fd_1,buf_r,sizeof(buf_r)))>0)
    {
        size_t total = 0;
        while (total < (size_t)n_r)
        {

            ssize_t n_w = write(fd_2,buf_r+total,(size_t)n_r-total);
            if(n_w == -1){
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

    if(n_r == -1){
        perror("read"); 
        close(fd_1);
        close(fd_2);
        return 1;
    }



    int exit_status = 0;

    if(close(fd_1)==-1){
        perror("close");
        exit_status =1;
        return 1;
    }
    
    if(close(fd_2)==-1){
        perror("close");
        exit_status =1;
        return 1;
    }
    return exit_status ;
}