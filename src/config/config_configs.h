
typedef struct base_configs_struct{
    char *nadiaDir;
    int staticCacheTimeout;
} CS;

int load_configs(char *path,CS *cs);