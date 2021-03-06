#include "stack.h"

static void resize(STACK *stack);

STACK *init_stack(unsigned int size){
    if(size==0){
        size = STACK_DEFAULT_INITIAL_CAPACITY;
    }
    STACK *stack_instance = malloc(sizeof(STACK));
    stack_instance->pop = &pop_stack;
    stack_instance->push = &push_stack;
    stack_instance->size = size;
    stack_instance->sp = -1;
    void **nodes = calloc(stack_instance->size,sizeof(void *));
    stack_instance->nodes = nodes;

    return stack_instance;
}


void push_stack(STACK *stack, void *node){
    if(stack->sp == stack->size){
        resize(stack);
    }
    stack->sp +=1;
    stack->nodes[stack->sp] = node;
}

void *pop_stack(STACK *stack){
    if(stack->sp < 0){
        return NULL;
    }
    void *node = stack->nodes[stack->sp];
    stack->sp -=1;
    return node;
}

static void resize(STACK *stack){
    unsigned int new_size = stack->size + (stack->size * STACK_DEFAULT_LOAD_FACTOR);

    void **new_nodes = calloc(new_size,sizeof(void *));

    for(int i =0;i<stack->size;i++){
        new_nodes[i] = (stack->nodes)[i];
    }
    Free(stack->nodes);

    stack->size = new_size;
    stack->nodes = new_nodes;
}