#include "proxy.h"

static int match_proxy(char *method,char *uri,SERVERS_CONFIG *server,SDI *sdi);
static int match_location(char *uri, LMS *lms, SDI *sdi, int (*routine)(char *,char *));

void parser_request(int connfd,SERVERS_CONFIG *server){
    rio_t rio;
    
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    Rio_readinitb(&rio, connfd);
    if (!Rio_readlineb(&rio, buf, MAXLINE)){
        return;
    }
    sscanf(buf, "%s %s %s", method, uri, version);
    fprintf(stdout, "Server received method:%s uri:%s version:%s\n",method,uri,version);

    //开始匹配代理信息
    SDI sdi;
    sdi.method = method;
    sdi.uri = uri;
    sdi.version = version;
    sdi.connfd = connfd;
    sdi.rio = &rio;
    if(match_proxy(method,uri,server,&sdi)<1){
        clienterror(connfd, uri, "404", "Not found",
                    "Nadia couldn't find this page");
    }

    if(sdi.isStatic){
        serve_static(&sdi);
    } else {
        serve_dynamic(&sdi);
    }
}

/*
根据当前请求匹配代理信息
    method 请求method
    uri 资源地址
    server 当前监听地址代理信息
    sds 代理目标
*/
static int match_proxy(char *method,char *uri,SERVERS_CONFIG *server,SDI *sdi){
    //1.精确匹配 =
    LMS *exactLocation = (LMS *)GET_HASHMAP(server->location_map,EXACT);
    if(exactLocation != NULL){
        //do精确匹配
        if(match_location(uri,exactLocation,sdi,&exact_match) > 0){
            //匹配成功
            return 1;
        }
    }
    //2.前缀匹配 前缀匹配 ^~
    LMS *prefixLocation = (LMS *)GET_HASHMAP(server->location_map,PREFIX);
    if(prefixLocation != NULL){
        //do前缀匹配
        if(match_location(uri,prefixLocation,sdi,&prefix_match)){
            //匹配成功
            return 1;
        }
    }
    //3.正则匹配 ~
    LMS *regexLocation = (LMS *)GET_HASHMAP(server->location_map,REGEX);
    if(regexLocation != NULL){
        //do正则匹配
        if(match_location(uri,regexLocation,sdi,&regex_match)){
            //匹配成功
            return 1;
        }
    }
    //4.无修饰匹配 
    LMS *noneLocation = (LMS *)GET_HASHMAP(server->location_map,NONE);
    if(noneLocation != NULL){
        //do无修饰匹配 
        if(match_location(uri,noneLocation,sdi,&none_match)){
            //匹配成功
            return 1;
        }
    }
    return 0;
}

/*
匹配代理规则
    uri 请求地址
    lms 代理信息map
    sdi 代理解析结果
    routine 匹配方法
*/
static int match_location(char *uri, LMS *lms, SDI *sdi, int (*routine)(char *,char *)){
    uint16_t locationSize = lms->locationSize;
    LOCATION_CONFIG **locations = lms->locations;

    int i;
    for(i = 0 ; i < locationSize ; i++){
        char *pattern = (locations[i])->pattern;
        if(routine(pattern,uri)){
            sdi->isStatic = (locations[i])->isStatic;
            if(sdi->isStatic){
                //静态代理
                char path[MAXBUF];
                if((locations[i])->static_proxy->alias != NULL){
                    //1.alias = alias + fileName
                    strcpy(path,(locations[i])->static_proxy->alias);
                }else {
                    //2.root = root + uri + fileName
                    strcpy(path,(locations[i])->static_proxy->root);
                    strcat(path,uri);
                }
                if((locations[i])->static_proxy->index != NULL && has_filetype(uri)==0){
                    strcat(path,(locations[i])->static_proxy->index);
                }
                sdi->path = path;
            }else {
                //动态代理
                sdi->server_proxy = (locations[i])->server_proxy;
            }
            return 1;
        }
    }
    return 0;
}


