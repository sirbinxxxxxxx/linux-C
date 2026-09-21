#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


int main(int argc, char *argv[])
{
    if(argc != 3){
        printf("用法：%s 目标文件 写入内容 \n",argv[0] );
        return 1;
    }


    int fd  = open(argv[1], O_WRONLY |  O_CREAT | O_APPEND ,0664);
    if(fd == -1){
        perror("open");
        return 1;
    }

    const char* buf = argv[2];
    size_t len = strlen(buf);
    ssize_t  ret_write ;
    size_t total = 0;
    int i = 0;
    while (total < len)
    {   
        size_t remaining = len - total;
        size_t once = remaining > 2? 2:remaining;

        ret_write = write(fd,buf+total,once);
        if(ret_write == -1){
        perror("write");
        close(fd);
        return 1;
        }
        if(ret_write == 0 ){
            fprintf(stderr,
                "write没有写入数据\n"
            );
            close(fd);
            return 1;
        }
   
        total+=(size_t)ret_write;
        i++;
    }
    printf("写了%d轮",i);
    printf("写入%zu字节\n",total);
 

    int ret_close = close(fd);
    if(ret_close == -1){
        perror("close");
        return 1;
    }


    return 0;
}