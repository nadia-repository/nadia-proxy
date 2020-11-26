#include "csapp.h"

typedef struct thread_struct{
    void **tasks;
    int maxTaskNum;
    int front;
    int rear;
    int workers;
    sem_t *mutex;
    sem_t *slots;
    sem_t *items;
} TS;


/*
基于生产者消费者模型初始化线程池，初始化workers数量的线程。
tasks为空置时，线程阻塞在tasks消费队列。
当tasks有值时，线程消费tasks任务并执行方法。
    workers 工作线程大小
    tasks 最大任务
    handler 线程执行方法
*/
void init_pthread_pool(TS *ts,unsigned int workers,unsigned int tasks, void *handler);

void put_pthread_item(TS *ts,void *item);

void *get_pthread_item(TS *ts);

void destroy_pthoread_pool(TS *ts);

