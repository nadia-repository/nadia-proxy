#include "csapp.h"
#include "constant.h"
#include "hashmap.h"
#include "stack.h"
#include "dynamic_string.h"
#include "arraylist.h"
#include <ctype.h>
#include <sys/stat.h>

enum state {INIT,HTTP,SERVERS,LISTEN,LOCATION,ROOT,ALIAS,INDEX,STRATEGY,PROXY,SERVER};

typedef struct finite_state_machine {
    enum state current_state;
    char *tag;
    void *(* parse)(char *line,void *config);

    ARRAYLIST *child_state_list;
    ARRAYLIST *frient_state_list;
} FSM;

enum config_node_type {PARENT,CHILD};

typedef struct config_node_struct{
    enum config_node_type node_type;
    void *content;

    struct config_node_struct *friend;
    struct config_node_struct *child;
} CONFIG_NODE;

/*
反向代理负载均衡策略
    ROUND_ROBIN:轮训
    WEIGHTED_ROUND_ROBIN:加权轮训
    RANDOM 随机
    HASH 哈希
*/
enum strategy {ROUND_ROBIN,WEIGHTED_ROUND_ROBIN,IP_HASH};

/*
location匹配规则
    exact 精确匹配，修饰符=
    prefix 前缀匹配，修饰符^~
    regex 正则匹配,修饰符，区分大小写~，不区分大小写~*
    none  无修饰符前缀匹配，无修饰符
*/
enum match_type {EXACT,PREFIX,REGEX,NONE};

/*
代理主机信息
    host 主机地址
    port 主机端口
    weight 权重
*/
typedef struct reverse_proxy_struct{
    char *host;
    char *port;
    int weight;
} REVERSE_PROXY;

/*
动态代理信息
    proxyStrategy 负责均衡策略
    reverse_proxys_list 负载均衡地址(REVERSE_PROXY **reverse_proxys)
*/
typedef struct server_proxy_struct{
    enum strategy proxyStrategy;
    ARRAYLIST *reverse_proxys_list;
    int request_count;
} SERVER_PROXY;

/*
静态代理信息
    alias 重命名目录
    root 文件目录
    index 默认页面
*/
typedef struct static_proxy_struct{
    char *alias;
    char *root;
    char *index;
} STATIC_PROXY;

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
    STATIC_PROXY *static_proxy;
    SERVER_PROXY *server_proxy;
} LOCATION_CONFIG;


/*
匹配规则对应的代理信息
    locationSize 代理数量
    locations 代理信息列表
*/
typedef struct location_map_struct{
    uint16_t locationSize;
    LOCATION_CONFIG **locations;
} LMS;


/*
服务信息
    listen 服务器监听端口
    location_list location列表
    location_map 监听端口代理信息 
        EXACT  -|- locationSize
                |- locations
        
        PREFIX -|- locationSize
                |- locations

        REGEX  -|- locationSize
                |- locations

        NONE   -|- locationSize
                |- locations
*/
typedef struct servers_struct{
    char *listen;
    ARRAYLIST *location_list;
    HASHMAP *location_map;
} SERVERS_CONFIG;

/*
代理配置信息
    servers_list 代理信息列表(SERVERS_CONFIG **servers)
*/
typedef struct http_struct{
    ARRAYLIST *servers_list;
} HTTP_CONFIG;

typedef struct nadia_proxy_struct{
    HTTP_CONFIG *http_config;
} NAIDA_PROXY_CONFIG;

/*
静态代理&动态代理信息

*/
typedef struct static_dynamic_info{
    int isStatic;
    char *path;
    char *method;
    char *uri;
    char *version;
    int connfd;
    rio_t *rio;
    SERVER_PROXY *server_proxy;
} SDI;

/*
根据文件路径加载Proxy配置文件
retrun 1 或 0
1: 读取配置成功
0: 读取配置失败
*/
int load_proxy(char *dir, NAIDA_PROXY_CONFIG *nadia_proxy_config);

void free_proxy(NAIDA_PROXY_CONFIG *nadia_proxy_config);

CONFIG_NODE *init_config_node(enum config_node_type node_type, void *content, CONFIG_NODE *friend, CONFIG_NODE *child);