#ifndef  __ARRAYLIST_H__
#define __ARRAYLIST_H__ 

#include "csapp.h"
#include <stdlib.h>


#define ARRAYLIST_DEFAULT_INITIAL_CAPACITY 20
#define ARRAYLIST_DEFAULT_LOAD_FACTOR 0.75

#define ARRAYLIST_INIT init_arraylist(0)
#define ARRAYLIST_ADD(list,data) list->add(list,data)
#define ARRAYLIST_ADD_INDEX(list,index,data) list->add_with_index(list,index,data)
#define ARRAYLIST_GET(list,index) list->get(list,index)
#define ARRAYLIST_LENGTH(list) list->arraylist_length(list)
#define ARRAYLIST_SIZE(list) list->arraylist_size(list)

typedef struct arraylist_struct{
    void **elementData;
    int size;
    int length;

    void (* add)(struct arraylist_struct *list, void *data);
    void (* add_with_index)(struct arraylist_struct *list, int index, void *data);
    void *(* get)(struct arraylist_struct *list, int index);
    int (* arraylist_length)(struct arraylist_struct *list);
    int (* arraylist_size)(struct arraylist_struct *list);
} ARRAYLIST;


ARRAYLIST *init_arraylist(int size);

void add_arraylist(ARRAYLIST *list, void *data);

void add_index_arraylist(ARRAYLIST *list, int index, void *data);

void *get_arraylist(ARRAYLIST *list,int index);

int arraylist_length(ARRAYLIST *list);

int arraylist_size(ARRAYLIST *list);

#endif