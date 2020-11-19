#include "config.h"

int main(int argc, char **argv, char **envp){

    //加载配置文件
    char *configPath = "/nadia/proxy.conf";
    while (*envp) {
        char *env = *(envp++);

        // printf("env    %s\n",env);
        if(!strcasecmp(env, "NADIA_CONFIG")){
            configPath = env;
            break;
        }
    }
    //读取配置文件
    CS cs;
    if(loadConfig(configPath,&cs) < 1){
        fprintf(stderr, "nadia config file loade faile<%s> \n", configPath);
        exit(1);
    }


    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    fprintf(stderr, "start listen port<%d> \n", (*(cs.servers))->listen);
    

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

    freeConfig(&cs);
    return 0;
}