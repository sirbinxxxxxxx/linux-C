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
#include <errno.h>


struct shared_data
{
    sem_t sem;
    int initialized;
    int value;
    char message[100];
};

int main()
{
    int fd;
    while (1)
    {
        fd = shm_open("/myshm0927",
             O_RDWR ,
             0
            );

            if(fd != -1){
                break;
            }
            if(fd == -1){
                if (errno == ENOENT){
                    printf("等待writer创建共享内存... \n");
                    sleep(1);
                    continue;
                }
                perror("open");
                return 1;
            }
            break;
    }


    struct shared_data * shmp = mmap(
                NULL,
                sizeof(struct shared_data),
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                fd,
                0
    );
    if(shmp == MAP_FAILED){
        perror("mmap");
        return 1 ;
    }

    close(fd);


     
    printf("等待写端写入中...\n");
    while (shmp->initialized == 0)
    {
    sleep(1);
    }


   
    sem_wait(&(shmp->sem));     
    printf("value:%d",shmp->value);
    printf("message:%s",shmp->message);
    sem_destroy(&(shmp->sem));
    munmap(shmp,sizeof(struct shared_data));
    shm_unlink("/myshm0927");

    return 0;
}