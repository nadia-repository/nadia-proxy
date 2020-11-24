#include "proxy.h"
#include "hashmap.h"

#define DEBUG_LOG 0

CS cs;
MAP_INSTANCE *lfdMap;
void signal_handler(int sig);
void *do_proxy(void *vargp);

int main(int argc, char **argv, char **envp){

    //nadia默认路径
    char *nadiaPath = "/Users/xiangshi/Documents/workspace_c/nadia-proxy/";
    fprintf(stderr, "Wellcome Naida Proxy \n");
    fprintf(stderr, "Naida config path<%s> \n",nadiaPath);

    //加载配置文件
    char *configPath = "/nadia/config/";
    while (*envp) {
        char *env = *(envp++);
        //从环境变量中获取配置目录
        if(!strcasecmp(env, "NADIA_CONFIG")){
            configPath = env;
            break;
        }
    }
    //初始化配置文件路径
    CP cp;
    init_file_path(configPath,&cp);

    //读取代理配置文件
    if(load_proxy(cp.proxyPath,&cs) < 1){
        fprintf(stderr, "Nadia proxy file load faile<%s> \n", cp.proxyPath);
        exit(1);
    }
    //读取日志配置 todo
    // if(load_log(cp.logPath)<1){
    //     fprintf(stderr, "Nadia log file redirect faile<%s> \n", cp.logPath);
    //     exit(1);
    // }
    //读取配置信息配置
    if(load_configs(cp.configsPath)<1){
        fprintf(stderr, "Nadia config file load faile<%s> \n", cp.configsPath);
        exit(1);
    }

    //注册信号事件
    Signal(SIGINT, signal_handler);   /* ctrl-c */
    Signal(SIGCHLD, signal_handler); 
    Signal(SIGTSTP, signal_handler); /* ctrl-z */
    Signal(SIGPIPE, SIG_IGN);//屏蔽SIGPIPE信号，如果服务端向一个已经关闭的客户端发送两次数据，服务端将会受到SIGPIPE信号并终止服务端进程

    int listenfd=0;
    fd_set read_set, ready_set;
    FD_ZERO(&read_set);

    __int16_t serverSize = cs.serverSize;
    SS ** servers = cs.servers;
    
    //key---value  ----> listenfd --- server
    lfdMap = init_hashmap(0);

    int *lfd = (int*)calloc(serverSize,sizeof(int)); //记录当前所有的监听文件描述符
    for(int i = 0;i<serverSize;i++,servers++){
        listenfd = Open_listenfd((*servers)->listen);
        fprintf(stderr, "start listen port<%s> \n", (*servers)->listen);
        FD_SET(listenfd, &read_set);
        lfd[i] = listenfd;
        //维护listenfd 和 server的映射关系
        lfdMap->put(lfdMap,listenfd,*servers);
    }

    while (1){
        ready_set = read_set;
        Select(listenfd+1, &ready_set, NULL, NULL, NULL);
         
        //todo 判断listenfd
        for(int i = 0;i<serverSize;i++){
            if (FD_ISSET(lfd[i] , &ready_set)){
                //todo 根据lfd[i] listenfd 获取server信息
                pthread_t tid[10];//todo需要实现线程池
                Pthread_create(&tid[i], NULL, do_proxy, &lfd[i]);
            }
        }
    }
    fprintf(stderr, "main process stop nadia server\n");
    return 0;
}

void *do_proxy(void *vargp){
    pthread_detach(pthread_self());//分离自己，防止被其他线程中断
    int connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    //根据listenfd获取代理信息
    int *lfp = (int*)vargp;
    SS *server = (SS *)(lfdMap->get(lfdMap,*lfp));

    clientlen = sizeof(struct sockaddr_storage); 
    connfd = Accept(*lfp, (SA *)&clientaddr, &clientlen);
    Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE,
        port, MAXLINE, 0);
    printf("Accepted connection from (%s, %s)\n", hostname, port);
    parser_request(connfd,server);
    Close(connfd);
    return NULL;
}

/*
信号处理方法，回收内存并退出进程
*/
void signal_handler(int sig) {
    fprintf(stderr, "stop nadia server\n");
    free_proxy(&cs);
    exit(0);
}