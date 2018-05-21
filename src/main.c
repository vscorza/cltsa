#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include "automaton_context.h"

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern int yyparse();
extern YY_BUFFER_STATE yy_scan_string(char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern FILE* yyin;
extern automaton_program_syntax* parsed_program;

char* automataFile  =  "automata.txt";

void run_parse_test(char* test_file){
	FILE *fd;
    if (!(yyin = fopen(test_file, "r")))
    {
        perror("Error: ");
    }
	yyparse();
    //printf("\n\n%d\n\n\n", parsed_program->count);
    automaton_automata_context* ctx		= automaton_automata_context_create_from_syntax(parsed_program, "test context");
    automaton_automata_context_destroy(ctx);
    automaton_program_syntax_destroy(parsed_program);
    fclose(yyin);
}

void run_fsp_tests(uint32_t test_count){
	uint32_t i;
	char *buf[255];
	for(i = 1; i <= test_count; i++){
		printf("\n\n==============\nRunning fsp test %d\n==============\n", i);
		sprintf(buf, "test%d.fsp", i);
		run_parse_test(buf);
	}
}

void run_tree_tests(){
	automaton_composite_tree* tree	= automaton_composite_tree_create(3);
	uint32_t key1[3]				= {1,2,3};
	uint32_t key2[3]				= {1,3,4};
	uint32_t key3[3]				= {1,3,5};
	uint32_t key4[3]				= {2,3,5};
	printf("new key %d\n", automaton_composite_tree_get_key(tree, key1));
	printf("new key %d\n", automaton_composite_tree_get_key(tree, key2));
	printf("new key %d\n", automaton_composite_tree_get_key(tree, key3));
	printf("new key %d\n", automaton_composite_tree_get_key(tree, key2));
	printf("new key %d\n", automaton_composite_tree_get_key(tree, key4));
	automaton_composite_tree_print(tree);
	automaton_composite_tree_destroy(tree);
	automaton_composite_tree* tree2	= automaton_composite_tree_create(5);
	uint32_t key0_2[5]				= {0,0,0,0,0};
	uint32_t key1_2[5]				= {1,2,3,6,8};
	uint32_t key2_2[5]				= {1,3,4,6,9};
	uint32_t key3_2[5]				= {1,3,5,8,8};
	uint32_t key4_2[5]				= {2,3,5,8,8};
	uint32_t key5_2[5]				= {5,5,5,5,5};
	printf("new key %d\n", automaton_composite_tree_get_key(tree2, key0_2));
	printf("new key %d\n", automaton_composite_tree_get_key(tree2, key1_2));
	printf("new key %d\n", automaton_composite_tree_get_key(tree2, key2_2));
	printf("new key %d\n", automaton_composite_tree_get_key(tree2, key3_2));
	printf("new key %d\n", automaton_composite_tree_get_key(tree2, key2_2));
	printf("new key %d\n", automaton_composite_tree_get_key(tree2, key4_2));
	printf("new key %d\n", automaton_composite_tree_get_key(tree2, key5_2));
	uint32_t i, j, limit = 1, values = 10;

	for(i = 0; i < 5; i++){key0_2[i]	= 0; limit *= values;}
	for(i = 0; i < limit; i++){
		j = 4;
		while(key0_2[j] == values && j >= 0){
			key0_2[j]	= 0;
			j--;
		}
		key0_2[j]++;
		automaton_composite_tree_get_key(tree2, key0_2);
	}
	automaton_composite_tree_print(tree2);
	automaton_composite_tree_destroy(tree2);

}

void run_automaton_tests(){
	automaton_signal_event* in		= automaton_signal_event_create("in", INPUT_SIG);
	automaton_signal_event* out		= automaton_signal_event_create("out", OUTPUT_SIG);
	automaton_signal_event* tau		= automaton_signal_event_create("tau", INTERNAL_SIG);
	automaton_alphabet* alphabet	= automaton_alphabet_create();
	automaton_alphabet_add_signal_event(alphabet, in);
	automaton_alphabet_add_signal_event(alphabet, out);
	automaton_alphabet_add_signal_event(alphabet, tau);
	automaton_fluent** fluents		= malloc(sizeof(automaton_fluent*) * 2);
	fluents[0]	= automaton_fluent_create("f_in", false);
	fluents[1]	= automaton_fluent_create("f_out", false);
	automaton_fluent_add_starting_signal(fluents[0], alphabet, in);
	automaton_fluent_add_ending_signal(fluents[0], alphabet, tau);
	automaton_fluent_add_starting_signal(fluents[1], alphabet, out);
	automaton_fluent_add_ending_signal(fluents[1], alphabet, in);

	uint32_t fluents_count			= 2;
	automaton_automata_context* ctx	= automaton_automata_context_create("Context 1", alphabet, fluents_count, fluents);
	automaton_automata_context_print(ctx, "\t", "\n");
	uint32_t* local_alphabet_1		= malloc(sizeof(uint32_t) * 2);
	local_alphabet_1[0]				= automaton_alphabet_get_signal_index(alphabet, in);
	local_alphabet_1[1]				= automaton_alphabet_get_signal_index(alphabet, tau);
	uint32_t* local_alphabet_2		= malloc(sizeof(uint32_t) * 2);
	local_alphabet_2[0]				= automaton_alphabet_get_signal_index(alphabet, tau);
	local_alphabet_2[1]				= automaton_alphabet_get_signal_index(alphabet, out);
	automaton_automaton* automaton_1= automaton_automaton_create("Automaton 1", ctx, 2, local_alphabet_1);
	automaton_transition* t1_1_2	= automaton_transition_create(1, 2);
	automaton_transition* t1_2_1	= automaton_transition_create(2, 1);
	automaton_transition_add_signal_event(t1_1_2, ctx, in);
	automaton_transition_add_signal_event(t1_2_1, ctx, tau);
	automaton_automaton_add_transition(automaton_1, t1_1_2);
	automaton_automaton_add_transition(automaton_1, t1_2_1);
	automaton_automaton_add_initial_state(automaton_1, 1);
	automaton_automaton* automaton_2= automaton_automaton_create("Automaton 2", ctx, 2, local_alphabet_2);
	automaton_transition* t2_1_2	= automaton_transition_create(1, 2);
	automaton_transition* t2_2_1	= automaton_transition_create(2, 1);
	automaton_transition_add_signal_event(t2_1_2, ctx, tau);
	automaton_transition_add_signal_event(t2_2_1, ctx, out);
	automaton_automaton_add_transition(automaton_2, t2_1_2);
	automaton_automaton_add_transition(automaton_2, t2_2_1);
	automaton_automaton_add_initial_state(automaton_2, 1);
	automaton_automaton_print(automaton_1, false, true, true, "\t", "\n");
	automaton_automaton_print(automaton_2, false, true, true, "\t", "\n");
	automaton_automaton** automata		= malloc(sizeof(automaton_automaton*) * 2);
	automata[0]							= automaton_1;
	automata[1]							= automaton_2;
	automaton_automaton* composition	= automaton_automata_compose(automata, 2, SYNCHRONOUS);
	free(automata); automata = NULL;
	automaton_automaton_print(composition, false, true, true, "\t", "\n");
	automaton_automaton_destroy(composition); composition = NULL;
	automaton_automaton_destroy(automaton_1); automaton_1 = NULL;
	automaton_automaton_destroy(automaton_2); automaton_2 = NULL;
	free(local_alphabet_1); local_alphabet_1 = NULL;
	free(local_alphabet_2); local_alphabet_2 = NULL;
	automaton_transition_destroy(t1_1_2, true); t1_1_2 = NULL;
	automaton_transition_destroy(t1_2_1, true); t1_2_1 = NULL;
	automaton_transition_destroy(t2_1_2, true); t2_1_2 = NULL;
	automaton_transition_destroy(t2_2_1, true); t2_2_1 = NULL;
	automaton_automata_context_destroy(ctx); ctx	= NULL;
	automaton_fluent_destroy(fluents[0], true);
	automaton_fluent_destroy(fluents[1], true);
	free(fluents);	fluents = NULL;
	automaton_alphabet_destroy(alphabet); alphabet = NULL;
	automaton_signal_event_destroy(in, true); in = NULL;
	automaton_signal_event_destroy(out, true); out = NULL;
	automaton_signal_event_destroy(tau, true); tau = NULL;
}

void run_all_tests(){
	run_tree_tests();
	run_automaton_tests();
	run_fsp_tests(5);
}

int main (void){
	/*
	run_all_tests();
	*/
	//run_parse_test("test5.fsp");
	//run_fsp_tests(5);
	run_parse_test("test6.fsp");
	return 0;    
}


