#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUF 3

void *task_double(int value)
{
    printf(
        "double任务：%d * 2 = %d\n",
        value,
        value * 2
    );
};
void *task_add100(int value)
{
    printf(
        "add100任务：%d + 100 = %d\n",
        value,
        value + 100
    );
};
void *task_square(int value)
{
    printf(
        "square任务：%d * %d = %d\n",
        value,
        value,
        value * value
    );
};

struct task
{
    int id;
    int value;
    char message[64];
    void (*func)(void *);
   
};

struct public_data
{   
    struct task buffer[BUF];
    int write_pos;
    int read_pos;
    int count;
  
    int stop;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    
};

struct private_data {
    int id;
    struct public_data* pd;
};

void* worker(void *arg){
    struct private_data* p = arg;
    
    
    while(1 ){
    printf("线程%d开始运行\n",p->id);
    pthread_mutex_lock(&p->pd->mutex);
        while(p->pd->count  == 0 && p->pd->stop == 0){
            printf("缓冲区没有数据读端进入等待...\n");
            pthread_cond_wait(&p->pd->not_empty,&p->pd->mutex);      
        }
        if(p->pd->stop == 1 && p->pd->count  == 0){
            pthread_mutex_unlock(&p->pd->mutex);
            printf("全部被唤醒准备退出...\n");
            break;
        }

    /* 先读取当前槽 */
    struct task value = p->pd->buffer[p->pd->read_pos % BUF];

     /* 然后移动读指针 */
    p->pd->read_pos =(p->pd->read_pos + 1) % BUF;
    
    p->pd->count--;
   
    pthread_cond_signal(&p->pd->not_full);
    pthread_mutex_unlock(&p->pd->mutex);
    
    char line[256];
    int n = snprintf(
        line,
        sizeof(line),
        "已处理任务 ： id=%d value=%d message=%s",
        value.id,
        value.value,
        value.message
    );
    if(n<0){
        fprintf(stderr,"snprintf 失败");
        return NULL;
    }

    value.func(value.value);
    printf("%s\n",line);
    sleep(1);
    }

    return NULL;
}

int main(){

    //数据初始化
    struct task buffer[20];
    memset(buffer,0,sizeof(buffer));
    
    struct public_data  pd;
    memset(&pd,0,sizeof(pd));
    pthread_mutex_init(&pd.mutex,NULL);
    pthread_cond_init(&pd.not_empty,NULL);
    pthread_cond_init(&pd.not_full,NULL);
  
    struct private_data va[3];
    memset(va,0,sizeof(va));

    pthread_t tid[3];
    for(int i = 0;i<3 ; i++){
        va[i].pd = &pd ;
        va[i].id = i+1;
        int ret_t = pthread_create(
        &tid[i],
        NULL,
        worker,
        &va[i]
        ); 

        if(ret_t != 0){
            fprintf(stderr,"pthread_create失败：%s/n",strerror(ret_t));
            return 1;
        }
    }
    // char line[256];
    // for(int i = 0 ; i< 20;i++){
    //     buffer[i].id = i+1;
    //     buffer[i].value = (i+1)*100; 
    //     strcpy(buffer[i].message,"dsasdasda");
    //         int n = snprintf(
    //             line,
    //             sizeof(line),
    //             "id=%d value=%d message=%s",
    //             buffer[i].id,
    //             buffer[i].value,
    //             buffer[i].message
    //         );
    //         if(n<0){
    //              fprintf(stderr,"snprintf 失败");
    //             return 1;
    // }
    // }

    //主线程来当生产者
    int total = 0;
    char  line[256];
    for(int i = 0 ; i < 20 ; i++){
        pthread_mutex_lock(&pd.mutex);
        while(pd.count == BUF){
            printf("缓冲区满，producer等待..\n");
            pthread_cond_wait(&pd.not_full,&pd.mutex);
        }

        buffer[i].id = i+1;
        buffer[i].value = (i+1)*100; 
        strcpy(buffer[i].message,"dsasdasda");
            int n = snprintf(
                line,
                sizeof(line),
                "id=%d value=%d message=%s",
                buffer[i].id,
                buffer[i].value,
                buffer[i].message
            );
            if(n<0){
                 fprintf(stderr,"snprintf 失败");
                return 1;
    }
        if(i%BUF==0){ buffer[i].func = task_add100;
        }
        if(i%BUF==1){ buffer[i].func = task_double;
        }
        if(i%BUF==2){ buffer[i].func = task_square;
        }

        pd.buffer[pd.write_pos%BUF] = buffer[i];
        pd.write_pos = (pd.write_pos+1)%BUF;
        pd.count++;
        total++;
        pthread_cond_signal(&pd.not_empty);
        pthread_mutex_unlock(&pd.mutex); 

        printf("任务%d: %s \n",total,line);
    }

    //主进程发送完毕 主备退出全部线程
    pthread_mutex_lock(&pd.mutex);
    pd.stop = 1;
    pthread_cond_broadcast(&pd.not_empty);
    pthread_mutex_unlock(&pd.mutex);



    for(int i = 0; i< 3 ;i++){
        pthread_join(tid[i],NULL);
    }
    pthread_mutex_destroy(&pd.mutex);
    pthread_cond_destroy(&pd.not_empty);
    pthread_cond_destroy(&pd.not_full);

    return 0;
}