#ifndef PUBLIC_H__
#define PUBLIC_H__

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUF 3

struct arg{
    int value;
    char message[64];
};



void task_double(void *arg);
void task_add100(void *arg);
void task_square(void *arg);

struct task
{
    int index;
    void (*func)(void *);
    void *arg;
};

struct public_data
{   
    struct task buffer[BUF];
    int write_pos;
    int read_pos;
    int count;
    int stop;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;  
};
//消费者
struct private_data {
    int id;
    struct public_data* pd;
};

//生产者
struct producer_arg
{
    int id;
    int start;
    int task_num;
    struct public_data *pd;
};

int task_queue_init(
   struct public_data* arg

);
int task_queue_push(
    struct public_data* arg,
    struct task task
);
int task_queue_pop(
   struct public_data *q,
    struct task *out

);
int task_queue_stop(
    struct public_data* arg
);

int task_queue_destroy(
    struct public_data* arg
);
#endif