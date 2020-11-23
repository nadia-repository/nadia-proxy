#include "hashmap.h"

int main(){

    MAP_INSTANCE *map = init_hashmap(0);
    printf("map size =%d \n",map->size);

    int key = 1;
    char *value = "123456";
    char *value2 = "123456";
    map->put(map,key,value);
    map->put(map,key,value2);
    map->put(map,11,value2);

    // void * vv = map->get(map,key);
    // printf("map key=%d  value=%s \n",key,(char *)vv);
    return 0;
}