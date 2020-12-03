#include "config_proxy.h"

static void mock_config(HTTP_CONFIG *http_config);
static void load_proxy_conf(char *path, HTTP_CONFIG *http_config);


static void generate_state_chain(FSM **fsm);
static void pares_file(FILE * fp,char *line,FSM *fsm);

static void parse_http(char *line,void *fsm);
static void parse_servers(char *,void *);
static void parse_listen(char *,void *);
static void parse_location(char *,void *);
static void parse_strategy(char *,void *);
static void parse_proxy(char *,void *);
static void parse_root(char *,void *);
static void parse_alias(char *,void *);
static void parse_server(char *,void *);

int load_proxy(char *dir, HTTP_CONFIG *http_config){
    char path[MAXLINE];
    strcpy(path,dir);
    strcat(path,"proxy.conf");
    fprintf(stderr, "start load nadia config file from<%s> \n", path);
    if(IS_MOCK){
        fprintf(stderr, "Load procy configs from mock \n");
        mock_config(http_config);
        return 1;
    }else{
        return 0;
    }
}

void free_proxy(HTTP_CONFIG *pcs){
    SERVERS_CONFIG **servers = pcs->servers;
    uint16_t serversSize = pcs->serverSize;
    fprintf(stderr, "free servers size =%d \n",serversSize);
    // for(int i = 0;i < serversSize;i++){
    //     SERVERS_CONFIG *server = servers[i];
    //     LS **locations = server->locations;
    //     uint16_t locationSize = server->locationSize;
    //     fprintf(stderr, "free location size =%d \n",locationSize);
    //     for(int j = 0;j < locationSize;j++){
    //        LS *location = locations[j];
    //        if(location->dps != NULL){
    //            Free(location->dps->server);
    //            Free(location->dps);
    //        }
    //        if(location->sps != NULL){
    //            Free(location->sps->alias);
    //            Free(location->sps->root); 
    //            Free(location->sps); 
    //        }
    //        Free(location);
    //     }
    //     Free(locations);
    //     Free(server);
    // }
    Free(servers);
}


static void mock_config(HTTP_CONFIG *pcs){
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
    SPS *sps = (SPS *)malloc(sizeof(SPS));
    #ifdef __APPLE__
        sps->root = "/Users/xiangshi/Documents/workspace_c/nadia-proxy/";

    #else
        sps->root = "/workspace_c/html";
    #endif
    sps->index = "index.html";
    ls->sps = sps;
    lss[0] = ls;
    lms->locations = lss;
    map->put(map,NONE,lms);

    server->locMap = map;
    servers[0] = server;

    pcs->servers = servers;
    pcs->serverSize = 1;
}

static void load_proxy_conf(char *path, HTTP_CONFIG *pcs){
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
        pares_file(fp,buf,status_machine_arrary[HTTP]);
        printf("%s",buf);
    }
}

static void pares_file(FILE * fp,char *line,FSM *fsm){
    if(*line != '\0' && *line != '#' && *line != '\n'){
        if(strcmp(line,fsm->tag)){
            //do 自己的parse
            (fsm->parse)(line,fsm);
            //解析下一行
            char buf[MAXLINE];
            if(fgets(buf, MAXLINE, fp) != NULL){
                for(int i =0;i<fsm->next_state_size;i++){
                    pares_file(fp,buf,fsm->next_states[i]);
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

static void parse_http(char *line,void *fsm){
    ((FSM *)fsm)->config = malloc(sizeof(HTTP_CONFIG));

}

static void parse_servers(char *line,void *fsm){

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

