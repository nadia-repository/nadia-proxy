#include "match.h"

int exact_match(char *pattern,char *target){
    if(strcmp(pattern,target)==0){
        return 1;
    }
    return 0;
}

int prefix_match(char *pattern,char *target){
    int cnt;
    char *pa = pattern;
    char *ta = target;
    while((*pa != '\0' && *ta != '\0') && (*pa == *ta)){
        pa++;
        ta++;
        cnt++;
    }
    return cnt;

}

int regex_match(char *pattern,char *target){
    regex_t compiled;

    regcomp(&compiled,pattern,REG_EXTENDED);

    regmatch_t pmatch[1];
    int status = regexex(&compiled,target,1,pmatch,REG_NOTEOL);
    if(status == REG_NOMATCH){
        return 0;
    }else {
        
    }

    regfree(&compiled);
    return 1;
}

int none_match(char *pattern,char *target){
    return prefix_match(pattern,target);
}