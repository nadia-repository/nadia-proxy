#include "config_proxy.h"

static void mock_config(PCS *pcs);

int load_proxy(char *path, PCS *pcs){
    fprintf(stderr, "start load nadia config file from<%s> \n", path);
    if(IS_MOCK){
        fprintf(stderr, "Load procy configs from mock \n");
        mock_config(pcs);
        return 1;
    }else{
        return 0;
    }
}

void free_proxy(PCS *pcs){
    SS **servers = pcs->servers;
    uint16_t serversSize = pcs->serverSize;
    fprintf(stderr, "free servers size =%d \n",serversSize);
    for(int i = 0;i < serversSize;i++){
        SS *server = servers[i];
        LS **locations = server->locations;
        uint16_t locationSize = server->locationSize;
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


static void mock_config(PCS *pcs){
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
    ls->pattern = "/";
    SPS *sps = (SPS *)malloc(sizeof(SPS));
    #ifdef __APPLE__
        sps->root = "/Users/xiangshi/Documents/workspace_c/nadia-proxy/";

    #else
        sps->root = "/workspace_c/nadia-proxy/";
    #endif
    ls->sps = sps;
    lss[0] = ls;
    server->locations = lss;

    pcs->servers = servers;
    pcs->serverSize = 3;
}