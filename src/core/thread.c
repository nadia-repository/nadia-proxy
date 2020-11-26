#include "thread.h"

void init_pthread_pool(TS *ts,unsigned int workers,unsigned int tasks, void *handler){
    ts->tasks = Calloc(tasks,sizeof(void *));
    ts->maxTaskNum = tasks;
    ts->front = ts->rear = 0;
    ts->workers = workers;
    Sem_init(&(ts->mutex), 0, 1);
    Sem_init(&(ts->slots), 0, tasks);
    Sem_init(&(ts->items), 0, 0);

    pthread_t tid[workers];
    for(int i = 0;i < workers ;i++){
        Pthread_create(&tid[i], NULL, handler, NULL);
    }
}

void put_pthread_item(TS *ts,void *item){
    P(&ts->slots);
    P(&ts->mutex);
    ts->tasks[(++ts->rear)%(ts->maxTaskNum)] = item;
    V(&ts->mutex);
    V(&ts->items);
}

void *get_pthread_item(TS *ts){
    void *item;
    P(&ts->items);
    P(&ts->mutex);
    item = ts->tasks[(++ts->front)%(ts->maxTaskNum)];
    V(&ts->mutex);
    V(&ts->slots);
    return item;
}

void destroy_pthoread_pool(TS *ts){
    Free(ts->tasks);
    Free(ts);
}