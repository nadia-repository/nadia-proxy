#include "thread.h"

void init_pthread_pool(THREAD_POOL *ts,unsigned int workers,unsigned int maxTaskNum, void *handler){
    ts->compartments = Calloc(maxTaskNum,sizeof(void *));
    ts->maxTaskNum = maxTaskNum;
    ts->front = ts->rear = 0;
    ts->workers = workers;
    #ifdef __APPLE__
        ts->mutex = sem_open("mutex",O_CREAT, S_IRUSR | S_IWUSR, 1);
        ts->slots = sem_open("slots",O_CREAT, S_IRUSR | S_IWUSR, maxTaskNum);
        ts->tasks = sem_open("items",O_CREAT, S_IRUSR | S_IWUSR, 0);
    #else
        ts->mutex = (sem_t*)malloc(sizeof(sem_t));
        Sem_init(ts->mutex, 0, 1);
        ts->slots = (sem_t*)malloc(sizeof(sem_t));
        Sem_init(ts->slots, 0, maxTaskNum);
        ts->tasks = (sem_t*)malloc(sizeof(sem_t));
        Sem_init(ts->tasks, 0, 0);
    #endif

    pthread_t tid[workers];
    for(int i = 0;i < workers ;i++){
        int *pid = (int *)malloc(sizeof(int));
        *pid = i;
        Pthread_create(&tid[i], NULL, handler, (void *)pid);
    }
}

void put_pthread_item(THREAD_POOL *ts,void *task){
    P(ts->slots);
    P(ts->mutex);
    int index = (++ts->rear)%(ts->maxTaskNum);
    ts->compartments[index] = task;
    V(ts->mutex);
    V(ts->tasks);
}

void *get_pthread_item(THREAD_POOL *ts){
    void *item;
    P(ts->tasks);
    P(ts->mutex);
    int index = (++ts->front)%(ts->maxTaskNum);
    item = ts->compartments[index];
    V(ts->mutex);
    V(ts->slots);
    return item;
}

void destroy_pthoread_pool(THREAD_POOL *ts){
    Free(ts->compartments);
    Free(ts);
}