#include "config_proxy.h"

static void mock_config(NAIDA_PROXY_CONFIG *nadia_proxy_config);
static void load_proxy_conf(char *path, NAIDA_PROXY_CONFIG *nadia_proxy_config);


static void generate_state_chain(FSM **fsm);
static void pares_file(FILE * fp, char *line, FSM *fsm, void *config);

static void parse_http(char *line,void *fsm,void *);
static void parse_servers(char *,void *,void *);
static void parse_listen(char *,void *);
static void parse_location(char *,void *);
static void parse_strategy(char *,void *);
static void parse_proxy(char *,void *);
static void parse_root(char *,void *);
static void parse_alias(char *,void *);
static void parse_server(char *,void *);

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
        return 0;
    }
}

void free_proxy(NAIDA_PROXY_CONFIG *nadia_proxy_config){
    // SERVERS_CONFIG **servers = pcs->servers;
    // uint16_t serversSize = pcs->server_size;
    // fprintf(stderr, "free servers size =%d \n",serversSize);
    // for(int i = 0;i < serversSize;i++){
    //     SERVERS_CONFIG *server = servers[i];
    //     LS **locations = server->locations;
    //     uint16_t locationSize = server->locationSize;
    //     fprintf(stderr, "free location size =%d \n",locationSize);
    //     for(int j = 0;j < locationSize;j++){
    //        LS *location = locations[j];
    //        if(location->server_proxy != NULL){
    //            Free(location->server_proxy->server);
    //            Free(location->server_proxy);
    //        }
    //        if(location->static_proxy != NULL){
    //            Free(location->static_proxy->alias);
    //            Free(location->static_proxy->root); 
    //            Free(location->static_proxy); 
    //        }
    //        Free(location);
    //     }
    //     Free(locations);
    //     Free(server);
    // }
    // Free(servers);
}


static void mock_config(NAIDA_PROXY_CONFIG *nadia_proxy_config){
    HTTP_CONFIG *http_config = malloc(sizeof(HTTP_CONFIG));
    SERVERS_CONFIG **servers = malloc(sizeof(SERVERS_CONFIG*) *1);

    SERVERS_CONFIG *server;
    server = malloc(sizeof(SERVERS_CONFIG));
    server->listen = "80";

    MAP_INSTANCE * map = init_hashmap(0);
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
    map->put(map,NONE,lms);

    server->location_map = map;
    servers[0] = server;

    http_config->servers = servers;
    http_config->server_size = 1;

    nadia_proxy_config->http_config = http_config;
}

static void load_proxy_conf(char *path, NAIDA_PROXY_CONFIG *nadia_proxy_config){
    struct stat sbuf;
    if (stat(path, &sbuf) < 0) {
        fprintf(stderr, "Load proxy.conf is not exist. path=%s \n",path);
        return;
    }  
    
    FSM **status_machine_arrary = calloc(sizeof(state_tag)/sizeof(char *),sizeof(FSM *));
    generate_state_chain(status_machine_arrary);

    FILE * fp = NULL;
    char buf[2014];
    char *p;
    fp = fopen("/Users/xiangshi/Documents/workspace_c/testregx/proxy.conf" ,  "r" );
    while (fgets(buf, MAXLINE, fp) != NULL) {
        pares_file(fp,buf,status_machine_arrary[HTTP],nadia_proxy_config);
        printf("%s",buf);
    }
}

static void pares_file(FILE * fp, char *line, FSM *fsm, void *config){
    void *temp_config = config;
    if(*line != '\0' && *line != '#' && *line != '\n'){
        if(strcmp(line,fsm->tag)){
            //do 自己的parse
            (fsm->parse)(line,fsm,temp_config);
            //解析下一行
            char buf[MAXLINE];
            if(fgets(buf, MAXLINE, fp) != NULL){
                for(int i =0;i<fsm->next_state_size;i++){
                    pares_file(fp,buf,fsm->next_states[i],temp_config);
                }
            }

        }
    }
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
    root->next_state_size = NULL;
    root->parse = &parse_root;
    fsm[root->current_state] = root;

    //alias
    FSM *alias = malloc(sizeof(FSM));
    alias->current_state = ALIAS;
    alias->tag = state_tag[alias->current_state];
    alias->next_state_size = 0;
    alias->next_state_size = NULL;
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

static void parse_http(char *line, void *fsm, void *config){
    NAIDA_PROXY_CONFIG *nadia_proxy_config = (NAIDA_PROXY_CONFIG *)config;

    HTTP_CONFIG *http_config = malloc(sizeof(HTTP_CONFIG));
    http_config->server_size = 0;

    nadia_proxy_config->http_config = http_config;

    config = http_config;
}

static void parse_servers(char *line, void *fsm, void *config){
    HTTP_CONFIG *http_config = (HTTP_CONFIG *)config;

    SERVERS_CONFIG *servers_config = malloc(sizeof(SERVERS_CONFIG));
    MAP_INSTANCE *location_map = init_hashmap(0);
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

static void parse_listen(char *line,void *fsm){

}

static void parse_location(char *line,void *fsm){

}

static void parse_strategy(char *line,void *fsm){

}

static void parse_proxy(char *line,void *fsm){

}

static void parse_root(char *line,void *fsm){

}

static void parse_alias(char *line,void *fsm){

}

static void parse_server(char *line,void *fsm){

}

enum config_node_type {PARENT,CHILD};

typedef struct config_node_struct{
    enum config_node_type node_type;
    char *content;

    struct config_node_struct *friend;
    struct config_node_struct *child;
} CONFIG_NODE;


void create_tree(STACK_INSTANCE *stack){
    CONFIG_NODE  *node = (CONFIG_NODE *)stack->pop(stack);
    if(node == NULL){
        return;
    }

    if(node->node_type == PARENT){
        node->node_type = CHILD;
        stack->push(stack,node);
    }else {
        while(node->node_type != PARENT){
            CONFIG_NODE  *pre_node = (CONFIG_NODE *)stack->pop(stack);
            if(pre_node->node_type == PARENT){
                pre_node->child = node;
                pre_node->node_type = CHILD;
                stack->push(stack,pre_node);
                return;
            }else if(pre_node->node_type == CHILD){
                pre_node->friend = node;
                stack->push(stack,pre_node);
            }
            node = (CONFIG_NODE *)stack->pop(stack);
        }
    }
}


void parse_file_to_tree(){
    FILE * fp = NULL;
    char buf[MAXLINE];
    
    STACK_INSTANCE *stack = init_stack(0);

    fp = fopen("/Users/xiangshi/Documents/workspace_c/testregx/proxy.conf" ,  "r" );
    while (fgets(buf, MAXLINE, fp) != NULL) {
        char *c;
        char *line = calloc(MAXLINE,sizeof(char));
        for(c = buf;*c != '\0' && *c != '#' && *c != '\n';c++){
            if(*c == '{'){
                CONFIG_NODE *parent_node = malloc(sizeof(CONFIG_NODE));
                parent_node->node_type = PARENT;
                parent_node->content = line;
                parent_node->child = NULL;
                parent_node->friend = NULL;
                stack->push(stack,parent_node);
            } else if(*c == '}'){
                create_tree(stack);
            } else if(*c == ';'){
                CONFIG_NODE *child_node = malloc(sizeof(CONFIG_NODE));
                child_node->node_type = CHILD;
                child_node->content = line;
                child_node->child = NULL;
                child_node->friend = NULL;
                stack->push(stack,child_node);
            } else {
                char nadia = *c;
                strcat(line,&nadia);
            }
        }
    }

    CONFIG_NODE  *node = (CONFIG_NODE *)stack->pop(stack);
}