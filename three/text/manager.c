#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h>  
#include <string.h>   
#include <semaphore.h> 
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>

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

volatile sig_atomic_t stop = 0;
void handler (int sig){
    stop = 1;
}

int main()
{
    //父进程 创建共享内存
    int fd = shm_open(SHM_NAME,O_RDWR | O_CREAT | O_EXCL ,0664);
    if(fd == -1 ){
        perror("shm_open");
        return 1;
    }

    //给他取空间大小
    if(ftruncate(fd,sizeof(struct shared_data)) == -1){
        perror("ftruncate");
        close(fd);
        shm_unlink(SHM_NAME);
        return 1 ;
    }
    //映射
    struct shared_data* shmp = mmap(NULL,
                sizeof(struct shared_data),
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                fd,
                0
    );
    if(shmp == MAP_FAILED){
        perror("mmap");
        
        close(fd);
        shm_unlink(SHM_NAME);
        return 1 ;
    }
    //映射完fd没用了  给他关了
    close(fd);

    //初始化共享内存的映射
    memset(shmp,0,sizeof(*shmp));

    //创建信号量 

    sem_t * empty = sem_open (SEM_EMPTY,O_CREAT | O_EXCL ,0664, 5);
    if(empty == SEM_FAILED){
        perror("sem_open empty");
        munmap(shmp,sizeof(struct shared_data));
        shm_unlink(SHM_NAME);
        return 1 ;
    }
     sem_t * full = sem_open (SEM_FULL,O_CREAT | O_EXCL ,0664, 0);
    if(full == SEM_FAILED){
        perror("sem_open full");
         munmap(shmp,sizeof(struct shared_data));
        sem_close(empty);
        shm_unlink(SHM_NAME);
        sem_unlink(SEM_EMPTY);
        return 1 ;
    }
     sem_t * muxte = sem_open (SEM_MUXTE,O_CREAT | O_EXCL ,0664, 1);

    if(muxte == SEM_FAILED){
        perror("sem_open muxte");
         munmap(shmp,sizeof(struct shared_data));
        sem_close(empty);
        sem_close(full);
        shm_unlink(SHM_NAME);
        sem_unlink(SEM_EMPTY);
        sem_unlink(SEM_FULL);
        return 1 ;
    }

       struct sigaction sa;
                memset(&sa,0,sizeof(sa));
                sa.sa_handler = handler;
                sigemptyset(&sa.sa_mask);

                if (sigaction(SIGINT, &sa, NULL) == -1) {
                perror("sigaction SIGINT");
                
                munmap(shmp,sizeof(struct shared_data));
                sem_close(empty);
                sem_close(full);
                sem_close(muxte);
                sem_unlink(SEM_EMPTY);
                sem_unlink(SEM_FULL);
                sem_unlink(SEM_MUXTE);
                shm_unlink(SHM_NAME);
                return 1;
                }

                if (sigaction(SIGTERM, &sa, NULL) == -1) {
                perror("sigaction SIGINT");
                munmap(shmp,sizeof(struct shared_data));
                sem_close(empty);
                sem_close(full);
                sem_close(muxte);
                sem_unlink(SEM_EMPTY);
                sem_unlink(SEM_FULL);
                sem_unlink(SEM_MUXTE);
                shm_unlink(SHM_NAME);
                return 1;
                }

    pid_t pid[2];
    for(int i = 0 ;i<2;i++){

    pid[i] = fork();
    if(pid[i] < 0){
        perror("fork");
            if(i == 1){
                waitpid(pid[0], NULL, 0);
                }
            sem_close(empty);
            sem_close(full);
            sem_close(muxte);
            sem_unlink(SEM_EMPTY);
            sem_unlink(SEM_FULL);
            sem_unlink(SEM_MUXTE);
            shm_unlink(SHM_NAME);
          
        return 1 ;
        }else if (pid[i] == 0)
        {   
            if(i==0){
                printf("humi进程已运行\n");
                fflush(stdout);
                execlp("./humi","humi",NULL);
                perror("execlp");
                _exit(127);
            }else if(i==1){
                printf("temp进程已运行\n");
                fflush(stdout);
                execlp("./temp","temp",NULL);
                perror("execlp");
                _exit(128);
                
            }else{
             
            }
        }


    }
    
   

    int fd_log = open("text.log",O_WRONLY | O_CREAT | O_APPEND ,0664 );
    if (fd_log == -1){
        perror("open");
         munmap(shmp,sizeof(struct shared_data));
            sem_close(empty);
            sem_close(full);
            sem_close(muxte);
            sem_unlink(SEM_EMPTY);
            sem_unlink(SEM_FULL);
            sem_unlink(SEM_MUXTE);
            shm_unlink(SHM_NAME);
            return 1;
    }
    while(1){
        char line[256];
        
        while(sem_wait(full) == -1){
            if(errno == EINTR){
                continue;
            }
            perror("sem_waiy full");
             munmap(shmp,sizeof(struct shared_data));
            sem_close(empty);
            sem_close(full);
            sem_close(muxte);
            sem_unlink(SEM_EMPTY);
            sem_unlink(SEM_FULL);
            sem_unlink(SEM_MUXTE);
            shm_unlink(SHM_NAME);
            return 1 ;
        }

        // sem_wait(muxte);
        if(shmp->class == 1){
            printf("序号=%d 类型=湿度 数值=%d 生产者PID=%ld\n",shmp->read_pos+1,shmp->data[shmp->read_pos%BUF] .humi,(long)shmp->data[shmp->read_pos%BUF].pid);    
        
        int n = snprintf(
                    line,
                    sizeof(line),
                    "序号=%d 类型=湿度 数值=%d 生产者PID=%ld\n",
                    shmp->read_pos+1,shmp->data[shmp->read_pos%BUF] .humi,
                    (long)shmp->data[shmp->read_pos%BUF].pid
            );
            if(n <= 0){
                fprintf(stderr,
                "字符串格式化失败\n"
                );
                 munmap(shmp,sizeof(struct shared_data));
                sem_close(empty);
                    sem_close(full);
                    sem_close(muxte);
                    sem_unlink(SEM_EMPTY);
                    sem_unlink(SEM_FULL);
                    sem_unlink(SEM_MUXTE);
                    shm_unlink(SHM_NAME);
                    return 1;
            }
            if(n>=sizeof(line)){
                fprintf(stderr,
                "缓冲区不够，内容被截断\n"
                );
                sem_close(empty);
                 munmap(shmp,sizeof(struct shared_data));
                    sem_close(full);
                    sem_close(muxte);
                    sem_unlink(SEM_EMPTY);
                    sem_unlink(SEM_FULL);
                    sem_unlink(SEM_MUXTE);
                    shm_unlink(SHM_NAME);
                    return 1 ;
            }
            int total = 0 ;
            while(total < n) {
                ssize_t n_w = write(fd_log,line + (size_t)total , n-total);
                if(n_w == -1){
                    if(errno == EINTR ){
                        continue;
                    }
                    perror("write");
                     munmap(shmp,sizeof(struct shared_data));
                    sem_close(empty);
                    sem_close(full);
                    sem_close(muxte);
                    sem_unlink(SEM_EMPTY);
                    sem_unlink(SEM_FULL);
                    sem_unlink(SEM_MUXTE);
                    shm_unlink(SHM_NAME);
                    return 1;
                }else if(n_w == 0){
                    fprintf(stderr,"写入失败");
                     munmap(shmp,sizeof(struct shared_data));
                    sem_close(empty);
                    sem_close(full);
                    sem_close(muxte);
                    sem_unlink(SEM_EMPTY);
                    sem_unlink(SEM_FULL);
                    sem_unlink(SEM_MUXTE);
                    shm_unlink(SHM_NAME);
                    return 1;
                }
                total+= (int)n_w;
            }
        }    

        if(shmp->class == 2){
            printf("序号=%d 类型=温度 数值=%d 生产者PID=%ld\n",shmp->read_pos+1,shmp->data[shmp->read_pos%BUF].temp,(long)shmp->data[shmp->read_pos%BUF].pid);    
        
          int n1 = snprintf(
                    line,
                    sizeof(line),
                    "序号=%d 类型=温度 数值=%d 生产者PID=%ld\n",
                    shmp->read_pos+1,shmp->data[shmp->read_pos%BUF].temp,
                    (long)shmp->data[shmp->read_pos%BUF].pid
            );
            if(n1 <= 0){
                fprintf(stderr,
                "字符串格式化失败\n"
                );
                sem_close(empty);
                 munmap(shmp,sizeof(struct shared_data));
                    sem_close(full);
                    sem_close(muxte);
                    sem_unlink(SEM_EMPTY);
                    sem_unlink(SEM_FULL);
                    sem_unlink(SEM_MUXTE);
                    shm_unlink(SHM_NAME);
                return 1 ;
            }
            if(n1>=sizeof(line)){
                fprintf(stderr,
                "缓冲区不够，内容被截断\n"
                );
                sem_close(empty);
                 munmap(shmp,sizeof(struct shared_data));
                    sem_close(full);
                    sem_close(muxte);
                    sem_unlink(SEM_EMPTY);
                    sem_unlink(SEM_FULL);
                    sem_unlink(SEM_MUXTE);
                    shm_unlink(SHM_NAME);
                    return 1;
            }
            int total = 0 ;
            while(total < n1) {
                ssize_t n_w = write(fd,line + (size_t)total , n1-total);
                if(n_w == -1){
                    if(errno = EINTR ){
                        continue;
                    }
                    perror("write");
                     munmap(shmp,sizeof(struct shared_data));
                    sem_close(empty);
                    sem_close(full);
                    sem_close(muxte);
                    sem_unlink(SEM_EMPTY);
                    sem_unlink(SEM_FULL);
                    sem_unlink(SEM_MUXTE);
                    shm_unlink(SHM_NAME);
                    return 1;
                }else if(n_w == 0){
                    fprintf(stderr,"写入失败");
                     munmap(shmp,sizeof(struct shared_data));
                    sem_close(empty);
                    sem_close(full);
                    sem_close(muxte);
                    sem_unlink(SEM_EMPTY);
                    sem_unlink(SEM_FULL);
                    sem_unlink(SEM_MUXTE);
                    shm_unlink(SHM_NAME);
                    return 1;
                }
                total+= (int)n_w;
            }
        }

        shmp->read_pos++;
        // sem_post(muxte);
        sem_post(empty);
        usleep(700000);
        if(shmp->read_pos  == 200){
            break;
        }
    }
    
   for(int i = 0 ; i < 2 ;i++){
    int status[2];
    while (1)
    {
        pid_t ret = waitpid(pid[i],&status[i],0);
        if(ret==-1){
            if(errno == EINTR){
                continue;
            }
            perror("waitpid");
                close(fd_log);
                munmap(shmp,sizeof(struct shared_data));
                sem_close(empty);
                sem_close(full);
                sem_close(muxte);
                sem_unlink(SEM_EMPTY);
                sem_unlink(SEM_FULL);
                sem_unlink(SEM_MUXTE);
                shm_unlink(SHM_NAME);
            return 1;
        }   
        if(WIFEXITED(status[i])){
            printf("进程%ld已成功退出 退出码为%d\n",(long)ret,WEXITSTATUS(status[i]));
        }
        if(WIFSIGNALED(status[i])){
             printf("进程%ld被打断 信号码为%d\n",(long)ret,WTERMSIG(status[i]));
        }
        break;
    }
    


   }
    
    close(fd_log);
    munmap(shmp,sizeof(struct shared_data));
    sem_close(empty);
    sem_close(full);
    sem_close(muxte);
    sem_unlink(SEM_EMPTY);
    sem_unlink(SEM_FULL);
    sem_unlink(SEM_MUXTE);
    shm_unlink(SHM_NAME);
    
    return 0;
}