#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_INITIAL_CAPACITY 16
#define DEFAULT_LOAD_FACTOR 0.75

/*
stack对象结构
    push 绑定push_stack方法
    pop 绑定pop_stack方法
    size stack大小
    sp 栈顶
*/
typedef struct stack_struct{
    void (* push)(struct stack_struct *, void *);
    void * (* pop)(struct stack_struct *);
    int size;
    int sp;
    void **nodes;
} STACK_INSTANCE;

/*
初始化stack实例
    参数
        size 栈大小
    返回 
        stack实例
*/
STACK_INSTANCE *init_stack(unsigned int size);

/*
入栈
    stack stack实例
    node 节点
*/
void push_stack(STACK_INSTANCE *stack, void *node);

/*
出栈
    参数
        stack stack实例
    返回
        节点
*/
void *pop_stack(STACK_INSTANCE *stack);