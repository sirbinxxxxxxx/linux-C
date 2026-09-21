#include <stdio.h>
#include <pthread.h>
#include "thread_pool.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main()
{
   
    pthread_t tid[5];
    int ready = 0;

    pthread_mutex_t mutex;
    pthread_cond_t cond;

    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);

    struct  shared same ;
    same.cond  =  &cond;
    same.mutex = &mutex;
    same.ready = &ready;
    
    struct  worker_arg date[5];
    
    
   for(int i = 0; i < 5; i++){
    
    date[i].same =  &same; 
    date[i].id = i+1;
    int ret = pthread_create(
        &tid[i],
        NULL,
        worker,
        &date[i]
    );
    if(ret !=0){
         fprintf(stderr,
            "pthread_create: %s\n",
            strerror(ret));

        
        return 1;
        }
    }
    printf("main线程继续运行\n");
    sleep(2);
    pthread_mutex_lock(&mutex);
    ready = 1;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    for(int i = 0; i < 5; i++){
    pthread_join(tid[i],NULL);
    }
   
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);


return 0;
}