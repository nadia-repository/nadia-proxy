#include "hashmap.h"

int main(){

    MAP_INSTANCE *map = init_hashmap(0);
    printf("map size =%d \n",map->size);

    int key = 1;
    int key2 = 17;
    char *value = "123456";
    char *value2 = "789067";
    char *value3 = "sdfsdfsfsd";

    map->put(map,key,value);
    map->put(map,key,value2);
    map->put(map,key2,value3);

    void * vv = map->get(map,key);
    printf("get map key=%d  value=%s \n",key,(char *)vv);

    vv = map->get(map,key2);
    printf("get map key2=%d  value=%s \n",key2,(char *)vv);
    return 0;
}