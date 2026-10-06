#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUF 3

struct arg{
    int value;
    char message[64];
};


void task_double(void* arg)
{
    struct arg* p = arg;
    printf(
        "double任务：%d * 2 = %d\n",
        p->value,
        p->value * 2
    );
};
void task_add100(void* arg)
{
    struct arg* p = arg;
    printf(
        "add100任务：%d + 100 = %d\n",
         p->value,
         p->value + 100
    );
};
void task_square(void* arg)
{
    struct arg* p = arg;
    printf(
        "square任务：%d * %d = %d\n",
         p->value,
         p->value,
         p->value *  p->value
    );
};

struct task
{
    int index;
    void (*func)(void *);
    void *arg;
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
    
    printf("线程%d执行任务%d\n",p->id,value.index);
    if(value.func != NULL)
    {
        value.func(value.arg);
    }
    free(value.arg);
    
    sleep(1);
    }
    return NULL;
}
/*==========================生产者的结构体*/
struct producer_arg
{
    int id;
    int start;
    int task_num;
    struct public_data *pd;
};
/*=================================================*/
void *producer(void *arg)
{
    struct producer_arg *p = arg;

    for(int i = 0; i < p->task_num; i++)
    {
        /*
         * 1. 先创建一个 task
         */ 
        struct task task;
        memset(&task,0,sizeof(task));
        /*
         * 2. malloc task.arg
         */
        struct arg *arg = malloc(sizeof(struct arg));
        if(arg == NULL){
            perror("malloc");
            return NULL;
        }
        /*
         * 3. 设置 id/value/message/func
         */
        task.arg = arg;
        task.index = p->start++;
        arg->value = (i+1)*100;
        snprintf(
            arg->message,
            sizeof(arg->message),
            "%dproducer 生产的任务%d",
            p->id,
            task.index
        );
        if(i%BUF==0){ task.func = task_add100;
        }
        else if(i%BUF==1){ task.func = task_double;
        }
        else{ task.func = task_square;
        }
        /*
         * 4. lock
         */
        pthread_mutex_lock(&p->pd->mutex);
        /*
         * 5. while(buffer满)
         *       cond_wait(not_full)
         */
        while (p->pd->count == BUF)
        {
            pthread_cond_wait(&p->pd->not_full,&p->pd->mutex);
        }
        
        /*
         * 6. 放入buffer
         */
        p->pd->buffer[p->pd->write_pos%BUF] = task;

        /*
         * 7. write_pos前进
         *    count++
         */
        p->pd->write_pos = (p->pd->write_pos+1)%BUF ;
        p->pd->count++;
        /*
         * 8. signal(not_empty)
         */
        pthread_cond_signal(&p->pd->not_empty);
        pthread_mutex_unlock(&p->pd->mutex);
        /*
         * 9. unlock
         */
        free(arg);
    }

    return NULL;
}

int main(){

    //数据初始化
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
        if(i==0){
        }
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
    /*================生产者数据初始化========================*/
    struct producer_arg producer_arg[2];
    memset(&producer_arg,0,sizeof(producer_arg));
    /*============================================================*/
    pthread_t tid_p[2];
    for(int i = 0;i<2 ; i++){
        producer_arg[i].pd = &pd ;
        producer_arg[i].id = i+1;
        producer_arg[i].task_num = 10;
        if(i == 0){
            producer_arg[i].start =  1;
        }else if(i == 1){
            producer_arg[i].start =  11;
        }
        int ret_t = pthread_create(
        &tid_p[i],
        NULL,
        producer,
        &producer_arg[i]
        ); 
        if(ret_t != 0){
            fprintf(stderr,"pthread_create失败：%s/n",strerror(ret_t));
            return 1;
        }
    }
    

    for(int i = 0;i < 2 ;i++){
          pthread_join(tid_p[i],NULL);
    }

    //任务发送完毕 主备退出全部线程
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