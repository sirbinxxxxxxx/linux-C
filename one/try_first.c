// #define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>

int  main(int argc,char* argv[])
{
    
    if (argc != 3){
        fprintf(stderr,
            "用法：%s 源文件  目标文件\n", argv[0] 
        );
        return 1;
    }

    int fd_from = open(argv[1],O_RDONLY);
    if(fd_from==-1){
        perror("open");
        return 1;
    }
    int fd_to = open(argv[2],O_WRONLY | O_CREAT ,0644);
    if(fd_to==-1){
        perror("open");
        close(fd_from);
        return 1;
    }

    struct  stat statbuf_from;
    struct  stat statbuf_to;

    if(fstat(fd_from,&statbuf_from)==-1){
        perror("fstat");
        close(fd_from);
            close(fd_to);
        return 1;
    }

    if(fstat(fd_to,&statbuf_to)==-1){
        perror("fstat");
        close(fd_from);
            close(fd_to);
        return 1;
    }

    if(statbuf_from.st_dev==statbuf_to.st_dev && statbuf_from.st_ino==statbuf_to.st_ino){
        fprintf(stderr,
        "两个文件是同一个文件，不合法\n"
        );
          close(fd_from);
            close(fd_to);
            return 1 ;
    }

    if(ftruncate(fd_to,0)==-1){
        perror("ftruncate");
        close(fd_from);
        close(fd_to);
        return 1;
    }

   
    while(1){
        
        char buf[100];
    
        ssize_t n_r = read(fd_from,buf,sizeof(buf));
        if(n_r == -1){
            if(errno == EINTR){
                continue;
            }
            perror("read");
            close(fd_from);
            close(fd_to);
            return 1 ;
        }
        if(n_r == 0){
           
            break;
        }
        size_t total=0;
        while(total<(size_t)n_r){
            ssize_t n_w = write(fd_to,buf+total,(size_t)n_r - total);
            if(n_w==-1){
                if(errno==EINTR){
                    continue;
                }
                    perror("write");
                    close(fd_from);
                    close(fd_to);
                    return 1 ;
            }
            if(n_w == 0){
                fprintf(stderr,
                    "没有写入任何字符\n"
                );
                close(fd_from);
                close(fd_to);
                return 1;
            }
            total+=(size_t)n_w;

        }

    }
    int result =0;
    if(close(fd_from)==-1){
        perror("close");
        result =1;
    }

    if(close(fd_to)==-1){
        perror("close");
        result =1;
    }


    return result;
}