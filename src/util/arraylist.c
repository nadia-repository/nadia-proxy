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
    list->add_with_index = &add_index_arraylist;
    list->get = &get_arraylist;
    // list->arraylist_length = &arraylist_length;
    // list->arraylist_size = &arraylist_size;
    return list;
}

void add_arraylist(ARRAYLIST *list, void *data){
    if(list->length >=list->size){
        resize(list);
    }
    list->elementData[list->length++] = data;
}

void add_index_arraylist(ARRAYLIST *list, int index, void *data){
    if(index > list->size || index < 0){
        return;
    }
    list->elementData[index] = data;
}


void *get_arraylist(ARRAYLIST *list, int index){
    if(list->length <= 0){
        return NULL;
    }
    return list->elementData[index];
}

int arraylist_length(ARRAYLIST *list){
    if(list == NULL){
        return 0;
    }
    return list->length;
}

int arraylist_size(ARRAYLIST *list){
    return list->size;
}

static void resize(ARRAYLIST *list){
    int size = list->size + list->size * ARRAYLIST_DEFAULT_LOAD_FACTOR;
    void **elementData = calloc(size,sizeof(void *));
    for(int i = 0;i<list->size;i++){
        elementData[i] = list->elementData[i];
    }
    Free(list->elementData);
    list->elementData = elementData;
    list->size = size;
}