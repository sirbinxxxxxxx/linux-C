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


    int fd  = open(argv[1], O_WRONLY |  O_CREAT | O_TRUNC ,0664);
    if(fd == -1){
        perror("open");
        return 1;
    }

    const char* buf = argv[2];
    size_t len = strlen(buf);
    ssize_t  ret_write = write(fd,buf,len);
    if(ret_write == -1){
        perror("write");
        close(fd);
        return 1;
    }
    if((size_t)ret_write != len){   
        fprintf(stderr,
            "写入不完整 ，要求%zu字节，实际%zd字节，\n",len,ret_write
        );

        
        return 1;
    }
    printf("写入%zu字节\n",len);

    close(fd);



    return 0;
}