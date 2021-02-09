#include "lex.h"
#include "token.h"
#include "syntax.h"

/* 全局变量 */
extern FILE* fin;
int ch;
extern int token;
extern int line_num;
extern int column_num;
char * filename;
extern DynString tkstr;        // 单词字符串
extern DynString sourcestr;    // 单词源码字符串
extern int tkvalue;            // 单词的词法值

/**
 * 词法分析程序初始化
 */ 
void init()
{
    line_num = 1;
    column_num = 0;
    init_lex();
}

/**
 * 清理工作
 */
void cleanup()
{
    int i;
    printf("\ntktable.count=%d\n", tktable.count);
    for(i = TK_IDENT; i < tktable.count; i++)
    {
        free(tktable.data[i]);
    }
    free(tktable.data);
}

/**
 * 获得取到的源码字符
 */
void getch()
{
    ch = getc(fin);
    column_num ++;
    // printf("Now the ch is %c\n", ch);
}

/**
 * 注释处理
 */
void parse_comment()
{
    getch();
    do
    {
        do
        {
            if (ch == '\n' || ch == '*' || ch == EOF)
                break;
            else 
                getch();
        } while (1);
        if (ch == '\n')
        {
            line_num ++;
            column_num = 0;
            getch();
        }
        else if (ch == '*')
        {
            getch();
            if (ch == '/')
            {
                getch();
                return;
            }
        }
        else
        {
            error("No End_Of_File found at the end of file.\n");
            return;
        }
    }while(1);
}

/**
 * 空白字符处理
 */
void skip_white_space()
{
    while(ch == ' ' || ch == '\t' || ch == '\r' ||ch == '\n')
    {
        if (ch == '\n')
        {
            // getch();
            line_num++;
            column_num = 0;
        }
        // printf("%c", ch);
        getch();
    }
} 

/**
 * 预处理，忽略空白字符及注释
 */
void preprocess()
{
    while (1)
    {
        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
            skip_white_space();
        else if (ch == '/')
        {
            // 看是否是注释
            getch();
            if (ch == '*')
            {
                parse_comment();
            }
            else
            {
                ungetc(ch, fin);
                column_num--;
                ch = '/';
                break;
            }
        } 
        else 
            break;
    }
    
}

/**
 * 判断c是否为字母或下划线
 */
int is_nodigit(char c)
{
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '_';
}

/**
 * 判断c是否为数字
 */
int is_digit(char c)
{
    return c>='0' && c<='9';
}

/**
 * 解析标识符
 */
void parse_identifier()
{
    dynstring_reset(&tkstr);
    dynstring_append(&tkstr, ch);
    getch();

    while (is_nodigit(ch) || is_digit(ch))
    {
        dynstring_append(&tkstr, ch);
        getch();
    }
    dynstring_append(&tkstr, '\0');
}

/**
 * 解析整形常量
 */
void parse_num()
{
    dynstring_reset(&tkstr);
    dynstring_reset(&sourcestr);
    do
    {
        dynstring_append(&tkstr, ch);
        dynstring_append(&sourcestr, ch);
        getch();
    } while (is_digit(ch));
    if (ch =='.')
    {
        do
        {
            dynstring_append(&tkstr, ch);
            dynstring_append(&sourcestr, ch);
        } while (is_digit(ch));   
    }
    dynstring_append(&tkstr, '\0');
    dynstring_append(&sourcestr, '\0');
    tkvalue = atoi(tkstr.data);
} 

/**
 * 解析字符常量和字符串常量
 * sep 单引号为 字符常量  双引号为字符串常量
 */
void parse_string(char sep)
{
    char c;
    dynstring_reset(&tkstr);
    dynstring_reset(&sourcestr);
    dynstring_append(&sourcestr, sep);
    getch();
    for(;;)
    {
        if (ch == sep)
            break;
        else if (ch == '\\')
        {
            dynstring_append(&sourcestr, ch);
            getch();
            // 解析转义字符
            switch (ch)
            {
            case '0':
                c = '\0';
                break;
            case 'b':
                c = '\b';
                break;
            case 'n':
                c = '\n';
                break;
            case '\'':
                c = '\'';
                break;
            case '\"':
                c = '\"';
                break;
            default:
                c = ch;
                warning("illegal escape character: \'\\%c\'", c);
                break;
            }
            dynstring_append(&tkstr, c);
            dynstring_append(&sourcestr, ch);
            getch();
        }
        else
        {
            dynstring_append(&tkstr, ch);
            dynstring_append(&sourcestr, ch);
            getch();
        }
    }
    dynstring_append(&tkstr, '\0');
    dynstring_append(&sourcestr, sep);
    dynstring_append(&sourcestr, '\0');
    getch();
} 

/**
 * 词法着色
 */

// void color_print(char *fmt, ...)
// {
//     va_list ap;
//     char buf[1024];
//     va_start(ap, fmt);
//     vsprintf(buf, fmt, ap);
//     printf("%s", buf);
//     va_end(ap);
// }

// void color_token()
// {
//     char *p = get_tkstr(token);
//     char fmt[128] = "";
//     if (token >= TK_IDENT)  {// 标识符 为白色
//         sprintf(fmt, "%%s");
//     }
//     else if (token >= KW_CHAR)  {// 关键字 蓝色 34
//         sprintf(fmt, "\033[%dm%%s\033[0m", BLUE);
//     }
//     else if (token >= TK_CINT) {// 常量 黄色 33
//         sprintf(fmt, "\033[%dm%%s\033[0m", YELLOW);
//     }
//     else {// 运算符等  红色 31
//         sprintf(fmt, "\033[%dm%%s\033[0m", RED);
//     }
//     color_print(fmt, p);
// }

/**
 * 取单词主程序
 */
void get_token()
{
    preprocess();
    if (is_nodigit(ch))
    {
        TKWord *tp;
        parse_identifier();
        tp = tkword_insert(tkstr.data);
        token = tp->tkcode;
    }
    else if (is_digit(ch))
    {
        parse_num();
        token = TK_CINT;
    }
    else
    {
        switch (ch)
        {
        case '+':
            getch();
            token = TK_PLUS;
            break;
        case '-':
            getch();
            if (ch == '>')
            {
                token = TK_POINTO;
                getch();
            }
            else
                token = TK_MINUS;
            break;
        case '/':
            token = TK_DIVIDE;
            getch();
            break;
        case '%':
            token = TK_MOD;
            getch();
            break;
        case '=':
            getch();
            if (ch == '=')
            {
                token = TK_EQ;
                getch();
            }
            else
                token = TK_ASSIGN;
            break;
        case '!':
            getch();
            if (ch == '=')
            {
                token = TK_NEQ;
                getch();
            }
            else
                error("illegal operator");
            break;
        case '<':
            getch();
            if (ch == '=')
            {
                token = TK_LEQ;
                getch();
            }
            else
                token = TK_LT;
            break;
        case '>':
            getch();
            if (ch == '=')
            {
                token = TK_GEQ;
                getch();
            }
            else
                token = TK_GT;
            break;
        case '.':
            getch();
            token = TK_DOT;
            break;
        case '&':
            token = TK_AND;
            getch();
            break;
        case '|':
            token = TK_OR;
            getch();
            break;
        case ';':
            token = TK_SEMICOLON;
            getch();
            break;
        case '(':
            token = TK_OPENPA;
            getch();
            break;
        case '[':
            token = TK_OPENBR;
            getch();
            break;
        case '{':
            token = TK_BEGIN;
            getch();
            break;
        case ')':
            token = TK_CLOSPA;
            getch();
            break;
        case ']':
            token = TK_CLOSBR;
            getch();
            break;
        case '}':
            token = TK_END;
            getch();
            break;
        case ',':
            token = TK_COMMA;
            getch();
            break;
        case '*':
            token = TK_STAR;
            getch();
            break;
        case '\'':
            parse_string(ch);
            token = TK_CCHAR;
            tkvalue = *(char *)tkstr.data;
            break;
        case '\"':
            parse_string(ch);
            token = TK_CSTR;
            break;
        case EOF:
            token = TK_EOF;
            break;
        default:
            error("illegal word! Lexical cannot recognise %c", ch);
            getch();
            break;
        }
    }
    #ifdef __SYNTAX_INDENT
        syntax_indent();
    #endif
}

/**
 * 词法分析主函数入口
 * argv 为传入的文件名
 */ 
// int main(int argc, char const *argv[])
// {
//     fin = fopen(argv[1], "rb");
//     if (!fin)
//     {
//         printf("Can not open the SC file %s !\n", argv[1]);
//         return 0;
//     }
//     init();

//     getch();
//     do
//     {
//         get_token();
//         color_token();
//     } while (token != TK_EOF);
//     printf("\n 代码行数：%d行, 代码列数：%d列\n", line_num, column_num);

//     cleanup();
//     fclose(fin);
    
//     return 0;
// }
