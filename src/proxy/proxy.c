#include "proxy.h"

static int match_location(char *uri, LMS *lms, SDI *sdi, int (*routine)(char *,char *));
static void read_requesthdrs(rio_t *rp);

void parser_request(int connfd,SS *server){
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
    sdi.rio = rio;
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


int match_proxy(char *method,char *uri,SS *server,SDI *sdi){

    //1.精确匹配 =
    LMS *exactLocation = (LMS *)server->locMap->get(server->locMap,EXACT);
    if(exactLocation != NULL){
        //do精确匹配
        if(match_location(uri,exactLocation,sdi,&exact_match) > 0){
            //匹配成功
            return 1;
        }
    }

    //2.前缀匹配 前缀匹配 ^~
    LMS *prefixLocation = (LMS *)server->locMap->get(server->locMap,PREFIX);
    if(prefixLocation != NULL){
        //do前缀匹配
        if(match_location(uri,prefixLocation,sdi,&prefix_match)){
            //匹配成功
            return 1;
        }
    }

    //3.正则匹配 ~
    LMS *regexLocation = (LMS *)server->locMap->get(server->locMap,REGEX);
    if(regexLocation != NULL){
        //do正则匹配
        if(match_location(uri,regexLocation,sdi,&regex_match)){
            //匹配成功
            return 1;
        }
    }

    //4.无修饰匹配 
    LMS *noneLocation = (LMS *)server->locMap->get(server->locMap,NONE);
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
    LS **locations = lms->locations;

    int i;
    for(i = 0 ; i < locationSize ; i++){
        char *pattern = (locations[i])->pattern;
        if(routine(pattern,uri)){
            sdi->isStatic = (locations[i])->isStatic;
            if(sdi->isStatic){
                //静态代理
                char path[MAXBUF];
                if((locations[i])->sps->alias != NULL){
                    //1.alias = alias + fileName
                    strcpy(path,(locations[i])->sps->alias);
                }else {
                    //2.root = root + uri + fileName
                    strcpy(path,(locations[i])->sps->root);
                    strcat(path,uri);
                }
                if((locations[i])->sps->index != NULL && has_filetype(uri)>0){
                    strcat(path,(locations[i])->sps->index);
                }
                sdi->path = path;
            }else {
                //动态代理
                sdi->dps = (locations[i])->dps;
            }
            return 1;
        }
    }
    return 0;
}


