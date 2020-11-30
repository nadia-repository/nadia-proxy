#include "match.h"

int exact_match(char *pattern,char *target){
    // regex_t compiled;

    // regcomp(&compiled,pattern,REG_EXTENDED);

    // regmatch_t pmatch[1];
    // int status = regexex(&compiled,target,1,pmatch,REG_NOTEOL);
    // if(status == REG_NOMATCH){
    //     return 0;
    // }else {
        
    // }

    // regfree(&compiled);
    return 1;
}

int prefix_match(char *pattern,char *target){

    return 1;

}

int regex_match(char *pattern,char *target){

    return 1;
}

int none_match(char *pattern,char *target){

    return 1;
}