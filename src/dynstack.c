#include "dynstack.h"

/**
 * 功能：初始化栈容量
 * pstack 指向栈的指针
 * initsize 栈大小
 */
void stack_init(Stack * pstack, int initsize)
{
    pstack->base = (void **)malloc(sizeof(void*)*initsize);
    if (!pstack->base)
    {
        error("memory alloc failed.\n");
    }
    else
    {
        pstack->top = pstack->base;
        pstack->stacksize = initsize;
    }
}

/**
 * 功能：在栈顶插入元素
 * pstack 指向栈的指针
 * element 要插入栈顶的元素
 * size 栈元素的实际大小
 * 返回值 栈顶元素指针
 */
void * stack_push(Stack *pstack, void *element, int size)
{
    int newsize;
    if (pstack->top >= pstack->base + pstack->stacksize)
    {
        newsize = pstack->stacksize * 2;
        pstack->base = (void **)realloc(pstack->base, sizeof(void*)*newsize);
        if (!pstack->base) return NULL;
        pstack->top = pstack->base + pstack->stacksize;
        pstack->stacksize = newsize;
    }
    *pstack->top = malloc(size);
    memcpy(*pstack->top, element, size);
    pstack->top++;
    return *(pstack->top-1);
}

/**
 * 功能：弹出栈顶元素
 * pstack 栈指针
 */
void stack_pop(Stack *pstack)
{
    if (pstack->top > pstack->base)
    {
        free(*(--pstack->top));
    }
}

/**
 * 功能：获取栈顶元素
 * pstack 栈指针
 * 返回值 栈顶元素
 */
void * stack_get_top(Stack *pstack)
{
    if (pstack->top > pstack->base)
    {
        return *(--pstack->top);
    }
}

/**
 * 功能：判断栈是否为空
 * pstack 栈指针
 * 返回值 1 为空 0 不为空
 */
int stack_is_empty(Stack *pstack)
{
    if (pstack->top == pstack->base)
        return 1;
    else return 0;
} 

/**
 * 功能：销毁栈
 * pstack 栈指针
 */
void stack_destroy(Stack *pstack)
{
    for(void **ele = pstack->base; ele < pstack->top; ele++)
    {
        free(*ele);
    }
    if (pstack->base)
        free(pstack->base);
    pstack->base = NULL;
    pstack->top = NULL;
    pstack->stacksize = 0;
} 