typedef struct hashmap_element{
    void *data;
    void *next;
} HE;

typedef struct hashmap_struct{
    int DEFAULT_CAPACITY = 10;
    int size;
    HE elementData[10];
} HM;

int put(HM *map,void *key,void *value);

void* get();