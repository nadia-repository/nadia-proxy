#include "config.h"

#define IS_MOCK 1

void mockConfig(CS *cs);

int loadConfig(char *path, CS *cs){
    fprintf(stderr, "start load nadia config file from<%s> \n", path);
    #ifdef IS_MOCK
        fprintf(stderr, "====================in mock \n");
        mockConfig(cs);
        return 1;
    #else
        fprintf(stderr, "====================not mock \n");
        return 0;
    #endif
}

void freeConfig(CS *cs){
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
            
           PS *proxy = location->proxy;
           char **proxyServer = proxy->server;
           Free(proxyServer);
           Free(proxy);
           Free(location);
        }
        Free(locations);
        Free(server);
    }
    Free(servers);
}


void mockConfig(CS *cs){
    SS *server = malloc(sizeof(SS));
    server->listen = "8888";
    server->locationSize = 0;


    SS **servers = malloc(sizeof(server) *1);

    servers[0] = server;

    cs->servers = servers;
    cs->serverSize = 1;
}