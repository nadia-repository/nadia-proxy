#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_INITIAL_CAPACITY 20
#define DEFAULT_LOAD_FACTOR 0.75

typedef struct string_struct{
    char *string;
    int size;
    int length;

    char *(* get_string)(STRING *dest);
    void (* value_char)(STRING *dest, char src);
    void (* value_chars)(STRING *dest, char *src);
    void (* join_char)(STRING *dest, char src);
    void (* join_chars)(STRING *dest, char *src);
    void (* join_string)(STRING *dest, STRING *src);
} STRING;

STRING *init_string(int size);

char *get_string(STRING *dest);

void value_char(STRING *dest, char src);

void value_chars(STRING *dest, char *src);

void join_char(STRING *dest, char src);

void join_chars(STRING *dest, char *src);

void join_string(STRING *dest, STRING *src);