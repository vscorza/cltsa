#CC=gcc
#CC=gcc-8
CC=clang
CPARAMS = -Wall -Wextra -pedantic -Wno-unused-variable -Wno-unused-parameter
CLIBS = -lm
CFLAGS= -g -O0
#CFLAGS= -g -O3
#CFLAGS= -fsanitize=address -g -O0 
#CFLAGS= -fsanitize=address -g -O3
#CFLAGS= -O3
#CFLAGS= -O2
#CFLAGS= -Ofast
#CFLAGS= -g -lasan -fsanitize=address -fno-omit-frame-pointer -fuse-ld=gold -O0
NASM=nasm
NASMFLAGS=-f elf64 -g -F DWARF
LEX = lex
YACC = yacc -d
#YACCFLAGS = --debug --verbose
#YACCFLAGS = -Wconflicts-sr -Wconflicts-rr --debug --verbose
#YACCFLAGS = -Wconflicts-sr -Wconflicts-rr --debug
YACCFLAGS = 


all: cltsa


cltsa: main.c parser_utils_c.o y.tab.o lex.yy.o automaton_c.o automaton_utils_c.o automaton_context_c.o bucket_list_c.o ordered_list_c.o max_heap_c.o obdd_c.o obdd_cache_c.o dictionary_c.o fast_pool_c.o bool_array_hash_table_c.o automaton_composite_hash_table_c.o
	$(CC) $(CPARAMS) $(CFLAGS) $^ -o $@ $(CLIBS) 

automaton_utils_c.o: automaton_utils.c
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS) 

dictionary_c.o: dictionary.c automaton_utils_c.o
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS) 

automaton_c.o: automaton.c bucket_list_c.o parser_utils_c.o
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS) 

automaton_context_c.o: automaton_context.c parser_utils_c.o 
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS) 

bucket_list_c.o: bucket_list.c automaton_utils_c.o parser_utils_c.o
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS) 

ordered_list_c.o: ordered_list.c automaton_utils_c.o parser_utils_c.o
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS) 

bool_array_hash_table_c.o: bool_array_hash_table.c automaton_utils_c.o parser_utils_c.o
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS)
	
automaton_composite_hash_table_c.o: automaton_composite_hash_table.c automaton_utils_c.o parser_utils_c.o
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS)

obdd_c.o: obdd.c automaton_utils_c.o
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS)
	
obdd_cache_c.o: obdd_cache.c obdd_c.o
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS)	

max_heap_c.o: max_heap.c automaton_utils_c.o parser_utils_c.o
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS)

fast_pool_c.o: fast_pool.c automaton_utils_c.o
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS)

parser: y.tab.o lex.yy.o parser_utils_c.o automaton_utils_c.o obdd_c.o
	$(CC) $(CPARAMS) $(CFLAGS) $^ -o $@ $(CLIBS)

parser_utils_c.o: parser_utils.c automaton_utils_c.o
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@  $(CLIBS)

lex.yy.o: lex.yy.c y.tab.h
lex.yy.o y.tab.o: parser_utils.h obdd.h

y.tab.c y.tab.h: henos.yacc
	$(YACC) $(YACCFLAGS) -v henos.yacc

y.tab.o: y.tab.c
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@  $(CLIBS)

lex.yy.c: henos.lex
	$(LEX) henos.lex

.PHONY: clean
clean:
	rm -f *.o
	rm -f main 
	rm -f *.o lex.yy.c *.tab.*  parser *.output
