#include "dyn.h"


/* 全局变量 */
DynString tkstr;        // 单词字符串
DynString sourcestr;    // 单词源码字符串
int tkvalue;            // 单词的词法值

/**
 * 初始化动态字符串存储容量
 * pstr 动态字符串存储结构
 * initsize 初始化分配的大小
 */ 
void dynstring_init(DynString *pstr, int initsize)
{
    if (pstr)
    {
        pstr->data = (char*)malloc(sizeof(char)*initsize);
        pstr->count = 0;
        pstr->capacity = initsize;
    }
}

/**
 * 释放动态字符串使用的空间
 * pstr 动态字符串存储结构
 */
void dynstring_free(DynString *pstr)
{
    if (pstr->data)
        free(pstr->data);
    pstr->capacity = 0;
    pstr->count = 0;
} 

/**
 * 重置动态字符串，先释放，再分配
 * pstr 动态字符串存储结构
 */
void dynstring_reset(DynString *pstr)
{
    dynstring_free(pstr);
    dynstring_init(pstr, 16);
}

/**
 * pstr 动态字符串存储结构
 * new_size 字符串新长度
 */
void dynstring_realloc(DynString *pstr, int new_size) 
{
    int capacity;
    char *data;
    capacity = pstr->capacity;
    while(capacity < new_size)
    {
        capacity = capacity * 2;
    }
    data = (char *)realloc(pstr->data, capacity);
    if (!data)
    {
        error("memory alloc failed.");
    }
    pstr->capacity = capacity;
    pstr->data = data;
}

/**
 * 追加字符到动态字符串中
 * pstr 动态字符串存储结构
 * ch 追加啊的字符
 */
void dynstring_append(DynString *pstr, char ch)
{
    int count;
    count = pstr->count+1;
    if (count > pstr->capacity)
        dynstring_realloc(pstr, count);
    pstr->data[count-1] = ch;
    pstr->count = count;
}

/**
 * 初始化动态数组存储容量
 * parr 动态数组存储结构
 * initsize 动态数组初始化分配空间
 */
void dynarray_init(DynArray *parr, int initsize) 
{
    if (parr)
    {
        parr->data = (void**)malloc(sizeof(char)*initsize);
        parr->capacity = initsize;
        parr->count = 0;
    }
}

/**
 * 释放动态数组使用的内存空间
 * parr 动态数组存储结构
 */
void dynarray_free(DynArray *parr)
{
    void **p;
    for(p=parr->data;parr->count;++p, --parr->count)
    {
        if (*p)
            free(*p);
    }
    free(parr->data);
    parr->data = NULL;
}

/**
 * 动态数组元素查找
 * parr 动态数组存储结构
 * key 要查找的元素
 */
int dynarray_search(DynArray *parr, int key)
{
    int i;
    int **p;
    p = (int**)parr->data;
    for(i = 0; i < parr->count; i++, p++)
        if (key == **p)
            return i;
    return -1;
}

/**
 * 重新分配动态数组容量
 * parr 动态数组存储结构
 * new_size 动态数组新的大小
 */
void dynarray_realloc(DynArray *parr, int new_size)
{
    int capacity;
    void **data;
    capacity = parr->capacity;
    while (capacity < new_size)
    {
        capacity = capacity * 2;
    }
    if (!data)
    {
        error("memory alloc failed.");
    }
    data = (void**)realloc(parr->data, capacity);
    parr->capacity = capacity;
    parr->data = data;
}

/**
 * 追加动态数组元素
 * parr 动态数组存储结构
 * data 追加的元素
 */
void dynarray_add(DynArray *parr, void *data)
{
    int count;
    count = parr->count+1;
    if (count * sizeof(void*) > parr->capacity)
        dynarray_realloc(parr, count * sizeof(void*));
    parr->data[count-1] = data;
    parr->count = count;
}