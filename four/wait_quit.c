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
    int stop;
    int task_count;
};

struct  SHARE
{
    struct public_data * data;
    int id ;
};


void* worker(void* arg){
    
    struct  SHARE* p = arg ;

    while(1){
    pthread_mutex_lock(&p->data->muxte);

    while(p->data->stop == 0 && p->data->task_count == 0){
        printf("新线程开始等待\n");
        int ret = pthread_cond_wait(&p->data->cond,&p->data->muxte);
        if(ret != 0){
            fprintf(stderr,"pthread_cond_wait失败：%s\n",strerror(ret));
            pthread_mutex_unlock(&p->data->muxte);
            return NULL;
        }
        }

        if(p->data->stop == 1 && p->data->task_count == 0)
        {
            pthread_mutex_unlock(&p->data->muxte);
            printf("线程退出\n");
            break;
        }
        //子线程领取任务 
        p->data->task_count--; 
        printf("线程%d领取一个人任务\n",p->id);
        pthread_mutex_unlock(&p->data->muxte);
        /* 锁外处理任务 */
        sleep(1);
        printf("线程%d处理一个任务\n", p->id);

        //更新完成数量
        pthread_mutex_lock(&p->data->muxte);
        p->data->count++;
        int tmp = p->data->count ;  
        pthread_mutex_unlock(&p->data->muxte);    
        printf("共处理任务数count:%d\n",p->data->count);
}

return NULL ;
}



int main()
{   
    //初始化公共数据
    struct public_data data;
    data.count =0;
    pthread_mutex_init(&data.muxte,NULL);
    pthread_cond_init(&data.cond,NULL);
    data.stop = 0;
    data.task_count =0;

    struct  SHARE  share_data[3] ;
    
    //创建3个新线程
    pthread_t tid[3];
    for(int i = 0; i < 3;i++){
        share_data[i].data = &data ;
        share_data[i].id = i+1;
        
        int ret = pthread_create(
            &tid[i],
            NULL,
            worker,
            &share_data[i]
        );
        if(ret != 0){
            fprintf(stderr,"pthread_create失败：%s",strerror(ret));
            return 1;
        }
    }

    //主线程发放任务
    for(int i = 0;i < 9; i++){
         pthread_mutex_lock(&data.muxte) ;
         data.task_count++;
        printf("主线程投递任务%d,当前任务数=%d\n",i+1,data.task_count);
        pthread_cond_signal(&data.cond);
        pthread_mutex_unlock(&data.muxte);
        sleep(1);
    }
    
        
    sleep(5);
        pthread_mutex_lock(&data.muxte);
        data.stop = 1 ;
        pthread_cond_broadcast(&data.cond);        
        pthread_mutex_unlock(&data.muxte);
     

    //清理 
    for(int i = 0; i < 3 ; i++){
        pthread_join(tid[i],NULL);
    }
    pthread_mutex_destroy(&data.muxte);
    pthread_cond_destroy(&data.cond);
    return 0;
}