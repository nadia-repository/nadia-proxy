#include "arraylist.h"

static void resize(ARRAYLIST *list);

ARRAYLIST *init_arraylist(int size){
    if(size == 0){
        size = ARRAYLIST_DEFAULT_INITIAL_CAPACITY;
    }
    ARRAYLIST *list = malloc(sizeof(ARRAYLIST));
    list->size = size;
    list->elementData = calloc(list->size,sizeof(void *));
    list->length = 0;
    list->add = &add_arraylist;
    list->get = &get_arraylist;
    list->length = &arraylist_length;
    return list;
}

void add_arraylist(ARRAYLIST *list, void *data){
    if(list->length >=list->size){
        resize(list);
    }
    list->elementData[list->length++] = data;
}


void *get_arraylist(ARRAYLIST *list){
    if(list->length <= 0){
        return NULL;
    }
    return list->elementData[list->length--];
}

int arraylist_length(ARRAYLIST *list){
    return list->length;
}

static void resize(ARRAYLIST *list){

}