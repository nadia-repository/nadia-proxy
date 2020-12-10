#include "proxy_static.h"
#include "proxy_dynamic.h"
#include "match.h"

/*
解析请求，并执行相应的代理
    connfd 连接描述符
    server 当前connfd对应的listenfd所代理的信息
*/
void parser_request(int connfd,SERVERS_CONFIG *server);
