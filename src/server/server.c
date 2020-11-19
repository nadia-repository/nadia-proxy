#include "config.h"

int main(int argc, char **argv, char **envp){

    //加载配置文件
    char *configPath = "/nadia/proxy.conf";
    while (*envp) {
        char *env = *(envp++);

        printf("env=%s\n",env);
        if(strcasecmp(env, "nadia_config")){
            configPath = env;
            break;
        }
    }

    CS *cs = loadConfig(configPath);




    // int listenfd, connfd;
    // char hostname[MAXLINE], port[MAXLINE];
    // socklen_t clientlen;
    // struct sockaddr_storage clientaddr;



    // /* Check command line args */
    // if (argc != 2) {
    //     fprintf(stderr, "usage: %s <port>\n", argv[0]);
    //     exit(1);
    // }

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