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
#include <fcntl.h>  

#define SHM_NAME "/myshm0927"
#define SEM_EMPTY "/mysem0927"
#define SEM_FULL "/mysem0928"

struct shared_data
{
    int value;
    char message[100];
};
int main ()
{   
    int fd = shm_open(SHM_NAME,O_RDWR|O_CREAT,0664);
    if(fd == -1){
        perror("shm_open");
        return 1 ;
    }

    if(ftruncate(fd,sizeof(struct shared_data)) == -1){
        perror("ftruncate");
        return 1;
    }
   
    struct shared_data* shmp = mmap(
                    NULL,
                    sizeof(struct shared_data),
                    PROT_WRITE,
                    MAP_SHARED,
                    fd,
                    0
    );
    if(shmp == MAP_FAILED){
        perror("mmap");
        return 1;
    }
    close(fd);
    
    shmp->value = 0;
    memset(&(shmp->message),0,sizeof(shmp->message));

    
    sem_t * empty =sem_open(SEM_EMPTY,O_RDWR|O_CREAT,0664,1);
    if(empty == SEM_FAILED){
        perror("sem_open");
        return 1 ;
    }
    sem_t * full =sem_open(SEM_FULL,O_RDWR|O_CREAT,0664,0);
    if(empty == SEM_FAILED){
        perror("sem_open");
        return 1 ;
    }
    
 

    sem_wait(empty); // 1 - 1 
    shmp->value = 24;
    strcpy(shmp->message,"hello world");
    sem_post(full); //  0 + 1

    sem_wait(empty);
    shmp->value = 25;
    strcpy(shmp->message,"hsdasdasd");
    sem_post(full);
    sem_wait(empty);
    shmp->value = 26;
    strcpy(shmp->message,"asdasdasd");
    sem_post(full);
    sem_wait(empty);
     shmp->value = 27;
    strcpy(shmp->message,"afasfdasdas");
    sem_post(full);
    sem_wait(empty);
    shmp->value = 28;
    strcpy(shmp->message,"adsadasdasda");
    sem_post(full);

    // sem_close(p);
    munmap(shmp,sizeof(struct shared_data));


    return 0;
}