#include "worker.h"

NADIA_CONFIG nadiaConfig;
pid_t workPid;

void signal_handler(int sig);
void main_reload_handler(int sig);

static void usage(void);
static void shotdown(void);
static void load(void);
static void launch(void);
static void reload(void);
static void fire(void);

int main(int argc, char **argv, char **envp){
    char c;
    while ((c = getopt(argc, argv, "htr")) != EOF) {
        switch(c){
            case 't':
                shotdown();
                break;
            case 'r':
                reload();
                break;
            case 'h':
                usage();
                exit(0);
            default:
                launch();
        }
    }
    launch();
    return 0;
}



/*
信号处理方法，回收内存并退出进程
*/
void signal_handler(int sig) {
    fprintf(stdout, "\nStop nadia server\n");
    // free_proxy(&cs);
    // destroy_pthoread_pool(&ts);
    shotdown();
}

void main_reload_handler(int sig){
    fprintf(stdout, "\n main_reload_handler \n");
    load();
    Kill(workPid,SIGTSTP);
}

void reap_worker(){
    fprintf(stdout, "Start reap worker process \n");
    pid_t pid;
    int status;
    while(1){
        while ((pid = waitpid(-1, &status, 0)) > 0){
            fprintf(stdout, "Reap worker status=%d\n",status);
            if(!WIFEXITED(status) || 
                (WIFEXITED(status) && WEXITSTATUS(status)==1)){
                fprintf(stdout, "Reap worker and reFire worker process  status=%d\n",status);
                fire(); //工作进程异常终止,或者由于reload终止时，重新拉起工作线程
            }else {
                fprintf(stdout, "Reap worker stop main process  status=%d\n",status);
                exit(0);
            }
        }
        if (errno != ECHILD)
            unix_error("waitpid error");
    }

}

static void usage(void){
    fprintf(stderr, "Usage: nadia proxy [-htr]\n");
    fprintf(stderr, "Options\n");
    fprintf(stderr, "\t-h         Print this message.\n");
    fprintf(stderr, "\t-t         Stop nadia proxy.\n");
    fprintf(stderr, "\t-r         Restart nadia proxy.\n");
}

static void load(void){
    //nadia默认路径
    char *nadiaPath = "/nadia/config/";
    // while (*envp) {
    //     char *env = *(envp++);
    //     //从环境变量中获取配置目录
    //     if(!strcasecmp(env, "NADIA_CONFIG")){
    //         nadiaPath = env;
    //         break;
    //     }
    // }
    fprintf(stderr, "Naida config path<%s> \n",nadiaPath);
    //加载nadia全部配置
    load_nadia_config(nadiaPath,&nadiaConfig);
}

/*
启动nadia proxy
*/
static void launch(void){
    fprintf(stderr, "Wellcome Naida Proxy \n");
    //加载配置
    load();
    //启动工作进程
    fire();
    //处理信号
    Signal(SIGINT, signal_handler);   /* ctrl-c */
    Signal(SIGTSTP, signal_handler); /* ctrl-z */
    Signal(SIGPIPE, SIG_IGN);//屏蔽SIGPIPE信号，如果服务端向一个已经关闭的客户端发送两次数据，服务端将会受到SIGPIPE信号并终止服务端进程
    //回收工作进程&重新拉起工作进程
    reap_worker();

    fprintf(stdout, "Main process stop nadia server\n");
}

static void reload(void){
    //检查当前nadia proxy主进程是否存在

    //存在时向nadia proxy主进程发送重新加载信号
    
    //不存在时启动服务
}

static void shotdown(void){
    fprintf(stdout, "Start shotdown workPid=%d \n", workPid);
    Kill(workPid,SIGILL);
}

/*
创建工作进程
*/
static void fire(void){
    if((workPid = Fork())==0){
        do_work();
    }
    fprintf(stdout, "start worker pid<%d> \n", workPid);
}