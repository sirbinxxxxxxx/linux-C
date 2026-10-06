#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
struct public_data {
    int count;
    pthread_mutex_t mutex0;
    pthread_mutex_t mutex1;    
    pthread_barrier_t barrier;
};

struct private_data
{
    int id;
    struct public_data* pd;
};

void* pworker( void* arg){

    struct private_data *p = arg;
    pthread_barrier_wait(&p->pd->barrier);

    if(p->id ==1 ){
    printf("新线程id : %d\n",p->id);
    printf("线程%d准备拿锁0\n",p->id);
    pthread_mutex_lock(&p->pd->mutex0);
    printf("线程%d拿到锁0\n",p->id);
    fflush(stdout);
    }else if(p->id == 2){
    printf("新线程id : %d\n",p->id);
    printf("线程%d准备拿锁0\n",p->id);
    pthread_mutex_lock(&p->pd->mutex0);
    printf("线程%d拿到锁0\n",p->id);
    }
    

    if(p->id == 1){
    printf("线程%d准备拿锁1\n",p->id);
    int  ret_1 = pthread_mutex_trylock(&p->pd->mutex1);
    if(ret_1 == EBUSY){
        printf("锁1正在被其他线程占用\n ");
        pthread_mutex_unlock(&p->pd->mutex0);
        return NULL;
    }else if(ret_1 != 0){
        fprintf(stderr,"trylock失败：%s",strerror(ret_1));
        pthread_mutex_unlock(&p->pd->mutex0);
        return NULL;
    }
    printf("线程%d拿到锁1\n",p->id);
    p->pd->count++;

    pthread_mutex_unlock(&p->pd->mutex1);
    pthread_mutex_unlock(&p->pd->mutex0);
    return NULL;
    }
    if(p->id == 2){
    printf("线程%d准备拿锁1\n",p->id);
  
    int  ret_2 = pthread_mutex_trylock(&p->pd->mutex1);
    if(ret_2 == EBUSY){
        printf("锁1正在被其他线程占用\n ");
        pthread_mutex_unlock(&p->pd->mutex0);
        return NULL;
    }else if(ret_2 != 0){
        fprintf(stderr,"trylock失败：%s",strerror(ret_2));
        pthread_mutex_unlock(&p->pd->mutex0);
        return NULL;
    }
    printf("线程%d拿到锁1\n",p->id);

    p->pd->count++;

    pthread_mutex_unlock(&p->pd->mutex1);
    pthread_mutex_unlock(&p->pd->mutex0);
    return NULL;
    }
   
  
}

int  main()
{
    pthread_t tid[2];

    struct public_data pb ;
    pb.count = 0;
    int ret_init0 = pthread_mutex_init(&pb.mutex0,NULL);
            if(ret_init0 !=0){
            fprintf(stderr,"锁初始化失败");
            return 1;
   }
  
    int ret_init1 = pthread_mutex_init(&pb.mutex1,NULL);
            if(ret_init1 !=0){
            fprintf(stderr,"锁初始化失败");
            pthread_mutex_destroy(&pb.mutex0);
            return 1;
   }
   int ret_barr = pthread_barrier_init(&pb.barrier,NULL,2);
   if(ret_barr !=0){
    fprintf(stderr,"初始化失败：%s",strerror(ret_barr));
    pthread_mutex_destroy(&pb.mutex0);
    pthread_mutex_destroy(&pb.mutex1);
    return 1;
   }


    struct private_data pa[2];
    int count_tid = 0;
    for(int i = 0; i < 2; i++){
        
        pa[i].pd =  &pb;
        pa[i].id = i+1;
        int ret = pthread_create(
            &tid[i],
            NULL,
            pworker,
            &pa[i]
        );
        if(ret != 0){
            fprintf(stderr,"创建线程失败");

            for(int i = 0; i < count_tid;i++){
                pthread_join(tid[i],NULL);
            }

            return 1;
        }
        count_tid ++ ;
    }


    for(int i = 0; i< 2 ; i++){
            pthread_join(tid[i],NULL);
    }
    printf("count = %d\n",pb.count);
    pthread_mutex_destroy(&pb.mutex0);
    pthread_mutex_destroy(&pb.mutex1);
    pthread_barrier_destroy(&pb.barrier);

    return 0;
}