#include "proxy_static.h"
#include "proxy_dynamic.h"
#include "match.h"

/*
解析请求，并执行相应的代理
    connfd 连接描述符
    server 当前connfd对应的listenfd所代理的信息
*/
void parser_request(int connfd,SERVERS_CONFIG *server);

/*
根据当前请求匹配代理信息
    method 请求method
    uri 资源地址
    server 当前监听地址代理信息
    sds 代理目标
*/
int match_proxy(char *method,char *uri,SERVERS_CONFIG *server,SDI *sdi);
