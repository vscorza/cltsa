#CC=gcc
CC=c99
CFLAGS= -Wall -Wextra -pedantic -O0 -g -lm -Wno-unused-variable -Wno-unused-parameter
NASM=nasm
NASMFLAGS=-f elf64 -g -F DWARF
LEX = lex
YACC = yacc -d
#YACCFLAGS = --debug --verbose
YACCFLAGS = -Wconflicts-sr -Wconflicts-rr --debug --verbose


all: main parser

main: main.c parser_utils_c.o y.tab.o lex.yy.o automaton_c.o automaton_utils_c.o automaton_context_c.o  
	$(CC) $(CFLAGS) $^ -o $@

automaton_utils_c.o: automaton_utils.c
	$(CC) $(CFLAGS) -c $< -o $@

automaton_c.o: automaton.c
	$(CC) $(CFLAGS) -c $< -o $@

automaton_context_c.o: automaton_context.c parser_utils_c.o
	$(CC) $(CFLAGS) -c $< -o $@

parser: y.tab.o lex.yy.o parser_utils_c.o automaton_utils_c.o
	$(CC) $(CFLAGS) $^ -o $@

parser_utils_c.o: parser_utils.c automaton_utils_c.o
	$(CC) $(CFLAGS) -c $< -o $@

lex.yy.o: lex.yy.c y.tab.h
lex.yy.o y.tab.o: parser_utils.h

y.tab.c y.tab.h: henos.yacc
	$(YACC) $(YACCFLAGS) -v henos.yacc

y.tab.o: y.tab.c
	$(CC) $(CFLAGS) -c $< -o $@

lex.yy.c: henos.lex
	$(LEX) henos.lex

clean:
	rm -f *.o
	rm -f main 
	rm -f *.o lex.yy.c *.tab.*  parser *.output