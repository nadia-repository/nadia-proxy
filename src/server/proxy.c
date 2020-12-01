#include "proxy.h"

static int match_location(char *uri, LMS *lms, SDI *sdi, int (*routine)(char *,char *));
static void pares_uri(char *uri,char *fileName, char *uriPath);
static void request_static(int fd, char *filename, int filesize);

void parser_request(int connfd,SS *server){
    rio_t rio;
    
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    Rio_readinitb(&rio, connfd);
    if (!Rio_readlineb(&rio, buf, MAXLINE)){
        return;
    }
    sscanf(buf, "%s %s %s", method, uri, version);
    fprintf(stdout, "Server received method:%s uri:%s version:%s\n",method,uri,version);

    SDI sdi;
    if(match_proxy(method,uri,server,&sdi)<1){
        clienterror(connfd, uri, "404", "Not found",
                    "Nadia couldn't find this page");
    }

    if(sdi.isStatic){
        serve_static(connfd,&sdi);
    } else {
        read_requesthdrs(&rio); 
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
                if((locations[i])->sps->index != NULL){
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


static void pares_uri(char *uri,char *fileName, char *uriPath){

}



void serve_static(int fd ,SDI *sdi) {
    struct stat sbuf;

    fprintf(stdout, "Server static file=%s\n",sdi->path);
    if (stat(sdi->path, &sbuf) < 0) {
        clienterror(fd, sdi->path, "404", "Not found",
                    "Nadia couldn't find this file");
        return;
    }  
    request_static(fd,sdi->path,(int)sbuf.st_size);
}

static void request_static(int fd, char *filename, int filesize){
    int srcfd;
    char *srcp, filetype[MAXLINE], buf[MAXBUF];
    //写入http头部信息
    get_filetype(filename, filetype);    
    sprintf(buf, "HTTP/1.0 200 OK\r\n"); 
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server: Naida Web Server\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n", filesize);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: %s\r\n\r\n", filetype);
    Rio_writen(fd, buf, strlen(buf));    
    //打开并映射文件，写回至客户端
    fprintf(stdout, "Open static file=%s\n",filename);
    srcfd = Open(filename, O_RDONLY, 0); 
    srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0); 
    Close(srcfd);                       
    Rio_writen(fd, srcp, filesize);     
    Munmap(srcp, filesize);       
    Free(filename);      
}



/*
服务端文件不存在时返回错误页
*/
void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg) {
    char buf[MAXLINE];

    /* Print the HTTP response headers */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n\r\n");
    Rio_writen(fd, buf, strlen(buf));

    /* Print the HTTP response body */
    sprintf(buf, "<html><title>Nadia Error</title>");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<body bgcolor=""ffffff"">\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<hr><em>The Nadia Web server</em>\r\n");
    Rio_writen(fd, buf, strlen(buf));
}

void get_filetype(char *filename, char *filetype) {
    if (strstr(filename, ".html"))
        strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif"))
        strcpy(filetype, "image/gif");
    else if (strstr(filename, ".png"))
        strcpy(filetype, "image/png");
    else if (strstr(filename, ".jpg"))
        strcpy(filetype, "image/jpeg");
    else
        strcpy(filetype, "text/plain");
}

void read_requesthdrs(rio_t *rp) {
    char buf[MAXLINE];
    //需要解析length等信息，并且需要缓存
    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
    while (strcmp(buf, "\r\n")) {          //line:netp:readhdrs:checkterm
        Rio_readlineb(rp, buf, MAXLINE);
        printf("%s", buf);
    }
    return;
}