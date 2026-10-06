#include <sys/mman.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>        
#include <fcntl.h>    
#include <semaphore.h>

struct shared_data
{
    sem_t sem;
    int initialized;
    int value;
    char message[100];
};

int main()
{
    int fd  = shm_open("/myshm0927",
            O_CREAT | O_RDWR ,
            0666
            );
            if(fd == -1){
                perror("open");
                return 1;
            }
    

    if(ftruncate(fd,sizeof(struct shared_data)) == -1){
        perror("ftruncate");
        return 1;
    }

    struct  shared_data *shmp = mmap(
                        NULL,
                        sizeof(struct shared_data),
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED,
                        fd,
                        0
    );
        if(shmp == MAP_FAILED){
            perror("mmap");
            return 1;
        }

    close(fd);

    //初始化数据
    shmp->initialized = 0;
    shmp->value = 0;
    strcpy (shmp->message,"hello world");

    //sem_init
    sem_init(&(shmp->sem),1,0);
    shmp->initialized = 1;


    printf("write开始写数据...\n");
    shmp -> value = 24;
    strcpy(shmp->message,"hello world");
    printf("write写完数据...\n");

    sem_post(&(shmp->sem));
    munmap(shmp,sizeof(struct shared_data));  //映射的什么 解除什么 

     //shm_unlink("/myshm0927");   //这个是系统层级的  只要有一个进程删除  最后一个使用者


    return 0 ;
}