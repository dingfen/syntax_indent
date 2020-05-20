// 错误处理程序
#ifndef __MY_SCC_ERROR_
#define __MY_SCC_ERROR_

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>


// 错误级别
enum ErrorLevel
{
    LEVEL_WARNING,
    LEVEL_ERROR
};

// 工作阶段
enum WorkStage
{
    STAGE_COMPILE,
    STAGE_LINK
};

/**
 * 异常处理
 * stage 编译阶段错误还是链接阶段错误
 * level 错误级别
 * fmt 参数输出格式
 * ap 可变参数列表
 */
void handle_exception(int stage, int level, char *fmt, va_list ap);

/**
 * 编译警告处理
 * fmt 参数输出格式
 * ap 可变参数列表
 */
void warning(char *fmt, ...);

/**
 * 编译错误处理
 * fmt 参数输出格式
 * ap 可变参数列表
 */
void error(char *fmt, ...);

/**
 * 提示错误，缺少某个语法成分
 * msg 需要的语法成分
 */
void expect(char *msg);

/**
 * 跳过单词c，取下一个单词
 * 如果不是单词c，提示错误
 * c 要跳过的单词
 */
void skip(int c);


#endif // __MY_SCC_ERROR_