#include "config.h"

typedef struct static_dynamic_struct{
    int isStatic;
    char *fileName;
    char *filePath;
    PS **proxy;
} SDS;


/*
解析请求，并执行相应的代理
    connfd 连接描述符
    server 当前connfd对应的listenfd所代理的信息
*/
void parser_request(int connfd,SS *server);

/*
根据当前请求匹配代理信息
*/
int match_proxy(char *method,char *uri,SS *server,SDS *sds);

void read_requesthdrs(rio_t *rp);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg);