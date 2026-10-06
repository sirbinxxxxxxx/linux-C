#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


void* worker(void* arg)
{
    int *p = arg ;
    printf("我是新线程%d\n",*p );
    return NULL;
}

int main()
{
    
    pthread_t tid[2];
    int ret[2];

    int * p = malloc(sizeof(int)*2);
    *p = 1;
    *(p+1) = 2;

    for(int i = 0; i<2 ;i++){

        ret[i] =  pthread_create (
                &tid[i],
                NULL,
                worker,
                p+i
    );

        if(ret[i]!=0){

            fprintf(stderr,"新线程%d创建失败\n",i);
            free(p);
            return 1;
        }
    }
     

    for(int i = 0;i<2 ;i++){
        pthread_join(tid[i],NULL);
    }
   
    printf("主线程结束\n");

    free(p);
    return 0;
}