#include "token.h"
#include "syntax.h"
#include "symbol.h"
#include "json.h"

/* 全局变量 */
extern int token;
extern int tkvalue;
extern int ch;
extern DynString tkstr;

/**
 * 功能：翻译单元，语法分析顶层
 * <translation_unit> ::= {<external_declaration>}<TK_EOF>
 */
JSONobj* translation_unit() 
{
    static JSONobj tu;
    initJSON(&tu);
    while(token != TK_EOF) 
    {
        JSONobj tmp = external_declaration(SC_GLOBAL);
        Value val;
        val.obj = &tmp;
        addJSON(&tu, "external_declaration", val, V_JSON);
    }
    return &tu;
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
JSONobj external_declaration(int l)
{
    JSONobj ed;
    Type btype, type;
    int v, has_init, r, addr;
    Symbol* sym;
    initJSON(&ed);
    if(!type_specifier(&btype))
    {
        expect("<type id>");
    }
    if (btype.t == T_STRUCT && token == TK_SEMICOLON)
    {
        get_token();
        return ed;
    }
    Value val;
    val.data = btype.t;
    addJSON(&ed, "Type", val, V_INT);
    while (1)
    {
        type = btype;
        declarator(&type, &v, NULL);
        if (token == TK_BEGIN)      // 函数定义
        {
            if (l == SC_LOCAL)
            {
                error("nested func declarator unsupported.\n");
            }
            if ((type.t & T_BTYPE) != T_FUNC)
                expect("<function definition>");
            
            sym = sym_search(v);
            if (sym)        // functions have been declared now the definition
            {
                if ((sym->type.t & T_BTYPE) != T_FUNC)
                    error("%s multi-defined", get_tkstr(v));
                sym->type = type;
            }
            else 
            {
                sym = func_sym_push(v, &type);
            }
            sym->r = SC_SYM | SC_GLOBAL;
            funcbody(sym);
            break;
        }
        else
        {
            if ((type.t & T_BTYPE)== T_FUNC)    // 函数声明
            {
                if (sym_search(v) == NULL)
                {
                    sym = sym_push(v, &type, SC_GLOBAL|SC_SYM, 0);
                }
            }
            else            // 变量声明
            {
                r = 0;
                if (!(type.t & T_ARRAY))
                    r |= SC_LVAL;
                r |= 0xf1;
                has_init = (token == TK_ASSIGN);

                if (has_init)
                {
                    get_token();
                    initializer(&type);
                }
                sym = var_sym_put(&type, r, v, addr);
            }
            if (token == TK_COMMA)
            {
                get_token();
            }
            else
            {
                skip(TK_SEMICOLON);
                break;
            }
        }
    }
    return ed;
}

/**
 * 功能：自行添加  制作指针
 * type 输入的类型
 */
void mk_pointer(Type *type) 
{
    type->t = T_PTR;
} 

/**
 * 功能：解析类型区分符
 * type 输出 数据类型
 * 返回值：是否发现合法的类型区分符
 * <type_specifier>
 *  --> <KW_INT>|<KW_CHAR>|<KW_SHORT>|<KW_VOID>|<struct_specifier>
 */
int type_specifier(Type *type)
{
    int t, type_found;
    Type typel;
    t= 0;
    switch (token)
    {
    case KW_CHAR:
        type_found = 1;
        t = T_CHAR;
        get_token();
        break;
    case KW_SHORT:
        type_found = 1;
        t = T_SHORT;
        get_token();
        break;
    case KW_VOID:
        type_found = 1;
        t = T_INT;
        get_token();
        break;
    case KW_INT:
        type_found = 1;
        get_token();
        break;
    case KW_STRUCT:
        struct_specifier(&typel);
        type->ref = typel.ref;
        type_found = 1;
        t = T_STRUCT;
        break;

    default:
        break;
    }
    type->t = t;
    return type_found;
}

/**
 * 功能：结构体类型区分符
 * type 输出 结构类型
 * <struct_specifier>
 *   --> <KW_STRUCT><IDENTIFIER><TK_BEGIN><struct_declaration_list><TK_END>
 *      | <KW_STRUCT><IDENTIFIER>
 */
void struct_specifier(Type *type)
{
    int v;
    Symbol *s;
    Type typel;

    get_token();        // struct
    v = token;
    
    get_token;          // identifier

    if (v < TK_IDENT)   // 关键字不作为结构名称
        expect("<struct name>");
    s = struct_search(v);
    if (!s)             // 结构体尚未定义
    {
        typel.t = KW_STRUCT;
        s = sym_push(v|SC_STRUCT, &typel, 0, -1);
        s->r = 0;
    }
    type->t = T_STRUCT;
    type->ref = s;

    if (token == TK_BEGIN)
        struct_declaration_list(type);
}

/**
 * 功能：结构体中声明的变量的全体
 * type 输出 结构类型
 * <struct_declaration_list> 
 *  --> <struct_declaration>{<struct_declaration>}
 */
void struct_declaration_list(Type * type)
{
    int maxalign, offset;
    Symbol *s, **ps;
    s = type->ref;

    get_token();
    if (s->c != -1)
        error("struct defined.");
    
    ps = &s->next;
    maxalign = 1;
    offset = 0;
    while (token != TK_END)
    {
        struct_declaration(&maxalign, &offset, &ps);
    }
    skip(TK_END);

    s->c = calc_align(offset, maxalign);
    s->r = maxalign;
}

/**
 * 功能：计算字节对齐位置
 * n 未对齐前值
 * align 对齐粒度
 */
int calc_align(int n, int align)
{
    return ((n+align-1)&(~(align-1)));
}

/**
 * 功能：结构体中声明的变量
 * maxalign 输入输出 成员最大对齐粒度
 * offset 输入输出 偏移量
 * ps 输出 结构定义符号
 * <strcut_declaration>
 *  --> <type_specifier><struct_declarator_list><TK_SEMICOLON>
 * <struct_declarator_list>
 *  --> <declarator>{<TK_COMMA><declarator>}
 */
void struct_declaration(int *maxalign, int *offset, Symbol ***ps)
{
    int v, size, align;
    Symbol *ss;
    Type typel, btype;
    int force_align;

    type_specifier(&btype);
    while (1)
    {
        v = 0;
        typel = btype;
        declarator(&typel, &v, &force_align);
        size = type_size(&typel, &align);

        // if (force_align & ALIGN_SET)
        //     align = force_align & ~ALIGN_SET;

        *offset = calc_align(*offset, align);

        if (align > *maxalign)
            *maxalign = align;
        ss = sym_push(v|SC_MEMBER, &typel, 0, *offset);
        *offset += size;
        **ps = ss;
        *ps = &ss->next;

        if (token == TK_SEMICOLON)
            break;
        skip(TK_COMMA);
    }
    skip(TK_SEMICOLON);
}


/**
 * 功能：结构成员对齐
 * force_align 强制对齐粒度
 */
void struct_member_alignment(int *force_align) 
{
    *force_align = 4;
}

/**
 * 功能：声明符的解析
 * type 数据类型
 * v   输出 单词编号
 * force_align 强制对齐粒度
 * <declarator>
 *  --> {<pointer>}<direct_declarator>
 */
void declarator(Type *type, int *v, int *force_align)
{
    int fc;
    while(token == TK_STAR)
    {
        mk_pointer(type);
        get_token();
    }
    function_calling_convention(&fc);
    if (force_align)
        struct_member_alignment(force_align);
    direct_declarator(type, v, fc);
}

/**
 * 功能： 函数调用约定
 * fc 输出 调用约定
 * 用于函数声明 不用在数据声明
 */
void function_calling_convention(int *fc)
{
    /*
    *fc = KW_CDECL;
    if (token == KW_CDECL || token == KW_STDCALL)
    {
        *fc = token;
        get_token();
    }
    */
} 

/**
 * 功能：直接声明符解析
 * type 输入输出 数据类型
 * v 输出  单词编号
 * func_call 函数调用约定
 * <direct_declarator>
 *  --> <IDENTIFIER><direct_declarator_postfix>
 */
void direct_declarator(Type *type, int *v, int func_call)
{
    if (token >= TK_IDENT)
    {
        *v = token;
        get_token();
    }
    else
    {
        expect("Identifier\n");
    }
    direct_declarator_postfix(type, func_call);
}

/**
 * 功能：直接声明符后缀
 * type 输入输出 数据类型
 * func_call 函数调用约定
 * <direct_declarator_postfix>
 *  --> {<TK_OPENBR><TK_CINT><TK_CLOSBR>}
 *    | <TK_OPENBR><TK_CLOSBR>
 *    | <TK_OPENPA><parameter_type_list><TK_CLOSPA>
 *    | <TK_OPENPA><TK_CLOSPA>
 */
void direct_declarator_postfix(Type *type, int func_call)
{
    int n;
    Symbol *s;

    if (token == TK_OPENPA)
    {
        parameter_type_list(type, func_call);
    }
    else if (token == TK_OPENBR)
    {
        get_token();
        n = -1;
        if (token == TK_CINT)
        {
            get_token();
            n = tkvalue;
        }
        skip(TK_CLOSBR);
        direct_declarator_postfix(type, func_call);

        s = sym_push(SC_ANOM, type, 0, n);
        type->t = T_ARRAY | T_PTR;
        type->ref = s;
    }
}

/**
 * 功能：解析形参类型表
 * type 输入输出 数据类型
 * func_call 函数调用约定
 * <parameter_type_list>
 *  --> <parameter_declaration>{<TK_COMMA><parameter_declaration>}
 * <parameter_declaration> --> <type_specifier><declarator>
 */
void parameter_type_list(Type *type, int func_call)
{
    int n;
    Symbol **plast, *s, *first;
    Type pt;

    get_token();
    first = NULL;
    plast = &first;
    while(token != TK_CLOSPA)
    {
        if (!type_specifier(&pt))
        {
            error("invalid type specifier");
        }
        declarator(&pt, &n, NULL);
        s = sym_push(n | SC_PARAMS, &pt, 0, 0);
        *plast = s;
        plast = &s->next;

        if (token == TK_CLOSPA)
            break;
        skip(TK_COMMA);
    }
    skip(TK_CLOSPA);
    // 此处将函数返回类型存储 然后指向参数 将type设为函数类型
    // 引用信息放在ref中
    s = sym_push(SC_ANOM, type, func_call, 0);
    s->next = first;
    type->t = T_FUNC;
    type->ref = s;    
}


/**
 * 功能：函数体解析
 * sym 函数符号
 * <funcbody> --> <compound_statement>
 */
void funcbody(Symbol *sym)
{
    // 放入一个匿名符号在局部符号表中
    sym_direct_push(&local_sym_stack, SC_ANOM, &int_type, 0);
    compound_statement(NULL, NULL);
    // 清空局部符号栈
    sym_pop(&local_sym_stack, NULL);
}

/**
 * 功能：解析初值符
 * <initializer> --> <assignment_expression>
 */
void initializer(Type *type)
{
    if (type->t & T_ARRAY)
    {
        get_token();
    }
    else
    {
        assignment_expression();
    }
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
void compound_statement(int *bsym, int *csym)
{
    Symbol *s;
    s = (Symbol *)stack_get_top(&local_sym_stack);

    get_token();

    while (is_type_specifier(token))
    {
        external_declaration(SC_LOCAL);
    }
    while(token != TK_END)
    {
        statement(bsym, csym);
    }
    sym_pop(&local_sym_stack, s);
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
    
    skip(TK_SEMICOLON);
}

/**
 * 功能：选择语句解析
 * <if_statement> --> <KW_IF><TK_OPENPA><expression><TK_CLOSPA><statement>
 *      [<KW_ELSE><statement>]
 */
void if_statement()
{
    get_token();
    skip(TK_OPENPA);
    expression();
    skip(TK_CLOSPA);
    statement();
    if (token == KW_ELSE)
    {
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

    skip(TK_CLOSPA);
    statement();
}

/**
 * 功能：continue语句解析
 */
void continue_statement()
{
    get_token();
    skip(TK_SEMICOLON);
}

/**
 * 功能：break语句解析
 */
void break_statement()
{
    get_token();
    skip(TK_SEMICOLON);
}

/**
 * 功能：return语句解析
 * <return_statement> --> <KW_RETURN><TK_SEMICOLON>
 *  | <KW_RETURN><expression><TK_SEMICOLON>
 */
void return_statement()
{
    get_token();

    if (token != TK_SEMICOLON)
        expression();
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
    int align, size;
    Type type;

    get_token();
    skip(TK_OPENPA);
    type_specifier(&type);
    skip(TK_CLOSPA);

    size = type_size(&type, &align);
    if (size < 0)
        error("sizeof() get size of type failed.\n");
}

/**
 * 功能：返回类型长度
 * t 数据类型指针
 * a 对齐值
 */
int type_size(Type *t, int *a)
{
    Symbol *s;
    int bt;

    int PTR_SIZE = 4;
    bt = t->t & T_BTYPE;
    switch (bt)
    {
    case T_STRUCT:
        s = t->ref;
        *a = s->r;
        return s->c;
    
    case T_PTR:
        if (t->t & T_ARRAY)
        {
            s = t->ref;
            return type_size(&s->type, a) * s->c;
        }
        else 
        {
            *a = PTR_SIZE;
            return PTR_SIZE;
        }
    case T_INT:
        *a = 4;
        return 4;
    
    case T_SHORT:
        *a = 2;
        return 2;
    
    default:
        *a = 1;
        return 1;
    }
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
    int t, addr;
    Type type;
    Symbol *s;
    switch (token)
    {
    case TK_CINT:
    case TK_CCHAR:
    case TK_CSTR:
        t = T_CHAR;
        type.t = t;
        mk_pointer(&type);
        type.t |= T_ARRAY;
        var_sym_put(&type, SC_GLOBAL, 0, addr);
        initializer(&type);
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
        s = sym_search(t);
        if (!s)
        {
            if (token != TK_OPENPA)
                error("%s undeclared\n", get_tkstr(t));
            s = func_sym_push(t, &default_func_type);   // 允许函数不声明直接引用
            s->r = SC_GLOBAL | SC_SYM;
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
