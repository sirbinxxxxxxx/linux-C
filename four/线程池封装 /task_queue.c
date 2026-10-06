#include "task_queue.h"
int task_queue_init(
    struct public_data* arg
)
{
    
    if(arg==NULL){        
        return 1;
    }
    struct public_data* p = arg ;
    memset(p,0,sizeof(*p));

    pthread_mutex_init(&p->mutex,NULL);
    pthread_cond_init(&p->not_empty,NULL);
    pthread_cond_init(&p->not_full,NULL);



    return 0;
}



int task_queue_push(
    struct public_data* arg,
    struct task task
)
{
    if(arg == NULL){
        return 1;
    }
    struct public_data* p = arg ;
    
    pthread_mutex_lock(&p->mutex);
    while(p->count == BUF){
        pthread_cond_wait(&p->not_full,&p->mutex);
    }
    p->buffer[p->write_pos%BUF] = task;
    p->write_pos=(p->write_pos+1)%BUF;
    p->count ++ ;
    pthread_cond_signal(&p->not_empty);
    pthread_mutex_unlock(&p->mutex);

    return 0 ;
}



int task_queue_pop(
    struct public_data *p,
    struct task *out
)
{
    if(p == NULL || out == NULL){
        return -1;
    }
    
    pthread_mutex_lock(&p->mutex);
    while(p->count == 0 && p->stop == 0){
        pthread_cond_wait(&p->not_empty,&p->mutex);
    }
    if(p->stop == 1 && p->count == 0){
        pthread_mutex_unlock(&p->mutex);
        printf("准备退出...");
        return 0;
    }
    *out = p->buffer[p->read_pos%BUF];
    p->read_pos = (p->read_pos+1)%BUF;
    p->count--;
    pthread_cond_signal(&p->not_full);
    pthread_mutex_unlock(&p->mutex);
    
   
    return 1 ; //成功取到任务
}


int task_queue_stop(
    struct public_data* arg
)
{   
    
    if(arg == NULL){

        return 1;
    }
    struct public_data* p = arg ;
    pthread_mutex_lock(&p->mutex);
    p->stop = 1;
    pthread_cond_broadcast(&p->not_empty);
    pthread_mutex_unlock(&p->mutex);


    return 0;
}

int task_queue_destroy(
    struct public_data *arg
)
{
    if(arg == NULL){

        return 1;
    }   
    struct public_data* p = arg ;


    pthread_mutex_destroy(&p->mutex);
    pthread_cond_destroy(&p->not_empty);
    pthread_cond_destroy(&p->not_full);

    return 0;
}