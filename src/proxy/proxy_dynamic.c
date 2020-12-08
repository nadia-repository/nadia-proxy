#include "proxy_dynamic.h"

static void request_dynamic(char *host, char *port, SDI *sdi);
static void select_server(char *host, char *port, SERVER_PROXY *server_proxy);

static void do_round_robin(char *host, char *port, SERVER_PROXY *server_proxy);
static void do_weighted_round_robin(char *host, char *port, SERVER_PROXY *server_proxy);
static void do_ip_hash(char *host, char *port, SERVER_PROXY *server_proxy);

void serve_dynamic(SDI *sdi){
    char host[MAXLINE], port[MAXLINE];
    select_server(host,port,sdi->server_proxy);
    request_dynamic(host,port,sdi);
}

static void select_server(char *host, char *port, SERVER_PROXY *server_proxy){
    switch (server_proxy->proxyStrategy){
        case(ROUND_ROBIN):
            do_round_robin(host,port,server_proxy);
            break;
        case(WEIGHTED_ROUND_ROBIN):
            do_weighted_round_robin(host,port,server_proxy);
            break;
        case(IP_HASH):
            do_ip_hash(host,port,server_proxy);
            break;
        default:
            do_round_robin(host,port,server_proxy);
    }
}

static void do_round_robin(char *host, char *port, SERVER_PROXY *server_proxy){
    int index;
    index = round_robin(&(server_proxy->request_count),ARRAYLIST_LENGTH(server_proxy->reverse_proxys_list));
    REVERSE_PROXY *info = ARRAYLIST_CAST(REVERSE_PROXY *,server_proxy->reverse_proxys_list,index);
    strcpy(host,info->host);
    strcpy(port,info->port);
}

static void do_weighted_round_robin(char *host, char *port, SERVER_PROXY *server_proxy){
    //todo
}

static void do_ip_hash(char *host, char *port, SERVER_PROXY *server_proxy){
    int index;
    index = ip_hash(host,ARRAYLIST_LENGTH(server_proxy->reverse_proxys_list));
    REVERSE_PROXY *info = ARRAYLIST_CAST(REVERSE_PROXY *,server_proxy->reverse_proxys_list,index);
    strcpy(host,info->host);
    strcpy(port,info->port);
}

static void request_dynamic(char *host, char *port, SDI *sdi){
    // rio_t rio;
    int proxyClientfd;
    char buf[MAXLINE];

    proxyClientfd = Open_clientfd(host, port);

    sprintf(buf, "%s %s %s\r\n",sdi->method,sdi->uri,sdi->version); 
    Rio_writen(proxyClientfd, buf, strlen(buf));
    Rio_readlineb(sdi->rio, buf, MAXLINE);
    while (strcmp(buf, "\r\n")) { 
        Rio_readlineb(sdi->rio, buf, MAXLINE);
    }
    Close(proxyClientfd);
}