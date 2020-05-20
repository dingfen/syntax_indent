[TOC]

# 具体需求

以C89为蓝本，开发一种Simple C语言的编译器。
有以下简化原则：
- 支持单字节、双字节、四字节的基本数据类型
- 支持数组、结构体
- 支持字符串
- 支持函数、局部变量、全局变量
- 支持条件语句、循环语句
- 支持基本的算术运算、关系运算

## 源码字符集
- 大小写字母和下划线
- 数字
- 标点以及特殊字符：+ - * / % = ! < > . & ( ) [ ] { } ; , \ " '
- 空白符
- 空字符

## SC语言词法定义
### 关键字
参照C89要求，有如下关键字，其语义与C89的相似或一致
- char
- short
- int
- void
- struct
- if
- else
- for
- continue
- break
- return
- sizeof

### 标识符
参照C89要求，标识符的语法定义与C89一致，语义也一致：
id -> letter_ | (letter_ | digit)*
letter_ -> [a-zA-Z_]
digit -> [0-9]

### 整数常量
目前仅考虑整数类型，因此，参照C89要求做如下定义：
int_num -> digit+
digit -> [0-9]

### 字符常量
参照C89要求，字符常量定义与C89一致，语义也一致：
constchar -> ' char '
char -> ([a-zA-Z0-9] | schar)
schar -> \n | \b | \r | \t | , | . | & | | | + ...

### 字符串常量
参照C89要求，字符串常量定义与C89一致，语义也一致：
str -> " constchar* "

### 运算以及分隔符
参照C89要求，运算符号的定义与C89一致，语义也一致：
支持如下运算符号：
- \+
- \-
- \*
- /
- %
- ==
- !=
- <
- \>
- <=
- \>=
- &
- |
- ->
- .
- (
- )
- [
- ]
- {
- }
- ,

分隔符为;

### 注释
仅支持/* */的注释符号

### 未来可能会支持的C语言语法

- 运算符 自加 自减操作
- 运算符 逻辑或、逻辑与、逻辑取反操作
- 运算符 按位取反操作
- 运算符 移位操作
- 各类复合赋值运算操作符

## SC语法定义
### 外部定义
由于还需要实现链接器，因此将每个SC源文件看作是一个翻译单元，由一系列的外部声明组成，它们出现在任何函数之外，具有全局作用域。
#### 外部定义
TranslateUnit -> (ExternDeclaration)* EOF

ExternDeclaration -> func_def | declaration

#### 函数定义
func_def -> type decl_id func_body

func_body -> multi_statement

#### 声明
声明规定一组标识符的解释和属性，与C89类似，在声明变量时，必须先明确类型，可以使用逗号声明多个标识符，可选地赋予初始值

declaration -> type (sdecls)+ ;

sdecls -> sdecl ( , sdecl)*

sdecl -> decl_id | decl_id = init_val

#### 类型
SC的类型除了没有浮点等类型外，与C89保持一致，结构体的声明情况值得注意。

type -> void | char | short | int | structid

structid -> struct id { structdecls } | struct id

structdecls -> (structdecl)+

structdecl -> type struct_decl_id ;

struct_decl_id -> decl_id ( , decl_id)*

#### 声明符
decl_id -> (pointer)* (direct_decl_id)

pointer -> *

direct_decl_id -> id direct_decl_id_suffix

direct_decl_id_suffix -> ( [ ] | [ int_num ] | ( ) | ( args_list ) )*


#### 初始值
init_val -> assign_expr

### 语句
参照C89标准，结合之前已经地定义的原则等，做出如下定义：

statement -> multi_statement | if_statement | for_statement |break_statement
            | for_statement | continue_statement | return_statement | expr_statement

#### 复合语句
相当于C89中用大括号括起来的语句

multi_statement -> { (declaration | statement)* }

#### 表达式语句
expr_statement -> expr ;

#### 选择语句
if_statement -> if ( expr ) statement (else statement)?

#### 循环语句
for_statement -> for ( expr_statement expr_statement expr )

#### 跳转语句
continue_statement -> continue ;

return_statement -> return expr ;

break_statement  ->  break ;

### 表达式
expr -> assign_expr ( , assign_expr)*

#### 赋值表达式
assign_expr -> equal_expr | unary_expr = assign_expr

#### 相等类表达式
equal_expr -> rel_expr (== rel_expr | != rel_expr)*

#### 关系表达式
rel_expr -> add_expr ( < add_expr | > add_expr | <= add_expr | >= add_expr )*

#### 加减表达式
add_expr -> mul_expr ( + mul_expr | - mul_expr )*

#### 乘除表达式
mul_expr -> unary_expr ( * unary_expr | / unary_expr | % unary_expr )*

#### 一元表达式
unary_expr -> suffix_expr | & unary_expr | | unary_expr | + unary_expr | - 
unary_expr | sizeof_expr

sizeof_expr -> sizeof (type)

#### 后缀表达式
suffix_expr -> primary_expr ( [ expr ] |  ( ) | ( ars ) | . id | -> id )*

ars -> assign_expr ( , assign_expr )*

#### 初等表达式
primary_expr -> id | int_num | strs | constchar | ( expr )

