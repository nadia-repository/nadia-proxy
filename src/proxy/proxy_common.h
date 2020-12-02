#ifndef  PROXY_COMMON_H 
#define PROXY_COMMON_H 

#include "config.h"

void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg);

void get_filetype(char *filename, char *filetype);

int has_filetype(char *filename);

#endif