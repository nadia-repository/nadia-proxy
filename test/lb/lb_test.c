#include "lb.h"
#include <stdio.h>

static void test_round_robin(void);
static void test_ip_hash(void);

int main(){
    printf("=================================== START test_round_robin\n");
    test_round_robin();
    printf("=================================== END test_round_robin\n");
    return 0;
}

static void test_round_robin(void){
    int count = 0;
    int size = 3;

    int index;
    index = round_robin(&count,size);
    printf("index=%d\n",index);

    index = round_robin(&count,size);
    printf("index=%d\n",index);

    index = round_robin(&count,size);
    printf("index=%d\n",index);

    index = round_robin(&count,size);
    printf("index=%d\n",index);

    index = round_robin(&count,size);
    printf("index=%d\n",index);

    index = round_robin(&count,size);
    printf("index=%d\n",index);

    index = round_robin(&count,size);
    printf("index=%d\n",index);

    index = round_robin(&count,size);
    printf("index=%d\n",index);
}

static void test_ip_hash(void){

}