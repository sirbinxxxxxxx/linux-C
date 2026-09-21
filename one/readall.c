#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
 #include <unistd.h>


int main (int argc, char *argv[])
{   
    // for(int i = 0;i < argc;i++)
    // {
    //     printf("argv[%d] = %s\n",i,argv[i]);
    // }

   
    if(argc != 2){
        printf("用法:%s 文件路径\n",argv[0]);
        return 1;
    }
    
     const char* path = argv[1];

    int fd = open(path,O_RDONLY);
    if(fd == -1){
        perror("open");
        return 1;
    }


    char buf[17];
    
    ssize_t n ;
    ssize_t i = 0;
    while ((n =read(fd,buf,sizeof(buf)-1))>0)
    {   
        buf[n] = '\0';
        i+=n; 
        printf("%s",buf);
       
    }
    
    if(n == -1){
        perror("read");
        close(fd);
        return 1;
    }
    printf("共读到%zu字节\n",i);

    
    close(fd);

    return 0;
}