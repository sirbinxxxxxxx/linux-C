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

#define SHM_NAME "/myshm0927"

#define SEM_EMPTY "/mysem0927"
#define SEM_FULL "/mysem0928"

struct shared_data
{
    
    int value;
    char message[100];
};
int main ()
{   int fd;
    while (1)
    {
    fd = shm_open(SHM_NAME,O_RDONLY,0664);
    if(fd == -1){
        if(errno == ENOENT){
            printf("等待write 创建共享内存\n");
            sleep(1);
            continue;
        }
        perror("shm_open");
        return 1;
    }
    break;
    }
    
    

    struct shared_data * shmp = mmap(
                        NULL,
                        sizeof(struct shared_data),
                        PROT_READ,
                        MAP_SHARED,
                        fd,
                        0
    );
    if(shmp == MAP_FAILED){
        perror("mmap");
        return  1;
    }
    close(fd);

    sem_t* empty =sem_open(SEM_EMPTY,1);
    if(empty== SEM_FAILED){
        perror("sem_open");
        return 1;
    }
    sem_t* full =sem_open(SEM_FULL,0);
        if(empty== SEM_FAILED){
        perror("sem_open");
        return 1;
    }
    sem_wait(full);  //  0  write后 +1
    printf("value:%d \nmessage:%s \n",shmp->value,shmp->message);
    sem_post(empty); //  0+ 1

    sem_wait(full);
    printf("value:%d \nmessage:%s \n",shmp->value,shmp->message);
    sem_post(empty);

    sem_wait(full);
    printf("value:%d \nmessage:%s \n",shmp->value,shmp->message);
    sem_post(empty);

    sem_wait(full);
    printf("value:%d \nmessage:%s \n",shmp->value,shmp->message);
    sem_post(empty);

    sem_wait(full);
    printf("value:%d \nmessage:%s \n",shmp->value,shmp->message);
    sem_post(empty);

    sem_close(empty);
    sem_close(full);
    sem_unlink(SEM_EMPTY);
    sem_unlink(SEM_FULL);
    munmap(shmp,sizeof(struct shared_data));

    shm_unlink(SHM_NAME);



    return 0;
}