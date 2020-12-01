#include "match.h"
#include <stdio.h>

int main(){

    int em = exact_match("/trade","/trade");
    printf("exact_match ret=%d\n",em);

    int pm = prefix_match("/trade","/tra");
    printf("prefix_match ret=%d\n",pm);

    int rm = regex_match("^/docu[a-z]+","/doc");
    printf("regex_match ret=%d\n",rm);

    int nm1 = none_match("/trade","/tra");
    printf("none_match1 ret=%d\n",nm1);

    int nm2 = none_match("/trade","/trad");
    printf("none_match2 ret=%d\n",nm2);

    return 0;
}