#include "thread.h"

void *do_proxy(void *vargp);

THREAD_POOL ts;
int main(){

    init_pthread_pool(&ts,10,3,do_proxy);

    for(int i = 0;i<2;i++){
        fprintf(stdout, "**************put item %d\n",i);
        int * ip = (int *)malloc(sizeof(int));
        *ip = i;
        put_pthread_item(&ts,ip);
    }

    while(1){

    }

    return 0;
}

void *do_proxy(void *vargp){
    fprintf(stdout, "====================into do_proxy\n");
    // pthread_detach(pthread_self());//分离自己，防止被其他线程中断
    while(1){
        int *lfp = (int *)get_pthread_item(&ts);
        fprintf(stdout, "~~~~~~~~~~~~~~~~~~~~~~~get item=%d \n",*lfp);

    }
    return NULL;
}