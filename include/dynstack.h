#ifndef __MY_SCC_STACK_
#define __MY_SCC_STACK_

#include <stdlib.h>
#include <string.h>
#include "error.h"

typedef struct Stack {
    void **base;        // 栈低指针
    void **top;         // 栈顶指针
    int stacksize;      // 栈最大容量值
} Stack;

/**
 * 功能：初始化栈容量
 * pstack 指向栈的指针
 * initsize 栈大小
 */
void stack_init(Stack * pstack, int initsize);

/**
 * 功能：在栈顶插入元素
 * pstack 指向栈的指针
 * element 要插入栈顶的元素
 * size 栈元素的实际大小
 * 返回值 栈顶元素指针
 */
void *stack_push(Stack *pstack, void *element, int size);

/**
 * 功能：弹出栈顶元素
 * pstack 栈指针
 */
void stack_pop(Stack *pstack);

/**
 * 功能：获取栈顶元素
 * pstack 栈指针
 * 返回值 栈顶元素
 */
void * stack_get_top(Stack *pstack); 

/**
 * 功能：判断栈是否为空
 * pstack 栈指针
 * 返回值 1 为空 0 不为空
 */
int stack_is_empty(Stack *pstack);

/**
 * 功能：销毁栈
 * pstack 栈指针
 */
void stack_destroy(Stack *pstack);

#endif // __MY_SCC_STACK_