#include "arraylist.h"
#include <stdio.h>

int main(){
    ARRAYLIST * list = ARRAYLIST_INIT;

    for(int i = 0;i<30;i++){
        int *v = malloc(sizeof(int));
        *v= i;
        ARRAYLIST_ADD(list,v);
    }
    printf("list size=%d  length=%d\n",ARRAYLIST_SIZE(list),ARRAYLIST_LENGTH(list));

    for(int i=0;i<ARRAYLIST_LENGTH(list);i++){
        int *v = (int *)ARRAYLIST_GET(list,i);
        printf("print elementData index=%d  data=%d\n",i,*v);
    }

    return 0;
}