
typedef struct base_configs_struct{
    char *nadiaDir;
    int staticCacheTimeout;
} BASE_CONFIGS;

int load_configs(char *path,BASE_CONFIGS *base_configs);

void free_base_configs(BASE_CONFIGS *base_configs);