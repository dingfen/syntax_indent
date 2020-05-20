// C语言中没有提供动态存储下的字符串和数组的便捷操作
// 需要自己实现

#ifndef __MY_SCC_DYN_
#define __MY_SCC_DYN_

#include <stdlib.h>

#include "error.h"

typedef struct DynString
{
    int count;      // 字符串长度
    int capacity;   // 分配到的缓冲区容量
    char *data;     // 指向字符串的指针
} DynString;

typedef struct DynArray
{
    int count;      // 数组元素个数
    int capacity;   // 缓冲区容量
    void **data;    // 指向数据指针数组
} DynArray;

/**
 * 初始化动态字符串存储容量
 * pstr 动态字符串存储结构
 * initsize 初始化分配的大小
 */ 
void dynstring_init(DynString *pstr, int initsize);

/**
 * 释放动态字符串使用的空间
 * pstr 动态字符串存储结构
 */
void dynstring_free(DynString *pstr);

/**
 * 重置动态字符串，先释放，再分配
 * pstr 动态字符串存储结构
 */
void dynstring_reset(DynString *pstr);

/**
 * pstr 动态字符串存储结构
 * new_size 字符串新长度
 */
void dynstring_realloc(DynString *pstr, int new_size);

/**
 * 追加字符到动态字符串中
 * pstr 动态字符串存储结构
 * ch 追加啊的字符
 */
void dynstring_append(DynString *pstr, char ch);

/**
 * 初始化动态数组存储容量
 * parr 动态数组存储结构
 * initsize 动态数组初始化分配空间
 */
void dynarray_init(DynArray *parr, int initsize);

/**
 * 释放动态数组使用的内存空间
 * parr 动态数组存储结构
 */
void dynarray_free(DynArray *parr);

/**
 * 动态数组元素查找
 * parr 动态数组存储结构
 * key 要查找的元素
 */
int dynarray_search(DynArray *parr, int key);

/**
 * 重新分配动态数组容量
 * parr 动态数组存储结构
 * new_size 动态数组新的大小
 */
void dynarray_realloc(DynArray *parr, int new_size);

/**
 * 追加动态数组元素
 * parr 动态数组存储结构
 * data 追加的元素
 */
void dynarray_add(DynArray *parr, void *data);



#endif // __MY_SCC_DYN_