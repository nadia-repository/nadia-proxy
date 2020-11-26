#include "proxy.h"
#include "hashmap.h"
#include "thread.h"
#include "worker.c"

#define DEBUG_LOG 0

CP cp;
CS cs;
void signal_handler(int sig);
void create_work_process();

int main(int argc, char **argv, char **envp){

    //nadia默认路径
    char *nadiaPath = "/Users/xiangshi/Documents/workspace_c/nadia-proxy/";
    fprintf(stderr, "Wellcome Naida Proxy \n");
    fprintf(stderr, "Naida config path<%s> \n",nadiaPath);

    //加载配置文件
    char *configPath = "/nadia/config/";
    while (*envp) {
        char *env = *(envp++);
        //从环境变量中获取配置目录
        if(!strcasecmp(env, "NADIA_CONFIG")){
            configPath = env;
            break;
        }
    }
    //初始化配置文件路径
    init_file_path(configPath,&cp);

    //读取代理配置文件
    if(load_proxy(cp.proxyPath,&cs) < 1){
        fprintf(stderr, "Nadia proxy file load faile<%s> \n", cp.proxyPath);
        exit(1);
    }
    //读取日志配置 todo
    // if(load_log(cp.logPath)<1){
    //     fprintf(stderr, "Nadia log file redirect faile<%s> \n", cp.logPath);
    //     exit(1);
    // }
    //读取配置信息配置
    if(load_configs(cp.configsPath)<1){
        fprintf(stderr, "Nadia config file load faile<%s> \n", cp.configsPath);
        exit(1);
    }

    //注册信号事件
    Signal(SIGINT, signal_handler);   /* ctrl-c */
    Signal(SIGCHLD, signal_handler); 
    Signal(SIGTSTP, signal_handler); /* ctrl-z */
    Signal(SIGPIPE, SIG_IGN);//屏蔽SIGPIPE信号，如果服务端向一个已经关闭的客户端发送两次数据，服务端将会受到SIGPIPE信号并终止服务端进程

    //创建子进程处理网络请求
    create_work_process();

    while (1){}
    
    fprintf(stdout, "Main process stop nadia server\n");
    return 0;
}

void create_work_process(){
    pid_t workPid;
    if((workPid = Fork())==0){
        fprintf(stderr, "start worker pid<%d> \n", workPid);
        do_work();
    }
}

/*
信号处理方法，回收内存并退出进程
*/
void signal_handler(int sig) {
    fprintf(stderr, "\nStop nadia server\n");
    // free_proxy(&cs);
    // destroy_pthoread_pool(&ts);
    exit(0);
}