#include "proxy_common.h"
/*
进行静态代理，静态资源写回客户端
    fd 连接文件描述符
    sds 代理目标
*/
void serve_static(SDI *sdi);