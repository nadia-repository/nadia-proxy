#include <csapp.h>
#include "hashmap.h"

#define INFOLOG(log_instance) write_log_file(NADIA_LOG,log_instance)
#define ERRORLOG(log_instance) write_log_file(ERROR_LOG,log_instance)
#define ACCESSLOG(log_instance) write_log_file(ACCESS_LOG,log_instance)

typedef enum log_file_type {NADIA_LOG,ERROR_LOG,ACCESS_LOG} LOG_TYPE;

typedef struct log_file_struct {
    LOG_TYPE log_type;
    char *name;
    char *dir;
    FILE *fp;
} LOG_FILE;

typedef struct log_file_instance{
    HASHMAP *log_file_map;
} LOG_INSTANCE;


LOG_INSTANCE *init_log_file(char *dir);

void write_log_file(LOG_TYPE type, LOG_INSTANCE *log_instance);











typedef enum {
    LOG_DEBUG, 
    LOG_INFO,
    TLOG_WARN,
    TLOG_ERROR
} log_level;