#include "worker.h"

THREAD_POOL thread_pool_instance;
extern NADIA_CONFIG nadiaConfig;
HASHMAP *listenfd_map;

void reload_handler(int sig);
void stop_handler(int sig);
void *do_proxy(void *vargp);

void do_work(){
    #ifdef __linux__
        prctl(PR_SET_NAME, "nadia worker", NULL, NULL, NULL);
    #endif

    //处理来自主进程的信号
    Signal(SIGTSTP, reload_handler);
    Signal(SIGUSR1, stop_handler);
    Signal(SIGINT, SIG_IGN);

    int listenfd=0;
    int *lfd = NULL;
    fd_set read_set, ready_set;
    FD_ZERO(&read_set);
    
    ARRAYLIST *servers_list = nadiaConfig.nadia_proxy_config->http_config->servers_list;
    int server_size = ARRAYLIST_LENGTH(servers_list);
    
    //key---value  ----> listenfd --- server
    listenfd_map = INIT_HASHMAP;

    //记录当前所有的监听文件描述符
    ARRAYLIST * listenfd_list = ARRAYLIST_INIT_SIZE(server_size);

    for(int i = 0;i<server_size;i++){
        SERVERS_CONFIG *servers = ARRAYLIST_CAST(SERVERS_CONFIG *,servers_list,i);
        listenfd = Open_listenfd(servers->listen);
        fprintf(stderr, "start listen port<%s> \n", (ARRAYLIST_CAST(SERVERS_CONFIG *,servers_list,i))->listen);
        FD_SET(listenfd, &read_set);
        lfd = malloc(sizeof(int));
        *lfd = listenfd;
        ARRAYLIST_ADD(listenfd_list,lfd);
        //维护listenfd 和 server的映射关系
        PUT_HASHMAP(listenfd_map,listenfd,servers);
    }

    //初始化工作线程池，工作线程处理每个客户端发来的请求
    init_pthread_pool(&thread_pool_instance,10,server_size,do_proxy);

    while (1){
        ready_set = read_set;
        Select(listenfd+1, &ready_set, NULL, NULL, NULL);
        int select_lfd;
        //判断listenfd
        for(int i = 0;i<server_size;i++){
            select_lfd = *ARRAYLIST_CAST(int *,listenfd_list,i);
            if (FD_ISSET(select_lfd , &ready_set)){
                int *item = (int *)malloc(sizeof(int)); //开辟单独内存空间，防止并发覆盖
                *item = select_lfd;
                put_pthread_item(&thread_pool_instance,item);
            }
        }
    }
}

void *do_proxy(void *vargp){
    fprintf(stdout, "Start proxy worker(%d)\n",*((int *)vargp));
    pthread_detach(pthread_self());//分离自己，防止被其他线程中断
    while(1){
        int *lfp = (int *)get_pthread_item(&thread_pool_instance);
        fprintf(stdout, "====================get item=%d \n",*lfp);
        int connfd;
        char hostname[MAXLINE], port[MAXLINE];
        socklen_t clientlen;
        struct sockaddr_storage clientaddr;

        //根据listenfd获取代理信息
        SERVERS_CONFIG *server = (SERVERS_CONFIG *)(listenfd_map->get(listenfd_map,*lfp));
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

/*
主进程向工作进程发起重启信号，工作进程首先退出进程。主进程回收工作进程后重新fork工作线程。
*/
void reload_handler(int sig){
    fprintf(stdout, "Worker process get reload single!\n");
    exit(1);
}

/*
主进程向工作进程发起停止信号，工作进程首先退出进程。主进程回收工作进程后退出主进程。
*/
void stop_handler(int sig){
    fprintf(stdout, "Worker process get stop single!\n");
    exit(0);
}