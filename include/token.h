// 词法分析
// 定义 关键字和词法记号的类型标识
// 使用enum结构
#ifndef __MY_SCC_TOKEN_
#define __MY_SCC_TOKEN_

#include "dyn.h"

enum Token
{
    TK_PLUS,        // +
    TK_MINUS,       // -
    TK_STAR,        // *
    TK_DIVIDE,      // /
    TK_MOD,         // %
    TK_EQ,          // ==
    TK_NEQ,         // !=
    TK_LT,          // <
    TK_LEQ,         // <=
    TK_GT,          // >
    TK_GEQ,         // >=
    TK_ASSIGN,      // =
    TK_POINTO,      // ->
    TK_DOT,         // .
    TK_AND,         // &
    TK_OR,          // |
    TK_OPENPA,      // (
    TK_CLOSPA,      // )
    TK_OPENBR,      // [
    TK_CLOSBR,      // ]
    TK_BEGIN,       // {
    TK_END,         // }
    TK_SEMICOLON,   // ;
    TK_COMMA,       // ,
    TK_EOF,         // EOF

    /* 常量 */
    TK_CINT,        // 整型常量
    TK_CCHAR,       // 字符常量
    TK_CSTR,        // 字符串常量

    /* 关键字 */
    KW_CHAR,        // char
    KW_SHORT,       // short
    KW_INT,         // int
    KW_VOID,        // void
    KW_STRUCT,      // struct
    KW_IF,          // if
    KW_ELSE,        // else
    KW_FOR,         // for
    KW_CONTINUE,    // continue
    KW_BREAK,       // break
    KW_RETURN,      // return
    KW_SIZEOF,      // sizeof
    
    /* 标识符 */
    TK_IDENT
};


// 数据结构
// 考虑到C89标准 规定最多32个字符
// 常量字符串长度可以很长，故使用动态存储
// 单词表查找频繁，可以借助哈系表

/* 单词存储结构定义 */
typedef struct TKWord
{
    int tkcode;             // 词法符号编码
    struct TKWord *next;    // 指向哈系冲突的同义词
    char *spelling;         // 词的字符串
    struct Symbol *sym_struct;  // 指向单词表示的结构定义
    struct Symbol *sym_id;      // 指向单词表示的标识符
} TKWord;

/* 全局变量 */
#define MAXKEY 1024
TKWord *tk_hashtable[MAXKEY];       // 词哈系表
DynArray tktable;                   // 单词表


void init_lex();
TKWord *tkword_insert(char *);
char * get_tkstr(int v);


#endif // __MY_SCC_TOKEN_