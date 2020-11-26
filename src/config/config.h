#include "csapp.h"

#define IS_MOCK 1

#ifdef __APPLE__
    #define FILE_FLAG O_CREAT|O_TRUNC|O_WRONLY
#elif __linux__
    #define FILE_FLAG O_CREAT|O_TRUNC|O_WRONLY|F_SHLCK
#endif

/*
反向代理负载均衡策略
    loop:轮训
    random:随机
*/
enum strategy {loop,ip};

/*
location匹配规则
    exact 精确匹配，修饰符=
    prefix 前缀匹配，修饰符^~
    regex 正则匹配,修饰符，区分大小写~，不区分大小写~*
    none  无修饰符前缀匹配，无修饰符
*/
enum match_type {exact,prefix,regex,none};

/*
动态代理信息
    proxyStrategy 负责均衡策略
    server 代理服务列表
*/
typedef struct dynamic_proxy_struct{
    enum strategy proxyStrategy;
    char **server;
} DPS;

/*
静态代理信息
    alias 重命名目录
    root 文件目录
*/
typedef struct static_proxy_struct{
    char *alias;
    char *root;
} SPS;

/*
代理信息
    path 代理路径
    isStatic 动态代理\静态代理
    root 静态代理文件路径
    proxy 动态代理信息
*/
typedef struct location_struct{
    char *pattern;
    enum match_type matchType;
    int isStatic;
    SPS *sps;
    DPS *dps;
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
配置文件路径结构
    proxyPath 代理文件
    logPath 日志文件
    configsPath 配置项
*/
typedef struct config_path{
  char *proxyPath;
  char *logPath;
  char *configsPath;
} CP;


/*
根据文件路径加载Proxy配置文件
retrun 1 或 0
1: 读取配置成功
0: 读取配置失败
*/
int load_proxy(char *path, CS *cs);

int load_log(char *path);

int load_configs(char *path);

void free_proxy(CS *cs);

/*
初始化配置文件路径
    dir 配置文件目录
    cp  配置文件路径结构
*/
void init_file_path(char *dir,CP* cp);