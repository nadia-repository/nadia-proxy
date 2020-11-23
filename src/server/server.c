#include "config.h"

CS cs;
void signal_handler(int sig);
void pipesig_handler(int sig);
void echo(int connfd);
void parser_server(int connfd);
void *proxy(void *vargp);
void read_requesthdrs(rio_t *rp);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg);

int main(int argc, char **argv, char **envp){

    //nadia默认路径
    char *nadiaPath = "/Users/xiangshi/Documents/workspace_c/nadia-proxy/";
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
        fprintf(stderr, "Nadia proxy file load faile<%s> \n", cp.proxyPath);
        exit(1);
    }
    //读取日志配置
    // if(loadLog(cp.logPath)<1){
    //     fprintf(stderr, "Nadia log file redirect faile<%s> \n", cp.logPath);
    //     exit(1);
    // }
    //读取配置信息配置
    if(loadConfigs(cp.configsPath)<1){
        fprintf(stderr, "Nadia config file load faile<%s> \n", cp.configsPath);
        exit(1);
    }

    //注册信号事件
    Signal(SIGINT, signal_handler);   /* ctrl-c */
    Signal(SIGCHLD, signal_handler); 
    Signal(SIGTSTP, signal_handler); /* ctrl-z */
    Signal(SIGPIPE, SIG_IGN);

    int listenfd;
    fd_set read_set, ready_set;
    FD_ZERO(&read_set);

    __int16_t serverSize = cs.serverSize;
    SS ** servers = cs.servers;
    
    int *lfd = (int*)calloc(serverSize,sizeof(int)); //记录当前所有的监听文件描述符
    for(int i = 0;i<serverSize;i++,servers++){
        listenfd = Open_listenfd((*servers)->listen);
        fprintf(stderr, "start listen port<%s> \n", (*servers)->listen);
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
                pthread_t tid[10];//todo需要实现线程池
                Pthread_create(&tid[i], NULL, proxy, &lfd[i]);
            }
        }
    }
    fprintf(stderr, "main process stop nadia server\n");
    return 0;
}

void signal_handler(int sig) {
    fprintf(stderr, "stop nadia server\n");
    freeProxy(&cs);
    exit(0);
}

void pipesig_handler(int sig){
    fprintf(stderr, "pipesig_handler in\n");
}

void echo(int connfd) {
    int n; 
    char buf[MAXLINE]; 
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        printf("server received %d bytes\n", n);
        printf("server received          %s",buf);
        Rio_writen(connfd, buf, n);
    }
}

void *proxy(void *vargp){
    pthread_detach(pthread_self());//分离自己，防止被其他线程中断
    int connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    int *lfp = (int*)vargp;
    clientlen = sizeof(struct sockaddr_storage); 
    connfd = Accept(*lfp, (SA *)&clientaddr, &clientlen);
    Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE,
        port, MAXLINE, 0);
    printf("Accepted connection from (%s, %s)\n", hostname, port);
    parser_server(connfd);
    Close(connfd);
    return NULL;
}

void parser_server(int connfd){
    rio_t rio;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    Rio_readinitb(&rio, connfd);
    if (!Rio_readlineb(&rio, buf, MAXLINE)){
        return;
    }
    sscanf(buf, "%s %s %s", method, uri, version);
    fprintf(stderr, "Server received method:%s uri:%s version:%s\n",method,uri,version);

    read_requesthdrs(&rio); 

    char filename[MAXLINE];
    sprintf(filename,"/Users/xiangshi/Documents/workspace_c/nadia-proxy/");
    sprintf(filename,"home.html");
    if (stat(filename, &sbuf) < 0) {                     //line:netp:doit:beginnotfound
        clienterror(connfd, filename, "404", "Not found",
                    "Nadia couldn't find this file");
        return;
    }  
    serve_static(connfd,filename,sbuf.st_size);



}

void serve_static(int fd, char *filename, int filesize) {
    int srcfd;
    char *srcp, filetype[MAXLINE], buf[MAXBUF];

    get_filetype(filename, filetype);    
    sprintf(buf, "HTTP/1.0 200 OK\r\n"); 
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server: Tiny Web Server\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n", filesize);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: %s\r\n\r\n", filetype);
    Rio_writen(fd, buf, strlen(buf));    

    srcfd = Open(filename, O_RDONLY, 0); 
    srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0); 
    Close(srcfd);                       
    Rio_writen(fd, srcp, filesize);     
    Munmap(srcp, filesize);             
}

void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg) {
    char buf[MAXLINE];

    /* Print the HTTP response headers */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n\r\n");
    Rio_writen(fd, buf, strlen(buf));

    /* Print the HTTP response body */
    sprintf(buf, "<html><title>Tiny Error</title>");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<body bgcolor=""ffffff"">\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<hr><em>The Tiny Web server</em>\r\n");
    Rio_writen(fd, buf, strlen(buf));
}

void get_filetype(char *filename, char *filetype) {
    if (strstr(filename, ".html"))
        strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif"))
        strcpy(filetype, "image/gif");
    else if (strstr(filename, ".png"))
        strcpy(filetype, "image/png");
    else if (strstr(filename, ".jpg"))
        strcpy(filetype, "image/jpeg");
    else
        strcpy(filetype, "text/plain");
}

void read_requesthdrs(rio_t *rp) {
    char buf[MAXLINE];
    //需要解析length等信息，并且需要缓存
    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
    while (strcmp(buf, "\r\n")) {          //line:netp:readhdrs:checkterm
        Rio_readlineb(rp, buf, MAXLINE);
        printf("%s", buf);
    }
    return;
}