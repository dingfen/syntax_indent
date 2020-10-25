#ifndef _MYSCC_JSON
#define _MYSCC_JSON

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

typedef union Value 
{
    int data;
    const char * str;
    char ch;
    struct JSONobj *obj;
} Value;

typedef enum State {
    V_INT,
    V_STR,
    V_CHAR,
    V_JSON
} State;

typedef struct JSONobj {
    const char* key;
    Value value;
    State state;
    struct JSONobj *next;
} JSONobj;

void initJSON(JSONobj *pj);

void destroyJSON(JSONobj *pj);

void addJSON(JSONobj *pj, const char* key, Value val, State s);

// void add(JSONobj *pj, const char *key, Value val, State s);

void printJSON(JSONobj *obj);


#endif // _MYSCC_JSON