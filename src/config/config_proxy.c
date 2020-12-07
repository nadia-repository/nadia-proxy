#include "config_proxy.h"

static void mock_config(NAIDA_PROXY_CONFIG *nadia_proxy_config);
static void load_proxy_conf(char *path, NAIDA_PROXY_CONFIG *nadia_proxy_config);

static void pares_proxy_file(char *filename, NAIDA_PROXY_CONFIG *nadia_proxy_config);
static void generate_state_chain(FSM **fsm);
static void generate_config_tree(STACK *stack);
static void generate_proxy_config(CONFIG_NODE  *node, void *config, FSM *fsm);

static void *parse_http(char *line,void *);
static void *parse_servers(char *,void *);
static void *parse_listen(char *,void *);
static void *parse_location(char *,void *);
static void *parse_strategy(char *,void *);
static void *parse_proxy(char *,void *);
static void *parse_root(char *,void *);
static void *parse_alias(char *,void *);
static void *parse_server(char *,void *);

static char *state_tag[] = { "init", "http", "servers","listen","location","root","alias","strategy","proxy","server"};

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
    HTTP_CONFIG *http_config = nadia_proxy_config->http_config;
    
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
    LS **lss = calloc(1,sizeof(LS *));
    LS *ls = malloc(sizeof(LS));;
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

    http_config->servers = servers;
    http_config->server_size = 1;

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
    //server
    FSM *server = malloc(sizeof(FSM));
    server->current_state = SERVER;
    server->tag=state_tag[server->current_state];
    server->next_state_size = 0;
    server->next_states = NULL;
    server->parse = &parse_server;
    fsm[server->current_state] = server;
    //----------------------------------------------------------------------
    //proxy
    FSM *proxy = malloc(sizeof(FSM));
    proxy->current_state = PROXY;
    proxy->tag=state_tag[proxy->current_state];
    proxy->next_state_size = 1;
    proxy->next_states = calloc(proxy->next_state_size,sizeof(FSM *));
    proxy->next_states[0] = server;
    proxy->parse = &parse_proxy;
    fsm[proxy->current_state] = proxy;

    //strategy
    FSM *strategy = malloc(sizeof(FSM));
    strategy->current_state = STRATEGY;
    strategy->tag=state_tag[strategy->current_state];
    strategy->next_state_size = 0;
    strategy->next_states = NULL;
    strategy->parse = &parse_strategy;
    fsm[strategy->current_state] = strategy;

    //root
    FSM *root = malloc(sizeof(FSM));
    root->current_state = ROOT;
    root->tag = state_tag[root->current_state];
    root->next_state_size = 0;
    root->next_states = NULL;
    root->parse = &parse_root;
    fsm[root->current_state] = root;

    //alias
    FSM *alias = malloc(sizeof(FSM));
    alias->current_state = ALIAS;
    alias->tag = state_tag[alias->current_state];
    alias->next_state_size = 0;
    alias->next_states = NULL;
    alias->parse = &parse_alias;
    fsm[alias->current_state] = alias;

    //----------------------------------------------------------------------
    //location
    FSM *location = malloc(sizeof(FSM));
    location->current_state = LOCATION;
    location->tag=state_tag[location->current_state];
    location->next_state_size = 4;
    location->next_states = calloc(location->next_state_size,sizeof(FSM *));
    location->next_states[0] = proxy;
    location->next_states[1] = strategy;
    location->next_states[2] = root;
    location->next_states[3] = alias;
    location->parse = &parse_location;
    fsm[location->current_state] = location;

    //strategy
    FSM *listen = malloc(sizeof(FSM));
    listen->current_state = LISTEN;
    listen->tag=state_tag[listen->current_state];
    listen->next_state_size = 0;
    listen->next_states = NULL;
    listen->parse = &parse_listen;
    fsm[listen->current_state] = listen;

    //----------------------------------------------------------------------
    //servers
    FSM *servers = malloc(sizeof(FSM));
    servers->current_state = SERVERS;
    servers->tag=state_tag[servers->current_state];
    servers->next_state_size = 2;
    servers->next_states = calloc(servers->next_state_size,sizeof(FSM *));
    servers->next_states[0] = listen;
    servers->next_states[1] = location;
    servers->parse = &parse_servers;
    fsm[servers->current_state] = servers;

    //----------------------------------------------------------------------
    //http
    FSM *http = malloc(sizeof(FSM));
    http->current_state = HTTP;
    http->tag=state_tag[http->current_state];
    http->next_state_size = 1;
    http->next_states = calloc(http->next_state_size,sizeof(FSM *));
    http->next_states[0] = servers;
    http->parse = &parse_http;
    fsm[http->current_state] = http;

    //----------------------------------------------------------------------
    //init
    FSM *init = malloc(sizeof(FSM));
    init->current_state = INIT;
    init->tag=state_tag[init->current_state];
    init->next_state_size = 1;
    init->next_states = calloc(init->next_state_size,sizeof(FSM *));
    init->next_states[0] = http;
    fsm[init->current_state] = init;

}

static void *parse_http(char *line, void *config){
    NAIDA_PROXY_CONFIG *nadia_proxy_config = (NAIDA_PROXY_CONFIG *)config;
    if(nadia_proxy_config->http_config != NULL){
        return;
    }
    HTTP_CONFIG *http_config = malloc(sizeof(HTTP_CONFIG));
    http_config->server_size = 0;

    nadia_proxy_config->http_config = http_config;

    config = http_config;
}

static void *parse_servers(char *line, void *config){
    HTTP_CONFIG *http_config = (HTTP_CONFIG *)config;

    SERVERS_CONFIG *servers_config = malloc(sizeof(SERVERS_CONFIG));
    HASHMAP *location_map = INIT_HASHMAP;
    servers_config->location_map = location_map;

    uint16_t server_size = http_config->server_size;
    SERVERS_CONFIG **servers = calloc(server_size+1,sizeof(SERVERS_CONFIG *));
    if(server_size>0){
        for(int i=0;i<server_size;i++){
            servers[i] = http_config->servers[i];
        }
        servers[server_size] = servers_config;
        Free(http_config->servers);
    }
    http_config->servers = servers;

    config = servers_config;
}

static void *parse_listen(char *line, void *config){

}

static void *parse_location(char *line, void *config){

}

static void *parse_strategy(char *line, void *config){

}

static void *parse_proxy(char *line,void *config){

}

static void *parse_root(char *line, void *config){

}

static void *parse_alias(char *line, void *config){

}

static void *parse_server(char *line, void *config){

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
    for(int i = 0;i< init->next_state_size; i++){
        generate_proxy_config(node,nadia_proxy_config,init->next_states[i]);
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
    for(int i = 0 ;fsm->next_state_size ; i++){
        generate_proxy_config(node->child,node_config,fsm->next_states[i]);
    }
    //parse friend
    generate_proxy_config(node->friend,config,fsm);
}