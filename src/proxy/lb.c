#include "lb.h"

int round_robin(int *count, int size){
    (*count)+=1;
    return (*count) % size;
}

int weighted_round_robin(){

    return 1;
}

int ip_hash(char *ip, int size){
    int hash = 0;
    char *h = ip;
    if(ip != NULL){
        while((*h != '\0')){
            hash += atoi(h);
        }
    }
    return hash % size;
}