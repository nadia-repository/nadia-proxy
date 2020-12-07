#ifndef  ARRAYLIST_H 
#define ARRAYLIST_H 

#include <stdlib.h>

#define ARRAYLIST_DEFAULT_INITIAL_CAPACITY 20
#define ARRAYLIST_DEFAULT_LOAD_FACTOR 0.75

#define ARRAYLIST_INIT init_arraylist(0)
#define ARRAYLIST_ADD(list,data) list->add(list,data)
#define ARRAYLIST_GET(list,data) list->get(list,data)
#define ARRAYLIST_LENGTH(list) list->length(list)

typedef struct arraylist_struct{
    void **elementData;
    int size;
    int length;

    void (* add)(ARRAYLIST *list, void *data);
    void *(* get)(ARRAYLIST *list);
    int (* length)(ARRAYLIST *list);
} ARRAYLIST;


ARRAYLIST *init_arraylist(int size);

void add_arraylist(ARRAYLIST *list, void *data);

void *get_arraylist(ARRAYLIST *list);

int arraylist_length(ARRAYLIST *list);


#endif