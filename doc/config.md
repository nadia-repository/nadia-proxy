## 配置文件的读取与解析

### 配置文件的结构介绍
nadia-proxy的代理配置文件参考了nginx的配置文件，并进行了一定的精简。

``` json
http {
    servers {
        listen 80;
        location / {
            root /var/html1;
            index index.html;
        }
        location = /exact {
            alias /var/html2;
        }
        location ^~ /prefix {
            proxy {
                strategy ROUND_ROBIN;
                server 127.0.0.1:8001;
                server 127.0.0.1:8002;
                server 127.0.0.1:8003;
            }
        }
        location ^~ /prefix {
            proxy {
                strategy WEIGHTED_ROUND_ROBIN;
                server 127.0.0.1:8001 weight=1;
                server 127.0.0.1:8002 weight=2;
                server 127.0.0.1:8003 weight=3;
            }
        }
    }
    servers {
        listen 4321;
        location / {
            proxy {
                strategy IP_HASH;
                server 127.0.0.1:8001;
                server 127.0.0.1:8002;
                server 127.0.0.1:8003;
            }
        }
    }
}
```

|节点|描述|层级|
|---|---|---|
|http|配置http协议的代理|0|
|servers|配置各个服务的代理|1|
|listen|需要被代理服务对外暴露的端口|2|
|location|代理服务相关的路径信息|2|
|root|静态服务代理的文件夹，不包含uri路径|3|
|alias|静态服务代理的文件夹，包含uri路径|3|
|index|静态服务代理的默认页面|3|
|proxy|动态代理的配置信息|3|
|strategy|动态代理的负载策略，默认为轮训|4|
|server|动态代理的服务列表|4|


### 如何读取配置文件
通过上面的介绍我们可以发现，配置文件是以层级的形式所体现的。为此我们为各个节点设计了数据结构，并已”伙伴“树的形式形成一颗语法书。

* 配置文件节点的数据结构

``` C
enum config_node_type {PARENT,CHILD}; //为合并语法树标识该节点的层级属性

typedef struct config_node_struct{
    enum config_node_type node_type;
    void *content; //当前节点的字符串

    struct config_node_struct *friend; //同级别的节点
    struct config_node_struct *child;  //孩子节点
} CONFIG_NODE;
```

### 文件解析
采用自顶向下的解析方式，按照每行逐字符解析字符：
1. 遇到"{"字符时，向配置节点栈中压入一个PARENT类型的节点。
2. 遇到";"字符时，向配置节点栈中压入一个CHILD类型的节点。
3. 遇到"}"字符时，对之前压入配置节点栈中的节点依次弹出，同为CHILD的节点形成伙伴节点，知道遇到PARENT节点后，将所有的CHILD节点挂载至父节点中，并将PARENT已CHILD节点的身份重新入栈。

重复上述1-3的流程，直至文件解析完成。



### 节点的语法树解析
直至上一个步骤，我们将配置文件读取完成并解析成一颗包含配置信息的语法树。

目前我们无法得知各个节点的具体配置内容及其配置信息，并且也无法得知解析的配置项是否满足我们的配置要求。

为此我们需要一种机制，自动解析并生成我们最终定义的配置数据结构。这里我们采用方式为有限状态机(finite-state machine)。

### 有限状态机(finite-state machine)
有限状态机能很好的将我们需要解析的配置收束至一个我们可控的范围。我们简化了状态机的工作原理，主要采用状态机的状态流转性。已配置语法树的前置遍历来驱动状态机的扭转。

* 有限状态机的数据结构

``` C
enum state {INIT,HTTP,SERVERS,LISTEN,LOCATION,ROOT,ALIAS,INDEX,STRATEGY,PROXY,SERVER}; //状态机中的各个状态


typedef struct finite_state_machine {
    enum state current_state; //当前状态
    char *tag; //状态名
    void *(* parse)(char *line,void *config); //当前状态的执行器

    ARRAYLIST *child_state_list; //子状态列表
    ARRAYLIST *frient_state_list; //同级状态列表
} FSM;
```

代码实现层面，我们通过”伙伴“树的递归遍历，并依靠状态机的列表来流转状态，最终生成我们需要的配置文件结构。

``` C
    CONFIG_NODE  *node = (CONFIG_NODE *)POP_STACK(stack);
    FSM *init = status_machine_arrary[INIT];
    for(int i = 0 ;i < ARRAYLIST_LENGTH(init->child_state_list) ; i++){
        generate_proxy_config(node,nadia_proxy_config,ARRAYLIST_GET(init->child_state_list,i));
    }

static void generate_proxy_config(CONFIG_NODE  *node, void *config, FSM *fsm){
    if(node == NULL){
        return;
    }
    STRING *string = (STRING *)node->content;
    void *node_config = NULL;
    if(strstr(string->string,fsm->tag) != NULL){
        node_config = fsm->parse(string->string,config);
    }
    //parse child
    for(int i = 0 ;i < ARRAYLIST_LENGTH(fsm->child_state_list) ; i++){
        generate_proxy_config(node->child,node_config,ARRAYLIST_GET(fsm->child_state_list,i));
    }
    //parse friend
    for(int i = 0 ;i < ARRAYLIST_LENGTH(fsm->frient_state_list) ; i++){
        generate_proxy_config(node->friend,config,ARRAYLIST_GET(fsm->frient_state_list,i));
    }
}
```