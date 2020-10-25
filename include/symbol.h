#ifndef __MY_SCC_SYMBOL_
#define __MY_SCC_SYMBOL_

#include <stdlib.h>
#include "dynstack.h"
#include "token.h"

enum TypeCode {             // 数据类型编码
    T_INT  = 0,
    T_CHAR = 1,
    T_SHORT= 2,
    T_VOID = 3,
    T_PTR  = 4,
    T_FUNC = 5,
    T_STRUCT=6,

    T_BTYPE = 0x000f,
    T_ARRAY = 0x0010
};

enum StorageClass {         // 存储类型
    SC_GLOBAL = 0x00f0,     // 包括常量 全局变量 函数定义
    SC_LOCAL  = 0x00f1,     // 栈中变量
    SC_LLOCAL = 0x00f2,     // 寄存器溢出 存放在栈中的变量
    SC_CMP    = 0x00f3,     // 使用标志寄存器
    SC_VALMASK= 0x00ff,     // 存储类型掩码
    SC_LVAL   = 0x0100,     // 左值
    SC_SYM    = 0x0200,     // 符号

    SC_ANOM   = 0x10000000, // 匿名符号
    SC_STRUCT = 0x20000000, // 结构体符号
    SC_MEMBER = 0x40000000, // 结构体成员变量
    SC_PARAMS = 0x80000000  // 函数参数
};


typedef struct Type         // 数据类型结构
{
    int t;                  // 数据类型
    struct Symbol *ref;     // 引用符号
} Type;


typedef struct Symbol       // 符号表项定义
{
    int v;                  // 在tktable 单词表中的编码
    int r;                  // 关联的寄存器
    int c;                  // 符号关联值
    Type type;              // 符号数据类型
    struct Symbol *next;    // 下一个符号
    struct Symbol *prev;    // 前一个符号
} Symbol;

/**
 * 功能：将符号放入到符号栈中
 * v 符号编号
 * type 符号数据类型
 * c 符号关联值
 */
Symbol *sym_direct_push(Stack *ss, int v, Type *type, int c);

/**
 * 功能：将符号放在符号栈中，动态判断放入全局符号栈还是局部符号栈
 * v 符号编号
 * type 符号数据类型
 * r 符号存储类型
 * c 符号关联值
 */
Symbol *sym_push(int v, Type *type, int r, int c);

/**
 * 功能：函数符号放入到全局符号表
 * v 符号编号
 * type 符号数据类型
 */
Symbol * func_sym_push(int v, Type *type);

/**
 * 功能：变量符号放入到全局符号表
 * type 符号数据类型
 * r 存储类型
 * v 单词编号
 * addr 地址
 */
Symbol *var_sym_put(Type *type, int r, int v, int addr);

/**
 * 功能：将节名称放入全局符号表
 * sec 节名称   
 * c 符号关联值
 */
Symbol * sec_sym_put(char *sec, int c);

/**
 * 功能：弹出栈中符号直到找到符号'b'
 * ptop：符号栈栈顶
 * b： 符号指针
 */
void sym_pop(Stack *ptop, Symbol *b);


/**
 * 功能：查找结构定义
 * v 符号编号
 */
Symbol *struct_search(int v);

/**
 * 功能：查找结构定义
 * v 符号编号
 */
Symbol *sym_search(int v);

#endif // __MY_SCC_SYMBOL_