#include "public.h"


void task_double(void* arg)
{
    struct arg* p = arg;
    printf(
        "double任务：%d * 2 = %d\n",
        p->value,
        p->value * 2
    );
}
void task_add100(void* arg)
{
    struct arg* p = arg;
    printf(
        "add100任务：%d + 100 = %d\n",
         p->value,
         p->value + 100
    );
}
void task_square(void* arg)
{
    struct arg* p = arg;
    printf(
        "square任务：%d * %d = %d\n",
         p->value,
         p->value,
         p->value *  p->value
    );
    
}