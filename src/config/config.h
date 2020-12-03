#include "config_proxy.h"
#include "config_configs.h"

/*
配置信息
    base_configs 基础配置信息
    pcs 代理配置信息
*/
typedef struct nadia_config_struct{
    BASE_CONFIGS *base_configs;
    NAIDA_PROXY_CONFIG *nadia_proxy_config;
} NADIA_CONFIG;


int load_nadia_config(char *dir,NADIA_CONFIG *config);
