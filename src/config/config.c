#include "config.h"

static void init_file_path();

int load_nadia_config(char *dir,NADIA_CONFIG *config){

    init_file_path(dir);

    //加载基础配置
    CS *cs = (CS *)malloc(sizeof(CS));
    load_configs(dir,cs);

    //加载代理配置
    PCS *pcs = (PCS *)malloc(sizeof(PCS));
    load_proxy(dir,pcs);

    //日志文件映射

    config->cs = cs;
    config->pcs = pcs;
    return 1;

}

static void init_file_path(char *dir){
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
