CC=c99
CFLAGS= -Wall -Wextra -pedantic -O0 -g -lm -Wno-unused-variable -Wno-unused-parameter
NASM=nasm
NASMFLAGS=-f elf64 -g -F DWARF
LEX = lex
YACC = yacc -d
YACCFLAGS = -Wconflicts-sr -Wconflicts-rr

all: main parser

main: main.c automaton_c.o
	$(CC) $(CFLAGS) $^ -o $@

automaton_c.o: automaton.c
	$(CC) $(CFLAGS) -c $< -o $@

parser: y.tab.o lex.yy.o parser_utils_c.o
	$(CC) $(CFLAGS) $^ -o $@

parser_utils_c.o: parser_utils.c
	$(CC) $(CFLAGS) -c $< -o $@

lex.yy.o: lex.yy.c y.tab.h
lex.yy.o y.tab.o: parser_utils.h

y.tab.c y.tab.h: henos.yacc
	$(YACC) $(YACCFLASGS) -v henos.yacc

lex.yy.c: henos.lex
	$(LEX) henos.lex

clean:
	rm -f *.o
	rm -f main 
	rm -f *.o lex.yy.c *.tab.*  parser *.output