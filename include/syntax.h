#ifndef __MY_SCC_SYNTAX_
#define __MY_SCC_SYNTAX_


#include <stdlib.h>
#include "json.h"
#include "symbol.h"


/* 全局变量 */
Type char_pointer_type,     // 字符串指针
     int_type,               // int 类型
     default_func_type;      // 默认函数类型

Stack global_sym_stack,     // 全局符号栈
      local_sym_stack;      // 局部符号栈

/* 语法状态枚举 */
enum SynTaxState
{
    SNTX_NUL,   // 空状态
    SNTX_SP,    // 空格
    SNTX_LF_HT, // 换行并缩进  声明 定义 语句结束都置为该状态
    SNTX_DELAY  // 延迟到取出下一个单词后确定输出格式
};

/**
 * 功能：翻译单元，语法分析顶层
 * <translation_unit> ::= {<external_declaration>}<TK_EOF>
 */
JSONobj* translation_unit();

/**
 * 功能： 解析外部声明
 * l： 存储类型 指明局部还是全局
 * ExternDeclaration 
 *  --> <type_specifier>(<TK_SEMICOLON>
 *      | <declarator><funcbody>
 *      | <declarator> [<TK_ASSIGN><initializer>]
 *       {<TK_COMMA><declarator>[<TK_ASSIGN><initializer>]}<TK_SEMICOLON>)
 */
JSONobj external_declaration(int l);

/**
 * 功能：解析类型区分符
 * 返回值：是否发现合法的类型区分符
 * <type_specifier>
 *  --> <KW_INT>|<KW_CHAR>|<KW_SHORT>|<KW_VOID>|<struct_specifier>
 */
int type_specifier();

/**
 * 功能：结构体类型区分符
 * <struct_specifier>
 *   --> <KW_STRUCT><IDENTIFIER><TK_BEGIN><struct_declaration_list><TK_END>
 *      | <KW_STRUCT><IDENTIFIER>
 */
void struct_specifier();

/**
 * 功能：结构体中声明的变量的全体
 * <struct_declaration_list> 
 *  --> <struct_declaration>{<struct_declaration>}
 */
void struct_declaration_list();

/**
 * 功能：结构体中声明的变量
 * <strcut_declaration>
 *  --> <type_specifier><struct_declarator_list><TK_SEMICOLON>
 * <struct_declarator_list>
 *  --> <declarator>{<TK_COMMA><declarator>}
 */
void struct_declaration();

/**
 * 功能：声明符的解析
 * <declarator>
 *  --> {<pointer>}<direct_declarator>
 */
void declarator();

/**
 * 功能：直接声明符解析
 * <direct_declarator>
 *  --> <IDENTIFIER><direct_declarator_postfix>
 */
void direct_declarator();

/**
 * 功能：直接声明符后缀
 * <direct_declarator_postfix>
 *  --> {<TK_OPENBR><TK_CINT><TK_CLOSBR>}
 *    | <TK_OPENBR><TK_CLOSBR>
 *    | <TK_OPENPA><parameter_type_list><TK_CLOSPA>
 *    | <TK_OPENPA><TK_CLOSPA>
 */
void direct_declarator_postfix();

/**
 * 功能：解析形参类型表
 * <parameter_type_list>
 *  --> <parameter_declaration>{<TK_COMMA><parameter_declaration>}
 * <parameter_declaration> --> <type_specifier><declarator>
 */
void parameter_type_list();

/**
 * 功能：函数体解析
 * <funcbody> --> <compound_statement>
 */
void funcbody();

/**
 * 功能：解析初值符
 * <initializer> --> <assignment_expression>
 */
void initializer();

/**
 * 功能：语句解析
 * <statement> --> <compound_statement>
 *  | <if_statement> | <return_statement> | <break_statement>
 *  | <continue_statement> | <for_statement> | <expression_statement>
 */
void statement();

/**
 * 功能：解析复合语句
 * <compound_statement> --> 
 *  <TK_BEGIN>{<declaration> | <statement>}<TK_END>
 */
void compound_statement();

/**
 * 功能：判断是否为类型区分符
 * v：单词编号
 */
int is_type_specifier(int v);

/**
 * 功能：表达式语句解析
 * <expression_statement> --> <TK_SEMICOLON>|<expression><TK_SEMICOLON>
 */
void expression_statement();

/**
 * 功能：选择语句解析
 * <if_statement> --> <KW_IF><TK_OPENPA><expression><TK_CLOSPA><statement>
 *      [<KW_ELSE><statement>]
 */
void if_statement();

/**
 * 功能：循环语句解析
 * <for_statement> --> <KW_FOR><TK_OPENPA>
 *   <expression_statement><expression_statement><expression>
 *   <TK_CLOSPA><statement>
 */
void for_statement();

/**
 * 功能：continue语句解析
 */
void continue_statement();

/**
 * 功能：break语句解析
 */
void break_statement();

/**
 * 功能：return语句解析
 * <return_statement> --> <KW_RETURN><TK_SEMICOLON>
 *  | <KW_RETURN><expression><TK_SEMICOLON>
 */
void return_statement();

/**
 * 功能：解析表达式
 * <expression>--><assignment_expression>{<TK_COMMA><assignment_expression>}
 */
void expression();

/**
 * 功能：解析赋值表达式
 * <assignment_expression>
 *  --> <equality_expression>|<unary_expression><TK_ASSIGN><assignment_expression>
 */
void assignment_expression();

/**
 * 功能：解析相等类表达式
 * <equality_expression> --> <relational_expression>
 *  {<TK_EQ><relational_expression>}|<TK_NEQ><relational_expression>
 */
void equality_expression();

/**
 * 功能：关系表达式
 * <relational_expression> --> <additive_expression>{
 *  <TK_LT><additive_expression>
 *  | <TK_GT><additive_expression>
 *  | <TK_LEQ><additive_expression>
 *  | <TK_GEQ><additive_expression>}
 */
void relational_expression();

/**
 * 功能：加减类表达式
 * <additive_expression> --> <multiplicative_expression>
 *  {<TK_PLUS>|<TK_MINUS><multiplicative_expression>}
 */
void additive_expression();

/**
 * 功能：乘法除法表达式
 * <multiplicative_expression> --> <unary_expression>
 *  {<TK_STAR>|<TK_DIVIDE>|<TK_MOD><unary_expression>}
 */ 
void multiplicative_expression();

/**
 * 功能：一元表达式解析
 * <unary_expression> --> <postfix_expression>
 *  | <TK_AND>|<TK_STAR> <unary_expression>
 *  |<sizeof_expression>
 */
void unary_expression();

/**
 * 功能：解析sizeof表达式
 * <sizeof_expression> --> <KW_SIZEOF><TK_OPENPA><type_specifier><TK_CLOSEPA>
 */
void sizeof_expression();

/**
 * 功能：后缀表达式
 * <postfix_expression> --> <primary_expression>
 *  {<TK_OPENBR><expression><TK_CLOSEBR>|<TK_OPENPA><TK_CLOSEPA>
 *  |<TK_DOT><IDENTIFIER>|<TK_POINTSTO><IDENTIFIER>}
 */
void postfix_expression();

/**
 * 功能：解析初值表达式
 * <primary_expression> --> <IDENTIFIER>|<TK_CINT>|<TK_CSTR>|<TK_CCHAR>|
 *  <TK_OPENPA><expression><TK_CLOSEPA>
 */
void primary_expression();

/**
 * 功能：解析实参表达式
 * <argument_expression_list> --> <assignment_expression>
 *  { <TK_COMMA><assignment_expression> }
 */
void argument_expression_list();

/**
 * 功能：语法缩进
 */
void syntax_indent();

/**
 * 取单词主程序
 */
void get_token();


void syntax_indent();

/**
 * 功能：计算字节对齐位置
 * n 未对齐前值
 * align 对齐粒度
 */
int calc_align(int n, int align);

/**
 * 功能：自行添加  制作指针
 * type 输入的类型
 */
void mk_pointer(Type *type);

/**
 * 功能： 函数调用约定
 * fc 输出 调用约定
 * 用于函数声明 不用在数据声明
 */
void function_calling_convention(int *fc);

/**
 * 功能：返回类型长度
 * t 数据类型指针
 * a 对齐值
 */
int type_size(Type *t, int *a);

#endif // __MY_SCC_SYNTAX_