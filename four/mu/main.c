#include "public.h"
void* worker_handler(void* arg){
    struct private_data* p =arg;
    while(1){
    struct task task;
    int ret_p = task_queue_pop(p->pd,&task);
        if(ret_p == 0){
           break;
        }else if(ret_p<0){
            fprintf(stderr,"task_queue_pop 失败\n");
            break;
        }
    printf("线程%d执行任务%d\n",p->id,task.index);
    if(task.func!=NULL){
        task.func(task.arg);
    }
    free(task.arg);
    }
    return NULL;
}

void* producer_handler(void* arg){
    struct producer_arg* p = arg;
  
    for(int i = 0 ;i <p->task_num ;i++){

          struct arg* ag = malloc(sizeof(*ag));
    if(arg==NULL){
        perror("malloc");
        return NULL;
    }
        struct task task;
        memset(&task,0,sizeof(task));
        ag->value = (i+1)*100;
        task.index = p->start;
        snprintf(
            ag->message,
            sizeof(ag->message),
            "producer%d产生任务%d\n",
            p->id,
            task.index
        );
        
        p->start++;
        task.arg = ag;
        if(i%BUF==0){
            task.func = task_add100;
        } else if (i%BUF==1){
            task.func = task_double;
        }else{
             task.func = task_square;
        }

        int ret_p = task_queue_push(p->pd,task);
        if(ret_p != 0){
            free(ag);
            fprintf(stderr,"task_queue_push 失败\n");
            return NULL;
        }
    }

    return NULL;
}

int main()
{
    struct public_data queue;
    //初始化锁 信号队列 计数器
    int ret_i = task_queue_init(&queue);
    if(ret_i == 1){
        fprintf(stderr,"task_queue_init 失败");
    }
    //新建生产线程
    struct producer_arg producer[2];
    pthread_t tid_p[2];
    for(int i = 0;i<2;i++){
        producer[i].pd = &queue;
        producer[i].id = i+1;
        producer[i].task_num = 10;
        if(i==0){
            producer[i].start = 1;
        } else if(i == 1){
            producer[i].start =11;
        }
        int ret = pthread_create(
            &tid_p[i],
            NULL,
            producer_handler,
            &producer[i]
        );
        if(ret != 0){
            fprintf(stderr,"pthread_create 失败");
            return 1;            
        }
    }
    //新建消费线程
    struct private_data worker[3];
    pthread_t tid_w[3];
    for(int i = 0;i<3;i++){
        worker[i].id = i+1;
        worker[i].pd = &queue;
        int ret = pthread_create(
            &tid_w[i],
            NULL,
            worker_handler,
            &worker[i]
        );
        if(ret != 0){
            fprintf(stderr,"pthread_create 失败");
            return 1;            
        }
    }
    //等待生成线程结束
    for(int i = 0; i<2 ; i++){
        pthread_join(tid_p[i],NULL);

    }
    //发送结束信号
    int ret_s = task_queue_stop(&queue);
    if(ret_s == 1){
        fprintf(stderr,"task_queue_stop 失败");
    }   

    //等待消费进程结束
    for(int i = 0;i<3 ;i++){
        pthread_join(tid_w[i],NULL);
    }

    //注销锁 信号量

    int ret_d = task_queue_destroy(&queue);
    if(ret_d == 1){
        fprintf(stderr,"task_queue_destroy 失败");
    }


    return 0;
}