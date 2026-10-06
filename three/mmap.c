#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

int  main ()
{

    int fd = open ("hello.txt",O_RDWR|O_CREAT, 0664);
    if(fd == -1){
        perror("open");
    }
    if(ftruncate(fd,20)==-1){
        perror("ftruncate");
        close(fd);
        return 1;
    }

    int *p = mmap(NULL,
            20,
            PROT_WRITE|PROT_READ,
            MAP_PRIVATE,
            fd,
            0 

        );
        if (p == MAP_FAILED){
        perror("mmap");
        return 1;
        }
        *p =10;

        pid_t pid = fork();
        if(pid == -1){
            perror("fork");
            return 1;

        }else if(pid == 0){
            printf("child 修改前: %d\n", *p);
            p[0] = 20;
            printf("child 修改后: %d\n", *p);
            fflush(stdout);
              munmap(p, 20);
                close(fd);

            _exit(20);
        }else{
            wait(NULL);
              printf("parent 读取: %d\n", *p);
        }


    munmap(p,20);
    close(fd);

    return  0; 
}