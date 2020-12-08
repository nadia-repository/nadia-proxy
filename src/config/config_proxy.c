#include "config_proxy.h"

static void mock_config(NAIDA_PROXY_CONFIG *nadia_proxy_config);
static void load_proxy_conf(char *path, NAIDA_PROXY_CONFIG *nadia_proxy_config);

static void pares_proxy_file(char *filename, NAIDA_PROXY_CONFIG *nadia_proxy_config);
static void generate_state_chain(FSM **fsm);
static void generate_config_tree(STACK *stack);
static void generate_proxy_config(CONFIG_NODE  *node, void *config, FSM *fsm);

static void *parse_http(char *,void *);
static void *parse_servers(char *,void *);
static void *parse_listen(char *,void *);
static void *parse_location(char *,void *);
static void *parse_strategy(char *,void *);
static void *parse_proxy(char *,void *);
static void *parse_root(char *,void *);
static void *parse_alias(char *,void *);
static void *parse_index(char *,void *);
static void *parse_server(char *,void *);

static char *state_tag[] = { "init", "http", "servers","listen","location","root","alias","index","strategy","proxy","server"};

int load_proxy(char *dir, NAIDA_PROXY_CONFIG *nadia_proxy_config){
    char path[MAXLINE];
    strcpy(path,dir);
    strcat(path,"proxy.conf");
    fprintf(stderr, "start load nadia config file from<%s> \n", path);
    if(IS_MOCK){
        fprintf(stderr, "Load procy configs from mock \n");
        mock_config(nadia_proxy_config);
        return 1;
    }else{
        load_proxy_conf(dir,nadia_proxy_config);
        return 0;
    }
}

void free_proxy(NAIDA_PROXY_CONFIG *nadia_proxy_config){
    // HTTP_CONFIG *http_config = nadia_proxy_config->http_config;
    
}

CONFIG_NODE *init_config_node(enum config_node_type node_type, void *content, CONFIG_NODE *friend, CONFIG_NODE *child){
    CONFIG_NODE *node = malloc(sizeof(CONFIG_NODE));
    node->node_type = node_type;
    node->content = content;
    node->child = child;
    node->friend = friend;
    return node;
}

static void mock_config(NAIDA_PROXY_CONFIG *nadia_proxy_config){
    HTTP_CONFIG *http_config = malloc(sizeof(HTTP_CONFIG));
    SERVERS_CONFIG **servers = malloc(sizeof(SERVERS_CONFIG*) *1);

    SERVERS_CONFIG *server;
    server = malloc(sizeof(SERVERS_CONFIG));
    server->listen = "80";

    HASHMAP * map = INIT_HASHMAP;
    LMS *lms = malloc(sizeof(LMS));
    lms->locationSize = 1;
    LOCATION_CONFIG **lss = calloc(1,sizeof(LOCATION_CONFIG *));
    LOCATION_CONFIG *ls = malloc(sizeof(LOCATION_CONFIG));;
    ls->isStatic = 1;
    ls->pattern = "/";
    STATIC_PROXY *static_proxy = (STATIC_PROXY *)malloc(sizeof(STATIC_PROXY));
    #ifdef __APPLE__
        static_proxy->root = "/Users/xiangshi/Documents/workspace_c/nadia-proxy/";

    #else
        static_proxy->root = "/workspace_c/html";
    #endif
    static_proxy->index = "index.html";
    ls->static_proxy = static_proxy;
    lss[0] = ls;
    lms->locations = lss;
    PUT_HASHMAP(map,NONE,lms);

    server->location_map = map;
    servers[0] = server;

    // http_config->servers = servers;
    // http_config->server_size = 1;

    nadia_proxy_config->http_config = http_config;
}

/*
加载代理配置文件
    path 配置文件路径
    nadia_proxy_config 代理配置文件
*/
static void load_proxy_conf(char *path, NAIDA_PROXY_CONFIG *nadia_proxy_config){
    struct stat sbuf;
    if (stat(path, &sbuf) < 0) {
        fprintf(stderr, "Load proxy.conf is not exist. path=%s \n",path);
        return;
    } 
    nadia_proxy_config->http_config = NULL;
    pares_proxy_file(path,nadia_proxy_config);
}

/*
 * 生成状态机链
 */
void generate_state_chain(FSM **fsm){
    FSM *server = NULL;
    FSM *proxy = NULL;
    FSM *strategy = NULL;
    FSM *root = NULL;
    FSM *alias = NULL;
    FSM *index = NULL;
    FSM *location = NULL;
    FSM *listen = NULL;
    FSM *servers = NULL;
    FSM *http = NULL;
    FSM *init = NULL;

    //server
    server = malloc(sizeof(FSM));
    server->current_state = SERVER;
    server->tag=state_tag[server->current_state];
    server->child_state_list = NULL;
    server->frient_state_list = ARRAYLIST_INIT_SIZE(1);
    ARRAYLIST_ADD(server->frient_state_list,server);
    server->parse = &parse_server;
    fsm[server->current_state] = server;
    //----------------------------------------------------------------------
    //proxy
    proxy = malloc(sizeof(FSM));
    proxy->current_state = PROXY;
    proxy->tag=state_tag[proxy->current_state];
    proxy->child_state_list = ARRAYLIST_INIT_SIZE(2);
    ARRAYLIST_ADD(proxy->child_state_list,server);
    ARRAYLIST_ADD(proxy->child_state_list,strategy);
    proxy->frient_state_list = NULL;
    proxy->parse = &parse_proxy;
    fsm[proxy->current_state] = proxy;

    //strategy
    strategy = malloc(sizeof(FSM));
    strategy->current_state = STRATEGY;
    strategy->tag=state_tag[strategy->current_state];
    strategy->child_state_list = NULL;
    strategy->frient_state_list = ARRAYLIST_INIT_SIZE(1);
    ARRAYLIST_ADD(strategy->frient_state_list,proxy);
    strategy->parse = &parse_strategy;
    fsm[strategy->current_state] = strategy;

    //root 互斥 alias
    root = malloc(sizeof(FSM));
    root->current_state = ROOT;
    root->tag = state_tag[root->current_state];
    root->child_state_list = NULL;
    root->frient_state_list = ARRAYLIST_INIT_SIZE(1);
    ARRAYLIST_ADD(root->frient_state_list,index);
    root->parse = &parse_root;
    fsm[root->current_state] = root;

    //alias 互斥 root
    alias = malloc(sizeof(FSM));
    alias->current_state = ALIAS;
    alias->tag = state_tag[alias->current_state];
    alias->child_state_list = NULL;
    alias->frient_state_list = ARRAYLIST_INIT_SIZE(1);
    ARRAYLIST_ADD(alias->frient_state_list,index);
    alias->parse = &parse_alias;
    fsm[alias->current_state] = alias;

    //index
    index = malloc(sizeof(FSM));
    index->current_state = INDEX;
    index->tag = state_tag[alias->current_state];
    index->child_state_list = NULL;
    index->frient_state_list = ARRAYLIST_INIT_SIZE(2);
    ARRAYLIST_ADD(index->frient_state_list,root);
    ARRAYLIST_ADD(index->frient_state_list,alias);
    index->parse = &parse_index;
    fsm[index->current_state] = index;

    //----------------------------------------------------------------------
    //location
    location = malloc(sizeof(FSM));
    location->current_state = LOCATION;
    location->tag=state_tag[location->current_state];
    location->child_state_list = ARRAYLIST_INIT_SIZE(5);
    ARRAYLIST_ADD(location->child_state_list,root);
    ARRAYLIST_ADD(location->child_state_list,alias);
    ARRAYLIST_ADD(location->child_state_list,index);
    ARRAYLIST_ADD(location->child_state_list,strategy);
    ARRAYLIST_ADD(location->child_state_list,proxy);
    location->frient_state_list = ARRAYLIST_INIT_SIZE(2);
    ARRAYLIST_ADD(location->frient_state_list,location);
    ARRAYLIST_ADD(location->frient_state_list,listen);
    location->parse = &parse_location;
    fsm[location->current_state] = location;

    //listen
    listen = malloc(sizeof(FSM));
    listen->current_state = LISTEN;
    listen->tag=state_tag[listen->current_state];
    listen->child_state_list = NULL;
    listen->frient_state_list = ARRAYLIST_INIT_SIZE(1);
    ARRAYLIST_ADD(listen->frient_state_list,server);
    listen->parse = &parse_listen;
    fsm[listen->current_state] = listen;

    //----------------------------------------------------------------------
    //servers
    servers = malloc(sizeof(FSM));
    servers->current_state = SERVERS;
    servers->tag=state_tag[servers->current_state];
    servers->child_state_list = ARRAYLIST_INIT_SIZE(2);
    ARRAYLIST_ADD(servers->child_state_list,listen);
    ARRAYLIST_ADD(servers->child_state_list,location);
    servers->frient_state_list = ARRAYLIST_INIT_SIZE(1);
    ARRAYLIST_ADD(servers->frient_state_list,servers);
    servers->parse = &parse_servers;
    fsm[servers->current_state] = servers;

    //----------------------------------------------------------------------
    //http
    http = malloc(sizeof(FSM));
    http->current_state = HTTP;
    http->tag=state_tag[http->current_state];
    http->child_state_list = ARRAYLIST_INIT_SIZE(1);
    ARRAYLIST_ADD(http->child_state_list,servers);
    http->frient_state_list = NULL;
    http->parse = &parse_http;
    fsm[http->current_state] = http;

    //----------------------------------------------------------------------
    //init
    init = malloc(sizeof(FSM));
    init->current_state = INIT;
    init->tag=state_tag[init->current_state];
    init->child_state_list = ARRAYLIST_INIT_SIZE(1);
    ARRAYLIST_ADD(init->child_state_list,http);
    init->frient_state_list = NULL;
    fsm[init->current_state] = init;

}

static void *parse_http(char *line, void *config){
    NAIDA_PROXY_CONFIG *nadia_proxy_config = (NAIDA_PROXY_CONFIG *)config;
    if(nadia_proxy_config->http_config != NULL){
        return nadia_proxy_config->http_config;
    }
    HTTP_CONFIG *http_config = malloc(sizeof(HTTP_CONFIG));
    http_config->servers_list = ARRAYLIST_INIT;
    nadia_proxy_config->http_config = http_config;

    return http_config;
}

static void *parse_servers(char *line, void *config){
    HTTP_CONFIG *http_config = (HTTP_CONFIG *)config;
    SERVERS_CONFIG *servers_config = malloc(sizeof(SERVERS_CONFIG));
    servers_config->location_list = ARRAYLIST_INIT;
    servers_config->location_map = INIT_HASHMAP;
    config = servers_config;
    return servers_config;
}

static void *parse_listen(char *line, void *config){
    SERVERS_CONFIG *servers_config = (SERVERS_CONFIG *)config;
    char port[MAXLINE];
    sscanf(line,"listen %s",port);
    servers_config->listen = port;

    return config;
}

static void *parse_location(char *line, void *config){
    SERVERS_CONFIG *servers_config = (SERVERS_CONFIG *)config;
    char modifier[MAXLINE],pattern[MAXLINE];
    sscanf(line,"location %s %s",modifier,pattern);

    LOCATION_CONFIG *location = malloc(sizeof(LOCATION_CONFIG));
    if(strcmp("=",modifier)>0){
        //exact 精确匹配
        location->matchType = EXACT;
        location->pattern = pattern;
    }else if(strcmp("^~",modifier)>0){
        //prefix 前缀匹配
        location->matchType = PREFIX;
        location->pattern = pattern;
    }else if(strcmp("~",modifier)>0){
        //regex 正则匹配
        location->matchType = REGEX;
        location->pattern = pattern;
    }else if(strcmp("~*",modifier)>0){
        //regex 正则匹配
        location->matchType = REGEX;
        location->pattern = pattern;
    }else {
        //none  无修饰符前缀匹配
        location->matchType = NONE;
        location->pattern = modifier;
    }
    ARRAYLIST_ADD(servers_config->location_list,location);
    PUT_HASHMAP(servers_config->location_map,location->matchType,servers_config->location_list);
    return location;
}

static void *parse_proxy(char *line,void *config){
    LOCATION_CONFIG *location = (LOCATION_CONFIG *)config;
    SERVER_PROXY *proxy = malloc(sizeof(SERVER_PROXY));
    location->server_proxy = proxy;
    return proxy;
}

static void *parse_strategy(char *line, void *config){
    SERVER_PROXY *proxy = (SERVER_PROXY *)config;
    
    proxy->request_count = 0;
    char strategy[MAXLINE];
    sscanf(line,"strategy %s",strategy);
    //ROUND_ROBIN,WEIGHTED_ROUND_ROBIN,IP_HASH
    if(strcmp("ROUND_ROBIN",strategy)>0){
        proxy->proxyStrategy = ROUND_ROBIN;
    }else if(strcmp("WEIGHTED_ROUND_ROBIN",strategy)>0){
        proxy->proxyStrategy = WEIGHTED_ROUND_ROBIN;
    }else if(strcmp("IP_HASH",strategy)>0){
        proxy->proxyStrategy = IP_HASH;
    }else{
        proxy->proxyStrategy = ROUND_ROBIN;
    }
    proxy->reverse_proxys_list = ARRAYLIST_INIT;

    return config;
}

static void *parse_root(char *line, void *config){
    LOCATION_CONFIG *location = (LOCATION_CONFIG *)config;

    char root[MAXLINE];
    sscanf(line,"root %s",root);
    STATIC_PROXY *static_proxy = NULL;
    if(location->static_proxy != NULL){
        static_proxy = location->static_proxy;
    }else {
        static_proxy = malloc(sizeof(STATIC_PROXY));
    }
    static_proxy->root = root;
    location->static_proxy = static_proxy;

    return static_proxy;
}   

static void *parse_alias(char *line, void *config){
    LOCATION_CONFIG *location = (LOCATION_CONFIG *)config;

    char alias[MAXLINE];
    sscanf(line,"alias %s",alias);
    STATIC_PROXY *static_proxy = NULL;
    if(location->static_proxy != NULL){
        static_proxy = location->static_proxy;
    }else {
        static_proxy = malloc(sizeof(STATIC_PROXY));
    }
    static_proxy->alias = alias;
    location->static_proxy = static_proxy;

    return static_proxy;
}

static void *parse_index(char *line, void *config){
    LOCATION_CONFIG *location = (LOCATION_CONFIG *)config;

    char index[MAXLINE];
    sscanf(line,"index %s",index);
    STATIC_PROXY *static_proxy = NULL;
    if(location->static_proxy != NULL){
        static_proxy = location->static_proxy;
    }else {
        static_proxy = malloc(sizeof(STATIC_PROXY));
    }
    static_proxy->index = index;
    location->static_proxy = static_proxy;

    return static_proxy;
}

static void *parse_server(char *line, void *config){
    SERVER_PROXY *proxy = (SERVER_PROXY *)config;
    char host[MAXLINE],port[MAXLINE],weight[MAXLINE];
    sscanf(line,"server %s:%s weight=%s",host,port,weight);
    REVERSE_PROXY *reverse_proxy = malloc(sizeof(REVERSE_PROXY));
    reverse_proxy->host = host;
    reverse_proxy->port = port;
    reverse_proxy->weight = atoi(weight);
    ARRAYLIST_ADD(proxy->reverse_proxys_list,reverse_proxy);

    return reverse_proxy;
}

static void pares_proxy_file(char *filename, NAIDA_PROXY_CONFIG *nadia_proxy_config){
    FILE * fp = NULL;
    char buf[MAXLINE];

    STACK *stack = INIT_STACK;

    fp = fopen(filename ,  "r" );
    while (fgets(buf, MAXLINE, fp) != NULL) {
        char *c;
        STRING *string = INIT_STRING;
        int pre_blank = 0;
        c = buf;
        while(*c != '\0' && *c != '#' && *c != '\n'){
            if(pre_blank > 0 && isblank(*c) > 0){
                c++;
                continue;
            }
            pre_blank = isblank(*c) > 0 ? 1 : 0;

            if(*c == '{'){
                CONFIG_NODE *parent_node = init_config_node(PARENT,string,NULL,NULL);
                PUSH_STACK(stack,parent_node);
            } else if(*c == '}'){
                generate_config_tree(stack);
            } else if(*c == ';'){
                CONFIG_NODE *child_node = init_config_node(CHILD,string,NULL,NULL);
                PUSH_STACK(stack,child_node);
            } else {
                JOIN_CHAR(string,*c);
            }
            c++;
        }
    }
    //初始化配置生产状态机
    FSM **status_machine_arrary = calloc(sizeof(state_tag)/sizeof(char *),sizeof(FSM *));
    generate_state_chain(status_machine_arrary);

    CONFIG_NODE  *node = (CONFIG_NODE *)POP_STACK(stack);
    FSM *init = status_machine_arrary[INIT];
    for(int i = 0 ;ARRAYLIST_LENGTH(init->child_state_list) ; i++){
        generate_proxy_config(node,nadia_proxy_config,ARRAYLIST_GET(init->child_state_list,i));
    }
}

static void generate_config_tree(STACK *stack){
    CONFIG_NODE  *node = (CONFIG_NODE *)POP_STACK(stack);
    
    if(node == NULL){
        return;
    }

    if(node->node_type == PARENT){
        node->node_type = CHILD;
        stack->push(stack,node);
    }else {
        while(node->node_type != PARENT){
            CONFIG_NODE  *pre_node = (CONFIG_NODE *)POP_STACK(stack);
            if(pre_node == NULL){
                PUSH_STACK(stack,node);
                return;
            }else if(pre_node->node_type == PARENT){
                pre_node->child = node;
                pre_node->node_type = CHILD;
                PUSH_STACK(stack,pre_node);
                return;
            }else if(pre_node->node_type == CHILD){
                pre_node->friend = node;
                PUSH_STACK(stack,pre_node);
            }
            node = (CONFIG_NODE *)POP_STACK(stack);
        }
    }
}

static void generate_proxy_config(CONFIG_NODE  *node, void *config, FSM *fsm){
    if(node == NULL){
        return;
    }
    STRING *string = (STRING *)node->content;
    void *node_config = NULL;
    if(strcmp(string->string,fsm->tag) > 0){
        node_config = fsm->parse(string->string,config);
    }
    //parse child
    for(int i = 0 ;ARRAYLIST_LENGTH(fsm->child_state_list) ; i++){
        generate_proxy_config(node->child,node_config,ARRAYLIST_GET(fsm->child_state_list,i));
    }
    //parse friend
        for(int i = 0 ;ARRAYLIST_LENGTH(fsm->frient_state_list) ; i++){
        generate_proxy_config(node->friend,node_config,ARRAYLIST_GET(fsm->frient_state_list,i));
    }
}