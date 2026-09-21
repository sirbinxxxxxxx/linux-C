#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){

    int fd = open("result.txt",O_RDWR|O_CREAT|O_TRUNC,0664);
    if(fd == -1){
        perror("open");
        return 1;
    }
    if(dup2(fd,STDOUT_FILENO)==-1){
        perror("dup2");
        close(fd);
        return 1;
    }
    close(fd);

    pid_t pid = fork();
    if(pid < 0){
        perror("fork");
        return 1;
    }else if(pid == 0){
        execlp(
            "ls",
            "ls",
            "-l"  ,
            NULL
        );
        perror("execlp");
        _exit(127);
    }{


        wait(NULL);
    }

    return 0;
}