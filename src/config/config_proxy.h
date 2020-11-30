#include "csapp.h"
#include "constant.h"
#include "hashmap.h"

/*
反向代理负载均衡策略
    loop:轮训
    random:随机
*/
enum strategy {LOOP,IP,RANDOM};

/*
location匹配规则
    exact 精确匹配，修饰符=
    prefix 前缀匹配，修饰符^~
    regex 正则匹配,修饰符，区分大小写~，不区分大小写~*
    none  无修饰符前缀匹配，无修饰符
*/
enum match_type {EXACT,PREFIX,REGEX,NONE};

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
    char *index;
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
匹配规则对应的代理信息
    locationSize 代理数量
    locations 代理信息列表
*/
typedef struct location_map_struct{
    uint16_t locationSize;
    LS **locations;
} LMS;


/*
服务信息
    listen 服务器监听端口
    locMap 监听端口代理信息
        EXACT  -|- locationSize
                |- locations
        
        PREFIX -|- locationSize
                |- locations

        REGEX  -|- locationSize
                |- locations

        NONE   -|- locationSize
                |- locations
*/
typedef struct server_struct{
    char *listen;
    MAP_INSTANCE *locMap;
} SS;

/*
代理配置信息
    servers 代理信息列表
*/
typedef struct proxy_config_struct{
    uint16_t serverSize;
    SS **servers;
} PCS;

/*
根据文件路径加载Proxy配置文件
retrun 1 或 0
1: 读取配置成功
0: 读取配置失败
*/
int load_proxy(char *path, PCS *pcs);

void free_proxy(PCS *pcs);