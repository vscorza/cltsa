#CC=gcc
#CC=gcc-8
CC=clang
#CPARAMS = -Wall -Wextra -pedantic -Wno-unused-variable -Wno-unused-parameter -Wno-incompatible-pointer-types
CPARAMS =  -Wno-unused-variable -Wno-unused-parameter -Wno-format -Wno-incompatible-pointer-types -Wno-int-conversion -Wno-pointer-integer-compare -Wno-string-compare -Wno-switch
CLIBS = -lm
CFLAGS= -g -O0 
#CFLAGS= -g -O3
NASM=nasm
NASMFLAGS=-f elf64 -g -F DWARF
LEX = lex
YACC = yacc -d
#YACCFLAGS = --debug --verbose
#YACCFLAGS = -Wconflicts-sr -Wconflicts-rr --debug --verbose
#YACCFLAGS = -Wconflicts-sr -Wconflicts-rr --debug
YACCFLAGS = 

format:
	clang-format -i *.h *.c

all: cltsa

cltsa: main.c cltsa_runner_c.o parser_utils_c.o y.tab.o lex.yy.o automaton_c.o automaton_utils_c.o \
	automaton_context_c.o automaton_importer_c.o bucket_list_c.o ordered_list_c.o \
	max_heap_c.o obdd_c.o obdd_cache_c.o dictionary_c.o fast_pool_c.o bool_array_hash_table_c.o \
	automaton_composite_hash_table_c.o automaton_serializer_c.o automaton_composition_c.o \
	automaton_gr1_c.o automaton_diagnosis_c.o automaton_context_from_obdd_c.o
	$(CC) $(CPARAMS) $(CFLAGS) $^ -o $@ $(CLIBS) 

cltsa_runner_c.o: cltsa_runner.c parser_utils_c.o y.tab.o lex.yy.o automaton_c.o automaton_utils_c.o \
	automaton_context_c.o automaton_importer_c.o bucket_list_c.o ordered_list_c.o \
	max_heap_c.o obdd_c.o obdd_cache_c.o dictionary_c.o fast_pool_c.o bool_array_hash_table_c.o \
	automaton_composite_hash_table_c.o automaton_serializer_c.o automaton_composition_c.o \
	automaton_gr1_c.o automaton_diagnosis_c.o automaton_context_from_obdd_c.o
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS) 

automaton_utils_c.o: automaton_utils.c automaton_utils.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS) 

dictionary_c.o: dictionary.c automaton_utils_c.o dictionary.h automaton_utils.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS) 

automaton_c.o: automaton.c bucket_list_c.o parser_utils_c.o automaton_serializer_c.o \
	automaton_composition_c.o automaton_gr1_c.o automaton_diagnosis_c.o automaton.h #automaton_commom_structs.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS) 

automaton_serializer_c.o: automaton_serializer.c bucket_list_c.o parser_utils_c.o automaton.h parser_utils.h #automaton_commom_structs.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS) 

automaton_composition_c.o: automaton_composition.c bucket_list_c.o parser_utils_c.o bucket_list.h automaton.h parser_utils.h #automaton_commom_structs.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS)

automaton_gr1_c.o: automaton_gr1.c bucket_list_c.o parser_utils_c.o bucket_list.h automaton.h parser_utils.h #automaton_commom_structs.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS)
	
automaton_diagnosis_c.o:  automaton_diagnosis.c  bucket_list_c.o parser_utils_c.o bucket_list.h automaton.h parser_utils.h #automaton_commom_structs.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS)	

automaton_context_c.o: automaton_context.c parser_utils_c.o automaton_context_from_obdd_c.o automaton_context.h bucket_list.h automaton.h parser_utils.h #automaton_commom_structs.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS) 
	
automaton_context_from_obdd_c.o: automaton_context_from_obdd.c parser_utils_c.o automaton_context.h automaton.h parser_utils.h #automaton_commom_structs.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS) 	

automaton_importer_c.o: automaton_importer.c automaton_utils_c.o  automaton_utils.h automaton.h parser_utils.h #automaton_commom_structs.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS) 

bucket_list_c.o: bucket_list.c automaton_utils_c.o parser_utils_c.o bucket_list.h automaton_utils.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS) 

ordered_list_c.o: ordered_list.c automaton_utils_c.o parser_utils_c.o ordered_list.h parser_utils.h automaton_utils.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS) 

bool_array_hash_table_c.o: bool_array_hash_table.c automaton_utils_c.o parser_utils_c.o bool_array_hash_table.h automaton_utils.h parser_utils.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS)
	
automaton_composite_hash_table_c.o: automaton_composite_hash_table.c automaton_utils_c.o parser_utils_c.o automaton_composite_hash_table.h automaton_utils.h parser_utils.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS)

obdd_c.o: obdd.c automaton_utils_c.o obdd.h automaton_utils.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS)
	
obdd_cache_c.o: obdd_cache.c obdd_c.o obdd.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS)	

max_heap_c.o: max_heap.c automaton_utils_c.o parser_utils_c.o max_heap.h automaton_utils.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS)

fast_pool_c.o: fast_pool.c automaton_utils_c.o fast_pool.h automaton_utils.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@ $(CLIBS)

parser: y.tab.o lex.yy.o parser_utils_c.o automaton_utils_c.o obdd_c.o obdd_cache_c.o dictionary_c.o fast_pool_c.o parser_utils.h automaton_utils.h obdd.h dictionary.h fast_pool.h
	$(CC) $(CPARAMS) $(CFLAGS) $^ -o $@ $(CLIBS)

parser_utils_c.o: parser_utils.c automaton_utils_c.o parser_utils.h
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@  $(CLIBS)

lex.yy.o: lex.yy.c y.tab.h
lex.yy.o y.tab.o: parser_utils.h obdd.h

y.tab.c y.tab.h: cltsa.yacc
	$(YACC) $(YACCFLAGS) -v cltsa.yacc

y.tab.o: y.tab.c
	$(CC) $(CPARAMS) $(CFLAGS) -c $< -o $@  $(CLIBS)

lex.yy.c: cltsa.lex
	$(LEX) cltsa.lex

.PHONY: clean
clean:
	rm -f *.o
	rm -f main 
	rm -f *.o lex.yy.c *.tab.*  parser *.output
