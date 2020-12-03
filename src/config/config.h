#include "config_proxy.h"
#include "config_configs.h"

/*
配置信息
    cs 基础配置信息
    pcs 代理配置信息
*/
typedef struct nadia_config_struct{
    CS *cs;
    HTTP_CONFIG *pcs;
} NADIA_CONFIG;


int load_nadia_config(char *dir,NADIA_CONFIG *config);
