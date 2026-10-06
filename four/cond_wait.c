
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
//定义公共数据 
struct public_data
{
    int count;
    pthread_mutex_t muxte;
    pthread_cond_t cond;
    int start;
};


void* worker(void* arg){
    printf("新线程开始运行\n");
    struct public_data* p = arg ;
    pthread_mutex_lock(&p->muxte);
    while(p->start == 0){
        printf("新线程开始等待\n");
        int ret = pthread_cond_wait(&p->cond,&p->muxte);
        if(ret != 0){
            fprintf(stderr,"pthread_cond_wait失败：%s",strerror(ret));
            pthread_mutex_unlock(&p->muxte);
            return NULL;
        }
    }
    
    p->count++;
    printf("count:%d\n",p->count);
    pthread_mutex_unlock(&p->muxte);
    return NULL ;

}

int main()
{
    //初始化公共数据
    struct public_data data;
    data.count =0;
    pthread_mutex_init(&data.muxte,NULL);
    pthread_cond_init(&data.cond,NULL);
    data.start = 0;

    //创建3个新线程
    pthread_t tid[3];
    for(int i = 0; i < 3;i++){
        int ret = pthread_create(
            &tid[i],
            NULL,
            worker,
            &data
        );
        if(ret != 0){
            fprintf(stderr,"pthread_create失败：%s",strerror(ret));
            return 1;
        }
    }

    sleep(2); //主线程先 睡2s让子线程等待

    pthread_mutex_lock(&data.muxte);
    data.count =100;
    data.start = 1;
    pthread_cond_broadcast(&data.cond);
    pthread_mutex_unlock(&data.muxte);

    for(int i = 0; i < 3 ; i++){
        pthread_join(tid[i],NULL);
    }
   
    pthread_mutex_destroy(&data.muxte);
    pthread_cond_destroy(&data.cond);



    return 0;
}