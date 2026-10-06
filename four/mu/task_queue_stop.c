#include "public.h"
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