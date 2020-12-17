#include "csapp.h"

/*
工作线程池对象
    compartments 工作隔间，存放每个task元素
    maxTaskNum 最大工作数
    front 第一个任务index
    rear 最后一个任务index
    workers  工作线程的数量
    mutex 生产消费者互斥量
    slots 工作隔间最大量(超多slots时挂起线程)
    tasks 当前任务量(小于tasks时挂起线程)
*/
typedef struct thread_struct{
    void **compartments;
    int maxTaskNum;
    int front;
    int rear;
    int workers;
    sem_t *mutex;
    sem_t *slots;
    sem_t *tasks;
} THREAD_POOL;

/*
基于生产者消费者模型初始化线程池，初始化workers数量的线程。
tasks为空置时，线程阻塞在tasks消费队列。
当tasks有值时，线程消费tasks任务并执行方法。
    ts 线程池对象
    workers 工作线程大小
    maxTaskNum 最大任务数
    handler 线程执行方法
*/
void init_pthread_pool(THREAD_POOL *ts,unsigned int workers,unsigned int maxTaskNum, void *handler);

/*
为线程池中线程生产工作
主线程需要调用该方法产生工作，超过最大工作上线时会挂起主线程
    ts 线程池对象
    task 工作
*/
void put_pthread_item(THREAD_POOL *ts,void *task);

/*
线程池中线程消费工作
线程执行方法中需要调用该方法获取task，没有task时挂起线程
    ts 线程池对象
*/
void *get_pthread_item(THREAD_POOL *ts);

/*
销毁线程池
    ts 线程池对象
*/
void destroy_pthoread_pool(THREAD_POOL *ts);

