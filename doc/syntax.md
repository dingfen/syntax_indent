# SC语法分析与实现

## 外部定义

### 外部定义

TranslateUnit -> (ExternDeclaration)* EOF

很简单，这个文法对应的程序是

```C
void translation_unit()
{
    while(token != TK_EOF)
    {
        external_declaration(SC_GLOBAL);
    }
}
```

但是有些时候，因为定义的文法不是LL(1)文法，实现起来不容易，此时就需要进行文法等价转换。

比如：ExternDeclaration -> func_def | declaration

在这一文法中因为 func_def -> type decl_id func_body 而 declaration -> type (sdecls)+ 。注意到它们的开头都是type，因此不符合LL(1)文法。

因此实现时，需要转换为：

ExternDeclaration -> type ( ; | decl_id func_body | decl_id (= init_val )? ( ,  decl_id(= init_val)? )* ;)

