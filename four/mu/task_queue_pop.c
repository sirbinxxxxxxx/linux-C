#include "public.h"
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