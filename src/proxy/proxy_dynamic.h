#include "proxy_common.h"

/*
进行动态代理
    fd 连接文件描述符
    sds 代理目标
*/
void serve_dynamic(int fd, SDI *sdi);