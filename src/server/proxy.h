#include "config.h"
#include "match.h"

typedef struct static_dynamic_info{
    int isStatic;
    char *fileName;
    char *filePath;
    DPS *dps;
} SDI;


/*
解析请求，并执行相应的代理
    connfd 连接描述符
    server 当前connfd对应的listenfd所代理的信息
*/
void parser_request(int connfd,SS *server);

/*
根据当前请求匹配代理信息
    method 请求method
    uri 资源地址
    server 当前监听地址代理信息
    sds 代理目标
*/
int match_proxy(char *method,char *uri,SS *server,SDI *sdi);

/*
进行静态代理，江景钛资源写回客户端
    fd 连接文件描述符
    sds 代理目标
*/
void serve_static(int fd, SDI *sdi);

void read_requesthdrs(rio_t *rp);

void get_filetype(char *filename, char *filetype);
void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg);