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

int main(int argc, char **argv){
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

/************************************************************************启动命令相关 start*/

static void usage(void){
    fprintf(stderr, "Usage: nadia proxy [-htr]\n");
    fprintf(stderr, "Options\n");
    fprintf(stderr, "\t-h         Print this message.\n");
    fprintf(stderr, "\t-t         Stop nadia proxy.\n");
    fprintf(stderr, "\t-r         Restart nadia proxy.\n");
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
    Signal(SIGILL, signal_handler);   /* ctrl-c */
    Signal(SIGPIPE, SIG_IGN);//屏蔽SIGPIPE信号，如果服务端向一个已经关闭的客户端发送两次数据，服务端将会受到SIGPIPE信号并终止服务端进程
    //回收工作进程&重新拉起工作进程
    reap_worker();
    fprintf(stdout, "Main process stop nadia server\n");
}

/*
重启nadia proxy
*/
static void reload(void){
    //检查当前nadia proxy主进程是否存在

    //存在时向nadia proxy主进程发送重新加载信号
    Kill(workPid,SIGUSER1);
}

/*
停止nadia proxy
*/
static void shotdown(void){
    fprintf(stdout, "Start shotdown workPid=%d \n", workPid);
    //停止工作进程，主线程回收工作进程后退出
    Kill(workPid,SIGILL);
}

/************************************************************************启动命令相关 end*/


/************************************************************************信号处理相关 start*/

/*
信号处理方法，回收内存并退出进程
*/
void signal_handler(int sig) {
    fprintf(stdout, "\nStop nadia server\n");
    // free_proxy(&cs);
    // destroy_pthoread_pool(&ts);
    shotdown();
}

/*
信号处理方法，重新加载配置，重新启动工作进程
*/
void main_reload_handler(int sig){
    fprintf(stdout, "\n main_reload_handler \n");
    //重新加载配置
    load();
    //结束当前工作进程，并回收后重新拉起工作进程
    Kill(workPid,SIGTSTP);
}

/************************************************************************信号处理相关 end*/

/*
回收工作进程
    1.工作线程非正常退出，或者正常退出状态为1（重启服务）时，重新拉起工作进程
    2.工作线程退出后，退出主进程
*/
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

/*
配置文件等加载流程
*/
static void load(void){
    //nadia默认路径
    char *nadiaPath = "/nadia/config/";
    fprintf(stderr, "Naida config path<%s> \n",nadiaPath);
    //加载nadia全部配置
    load_nadia_config(nadiaPath,&nadiaConfig);
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