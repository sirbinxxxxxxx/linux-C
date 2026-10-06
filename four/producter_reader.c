#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUF 3
struct public_data
{   
    int buffer[BUF];
    int write_pos;
    int read_pos;
    int count;
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    
};

void* worker(void *arg){
    struct public_data* p = arg;
    int total = 0;
    while(total<20 ){
    
    pthread_mutex_lock(&p->mutex);
        while(p->count  == 0 ){
            printf("缓冲区没有数据读端进入等待...\n");
            pthread_cond_wait(&p->not_empty,&p->mutex);      
        }
    /* 先读取当前槽 */
    int value = p->buffer[p->read_pos % BUF];
     /* 然后移动读指针 */
    p->read_pos =(p->read_pos + 1) % BUF;
    
    p->count--;
    total++;
    pthread_cond_signal(&p->not_full);
    pthread_mutex_unlock(&p->mutex);

    printf("处理任务%d条数据value=%d\n",total,value);
    sleep(1);
    }

    return NULL;
}

int main(){

    //数据初始化
    struct public_data  pd;
    memset(&pd,0,sizeof(pd));
    pthread_mutex_init(&pd.mutex,NULL);
    pthread_cond_init(&pd.not_empty,NULL);
    pthread_cond_init(&pd.not_full,NULL);
  

    pthread_t tid;
    int ret_t = pthread_create(
        &tid,
        NULL,
        worker,
        &pd
    ); 
    if(ret_t != 0){
        fprintf(stderr,"pthread_create失败：%s/n",strerror(ret_t));
        return 1;
    }

    //主线程来当生产者
    int total = 0;
    for(int i = 0 ; i < 20 ; i++){
        pthread_mutex_lock(&pd.mutex);
        while(pd.count == BUF){

            printf("缓冲区满，producer等待..\n");
            pthread_cond_wait(&pd.not_full,&pd.mutex);

        }
        pd.value = (i+1)*100; 
        // int value =  pd.value ;   
        pd.buffer[pd.write_pos%BUF] = pd.value;
        pd.write_pos = (pd.write_pos+1)%BUF;
      
        pd.count++;
        total++;
        pthread_cond_signal(&pd.not_empty);
        pthread_mutex_unlock(&pd.mutex);
        printf("主线程派发%d条任务 value=%d\n",total,pd.value);
    }
    pthread_join(tid,NULL);
    pthread_mutex_destroy(&pd.mutex);
    pthread_cond_destroy(&pd.not_empty);
    pthread_cond_destroy(&pd.not_full);

    return 0;
}