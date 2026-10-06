#ifndef THREAD_POOL_H__
#define THREAD_POOL_H__

#include "task_queue.h" 
#define MAX_WORKERS 8

struct thread_pool 
{
    //保存每个工作线程的tid
    pthread_t threads[MAX_WORKERS];
    //每个worker_handler 自己的参数
    struct private_data worker[MAX_WORKERS];
    //实际创建了多少线程
    int thread_num;
    //所有线程共享的任务队列
    struct public_data queue;
};

/*初始化线程*/
int thread_pool_init(
    struct thread_pool *pool,
    int thread_num
);

//提交任务

int thread_pool_submit(
    struct thread_pool *pool,
    struct task task
);

//停止线程池 ，并等待所有worker_handler退出 
int thread_pool_shutdown(
    struct thread_pool *pool
);

//销毁线程池资源
int thread_pool_destroy(
    struct thread_pool *pool
);

#endif