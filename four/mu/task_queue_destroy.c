#include "public.h"
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