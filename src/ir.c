#include <stdlib.h>
#include <stdio.h>
#include "syntax.h"
#include "lex.h"
#include "json.h"
#include "symbol.h"
#include "token.h"
#include "dynstack.h"

FILE *fin;
int token;
int line_num;
int column_num;
Symbol *sym_sec_rdata;

void init() {
    line_num = 1;
    column_num = 0;
    init_lex();

    stack_init(&local_sym_stack, 16);
    stack_init(&global_sym_stack, 16);
    sym_sec_rdata = sec_sym_put(".rdata", 0);

    int_type.t = T_INT;
    char_pointer_type.t = T_CHAR;
    mk_pointer(&char_pointer_type);
    default_func_type.t = T_FUNC;
    default_func_type.ref = sym_push(SC_ANOM, &int_type, 0, 0);
}

void cleanup() 
{
    int i;
    sym_pop(&global_sym_stack, NULL);
    stack_destroy(&local_sym_stack);
    stack_destroy(&global_sym_stack);

    printf("\ntktable.count = %d\n", tktable.count);
    for(i = TK_IDENT; i < tktable.count; i++)
        free(tktable.data[i]);
    free(tktable.data);
}

int main(int argc, char const *argv[])
{
    fin  = fopen(argv[1], "rb");

    if (!fin) {
        printf("cannot open SC source file\n");
        return 0;
    }

    init();
    getch();
    get_token();
    JSONobj *obj = translation_unit();
    printJSON(obj);
    destroyJSON(obj);
    cleanup();
    fclose(fin);
    return 0;
}
