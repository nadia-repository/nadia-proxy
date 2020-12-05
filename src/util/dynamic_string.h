#ifndef  DYNAMIC_STRING_H 
#define DYNAMIC_STRING_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csapp.h"

#define STRING_DEFAULT_INITIAL_CAPACITY 20
#define STRING_DEFAULT_LOAD_FACTOR 0.75

// #define INIT_STRING init_string(0)
// #define GET_STRING(dest) dest->get_string(dest)
// #define VALUE_CHAR(dest,src) dest->value_char(dest,src)
// #define VALUE_CHARS(dest,src) dest->value_chars(dest,src)
// #define JOIN_CHAR(dest,src) dest->join_char(dest,src)
// #define JOIN_CHARS(dest,src) dest->join_chars(dest,src)
// #define JOIN_STRING(dest,src) dest->join_string(dest,src)

typedef struct string_struct{
    char *string;
    int size;
    int length;

    char *(* get_string)(struct string_struct *dest);
    void (* value_char)(struct string_struct *dest, char src);
    void (* value_chars)(struct string_struct *dest, char *src);
    void (* join_char)(struct string_struct *dest, char src);
    void (* join_chars)(struct string_struct *dest, char *src);
    void (* join_string)(struct string_struct *dest, struct string_struct *src);
} STRING;

STRING *init_string(int size);

char *get_string(STRING *dest);

void value_char(STRING *dest, char src);

void value_chars(STRING *dest, char *src);

void join_char(STRING *dest, char src);

void join_chars(STRING *dest, char *src);

void join_string(STRING *dest, STRING *src);

#endif