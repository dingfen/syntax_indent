#include <stdlib.h>
#include <stdio.h>
#include "syntax.h"
#include "lex.h"
#include "token.h"


FILE *fin;
int token;
int line_num;
int column_num;

/**
 * 词法着色
 */
void color_print(char *fmt, ...)
{
    va_list ap;
    char buf[1024];
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    printf("%s", buf);
    va_end(ap);
}

void color_token()
{
    char *p = get_tkstr(token);
    char fmt[128] = "";
    if (token >= TK_IDENT)  {// 标识符 为白色
        sprintf(fmt, "%%s");
    }
    else if (token >= KW_CHAR)  {// 关键字 蓝色 34
        sprintf(fmt, "\033[%dm%%s\033[0m", BLUE);
    }
    else if (token >= TK_CINT) {// 常量 黄色 33
        sprintf(fmt, "\033[%dm%%s\033[0m", YELLOW);
    }
    else {// 运算符等  红色 31
        sprintf(fmt, "\033[%dm%%s\033[0m", RED);
    }
    color_print(fmt, p);
}

/**
 * 功能：缩进n个tab
 * n：缩进个数
 */
void print_tab(int n)
{
    for(int i = 0; i < n; i++)
        printf("\t");
}

/**
 * 功能：语法缩进
 */
void syntax_indent()
{
    switch (syntax_state)
    {
    case SNTX_NUL:
        color_token();
        break;
    case SNTX_SP:
        printf(" ");
        color_token();
        break;
    case SNTX_LF_HT:{
        if (token == TK_END)
            syntax_level--;
        printf("\n");
        print_tab(syntax_level);
        color_token();
        break;
    }
    case SNTX_DELAY:
        break;
    }
    syntax_state = SNTX_NUL;
}

/**
 * 功能：语法缩进主函数
 */ 
int main(int argc, char const *argv[])
{
    fin = fopen(argv[1], "rb");
    if (!fin) {
        printf("Cannot open SC source file.\n");
        return 1;
    }

    init();
    getch();
    get_token();
    translation_unit();
    cleanup();
    fclose(fin);
    printf("\n%s syntax analysis passed!\n", argv[1]);
    return 0;
}
