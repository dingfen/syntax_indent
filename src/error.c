#include "error.h"


extern int token;
extern char * filename;
extern int line_num;
extern int column_num;
extern void get_token();
extern char * get_tkstr(int c);


/**
 * 异常处理
 * stage 编译阶段错误还是链接阶段错误
 * level 错误级别
 * fmt 参数输出格式
 * ap 可变参数列表
 */
void handle_exception(int stage, int level, char *fmt, va_list ap)
{
    char buf[1024];
    vsprintf(buf, fmt, ap);
    if (stage == STAGE_COMPILE)
    {
        if (level == LEVEL_WARNING)
            printf("%s(第%d行 第%d列)：编译警告：%s!\n", filename, line_num, column_num, buf);
        else
        {
            printf("%s(第%d行 第%d列)：编译错误：%s!\n", filename, line_num, column_num, buf);
            exit(-1);
        }
    }
    else
    {
        printf("链接错误：%s!\n", buf);
        exit(-1);
    }
} 

/**
 * 编译警告处理
 * fmt 参数输出格式
 * ap 可变参数列表
 */
void warning(char *fmt, ...)
{
    va_list ap;
    
    va_start(ap, fmt);
    handle_exception(STAGE_COMPILE, LEVEL_WARNING, fmt, ap);
    va_end(ap);
}

/**
 * 编译错误处理
 * fmt 参数输出格式
 * ap 可变参数列表
 */
void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    handle_exception(STAGE_COMPILE, LEVEL_ERROR, fmt, ap);
    va_end(ap);
}

/**
 * 提示错误，缺少某个语法成分
 * msg 需要的语法成分
 */
void expect(char *msg)
{
    error("缺少 %s", msg);
}

/**
 * 跳过单词c，取下一个单词
 * 如果不是单词c，提示错误
 * c 要跳过的单词
 */
void skip(int c)
{
    if (token != c)
        error("lack of %s", get_tkstr(c));
    get_token();
} 