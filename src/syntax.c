#include "token.h"
#include "syntax.h"


/* 全局变量 */
extern int token;
extern int tkvalue;
extern int ch;
extern DynString tkstr;

/**
 * 功能：翻译单元，语法分析顶层
 * <translation_unit> ::= {<external_declaration>}<TK_EOF>
 */
void translation_unit() 
{
    while(token != TK_EOF) 
    {
        external_declaration(SC_GLOBAL);
    }
}


/**
 * 功能： 解析外部声明
 * l： 存储类型 指明局部还是全局
 * ExternDeclaration 
 *  --> <type_specifier>(<TK_SEMICOLON>
 *      | <declarator><funcbody>
 *      | <declarator> [<TK_ASSIGN><initializer>]
 *       {<TK_COMMA><declarator>[<TK_ASSIGN><initializer>]}<TK_SEMICOLON>)
 */
void external_declaration(int l)
{
    if(!type_specifier())
    {
        expect("<type id>");
    }
    if (token == TK_SEMICOLON)
    {
        get_token();
        return ;
    }
    while (1)
    {
        declarator();
        if (token == TK_BEGIN)
        {
            if (l == SC_LOCAL)
            {
                error("nested func declarator unsupported.\n");
            }
            funcbody();
            break;
        }
        else
        {
            if (token == TK_ASSIGN)
            {
                get_token();
                initializer();
            }
            if (token == TK_COMMA)
            {
                get_token();
            }
            else
            {
                syntax_state = SNTX_LF_HT;
                skip(TK_SEMICOLON);
                break;
            }
        }
    }
}

/**
 * 功能：解析类型区分符
 * 返回值：是否发现合法的类型区分符
 * <type_specifier>
 *  --> <KW_INT>|<KW_CHAR>|<KW_SHORT>|<KW_VOID>|<struct_specifier>
 */
int type_specifier()
{
    int type_found = 0;
    switch (token)
    {
    case KW_CHAR:
        type_found = 1;
        syntax_state = SNTX_SP;
        get_token();
        break;
    case KW_SHORT:
        type_found = 1;
        syntax_state = SNTX_SP;
        get_token();
        break;
    case KW_VOID:
        type_found = 1;
        syntax_state = SNTX_SP;
        get_token();
        break;
    case KW_INT:
        type_found = 1;
        syntax_state = SNTX_SP;
        get_token();
        break;
    case KW_STRUCT:
        syntax_state = SNTX_SP;
        type_found = 1;
        struct_specifier();
        break;

    default:
        break;
    }
    return type_found;
}

/**
 * 功能：结构体类型区分符
 * <struct_specifier>
 *   --> <KW_STRUCT><IDENTIFIER><TK_BEGIN><struct_declaration_list><TK_END>
 *      | <KW_STRUCT><IDENTIFIER>
 */
void struct_specifier()
{
    int v;

    get_token();       // struct
    v = token;
    syntax_state = SNTX_DELAY;
    get_token;          // identifier

    if (token == TK_BEGIN)  
        syntax_state = SNTX_LF_HT;  // for define
    else if (token == TK_CLOSPA)    // for sizeof(struct id)
        syntax_state = SNTX_NUL;
    else syntax_state = SNTX_SP;    // for declarator

    syntax_indent();

    if (v < TK_IDENT)
        expect("struct identifier name");
    if (token == TK_BEGIN)
        struct_declaration_list();
}

/**
 * 功能：结构体中声明的变量的全体
 * <struct_declaration_list> 
 *  --> <struct_declaration>{<struct_declaration>}
 */
void struct_declaration_list()
{
    int maxalign, offset;

    syntax_state = SNTX_LF_HT;
    syntax_level++;

    get_token();
    while (token != TK_END)
    {
        struct_declaration(&maxalign, &offset);
    }
    skip(TK_END);

    syntax_state = SNTX_LF_HT;
}

/**
 * 功能：结构体中声明的变量
 * <strcut_declaration>
 *  --> <type_specifier><struct_declarator_list><TK_SEMICOLON>
 * <struct_declarator_list>
 *  --> <declarator>{<TK_COMMA><declarator>}
 */
void struct_declaration()
{
    type_specifier();
    while (1)
    {
        declarator();

        if (token == TK_SEMICOLON)
            break;
        skip(TK_COMMA);
    }
    syntax_state = SNTX_LF_HT;
    skip(TK_SEMICOLON);
}

/**
 * 功能：声明符的解析
 * <declarator>
 *  --> {<pointer>}<direct_declarator>
 */
void declarator()
{
    while(token == TK_STAR)
    {
        get_token();
    }
    direct_declarator();
}


/**
 * 功能：直接声明符解析
 * <direct_declarator>
 *  --> <IDENTIFIER><direct_declarator_postfix>
 */
void direct_declarator()
{
    if (token >= TK_IDENT)
    {
        get_token();
    }
    else
    {
        expect("Identifier\n");
    }
    direct_declarator_postfix();
}


/**
 * 功能：直接声明符后缀
 * <direct_declarator_postfix>
 *  --> {<TK_OPENBR><TK_CINT><TK_CLOSBR>}
 *    | <TK_OPENBR><TK_CLOSBR>
 *    | <TK_OPENPA><parameter_type_list><TK_CLOSPA>
 *    | <TK_OPENPA><TK_CLOSPA>
 */
void direct_declarator_postfix()
{
    int n;
    if (token == TK_OPENPA)
    {
        parameter_type_list();
    }
    else if (token == TK_OPENBR)
    {
        get_token();
        if (token == TK_CINT)
        {
            get_token();
            n = tkvalue;
        }
        skip(TK_CLOSBR);
        direct_declarator_postfix();
    }
}

/**
 * 功能：解析形参类型表
 * <parameter_type_list>
 *  --> <parameter_declaration>{<TK_COMMA><parameter_declaration>}
 * <parameter_declaration> --> <type_specifier><declarator>
 */
void parameter_type_list()
{
    get_token();
    while(token != TK_CLOSPA)
    {
        if (!type_specifier())
        {
            error("invalid type specifier");
        }
        declarator();
        if (token == TK_CLOSPA)
            break;
        skip(TK_COMMA);
    }
    syntax_state = SNTX_DELAY;
    skip(TK_CLOSPA);
    if (token == TK_BEGIN)
        syntax_state = SNTX_LF_HT;  // function define
    else syntax_state = SNTX_NUL;   // function declaration

    syntax_indent();
}


/**
 * 功能：函数体解析
 * <funcbody> --> <compound_statement>
 */
void funcbody()
{
    compound_statement();
}

/**
 * 功能：解析初值符
 * <initializer> --> <assignment_expression>
 */
void initializer()
{
    assignment_expression();
}

/**
 * 功能：语句解析
 * <statement> --> <compound_statement>
 *  | <if_statement> | <return_statement> | <break_statement>
 *  | <continue_statement> | <for_statement> | <expression_statement>
 */
void statement()
{
    switch (token)
    {
    case TK_BEGIN:
        compound_statement();
        break;
    case KW_IF:
        if_statement();
        break;
    case KW_RETURN:
        return_statement();
        break;
    case KW_BREAK:
        break_statement();
        break;
    case KW_CONTINUE:
        continue_statement();
        break;
    case KW_FOR:
        for_statement();
        break;
    
    default:
        expression_statement();
        break;
    }
}

/**
 * 功能：解析复合语句
 * <compound_statement> --> 
 *  <TK_BEGIN>{<declaration> | <statement>}<TK_END>
 */
void compound_statement()
{
    syntax_state = SNTX_LF_HT;
    syntax_level++;

    get_token();

    while (token != TK_END)
    {
        if (is_type_specifier(token))
            external_declaration(SC_LOCAL);
        else statement();
    }
    syntax_state = SNTX_LF_HT;
    get_token();
}


/**
 * 功能：判断是否为类型区分符
 * v：单词编号
 */
int is_type_specifier(int v)
{
    switch (v)
    {
    case KW_SHORT:
    case KW_INT:
    case KW_VOID:
    case KW_STRUCT:
    case KW_CHAR:
        return 1;
    
    default:
        break;
    }
    return 0;
}

/**
 * 功能：表达式语句解析
 * <expression_statement> --> <TK_SEMICOLON>|<expression><TK_SEMICOLON>
 */
void expression_statement()
{
    if (token != TK_SEMICOLON)
        expression();
    
    syntax_state = SNTX_LF_HT;
    skip(TK_SEMICOLON);
}

/**
 * 功能：选择语句解析
 * <if_statement> --> <KW_IF><TK_OPENPA><expression><TK_CLOSPA><statement>
 *      [<KW_ELSE><statement>]
 */
void if_statement()
{
    syntax_state = SNTX_SP;
    get_token();
    skip(TK_OPENPA);
    expression();
    syntax_state = SNTX_LF_HT;
    skip(TK_CLOSPA);
    statement();
    if (token == KW_ELSE)
    {
        syntax_state = SNTX_LF_HT;
        get_token();
        statement();
    }
}


/**
 * 功能：循环语句解析
 * <for_statement> --> <KW_FOR><TK_OPENPA>
 *   <expression_statement><expression_statement><expression>
 *   <TK_CLOSPA><statement>
 */
void for_statement()
{
    get_token();
    skip(TK_OPENPA);
    if (token != TK_SEMICOLON)
        expression();
    skip(TK_SEMICOLON);
    if (token != TK_SEMICOLON)
        expression();
    skip(TK_SEMICOLON);
    if(token != TK_CLOSPA)
        expression();

    syntax_state = SNTX_LF_HT;
    skip(TK_CLOSPA);
    statement();
}

/**
 * 功能：continue语句解析
 */
void continue_statement()
{
    get_token();
    syntax_state = SNTX_LF_HT;
    skip(TK_SEMICOLON);
}

/**
 * 功能：break语句解析
 */
void break_statement()
{
    get_token();
    syntax_state = SNTX_LF_HT;
    skip(TK_SEMICOLON);
}

/**
 * 功能：return语句解析
 * <return_statement> --> <KW_RETURN><TK_SEMICOLON>
 *  | <KW_RETURN><expression><TK_SEMICOLON>
 */
void return_statement()
{
    syntax_state = SNTX_DELAY;
    get_token();
    if (token==TK_SEMICOLON)
        syntax_state = SNTX_NUL;
    else
        syntax_state = SNTX_SP;
    syntax_indent();

    if (token != TK_SEMICOLON)
        expression();
    syntax_state = SNTX_LF_HT;
    skip(TK_SEMICOLON);
}

/**
 * 功能：解析表达式
 * <expression>--><assignment_expression>{<TK_COMMA><assignment_expression>}
 */
void expression()
{
    while (1)
    {
        assignment_expression();
        if (token != TK_COMMA)
            break;
        get_token();
    }
}

/**
 * 功能：解析赋值表达式
 * <assignment_expression>
 *  --> <equality_expression>|<unary_expression><TK_ASSIGN><assignment_expression>
 */
void assignment_expression()
{
    equality_expression();
    if(token == TK_ASSIGN)
    {
        get_token();
        assignment_expression();
    }
}

/**
 * 功能：解析相等类表达式
 * <equality_expression> --> <relational_expression>
 *  {<TK_EQ><relational_expression>}|<TK_NEQ><relational_expression>
 */
void equality_expression()
{
    relational_expression();
    while (token == TK_EQ || token == TK_NEQ)
    {
        get_token();
        relational_expression();
    }
}

/**
 * 功能：关系表达式
 * <relational_expression> --> <additive_expression>{
 *  <TK_LT><additive_expression>
 *  | <TK_GT><additive_expression>
 *  | <TK_LEQ><additive_expression>
 *  | <TK_GEQ><additive_expression>}
 */
void relational_expression()
{
    additive_expression();
    while ((token == TK_LT || token == TK_LEQ) ||
           (token == TK_GT || token == TK_GEQ))
    {
        get_token();
        additive_expression();
    }
}

/**
 * 功能：加减类表达式
 * <additive_expression> --> <multiplicative_expression>
 *  {<TK_PLUS>|<TK_MINUS><multiplicative_expression>}
 */
void additive_expression()
{
    multiplicative_expression();
    while (token==TK_PLUS || token==TK_MINUS)
    {
        get_token();
        multiplicative_expression();
    }
}

/**
 * 功能：乘法除法表达式
 * <multiplicative_expression> --> <unary_expression>
 *  {<TK_STAR>|<TK_DIVIDE>|<TK_MOD><unary_expression>}
 */ 
void multiplicative_expression()
{
    unary_expression();
    while (token == TK_STAR || token ==TK_DIVIDE || token == TK_MOD)
    {
        get_token();
        unary_expression();
    }
}

/**
 * 功能：一元表达式解析
 * <unary_expression> --> <postfix_expression>
 *  | <TK_AND>|<TK_STAR> <unary_expression>
 *  |<sizeof_expression>
 */
void unary_expression()
{
    switch (token)
    {
    case TK_AND:
        get_token();
        unary_expression();
        break;
    case TK_OR:
        get_token();
        unary_expression();
        break;
    case TK_STAR:
        get_token();
        unary_expression();
        break;
    case TK_PLUS:
        get_token();
        unary_expression();
        break;
    case TK_MINUS:
        get_token();
        unary_expression();
        break;
    case KW_SIZEOF:
        sizeof_expression();
        break;
    default:
        postfix_expression();
        break;
    }
}


/**
 * 功能：解析sizeof表达式
 * <sizeof_expression> --> <KW_SIZEOF><TK_OPENPA><type_specifier><TK_CLOSEPA>
 */
void sizeof_expression()
{
    get_token();
    skip(TK_OPENPA);
    type_specifier();
    skip(TK_CLOSPA);
}

/**
 * 功能：后缀表达式
 * <postfix_expression> --> <primary_expression>
 *  {<TK_OPENBR><expression><TK_CLOSEBR>|<TK_OPENPA><TK_CLOSEPA>
 *  |<TK_DOT><IDENTIFIER>|<TK_POINTSTO><IDENTIFIER>}
 */
void postfix_expression()
{
    primary_expression();
    while (1)
    {
        if (token == TK_DOT || token == TK_POINTO)
        {
            get_token();
            token |= SC_MEMBER;
            get_token();
        } else if (token == TK_OPENBR)
        {
            get_token();
            expression();
            skip(TK_CLOSBR);
        } else if (token == TK_OPENPA)
        {
            argument_expression_list();
        } else break;
    }
} 

/**
 * 功能：解析初值表达式
 * <primary_expression> --> <IDENTIFIER>|<TK_CINT>|<TK_CSTR>|<TK_CCHAR>|
 *  <TK_OPENPA><expression><TK_CLOSEPA>
 */
void primary_expression()
{
    int t;
    switch (token)
    {
    case TK_CINT:
    case TK_CCHAR:
    case TK_CSTR:
        get_token();
        break;
    case TK_OPENPA:
        get_token();
        expression();
        skip(TK_CLOSPA);
        break;
    default:
        t = token;
        get_token();
        if (t < TK_CINT) {
            expect("Identifier or constant value.");
        }
        break;
    }
}

/**
 * 功能：解析实参表达式
 * <argument_expression_list> --> <assignment_expression>
 *  { <TK_COMMA><assignment_expression> }
 */
void argument_expression_list()
{
    get_token();
    if (token != TK_CLOSPA)
    {
        for(;;)
        {
            assignment_expression();
            if (token == TK_CLOSPA)
                break;
            skip(TK_COMMA);
        }
    }
    skip(TK_CLOSPA);
}
