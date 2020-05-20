CC=gcc
CFLAGS=-I include/
LD=ld
LDFLAGS=-I include/
BIN=lex syntax

.PHONY: clean

TARGET: lex syntax
	mkdir build
	mv $^ build/

lex : src/lexcolor.c src/lex.c src/token.c src/dyn.c src/error.c
	$(CC) $(CFLAGS) -o $@ $^

syntax : src/syntaxindent.c src/syntax.c src/lex.c src/token.c src/error.c src/dyn.c 
	$(CC) $(CFLAGS) -o $@ $^ -D __SYNTAX_INDENT

clean:
	rm -rf build/

