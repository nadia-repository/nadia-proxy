#include "thread.h"

void init_pthread_pool(TS *ts,unsigned int workers,unsigned int tasks, void *handler){
    ts->tasks = Calloc(tasks,sizeof(void *));
    ts->maxTaskNum = tasks;
    ts->front = ts->rear = 0;
    ts->workers = workers;
    #ifdef __APPLE__
        ts->mutex = sem_open("/mutex",O_CREAT, S_IRUSR | S_IWUSR, 1);
        ts->slots = sem_open("/slots",O_CREAT, S_IRUSR | S_IWUSR, tasks);
        ts->items = sem_open("/items",O_CREAT, S_IRUSR | S_IWUSR, 0);
    #else
        ts->mutex = (sem_t*)malloc(sizeof(sem_t));
        Sem_init(ts->mutex, 0, 1);
        ts->slots = (sem_t*)malloc(sizeof(sem_t));
        Sem_init(ts->slots, 0, tasks);
        ts->items = (sem_t*)malloc(sizeof(sem_t));
        Sem_init(ts->items, 0, 0);
    #endif

    pthread_t tid[workers];
    for(int i = 0;i < workers ;i++){
        int *pid = (int *)malloc(sizeof(int));
        *pid = i;
        Pthread_create(&tid[i], NULL, handler, (void *)pid);
    }
}

void put_pthread_item(TS *ts,void *item){
    P(ts->slots);
    P(ts->mutex);
    int index = (++ts->rear)%(ts->maxTaskNum);
    ts->tasks[index] = item;
    V(ts->mutex);
    V(ts->items);
}

void *get_pthread_item(TS *ts){
    void *item;
    P(ts->items);
    P(ts->mutex);
    int index = (++ts->front)%(ts->maxTaskNum);
    item = ts->tasks[index];
    V(ts->mutex);
    V(ts->slots);
    return item;
}

void destroy_pthoread_pool(TS *ts){
    Free(ts->tasks);
    Free(ts);
}