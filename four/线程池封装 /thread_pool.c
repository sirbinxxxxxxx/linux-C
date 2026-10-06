
#include "thread_pool.h"

static void *worker_handler(void *arg)
{
struct private_data* p =arg;
    while(1){
    struct task task;
    int ret_p = task_queue_pop(p->pd,&task);
        if(ret_p == 0){
           break;
        }else if(ret_p<0){
            fprintf(stderr,"task_queue_pop 失败\n");
            break;
        }
    printf("线程%d执行任务%d\n",p->id,task.index);
    if(task.func!=NULL){
        task.func(task.arg);
    }
    free(task.arg);
    }
    return NULL;

}



//初始化线程池
int thread_pool_init(
    struct thread_pool *pool,
    int thread_num  
)
{
    //1.判断pool是否为空
    if(pool==NULL){
        return -1 ;
    }
    //2.thread_num是否合法
    if(thread_num < 1 || thread_num >MAX_WORKERS){
        fprintf(stderr,"线程池数范围：1~%d\n",MAX_WORKERS);
        return -1 ;
    }
    //3.先清空线程池
    memset(pool,0,sizeof(*pool));

    //4.保存线程池数量
    pool->thread_num = thread_num ;

    //5.初始化任务队列
    int ret_init = task_queue_init(&pool->queue);
    if(ret_init != 0){
        fprintf(stderr,"task_queue_init失败\n");
        return -1;
    }

    //在这里创建worker_handler


    //6.循环创建 thread_num 个worker
    for(int i  = 0; i <thread_num ; i++ ){
        pool->worker[i].id = i+1;
        pool->worker[i].pd= &pool->queue;
        int ret_ph = pthread_create(
            &pool->threads[i],
            NULL,
            worker_handler,
            &pool->worker[i]
        );
        if(ret_ph != 0 ){
            fprintf(stderr,"pthread_create 失败\n");
            return -1;
        }
    }

    return 0;
}


int thread_pool_submit(
    struct thread_pool *pool,
    struct task task
)
{
    // 1. pool为空怎么办？
    if(pool==NULL){
        return -1;
    }

    // 2. 线程池本身不要直接操作
    //    buffer / count / write_pos
    //
    // 应该调用已有的哪个函数？
    int ret_s = task_queue_push(&pool->queue,task);
    if(ret_s != 0){
        fprintf(stderr,"task_queue_stop 失败\n");
    }


    // 3. 把那个函数的返回值返回出去
    return ret_s;
}



//停止线程池
int thread_pool_shutdown(
struct thread_pool *pool
)
{
    //1.检查pool
    if(pool == NULL){
        return -1;
    }
    //2.通知队列不会再有任务了
    int ret_s = task_queue_stop(
        &pool->queue
    );
    if(ret_s != 0){
        fprintf(stderr,"task_queue_stop 失败\n");
        return -1;
        
    }

    //3.等待全部worker线程退出
    for(int i = 0; i < pool->thread_num;i++){
        pthread_join(pool->threads[i],NULL);
    }
    return 0;

}


int thread_pool_destroy(
    struct thread_pool *pool
)
{
    //1.判断pool
    if(pool == NULL){
        return -1;
        }

    //2.mutex和cond现在属于谁？
    
    int ret_d = task_queue_destroy(&pool->queue);
    if(ret_d != 0){
        return -1;
    }
    return 0;
}