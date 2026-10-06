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
    int fd = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL ,0664);
    if(fd == -1){
        perror("shm_open");
        return 1 ;
    }

    if(ftruncate(fd,sizeof(struct shared_date)) == -1 ){
        perror("ftruncate");
        close(fd);
        return 1;
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

    //数据初始化

    shmp->date->value = 0;
    memset(shmp,0,sizeof(*shmp));


    //创建两个信号量

    sem_t *full = sem_open(FULL, O_CREAT | O_EXCL , 0664 , 0); //这个是 表示已有数据数量 empty
    if(full == SEM_FAILED){
        perror("sem_open");     
        shm_unlink(SHM_NAME);
        return 1;
    }
    sem_t *empty = sem_open(EMPTY, O_CREAT | O_EXCL , 0664 , BUF);    //假设这个是空槽位 full
    if(empty == SEM_FAILED){
        perror("sem_open");   
        sem_unlink(FULL);
        shm_unlink(SHM_NAME);
        return 1;
    }

   
//定义5个不同的数据 

    int values[] = {24 ,25,26,27,28 ,29,30,31,32,33}; 
    char *messages[] = {
    "hello world",
    "sdadsadasd",
    "fbdfbdffgdfg",
    "uykuikykukyk",
    "dsfdsfsdfsdf",
    "sdasdasd",
    "sdasd",
    "dsada",
    "dsadas",
    "dsdasd"
    };


//后边环形缓冲发送的逻辑
    
    for( int i=0; i<10;i++){

        while(sem_wait(empty) == -1){
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
       
        shmp->date[shmp->write_pos % BUF].value = values[i];
        strcpy(shmp->date[shmp->write_pos % BUF].message,messages[i]);
        printf("第%d条数据\n",shmp->write_pos+1);
        printf("writer写入槽位 %d\n", shmp->write_pos%BUF);


        shmp->write_pos++;
        if(sem_post(full) == -1 ){
            perror("sem_post full");
            sem_close(full);
            sem_close(empty);
            munmap(shmp,sizeof(struct shared_date));
            sem_unlink(FULL); 
            sem_unlink(EMPTY); 
            shm_unlink(SHM_NAME);
            return 1;
        }
       
    
}
    sem_close(full);
    sem_close(empty);
    munmap(shmp,sizeof(struct shared_date));
        
    return 0;
}