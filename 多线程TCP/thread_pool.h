#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
struct  shared 
{
    int* ready;
    pthread_mutex_t *mutex;
    pthread_cond_t *cond;
};



struct worker_arg
{
    int id;
    struct shared *same;
};


void* worker(void* arg);

#endif