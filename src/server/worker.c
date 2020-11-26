#include "proxy.h"
#include "hashmap.h"
#include "thread.h"

TS ts;
extern CP cp;
extern CS cs;
void *do_proxy(void *vargp);

void do_work(){
    int listenfd=0;
    fd_set read_set, ready_set;
    FD_ZERO(&read_set);

    __int16_t serverSize = cs.serverSize;
    SS ** servers = cs.servers;
    
    //key---value  ----> listenfd --- server
    MAP_INSTANCE *lfdMap = init_hashmap(0);

    //记录当前所有的监听文件描述符
    int *lfd = (int*)calloc(serverSize,sizeof(int)); 

    for(int i = 0;i<serverSize;i++,servers++){
        listenfd = Open_listenfd((*servers)->listen);
        fprintf(stderr, "start listen port<%s> \n", (*servers)->listen);
        FD_SET(listenfd, &read_set);
        lfd[i] = listenfd;
        //维护listenfd 和 server的映射关系
        lfdMap->put(lfdMap,listenfd,*servers);
    }

    //初始化工作线程池，工作线程处理每个客户端发来的请求
    init_pthread_pool(&ts,10,serverSize,do_proxy);

    while (1){
        ready_set = read_set;
        Select(listenfd+1, &ready_set, NULL, NULL, NULL);
         
        //todo 判断listenfd
        for(int i = 0;i<serverSize;i++){
            if (FD_ISSET(lfd[i] , &ready_set)){
                int *item = (int *)malloc(sizeof(int)); //开辟单独内存空间，防止并发覆盖
                *item = lfd[i];
                put_pthread_item(&ts,item);
            }
        }
    }

}

void *do_proxy(void *vargp){
    fprintf(stdout, "Start proxy worker(%d)\n",*((int *)vargp));
    pthread_detach(pthread_self());//分离自己，防止被其他线程中断
    while(1){
        int *lfp = (int *)get_pthread_item(&ts);
        fprintf(stdout, "====================get item=%d \n",*lfp);
        int connfd;
        char hostname[MAXLINE], port[MAXLINE];
        socklen_t clientlen;
        struct sockaddr_storage clientaddr;

        //根据listenfd获取代理信息
        SS *server = (SS *)(lfdMap->get(lfdMap,*lfp));
        clientlen = sizeof(struct sockaddr_storage); 
        connfd = Accept(*lfp, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE,
            port, MAXLINE, 0);
        printf("Accepted connection from (%s, %s), listen port<%s>\n", hostname, port,server->listen);
        parser_request(connfd,server);
        Close(connfd);
    }
    return NULL;
}