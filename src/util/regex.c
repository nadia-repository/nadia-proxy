#include "regex.h"

int match(char *pattern,char *target){
    regex_t compiled;

    regcomp(&compiled,pattern,REG_EXTENDED);

    regmatch_t pmatch[1];
    int status = regexex(&compiled,target,1,pmatch,REG_NOTEOL);
    if(status == REG_NOMATCH){
        return 0;
    }else {
        
    }

    regfree(&compiled);
}