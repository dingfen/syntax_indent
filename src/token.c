#include "token.h"
#include <string.h>

#include "dyn.h"
#include "error.h"

extern int token;
extern DynString sourcestr;

/**
 * elf - 哈系表
 * key 传入的字符串
 */ 
int elf_hash(char *key)
{
    int h = 0;
    int g;
    while (*key)
    {
        h = (h << 4) + *key++;
        g = h & 0xf0000000;
        if (g)
            h ^= g >> 24;
        h &= ~g;
    }
    return h % MAXKEY;
}

/**
 * 分配堆内存并将数据初始化
 * size 分配大小
 */
void* mallocz(int size)
{
    void *ptr;
    ptr = malloc(size);
    if (!ptr && size)
        error("memory alloc failed.\n");
    memset(ptr, 0, size);
    return ptr;
}

/**
 * 单词表操作，直接放入单词表
 * 用于运算符、关键字、常量
 */
TKWord * tkword_direct_insert(TKWord * tp)
{
    int keyno;
    dynarray_add(&tktable, tp);
    keyno = elf_hash(tp->spelling);
    tp->next = tk_hashtable[keyno];
    tk_hashtable[keyno] = tp;
    return tp;
}

/**
 * 查找单词
 * p 要查找的单词
 * keyno 要查找的单词哈系值
 */
TKWord * tkword_find(char *p, int keyno)
{
    TKWord *tp = NULL;
    TKWord *tp1;
    for (tp1 = tk_hashtable[keyno];tp1;tp1=tp1->next)
    {
        if (!strcmp(p, tp1->spelling))
        {
            token = tp1->tkcode;
            tp = tp1;
        }
    }
    return tp;
}

/**
 * 标识符插入单词表，先查找，查找不到再插入
 */
TKWord * tkword_insert(char *p)
{
    TKWord *tp;
    int keyno;
    char *s;
    char *end;
    int length;

    keyno = elf_hash(p);
    tp = tkword_find(p, keyno);
    if (tp == NULL)
    {
        length = strlen(p);
        tp = (TKWord *)mallocz(sizeof(TKWord)+length+1);

        tp->next = tk_hashtable[keyno];
        tk_hashtable[keyno] = tp;
        dynarray_add(&tktable, tp);
        tp->tkcode = tktable.count - 1;
        s = (char *)tp + sizeof(TKWord);
        tp->spelling = (char *)s;
        for(end = p + length; p < end;)
        {
            *s++ = *p++;
        }
        *s = (char)'\0';
    }
    return tp;
}

/**
 * 词法分析初始化
 */
void init_lex()
{
    TKWord *tp;
    static TKWord keywords[] = {
        {TK_PLUS,    NULL, "+",      NULL, NULL},
        {TK_MINUS,   NULL, "-",      NULL, NULL},
        {TK_STAR,    NULL, "*",      NULL, NULL},
        {TK_DIVIDE,  NULL, "/",      NULL, NULL},
        {TK_MOD,     NULL, "%",      NULL, NULL},
        {TK_EQ,      NULL, "==",     NULL, NULL},
        {TK_NEQ,     NULL, "!=",     NULL, NULL},
        {TK_LT,      NULL, "<",      NULL, NULL},
        {TK_LEQ,     NULL, "<=",     NULL, NULL},
        {TK_GT,      NULL, ">",      NULL, NULL},
        {TK_GEQ,     NULL, ">=",     NULL, NULL},
        {TK_ASSIGN,  NULL, "=",      NULL, NULL},
        {TK_POINTO,  NULL, "->",     NULL, NULL},
        {TK_DOT,     NULL, ".",      NULL, NULL},
        {TK_AND,     NULL, "&",      NULL, NULL},
        {TK_OR,      NULL, "|",      NULL, NULL},
        {TK_OPENPA,  NULL, "(",      NULL, NULL},
        {TK_CLOSPA,  NULL, ")",      NULL, NULL},
        {TK_OPENBR,  NULL, "[",      NULL, NULL},
        {TK_CLOSBR,  NULL, "]",      NULL, NULL},
        {TK_BEGIN,   NULL, "{",      NULL, NULL},
        {TK_END,     NULL, "}",      NULL, NULL},
        {TK_SEMICOLON, NULL, ";",    NULL, NULL},
        {TK_COMMA,   NULL, ",",      NULL, NULL},
        {TK_EOF,     NULL, "End_Of_File", NULL, NULL},
        {TK_CINT,    NULL, "CINT",   NULL, NULL},
        {TK_CCHAR,   NULL, "CCHAR",  NULL, NULL},
        {TK_CSTR,    NULL, "CSTR",   NULL, NULL},

        {KW_CHAR,    NULL, "char",   NULL, NULL},
        {KW_SHORT,   NULL, "short",  NULL, NULL},
        {KW_INT,     NULL, "int",    NULL, NULL},
        {KW_VOID,    NULL, "void",   NULL, NULL},
        {KW_STRUCT,  NULL, "struct", NULL, NULL},
        {KW_IF,      NULL, "if",     NULL, NULL},
        {KW_ELSE,    NULL, "else",   NULL, NULL},
        {KW_FOR,     NULL, "for",    NULL, NULL},
        {KW_CONTINUE,NULL, "continue",   NULL, NULL},
        {KW_BREAK,   NULL, "break",  NULL, NULL},
        {KW_RETURN,  NULL, "return", NULL, NULL},
        {KW_SIZEOF,  NULL, "sizeof", NULL, NULL},
        {0,          NULL, NULL,     NULL, NULL}
    };

    dynarray_init(&tktable, 16);
    for(tp=&keywords[0];tp->spelling!=NULL;tp++)
        tkword_direct_insert(tp);
}

/**
 * 取得单词v所代表的源码字符串
 * v 单词编号
 */
char * get_tkstr(int v)
{
    if (v > tktable.count)
        return NULL;
    else if (v >= TK_CINT && v <= TK_CSTR)
        return sourcestr.data;
    else
        return ((TKWord*)tktable.data[v])->spelling;
}