#include "config.h"

#define IS_MOCK 1

void mockConfig(CS *cs);

int loadProxy(char *path, CS *cs){
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

void freeProxy(CS *cs){
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
    SS **servers = malloc(sizeof(SS*) *3);

    SS *server = malloc(sizeof(SS));
    server->listen = "8888";
    server->locationSize = 0;
    servers[0] = server;

    server = malloc(sizeof(SS));
    server->listen = "7777";
    server->locationSize = 0;
    servers[1] = server;

    server = malloc(sizeof(SS));
    server->listen = "80";
    server->locationSize = 0;
    servers[2] = server;

    cs->servers = servers;
    cs->serverSize = 3;
}

void initFilePath(char *dir,CP* cp){
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

int loadLog(char *path){
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

int loadConfigs(char *path){

    return 1;
}