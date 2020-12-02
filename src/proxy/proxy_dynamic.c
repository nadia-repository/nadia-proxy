#include "proxy_dynamic.h"

static void read_requesthdrs(rio_t *rp);
static void request_dynamic(char *host, char *port);
static void select_server(char *host, char *port, DPS *dps);

static void do_round_robin(char *host, char *port, char **server);
static void do_weighted_round_robin(char *host, char *port, char **server);
static void do_random(char *host, char *port, char **server);
static void do_hash(char *host, char *port, char **server);


void serve_dynamic(SDI *sdi){
    char *host, *port;
    
    select_server(host,port,sdi->dps);
    
    request_dynamic(host,port);
}

static void select_server(char *host, char *port, DPS *dps){
    switch (dps->proxyStrategy){
        case(ROUND_ROBIN):
            do_round_robin(host,port,dps->server);
            break;
        case(WEIGHTED_ROUND_ROBIN):
            do_weighted_round_robin(host,port,dps->server);
            break;
        case(RANDOM):
            do_random(host,port,dps->server);
            break;
        case(HASH):
            do_hash(host,port,dps->server);
            break;
        default:
            do_round_robin(host,port,dps->server);
    }
}

static void do_round_robin(char *host, char *port, char **server){

}

static void do_weighted_round_robin(char *host, char *port, char **server){

}

static void do_random(char *host, char *port, char **server){

}

static void do_hash(char *host, char *port, char **server){

}


static void read_requesthdrs(rio_t *rp) {
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

static void request_dynamic(char *host, char *port){
    rio_t rio;
    int clientfd;
    char buf[MAXLINE];

    clientfd = Open_clientfd(host, port);

    Rio_writen(clientfd, buf, strlen(buf));

    Close(clientfd);
}