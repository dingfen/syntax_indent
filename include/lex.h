#ifndef __MY_SCC_LEX_
#define __MY_SCC_LEX_

#include <stdio.h>
#include <string.h>

#include "error.h"
#include "token.h"
#include "dyn.h"

#define BLUE   34
#define YELLOW 33
#define RED    31

/**
 * 词法分析程序初始化
 */ 
void init();


/**
 * 清理工作
 */
void cleanup();

/**
 * 获得取到的源码字符
 */
void getch();

/**
 * 注释处理
 */
void parse_comment();

/**
 * 空白字符处理
 */
void skip_white_space();

/**
 * 预处理，忽略空白字符及注释
 */
void preprocess();

/**
 * 判断c是否为字母或下划线
 */
int is_nodigit(char c);

/**
 * 判断c是否为数字
 */
int is_digit(char c);

/**
 * 解析标识符
 */
void parse_identifier();

/**
 * 解析整形常量
 */
void parse_num();

/**
 * 解析字符常量和字符串常量
 * sep 单引号为 字符常量  双引号为字符串常量
 */
void parse_string(char sep);

/**
 * 取单词主程序
 */
void get_token();



#endif // __MY_SCC_LEX_