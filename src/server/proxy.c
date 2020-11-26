#include "proxy.h"

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
    LS ** locations = server->locations;
    for(int i = 0;i< (server->locationSize);i++){
        char *pattern = (locations[i])->pattern;
        fprintf(stdout, "Server do proxy is pattern=%s \n",pattern);

        int isStatic = (locations[i])->isStatic;
        fprintf(stdout, "Server do proxy is static=%d \n",isStatic);

        //todo 正则匹配
        if(isStatic){
            //静态资源代理
            sdi->isStatic = 1;
            sdi->filePath = (locations[i])->sps->root;
            
            sdi->fileName = "home.html";

            return 1;
        }else {
            //动态资源代理
            sdi->isStatic = 0;



            return 1;
        }
        
    }
    return 0;
}

void serve_static(int fd ,SDI *sdi) {
    struct stat sbuf;
    int srcfd;
    char *srcp, filetype[MAXLINE], buf[MAXBUF];

    char *file = (char *) malloc(strlen(sdi->filePath) + strlen(sdi->fileName));
    strcpy(file,sdi->filePath);
    strcat(file,sdi->fileName);
  
    if (stat(file, &sbuf) < 0) {
        clienterror(fd, file, "404", "Not found",
                    "Nadia couldn't find this file");
        return;
    }  
    //写入http头部信息
    get_filetype(file, filetype);    
    sprintf(buf, "HTTP/1.0 200 OK\r\n"); 
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server: Naida Web Server\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n", (int)sbuf.st_size);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: %s\r\n\r\n", filetype);
    Rio_writen(fd, buf, strlen(buf));    
    //打开并映射文件，写回至客户端
    srcfd = Open(file, O_RDONLY, 0); 
    srcp = Mmap(0, sbuf.st_size, PROT_READ, MAP_PRIVATE, srcfd, 0); 
    Close(srcfd);                       
    Rio_writen(fd, srcp, sbuf.st_size);     
    Munmap(srcp, sbuf.st_size);       
    Free(file);      
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