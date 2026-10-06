#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct public_data {
    int da;
    int ready;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

void* worker(void *arg){
    struct public_data*p = arg ;

    printf(" 新进程开始运行\n");

    pthread_mutex_lock(&p->mutex);

    while(p->ready == 0){

        printf("工作线程：没有数据，开始等待\n");
        int ret = pthread_cond_wait(&p->cond,&p->mutex);
        if(ret !=0 ){
            fprintf(stderr,"pthread_cond_wait失败：%s\n",strerror(ret));
            pthread_mutex_lock(&p->mutex);
            return NULL;

        }
       
    }
    printf("da = %d\n",p->da)  ;
    p->ready = 0;
        
    pthread_mutex_unlock(&p->mutex);
    return NULL;

}

int main(){

    pthread_t tid;
    struct public_data data;
    pthread_mutex_init(&data.mutex,NULL);
    pthread_cond_init(&data.cond,NULL);

    //主线程准备数据
    data.da = 100;
    data.ready =0;

    int ret_create = pthread_create(
        &tid,
        NULL,
        worker,
        &data
    );
    if(ret_create != 0){
        fprintf(stderr,"创建线程失败");
        return 1;
    }
    sleep(1);
    pthread_mutex_lock(&data.mutex);
    data.da = 200;
    data.ready = 1;
    pthread_cond_signal(&data.cond);
    pthread_mutex_unlock(&data.mutex);
    


    pthread_join(tid,NULL);
    pthread_mutex_destroy(&data.mutex);
    pthread_cond_destroy(&data.cond);



    return 0 ;
}