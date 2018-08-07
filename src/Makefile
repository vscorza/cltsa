#CC=gcc
CC=gcc-8
CFLAGS= -Wall -Wextra -pedantic -g -lm -Wno-unused-variable -Wno-unused-parameter -O0
#CFLAGS= -Wall -Wextra -pedantic -lm -Wno-unused-variable -Wno-unused-parameter -O3
NASM=nasm
NASMFLAGS=-f elf64 -g -F DWARF
LEX = lex
YACC = yacc -d
#YACC = yacc -d
#YACCFLAGS = --debug --verbose
#YACCFLAGS = -Wconflicts-sr -Wconflicts-rr --debug --verbose
#YACCFLAGS = -Wconflicts-sr -Wconflicts-rr --debug
YACCFLAGS = 

all: main

main: main.c parser_utils_c.o y.tab.o lex.yy.o automaton_c.o automaton_utils_c.o automaton_context_c.o  bucket_list_c.o automaton_mu_calculus_c.o ordered_list_c.o max_heap_c.o obdd_c.o dictionary_c.o
	$(CC) $(CFLAGS) $^ -o $@

automaton_utils_c.o: automaton_utils.c
	$(CC) $(CFLAGS) -c $< -o $@

dictionary_c.o: dictionary.c automaton_utils_c.o
	$(CC) $(CFLAGS) -c $< -o $@


automaton_c.o: automaton.c bucket_list_c.o automaton_mu_calculus_c.o
	$(CC) $(CFLAGS) -c $< -o $@

automaton_context_c.o: automaton_context.c parser_utils_c.o automaton_mu_calculus_c.o
	$(CC) $(CFLAGS) -c $< -o $@

automaton_mu_calculus_c.o: automaton_mu_calculus.c automaton_utils_c.o
	$(CC) $(CFLAGS) -c $< -o $@

bucket_list_c.o: bucket_list.c automaton_utils_c.o
	$(CC) $(CFLAGS) -c $< -o $@

ordered_list_c.o: ordered_list.c automaton_utils_c.o
	$(CC) $(CFLAGS) -c $< -o $@

obdd_c.o: obdd.c automaton_utils_c.o
	$(CC) $(CFLAGS) -c $< -o $@

max_heap_c.o: max_heap.c automaton_utils_c.o
	$(CC) $(CFLAGS) -c $< -o $@

parser: y.tab.o lex.yy.o parser_utils_c.o automaton_utils_c.o obdd_c.o
	$(CC) $(CFLAGS) $^ -o $@

parser_utils_c.o: parser_utils.c automaton_utils_c.o
	$(CC) $(CFLAGS) -c $< -o $@

lex.yy.o: lex.yy.c y.tab.h
lex.yy.o y.tab.o: parser_utils.h obdd.h

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