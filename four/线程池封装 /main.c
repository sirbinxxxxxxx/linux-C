#include "thread_pool.h"

int main(){

    struct thread_pool pool;

    //1.初始化线程池，创建3个worker
    int ret = thread_pool_init(
        &pool,
        3
    );
    if(ret != 0){
        fprintf(stderr,"thread_loop_init 失败");
        return 1;
    }

    //2.创建20个任务并提交
    for(int i =0 ;i < 20 ;i++){
         struct task task;
         memset(&task,0,sizeof(task));

        struct arg  *ag = malloc(sizeof(*ag));
        if(ag ==NULL){
            perror("malloc");
            break;
        }
        //任务编号
        task.index = i+1;
        //参数内容
        ag->value = (i+1)*100;
        snprintf(
            ag->message,
            sizeof(ag->message),
            "这是任务%d",
            task.index
        );
        task.arg = ag;

        /*
         * 给不同任务绑定不同函数
         */

          if(i % 3 == 0)
        {
            task.func = task_add100;
        }
        else if(i % 3 == 1)
        {
            task.func = task_double;
        }
        else
        {
            task.func = task_square;
        }

        //把任务和pool交给线程池
        ret = thread_pool_submit(
            &pool,
            task
        );
        if(ret != 0)
        {
            fprintf(
                stderr,
                "任务%d提交失败\n",
                task.index
            );
             free(ag);

            break;
        }
          printf(
            "main提交任务%d\n",
            task.index);

    }
/*
     * 4. 不再提交任务
     *
     * 通知worker：
     * 把剩余任务处理完，然后退出
     */

     ret =thread_pool_shutdown(&pool);
     if(ret!=0){
        fprintf(
            stderr,
            "thread_pool_shutdown 失败\n"
        );
     }

       /*
     * 5. 销毁mutex、cond等资源
     */

       ret = thread_pool_destroy(
        &pool
    );

    if(ret != 0)
    {
        fprintf(
            stderr,
            "thread_pool_destroy失败\n"
        );

        return 1;
    }



    return 0;
}