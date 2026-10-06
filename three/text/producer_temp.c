#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h>  
#include <string.h>   
#include <semaphore.h> 
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define BUF 5
#define SHM_NAME "/myshm"
#define SEM_EMPTY "/myempty"
#define SEM_FULL "/myfull"
#define SEM_MUXTE "/mymuxte"
struct detal_data
{
    int temp;
    int humi;
    pid_t pid;


};
struct shared_data
{
    struct detal_data data[BUF];    
    int write_pos;
    int read_pos;
    int muxte;
    int temp_index;
    int humi_index;
    int class;
};

int main()
{   
    //等待manager创建共享内存
    int fd ;
    while (1){
        fd = shm_open(SHM_NAME,O_RDWR,0);
        if(fd == -1){
            if(errno == ENOENT){
                printf("等待manager创建共享内存...\n");
                sleep(1);
                continue;
            }
            perror("shm_open");
            return 1;
        }
        break;
    }
    //manager已经初始化过了 


    //创建映射
    struct shared_data *shmp = mmap(NULL,
                sizeof(struct shared_data),
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
    //打开信号量

    sem_t* empty = sem_open(SEM_EMPTY,0);
    if(empty == SEM_FAILED) {
        perror("sem_open enpty");
        shm_unlink(SHM_NAME);
        return 1;
    }
     sem_t* full = sem_open(SEM_FULL,0);
    if(full == SEM_FAILED) {
        perror("sem_open full");
        shm_unlink(SHM_NAME);
        sem_close(empty);
        return 1;
    }
     sem_t* muxte = sem_open(SEM_MUXTE,0);
    if(muxte == SEM_FAILED) {
        perror("sem_open enpty");
        shm_unlink(SHM_NAME);
        sem_close(empty);
        sem_close(full);
        return 1;
    }
    
    
        //处理数据
    int temp[100];
    pid_t pid;
    pid = getpid();
    for(int i =0;i<100;i++){
            temp[i] =  -20 + rand()%61;
    }

   while (1)
    {
        while (sem_wait(empty) == -1)
        {
            if(errno == EINTR){
                continue;
            }
            perror("sem_wait empty");
            sem_close(empty);
            sem_close(full);
            sem_close(muxte);
            munmap(shmp,sizeof(struct shared_data));
            return 1;
        }
        sem_wait(muxte);
        shmp->class = 2;
        printf("[温度进程 PID=%ld]产生数据：%d℃\n",(long)(shmp->data[shmp->temp_index % BUF].pid = pid),shmp->data[shmp->temp_index % BUF].temp = temp[shmp->temp_index]);
        shmp->temp_index++;
        sem_post(muxte);
        sem_post(full);
        if(shmp->temp_index == 100){
            break;
        }
        usleep(800000);
    }
    
    
    sem_close(empty);
    sem_close(full);
    sem_close(muxte);
    munmap(shmp,sizeof(struct shared_data));

    return 0;

}