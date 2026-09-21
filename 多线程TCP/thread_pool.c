#include <stdio.h>
#include <pthread.h>
#include "thread_pool.h"

void* worker(void* arg)
{   
    struct worker_arg *p = (struct worker_arg*) arg;

    pthread_mutex_lock(p->same->mutex);
    while (*(p->same->ready) == 0 )
    {
        printf("线程%d没有任务，开始睡眠\n", p->id);
        pthread_cond_wait(p->same->cond,p->same->mutex);

    }
        printf("线程%d被唤醒",p->id);
        printf("线程%d开始运行\n", p->id);
    
    pthread_mutex_unlock(p->same->mutex);

    return NULL;
}

