#include "proxy_static.h"

static void request_static(int fd, char *filename, int filesize);


void serve_static(int fd ,SDI *sdi) {
    struct stat sbuf;

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
    srcfd = Open(filename, O_RDONLY, 0); 
    srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0); 
    Close(srcfd);                       
    Rio_writen(fd, srcp, filesize);     
    Munmap(srcp, filesize);       
}

