#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>



//共享数据 
struct data
{
    int count;
};

//包含独享数据

struct shared_data
{
    int id;
    struct data * da;
};

void* worker(void* arg)
{
    struct shared_data * p =arg;
  
    printf("我是新线程%d\n",p->id + 1);
    for(int i = 0 ;i <100 ;i++){
     (p->da->count)++;
     
    }
  
    return NULL;
}


int main(){

    pthread_t tid[10000];
    struct data da;
    da.count = 0;
    struct shared_data ags[10000];
    memset(&ags,0,sizeof(ags));
   

    for(int i = 0; i< 10000;i++){
        int ret[10000];
        ags[i].da = &da; 
        ags[i].id=i;
        ret[i] = pthread_create(
            &tid[i],
            NULL,
            worker,
            &ags[i]
        );
        if(ret[i] !=0){
            fprintf(stderr,"新进程%d创建失败",i+1);
            return 1;
        }
    }

     for(int i = 0;i<10000 ;i++){
        pthread_join(tid[i],NULL);
    }
   
    printf("count:%d\n",ags->da->count);
    printf("主线程结束\n");
    return 0;
}