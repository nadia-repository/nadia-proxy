#include "proxy.h"


void serve_static(int fd ,SDI *sdi) {
    struct stat sbuf;
    int srcfd;
    char *srcp, filetype[MAXLINE], buf[MAXBUF],*filename = sdi->path;
    fprintf(stdout, "Server static file=%s\n",filename);
    if (stat(filename, &sbuf) < 0) {
        clienterror(fd, filename, "404", "Not found",
                    "Nadia couldn't find this file");
        return;
    }  
    //写入http头部信息
    get_filetype(filename, filetype);    
    sprintf(buf, "HTTP/1.0 200 OK\r\n"); 
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server: Naida Web Server\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n", (int)sbuf.st_size);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: %s\r\n\r\n", filetype);
    Rio_writen(fd, buf, strlen(buf));    
    //打开并映射文件，写回至客户端
    fprintf(stdout, "Open static file=%s\n",filename);
    srcfd = Open(filename, O_RDONLY, 0); 
    srcp = Mmap(0, sbuf.st_size, PROT_READ, MAP_PRIVATE, srcfd, 0); 
    Close(srcfd);                       
    Rio_writen(fd, srcp, sbuf.st_size);     
    Munmap(srcp, sbuf.st_size);       
    Free(filename);      
}

int main(){
    SDI sdi;
    sdi.path = "/workspace_c/nadia-proxy/home.html";
    serve_static(stdout,&sdi);

    return 0;
}