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
#include <errno.h>

#define SHM_NAME "/myshm0927"
#define EMPTY "/mysem0927"
#define FULL "/mysem0928"

#define BUF   5


struct idos 
{
    int value;
    char message[100];
};

struct  shared_date
{
    struct idos  date[BUF];

    int write_pos;
    int read_pos;
    
};

int main(){

    int fd ;
    while (1)
    {
        
        fd = shm_open(SHM_NAME,O_RDWR,0);
        if(fd == -1){
            if(errno == ENOENT){
                printf("等待写段创建共享内存...\n");
                sleep(1);
                continue;
            }

        perror("shm_open");
        return 1 ;
        }
        break;

    }
    
    
     struct shared_date* shmp = mmap(
                    NULL,
                    sizeof(struct shared_date),
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED,
                    fd,
                    0
    );
    if(shmp == MAP_FAILED){
        perror("mmap");
        close(fd);
        return 1;
    }
    close(fd);


    //消费者只打开已有信号量时
    sem_t *full = sem_open(FULL,0);  //假设这个是空槽位 full
    if(full == SEM_FAILED){
        perror("sem_open");
        shm_unlink(SHM_NAME);
        return 1;
    }
    sem_t *empty = sem_open(EMPTY,0);  //这个是 表示已有数据数量 empty
    if(empty == SEM_FAILED){
        perror("sem_open");
        shm_unlink(SHM_NAME);
        return 1;
    }

    while(1){
        while(sem_wait(full)){
            if(errno == EINTR){
                continue;
            }
            perror("sem_wait empty");
            sem_close(full);
            sem_close(empty);
            munmap(shmp,sizeof(struct shared_date));
            sem_unlink(FULL); 
            sem_unlink(EMPTY); 
            shm_unlink(SHM_NAME);
            return 1;
        }
        printf("第%d条数据为：\nvalue:%d message:%s\n",shmp->read_pos+1,shmp->date[shmp->read_pos%BUF].value,shmp->date[shmp->read_pos%BUF].message);
        shmp->read_pos++;
        if(sem_post(empty) == -1){
            perror ("sem_post empty");
             sem_close(full);
            sem_close(empty);
            munmap(shmp,sizeof(struct shared_date));        
            sem_unlink(FULL); 
            sem_unlink(EMPTY); 
            shm_unlink(SHM_NAME);
        }
        sleep(1);

        if(shmp->read_pos   == 10){
            break;
        }
    }

    sem_close(full);
    sem_close(empty);
    munmap(shmp,sizeof(struct shared_date));        
    sem_unlink(FULL); 
    sem_unlink(EMPTY); 
    shm_unlink(SHM_NAME);
    return 0;
}
