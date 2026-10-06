#include "public.h"
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