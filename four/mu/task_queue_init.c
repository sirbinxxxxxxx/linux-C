#include "public.h"
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