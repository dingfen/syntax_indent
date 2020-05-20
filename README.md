# 自己动手写编译器、链接器的前5章实现

在项目文件夹下使用make工具即可编译完成

`lex`是分词功能，对C语言中的关键字、常量、符号、变量名等涂色区分。
`syntax`不仅实现了对不同词的涂色区分，还有根据语法的缩进功能。

目前支持的功能：
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