#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_INITIAL_CAPACITY 16
#define DEFAULT_LOAD_FACTOR 0.75

/*
hashmap中node对象
    hash
*/
typedef struct node_struct{
    int hash;
    int key;
    void *data;
    struct node_struct *next;
} NODE;

/*
hashmap对象结构
    put 绑定put_hashmap方法
    get 绑定get_hashmap方法
*/
typedef struct hashmap_struct{
    void (* put)(struct hashmap_struct *map ,int key,void *value);
    void * (* get)(struct hashmap_struct *map ,int key);
    void (* delete)(struct hashmap_struct *map ,int key);
    unsigned int size;
    unsigned int elements;
    NODE **nodes;
} MAP_INSTANCE;

/*
初始化hashmap
    size 为空时，默认大小为DEFAULT_INITIAL_CAPACITY
*/
MAP_INSTANCE *init_hashmap(unsigned int size);

/*
往hashmap中添加元素
    map hashmap对象
    key
    value
*/
void put_hashmap(MAP_INSTANCE *map ,int key,void *value);

/*
根据key获取value
    map hashmap对象
    key
*/
void *get_hashmap(MAP_INSTANCE *map ,int key);

/*
根据key删除hashcode
    map hashmap对象
    key
*/
void delete_hashmap(MAP_INSTANCE *map, int key);

/*
根据key计算hashcode
    map hashmap对象
    key
*/
int hash(MAP_INSTANCE *map, int key);

/*
超过当前容量时扩容
    map hashmap对象
*/
void resize(MAP_INSTANCE *map);