#include "config.h"

void mock_config(CS *cs);

int load_proxy(char *path, CS *cs){
    fprintf(stderr, "start load nadia config file from<%s> \n", path);
    #ifdef IS_MOCK
        fprintf(stderr, "Load procy configs from mock \n");
        mock_config(cs);
        return 1;
    #else

        return 0;
    #endif
}

void free_proxy(CS *cs){
    SS **servers = cs->servers;
    __int16_t serversSize = cs->serverSize;
    fprintf(stderr, "free servers size =%d \n",serversSize);
    for(int i = 0;i < serversSize;i++){
        SS *server = servers[i];
        LS **locations = server->locations;
        __int16_t locationSize = server->locationSize;
        fprintf(stderr, "free location size =%d \n",locationSize);
        for(int j = 0;j < locationSize;j++){
           LS *location = locations[j];
           if(location->dps != NULL){
               Free(location->dps->server);
               Free(location->dps);
           }
           if(location->sps != NULL){
               Free(location->sps->alias);
               Free(location->sps->root); 
               Free(location->sps); 
           }
           Free(location);
        }
        Free(locations);
        Free(server);
    }
    Free(servers);
}


void mock_config(CS *cs){
    SS **servers = malloc(sizeof(SS*) *3);

    SS *server = malloc(sizeof(SS));
    server->listen = "8701";
    server->locationSize = 0;
    servers[0] = server;

    server = malloc(sizeof(SS));
    server->listen = "4321";
    server->locationSize = 0;
    servers[1] = server;

    server = malloc(sizeof(SS));
    server->listen = "80";
    server->locationSize = 1;
    servers[2] = server;
    LS **lss = calloc(1,sizeof(LS *));
    LS *ls = malloc(sizeof(LS));;
    ls->isStatic = 1;
    // ls->path = "/";
    #ifdef __APPLE__
        // ls->root = "/Users/xiangshi/Documents/workspace_c/nadia-proxy/";

    #else
        // ls->root = "/workspace_c/nadia-proxy/";
    #endif
    lss[0] = ls;
    server->locations = lss;

    cs->servers = servers;
    cs->serverSize = 3;
}

void init_file_path(char *dir,CP* cp){
    // char buf[MAXBUF];
    // sprintf(buf,dir);
    // sprintf(buf,"proxy.conf");
    // cp->proxyPath = buf;

    // sprintf(buf,dir);
    // sprintf(buf,"log.conf");
    // cp->logPath = buf;

    // sprintf(buf,dir);
    // sprintf(buf,"config.conf");
    // cp->configsPath = buf;
}

int load_log(char *path){
    int logfd,errorfd;
    umask(DEF_UMASK);
    logfd = Open("/Users/xiangshi/Documents/workspace_c/nadia-proxy/log/logfile.txt", FILE_FLAG, 0777);
    if(Dup2(logfd,STDOUT_FILENO)<0){
        return 0;
    }

    umask(DEF_UMASK);
    errorfd = Open("/Users/xiangshi/Documents/workspace_c/nadia-proxy/log/errorfile.txt", FILE_FLAG, 0777);
    if(Dup2(errorfd,STDERR_FILENO)<0){
        return 0;
    }

    return 1;
}

int load_configs(char *path){

    return 1;
}