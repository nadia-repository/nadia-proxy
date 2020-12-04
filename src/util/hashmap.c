#include "hashmap.h"

HASHMAP *init_hashmap(unsigned int size){
    if(size <=0){
        size = DEFAULT_INITIAL_CAPACITY;
    }
    NODE **nodes = calloc(size,sizeof(NODE*));
    HASHMAP *map = malloc(sizeof(HASHMAP));
    map->size = size;
    map->elements = 0;
    map->nodes = nodes;
    map->put = &put_hashmap;
    map->get = &get_hashmap;
    map->delete = &delete_hashmap;
    return map;
}

//todo 需要考虑并发安全
void put_hashmap(HASHMAP *map ,int key,void *value){
    if(DEBUG_INFO)
        fprintf(stdout, "into put_hashmap key=%d \n", key);
    int hashcode = hash(map,key);
    NODE *headNode,*entry;
    headNode = entry = (map->nodes)[hashcode];
    
    while (1){
        if(entry == NULL){
            if(DEBUG_INFO)
                fprintf(stdout, "put_hashmap insert node hashcode=%d key=%d \n", hashcode,key);
            NODE *newNode = malloc(sizeof(NODE));
            newNode->data = value;
            newNode->hash = hashcode;
            newNode->key = key;
            newNode->next = headNode;
            NODE **nodes = map->nodes;
            nodes[hashcode] = newNode;
    
            map->elements += 1;
            return;
        }else if(entry->hash == hashcode && entry->key == key){
            if(DEBUG_INFO)
                fprintf(stdout, "put_hashmap update node hashcode=%d key=%d \n", hashcode,key);
            entry->data = value;
            return;
        }else {
            entry = entry->next;
        }
    }
}

//todo 需要考虑并发安全
void *get_hashmap(HASHMAP *map ,int key){
    if(DEBUG_INFO)
        fprintf(stdout, "into get_hashmap key=%d \n", key);
    int hashcode = hash(map,key);
    NODE *entry;
    entry = (map->nodes)[hashcode];
    while(1){
        if(entry == NULL){
            return NULL;
        }else if(entry->hash == hashcode && entry->key == key){
            return entry->data;
        }else{
            entry = entry->next;
        }
    }
}

void delete_hashmap(HASHMAP *map, int key){
    if(DEBUG_INFO)
        fprintf(stdout, "into delete_hashmap key=%d \n", key);
    int hashcode = hash(map,key);
    NODE *pre,*entry;
    pre = entry = (map->nodes)[hashcode];
    while(1){
        if(entry == NULL){
            return;
        }else if(entry->hash == hashcode && entry->key == key){
            pre->next = entry->next;
            free(entry);
            map->elements -=1;
            return;
        }else{
            pre = entry;
            entry = entry->next;
        }
    }
}

int hash(HASHMAP *map ,int key){
    unsigned int size = map->size;
    return key % size;
}

void resize(HASHMAP *map){
    
}