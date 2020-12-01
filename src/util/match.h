#include <regex.h>
#include <string.h>

/*
匹配代理路径
    参数
        pattern 代理路径表达式
        target 请求uri
    返回 
        0 未匹配
        1 匹配
*/
int exact_match(char *pattern,char *target);

int prefix_match(char *pattern,char *target);

int regex_match(char *pattern,char *target);

int none_match(char *pattern,char *target);