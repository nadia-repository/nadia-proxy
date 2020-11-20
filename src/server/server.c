#include "config.h"

CS cs;
void signal_handler(int sig);
void echo(int connfd);

int main(int argc, char **argv, char **envp){

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
    initFilePath(configPath,&cp);

    //读取代理配置文件
    if(loadProxy(cp.proxyPath,&cs) < 1){
        fprintf(stderr, "nadia config file loade faile<%s> \n", cp.proxyPath);
        exit(1);
    }
    //读取日志配置
    if(loadLog(cp.logPath)<1){
        fprintf(stderr, "nadia log file redirect faile<%s> \n", cp.logPath);
        exit(1);
    }
    //读取配置信息配置



    //注册信号事件
    Signal(SIGINT, signal_handler);   /* ctrl-c */
    Signal(SIGCHLD, signal_handler); 
    Signal(SIGTSTP, signal_handler); /* ctrl-z */

    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    fd_set read_set, ready_set;
    FD_ZERO(&read_set);

    __int16_t serverSize = cs.serverSize;
    SS ** servers = cs.servers;
    
    int *lfd = (int*)calloc(serverSize,sizeof(int)); //记录当前所有的监听文件描述符
    for(int i = 0;i<serverSize;i++,servers++){
        listenfd = Open_listenfd((*servers)->listen);
        fprintf(stderr, "start listen port<%s> \n", (*(cs.servers))->listen);
        FD_SET(listenfd, &read_set);
        lfd[i] = listenfd;
    }

    while (1){
        ready_set = read_set;
        Select(listenfd+1, &ready_set, NULL, NULL, NULL);
        //todo 判断listenfd
        for(int i = 0;i<serverSize;i++){
            if (FD_ISSET(lfd[i] , &ready_set)){
                //todo 根据lfd[i] listenfd 获取server信息
                clientlen = sizeof(struct sockaddr_storage); 
                connfd = Accept(lfd[i], (SA *)&clientaddr, &clientlen);
                Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE,
                    port, MAXLINE, 0);
                printf("Accepted connection from (%s, %s)\n", hostname, port);
                echo(connfd);
                Close(connfd);
            }
        }
    }
    

    // listenfd = Open_listenfd(argv[1]);
    // while (1) {
    //     clientlen = sizeof(clientaddr);
    //     connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen); //line:netp:tiny:accept
    //     Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE,
    //                 port, MAXLINE, 0);
    //     printf("Accepted connection from (%s, %s)\n", hostname, port);
    //     doit(connfd);                                             //line:netp:tiny:doit
    //     Close(connfd);                                            //line:netp:tiny:close
    // }
    return 0;
}

void signal_handler(int sig) {
    fprintf(stderr, "stop nadia server\n");
    freeProxy(&cs);
    exit(0);
}

void echo(int connfd) {
    int n; 
    char buf[MAXLINE]; 
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        printf("server received %d bytes\n", n);
        Rio_writen(connfd, buf, n);
    }
}