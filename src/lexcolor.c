#include "lex.h"
#include "token.h"

/* 全局变量 */
FILE* fin;
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
 * 词法分析主函数入口
 * argv 为传入的文件名
 */ 
int main(int argc, char const *argv[])
{
    fin = fopen(argv[1], "rb");
    if (!fin)
    {
        printf("Can not open the SC file %s !\n", argv[1]);
        return 0;
    }
    init();

    getch();
    do
    {
        get_token();
        color_token();
    } while (token != TK_EOF);
    printf("\n 代码行数：%d行, 代码列数：%d列\n", line_num, column_num);

    cleanup();
    fclose(fin);
    
    return 0;
}
