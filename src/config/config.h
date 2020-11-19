#include "csapp.h"
/*
反向代理负载均衡策略
    loop:轮训
    random:随机
*/
enum strategy {loop,ip};

/*
动态代理信息
    proxyStrategy 负责均衡策略
    server 代理服务列表
*/
typedef struct proxy_struct{
    enum strategy proxyStrategy;
    char **server;
} PS;

/*
代理信息
    path 代理路径
    isStatic 动态代理\静态代理
    root 静态代理文件路径
    proxy 动态代理信息
*/
typedef struct location_struct{
    char *path;
    int isStatic;
    char *root;
    PS *proxy;
} LS;

/*
服务信息
    listen 服务器监听端口
    locations 监听端口代理信息
*/
typedef struct server_struct{
    char *listen;
    __int16_t locationSize;
    LS **locations;
} SS;

/*
服务器配置信息
    servers 代理信息列表
*/
typedef struct config_struct{
    __int16_t serverSize;
    SS **servers;
} CS;

/*
根据文件路径加载配置文件
retrun 1 或 0
1: 读取配置成功
0: 读取配置失败
*/
int loadConfig(char *path, CS *cs);

void freeConfig(CS *cs);