#ifndef  __PROXY_COMMON_H__
#define __PROXY_COMMON_H__ 

#include "config.h"
#include <stdlib.h>

void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg);

void get_filetype(char *filename, char *filetype);

int has_filetype(char *filename);

#endif