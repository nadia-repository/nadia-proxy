#include "dynamic_string.h"

static void resize(STRING *dest);
static int chars_size(char *cs);

STRING *init_string(int size){
    if(size==0){
        size = STRING_DEFAULT_INITIAL_CAPACITY;
    }
    STRING *string = malloc(sizeof(STRING));
    string->size = size;
    string->length = 0;

    string->get_string = &get_string;
    string->value_char = &value_char;
    string->value_chars = &value_chars;
    string->join_char = &join_char;
    string->join_chars = &join_chars;
    string->join_string = &join_string;

    return string;
}

char *get_string(STRING *dest){
    char *str = calloc(dest->length,sizeof(char));
    strncpy(str,dest->string,dest->length);
    return str;
}

void value_char(STRING *dest, char src){
    dest->length = 0;
    dest->string = calloc(1,sizeof(char));
    (dest->string)[0] = src;
}

void value_chars(STRING *dest, char *src){
    int size = chars_size(src);
    dest->length = size;
    dest->string = calloc(dest->length,sizeof(char));
    dest->string = src;
}

void join_char(STRING *dest, char src){
    if((dest->length + 1) >= dest->size ){
        resize(dest);
        join_char(dest,src);
        return;
    }
    (dest->string)[dest->length] = src;
    dest->length+=1;
}

void join_chars(STRING *dest, char *src){
    int size = chars_size(src);
    if((dest->length + size) >= dest->size ){
        resize(dest);
        join_chars(dest,src);
        return;
    }
    strcat(dest->string,src);
    dest->length +=size;
}

void join_string(STRING *dest, STRING *src){
        if((dest->length + src->length) >= dest->size ){
        resize(dest);
        join_string(dest,src);
        return;
    }
    strcat(dest->string,src->string);
    dest->length +=(src->length);
}

static void resize(STRING *dest){
    dest->size = dest->size + dest->size * STRING_DEFAULT_LOAD_FACTOR;
    char *string = calloc(dest->size,sizeof(char));
    strcpy(string,dest->string);
    Free(dest->string);
    dest->string = string;
}

static int chars_size(char *cs){
    int i;
    for(i=0;cs[i] != '\0';i++);
    return i;
}