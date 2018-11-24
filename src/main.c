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

void run_parse_test(char* test_file, char* test_name){
	FILE *fd;
    if (!(yyin = fopen(test_file, "r")))
    {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        printf("Current working dir: %s\n", cwd);
        perror("Error: ");
        return;
    }
	yyparse();
    //printf("\n\n%d\n\n\n", parsed_program->count);
	char buf[255];
	sprintf(buf, "results/%s", test_name);
	bool FIRST_IS_CONCURRENT	= false;
	bool PRINT_FSP				= true;
	automaton_automata_context* ctx		= automaton_automata_context_create_from_syntax(parsed_program, buf, FIRST_IS_CONCURRENT, PRINT_FSP);
    automaton_automata_context_destroy(ctx);
    automaton_program_syntax_destroy(parsed_program);
	obdd_mgr* mgr	= parser_get_obdd_mgr();
	obdd_mgr_destroy(mgr);
    fclose(yyin);
}
void run_obdd_valuations(){
	obdd_mgr* new_mgr	= obdd_mgr_create();
	//compare x1 & !(x2 | x3) == x1 & !x2 & !x3
	obdd* x1_obdd		= obdd_mgr_var(new_mgr, "x1");
	obdd* x1_p_obdd		= obdd_mgr_var(new_mgr, "x1_p");
	obdd* x2_obdd		= obdd_mgr_var(new_mgr, "x2");
	obdd* x2_p_obdd		= obdd_mgr_var(new_mgr, "x2_p");
	obdd* not_x1_obdd	= obdd_apply_not(x1_obdd);
	obdd* not_x2_obdd	= obdd_apply_not(x2_obdd);
	obdd* not_x1_and_not_x2_obdd	= obdd_apply_and(not_x1_obdd, not_x2_obdd);


	obdd_print(not_x1_and_not_x2_obdd);

	obdd_mgr_print(new_mgr);
	uint32_t valuations_count;
	uint32_t valuations_size	= LIST_INITIAL_SIZE * new_mgr->vars_dict->size;
	bool* valuations			= calloc(sizeof(bool), valuations_size);

	uint32_t img_count;
	uint32_t* total_img;
	uint32_t i;

	uint32_t variables_count	= new_mgr->vars_dict->size - 2;
	bool* dont_care_list		= malloc(sizeof(bool) * variables_count);
	bool* partial_valuation		= calloc(variables_count, sizeof(bool));
	bool* initialized_values	= calloc(variables_count, sizeof(bool));
	bool* valuation_set			= malloc(sizeof(bool) * variables_count);
	//keeps a stack of visited nodes
	obdd_node** last_nodes		= malloc(sizeof(obdd_node*) * variables_count);
	//keeps a stack of predecessors as track of the path taken
	int32_t* last_succ_index	= calloc(sizeof(int32_t), variables_count);

	printf("!X1 over x2\n");
	img_count	= 1;
	total_img	= malloc(sizeof(uint32_t) * img_count);
	total_img[0]	= 2 + 2;
	obdd_get_valuations(new_mgr, not_x1_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes, last_succ_index);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count);
	free(total_img);

	printf("!X1 over x1, x2\n");
	img_count	= 2;
	total_img	= malloc(sizeof(uint32_t) * img_count);
	total_img[0]	= 2;
	total_img[1]	= 2 + 2;
	obdd_get_valuations(new_mgr, not_x1_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes, last_succ_index);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count);
	free(total_img);

	printf("!X1 && !X2 over x1,x2\n");
	img_count	= (new_mgr->vars_dict->size - 2) / 2;
	total_img	= malloc(sizeof(uint32_t) * img_count);
	for(i = 0; i < img_count; i++)
		total_img[i]	= (i * 2) + 2;
	obdd_get_valuations(new_mgr, not_x1_and_not_x2_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes, last_succ_index);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count);
	free(total_img);

	printf("!X1 && !X2 over x1\n");
	img_count	= 1;
	total_img	= malloc(sizeof(uint32_t) * img_count);
	total_img[0]= 2;
	obdd_get_valuations(new_mgr, not_x1_and_not_x2_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes, last_succ_index);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count);
	free(valuations);
	free(total_img);
	free(initialized_values);
	free(last_succ_index);
	free(valuation_set);
	free(last_nodes);
	free(dont_care_list);
	free(partial_valuation);

	obdd_destroy(x1_obdd);
	obdd_destroy(x2_obdd);
	obdd_destroy(x1_p_obdd);
	obdd_destroy(x2_p_obdd);
	obdd_destroy(not_x1_obdd);
	obdd_destroy(not_x2_obdd);
	obdd_destroy(not_x1_and_not_x2_obdd);
	obdd_mgr_destroy(new_mgr);

}
void run_small_obdd_tests(){
	obdd_mgr* new_mgr	= obdd_mgr_create();
	//compare x1 & !(x2 | x3) == x1 & !x2 & !x3
	obdd* x1_obdd		= obdd_mgr_var(new_mgr, "x1");
	obdd* not_x1_obdd	= obdd_apply_not(x1_obdd);
	obdd* x2_obdd		= obdd_mgr_var(new_mgr, "x2");
	obdd* x1_or_x2_obdd	= obdd_apply_or(x1_obdd, x2_obdd);
	obdd* x1_then_x2_obdd	= obdd_apply_or(not_x1_obdd, x2_obdd);
	printf("X1 -> X2\n");
	obdd_print(x1_then_x2_obdd);
	uint32_t valuations_count;
	uint32_t img_count	= new_mgr->vars_dict->size - 2;
	uint32_t* total_img	= malloc(sizeof(uint32_t) * img_count);
	uint32_t i;
	uint32_t valuations_size	= LIST_INITIAL_SIZE * new_mgr->vars_dict->size;
	bool* valuations			= calloc(sizeof(bool), valuations_size);
	uint32_t variables_count	= new_mgr->vars_dict->size - 2;
	bool* dont_care_list		= malloc(sizeof(bool) * variables_count);
	bool* partial_valuation		= calloc(variables_count, sizeof(bool));
	bool* initialized_values	= calloc(variables_count, sizeof(bool));
	bool* valuation_set			= malloc(sizeof(bool) * variables_count);
	//keeps a stack of visited nodes
	obdd_node** last_nodes		= malloc(sizeof(obdd_node*) * variables_count);
	//keeps a stack of predecessors as track of the path taken
	int32_t* last_succ_index	= calloc(sizeof(int32_t), variables_count);

	for(i = 0; i < img_count; i++)
		total_img[i]	= i + 2;
	obdd_get_valuations(new_mgr, x1_then_x2_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes, last_succ_index);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count);
	free(valuations);
	free(total_img);
	free(initialized_values);
	free(last_succ_index);
	free(valuation_set);
	free(last_nodes);
	free(dont_care_list);
	free(partial_valuation);
	obdd_destroy(x1_obdd);
	obdd_destroy(not_x1_obdd);
	obdd_destroy(x2_obdd);
	obdd_destroy(x1_or_x2_obdd);
	obdd_destroy(x1_then_x2_obdd);
	obdd_mgr_destroy(new_mgr);
}

void run_obdd_tests(){
	obdd_mgr* new_mgr	= obdd_mgr_create();
	//compare x1 & !(x2 | x3) == x1 & !x2 & !x3
	obdd* x1_obdd		= obdd_mgr_var(new_mgr, "x1");
	obdd* x2_obdd		= obdd_mgr_var(new_mgr, "x2");
	obdd* x3_obdd		= obdd_mgr_var(new_mgr, "x3");
	obdd* x1_or_x2_obdd	= obdd_apply_or(x1_obdd, x2_obdd);
	obdd* x1_or_x2_or_x3_obdd	= obdd_apply_or(x1_or_x2_obdd, x3_obdd);
	obdd* x1_and_x2_obdd	= obdd_apply_and(x1_obdd, x2_obdd);
	obdd* x1_and_x2_and_x3_obdd	= obdd_apply_and(x1_and_x2_obdd, x3_obdd);

	obdd* not_x1_obdd	= obdd_apply_not(x1_obdd);
	obdd* x1_and_not_x1_obdd= obdd_apply_and(x1_obdd,not_x1_obdd);

	obdd* not_x1_and_x2_obdd= obdd_apply_not(x1_and_x2_obdd);

	obdd* x1_and_x2_then_x1	= obdd_apply_or(not_x1_and_x2_obdd, x1_obdd);

	obdd* x2_eq_fallacy_obdd= obdd_apply_equals(x2_obdd, x1_and_not_x1_obdd);
	obdd* exists_obdd	= obdd_exists(x2_eq_fallacy_obdd, "x2");

	obdd_print(x1_or_x2_obdd);
	printf("x1 || x2 sat? : %s \n", obdd_is_sat(new_mgr, x1_or_x2_obdd->root_obdd) ? "yes" : "no");
	printf("x1 || x2 taut? : %s \n", obdd_is_tautology(new_mgr, x1_or_x2_obdd->root_obdd) ? "yes" : "no");

	obdd_print(x1_and_x2_obdd);
	printf("x1 && x2 sat? : %s \n", obdd_is_sat(new_mgr, x1_and_x2_obdd->root_obdd) ? "yes" : "no");
	printf("x1 && x2 taut? : %s \n", obdd_is_tautology(new_mgr, x1_and_x2_obdd->root_obdd) ? "yes" : "no");

	obdd_print(x1_and_not_x1_obdd);
	printf("x1 && !x1 sat? : %s \n", obdd_is_sat(new_mgr, x1_and_not_x1_obdd->root_obdd) ? "yes" : "no");
	printf("x1 && !x1 taut? : %s \n", obdd_is_tautology(new_mgr, x1_and_not_x1_obdd->root_obdd) ? "yes" : "no");

	obdd_print(x1_and_x2_then_x1);
	printf("(x1 && x2)->x1 sat? : %s \n", obdd_is_sat(new_mgr, x1_and_x2_then_x1->root_obdd) ? "yes" : "no");
	printf("(x1 && x2)->x1 taut? : %s \n", obdd_is_tautology(new_mgr, x1_and_x2_then_x1->root_obdd) ? "yes" : "no");

	obdd_print(exists_obdd);
	printf("E x2.(x2 = (x1 && !x1)) sat? : %s \n", obdd_is_sat(new_mgr, exists_obdd->root_obdd) ? "yes" : "no");
	printf("E x2.(x2 = (x1 && !x1)) taut? : %s \n", obdd_is_tautology(new_mgr, exists_obdd->root_obdd) ? "yes" : "no");

	obdd_mgr_print(new_mgr);
	uint32_t valuations_count;

	uint32_t valuations_size	= LIST_INITIAL_SIZE * new_mgr->vars_dict->size;
	bool* valuations			= calloc(sizeof(bool), valuations_size);
	uint32_t variables_count	= new_mgr->vars_dict->size - 2;
	bool* dont_care_list		= malloc(sizeof(bool) * variables_count);
	bool* partial_valuation		= calloc(variables_count, sizeof(bool));
	bool* initialized_values	= calloc(variables_count, sizeof(bool));
	bool* valuation_set			= malloc(sizeof(bool) * variables_count);
	//keeps a stack of visited nodes
	obdd_node** last_nodes		= malloc(sizeof(obdd_node*) * variables_count);
	//keeps a stack of predecessors as track of the path taken
	int32_t* last_succ_index	= calloc(sizeof(int32_t), variables_count);
	uint32_t img_count	= new_mgr->vars_dict->size - 3;
	uint32_t* total_img	= malloc(sizeof(uint32_t) * img_count);
	uint32_t i;

	for(i = 0; i < img_count; i++)
		total_img[i]	= i + 2;

	printf("X1 && X2\n");
	obdd_print(x1_and_x2_obdd);
	obdd_get_valuations(new_mgr, x1_and_x2_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes, last_succ_index);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count);

	printf("X1 & X2 && X3\n");
	obdd_print(x1_and_x2_and_x3_obdd);
	obdd_get_valuations(new_mgr, x1_and_x2_and_x3_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes, last_succ_index);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count);

	printf("X1 || X2\n");
	obdd_print(x1_or_x2_obdd);
	obdd_get_valuations(new_mgr, x1_or_x2_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes, last_succ_index);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count);
	printf("X1 || X2 || X3\n");
	obdd_print(x1_or_x2_or_x3_obdd);
	obdd_get_valuations(new_mgr, x1_or_x2_or_x3_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes, last_succ_index);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count);
	free(valuations);
	free(total_img);
	free(initialized_values);
	free(last_succ_index);
	free(valuation_set);
	free(last_nodes);
	free(dont_care_list);
	free(partial_valuation);
	obdd_destroy(x1_obdd);
	obdd_destroy(x2_obdd);
	obdd_destroy(x3_obdd);
	obdd_destroy(x1_or_x2_obdd);
	obdd_destroy(x1_or_x2_or_x3_obdd);
	obdd_destroy(x1_and_x2_obdd);
	obdd_destroy(x1_and_x2_and_x3_obdd);
	obdd_destroy(not_x1_obdd);
	obdd_destroy(x1_and_not_x1_obdd);

	obdd_destroy(not_x1_and_x2_obdd);

	obdd_destroy(x1_and_x2_then_x1);
	obdd_destroy(x2_eq_fallacy_obdd);
	obdd_destroy(exists_obdd);

	obdd_mgr_destroy(new_mgr);

}

typedef struct test_item_bucket_str{
	uint32_t a; uint32_t b;
}test_item_bucket;

uint32_t test_item_compare(void* left, void* right){
	test_item_bucket* left_entry	= (test_item_bucket*)left;
	test_item_bucket* right_entry	= (test_item_bucket*)right;
	if(left_entry->a > right_entry->a){
		return 1;
	}else if(left_entry->a == right_entry->a){
		return 0;
	}else{
		return -1;
	}
}
uint32_t test_item_extractor(void* entry){ return ((test_item_bucket*)entry)->a;}
void run_max_heap_tests(){
	automaton_max_heap* heap	= automaton_max_heap_create(sizeof(test_item_bucket), test_item_compare);
	test_item_bucket current_item;
	uint32_t i;
	uint32_t cycles = 100;
	for(i = 0; i < cycles; i++){
		current_item.b = i; current_item.a = i + cycles;
		automaton_max_heap_add_entry(heap, &current_item);
		printf("PUSHING ITEM <%d,%d>\n", current_item.a, current_item.b);
	}
	for(i = 0; i < cycles; i++){
		current_item.b = i; current_item.a = i;
		automaton_max_heap_add_entry(heap, &current_item);
		printf("PUSHING ITEM <%d,%d>\n", current_item.a, current_item.b);
	}
	for(i = 0; i < cycles * 2; i++){
		automaton_max_heap_pop_entry(heap, &current_item);
		printf("POPPING ITEM: <%d,%d>\n", i, current_item.a, current_item.b);
	}
	automaton_max_heap_destroy(heap);
}

void run_concrete_bucket_list_tests(){
	automaton_concrete_bucket_list* list	= automaton_concrete_bucket_list_create(RANKING_BUCKET_SIZE, test_item_extractor, sizeof(test_item_bucket));
	test_item_bucket current_item;
	uint32_t i;
	uint32_t cycles = 60;
	for(i = 0; i < cycles; i++){
		current_item.b = i; current_item.a = i % 2;
		automaton_concrete_bucket_add_entry(list, &current_item);
	}
	for(i = 0; i < cycles; i++){
		automaton_concrete_bucket_pop_entry(list, &current_item);
		if(i % 500){
			printf("ITEM at %d: <%d,%d>\n", i, current_item.a, current_item.b);
		}
	}
	automaton_concrete_bucket_destroy(list);
}

void run_ordered_list_tests(){
	test_item_bucket current_item;
	uint32_t i;
	automaton_ordered_list* ordered_list	= automaton_ordered_list_create(2, test_item_extractor, sizeof(test_item_bucket));
	uint32_t cycles = 6;//100000;
	for(i = 0; i < cycles; i++){
		current_item.b = i; current_item.a = i + cycles;
		printf("PUSHING ITEM at %d: <%d,%d>\n", i, current_item.a, current_item.b);
		automaton_ordered_list_add_entry(ordered_list, &current_item);
	}
	for(i = 0; i < cycles; i++){
		current_item.b = i; current_item.a = i;
		printf("PUSHING ITEM at %d: <%d,%d>\n", i, current_item.a, current_item.b);
		automaton_ordered_list_add_entry(ordered_list, &current_item);
	}
	test_item_bucket* print_bucket;
	for(i = 0; i < ordered_list->temporary_count; i++){
		print_bucket = GET_ORDERED_LIST_SINGLE_ENTRY(ordered_list, ordered_list->temporary, i);
		printf("temporary item at %d: <%d,%d>\n", i, print_bucket->a, print_bucket->b);
	}
	for(i = 0; i < ordered_list->count; i++){
			print_bucket = GET_ORDERED_LIST_SINGLE_ENTRY(ordered_list, ordered_list->values, i);
			printf("values item at %d: <%d,%d>\n", i, print_bucket->a, print_bucket->b);
		}
	for(i = 0; i < cycles * 2; i++){
		automaton_ordered_list_pop_entry(ordered_list, &current_item);
		printf("POPPING ITEM at %d: <%d,%d>\n", i, current_item.a, current_item.b);
	}
	automaton_ordered_list_destroy(ordered_list);
}

void run_fsp_tests(uint32_t test_count){
	uint32_t i;
	char buf[255], buf2[255];
	for(i = 1; i <= test_count; i++){
		printf("\n\n==============\nRunning fsp test %d\n==============\n", i);
		sprintf(buf, "tests/test%d.fsp", i);
		sprintf(buf2, "test%d", i);
		run_parse_test(buf, buf2);
	}
}

void run_small_obdd_tree_tests(){
	obdd_state_tree* tree	= obdd_state_tree_create(5);
	bool key1[5]				= {true, true, true, true, true};
	bool key2[5]				= {true, false, true, true, true};
	bool key3[5]				= {true, true, false, true, true};
	bool key4[5]				= {false, false, false, true, false};
	bool key5[5]				= {false, false, false, true, true};
	printf("new key %d\n", obdd_state_tree_get_key(tree, key1, -1));
	printf("new key %d\n", obdd_state_tree_get_key(tree, key2, -1));
	printf("new key %d\n", obdd_state_tree_get_key(tree, key3, -1));
	printf("new key %d\n", obdd_state_tree_get_key(tree, key4, -1));
	printf("new key %d\n", obdd_state_tree_get_key(tree, key5, -1));
	obdd_state_tree_print(tree);
	obdd_state_tree_destroy(tree);
}
void run_obdd_tree_tests(){
	obdd_state_tree* tree	= obdd_state_tree_create(7);
	bool key1[7]				= {false, false, false, false, false, false, false};
	uint32_t i, j;
	for(i = 0; i < 128; i++){
		for(j = 0; j < 7; j++){
			key1[j] = ((i >> j) & 0x1) == 0;
		}
		printf("%d:%d %s", i, obdd_state_tree_get_key(tree, key1, -1), i % 5 == 0 ? "\n" : "  ");
	}
	printf("\nREPEATING VALUES\n");
	for(i = 0; i < 128; i++){
		for(j = 0; j < 7; j++){
			key1[j] = ((i >> j) & 0x1) == 0;
		}
		printf("%d:%d %s", i, obdd_state_tree_get_key(tree, key1, -1), i % 5 == 0 ? "\n" : "  ");
	}
	//obdd_state_tree_print(tree);
	obdd_state_tree_destroy(tree);
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
		while(key0_2[j] == values){
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
	automaton_automata_context* ctx	= automaton_automata_context_create("Context 1", alphabet, fluents_count, fluents, 0, NULL, NULL);
	automaton_automata_context_print(ctx, "\t", "\n");
	uint32_t* local_alphabet_1		= malloc(sizeof(uint32_t) * 2);
	local_alphabet_1[0]				= automaton_alphabet_get_signal_index(alphabet, in);
	local_alphabet_1[1]				= automaton_alphabet_get_signal_index(alphabet, tau);
	uint32_t* local_alphabet_2		= malloc(sizeof(uint32_t) * 2);
	local_alphabet_2[0]				= automaton_alphabet_get_signal_index(alphabet, tau);
	local_alphabet_2[1]				= automaton_alphabet_get_signal_index(alphabet, out);
	automaton_automaton* automaton_1= automaton_automaton_create("Automaton 1", ctx, 2, local_alphabet_1, false, false);
	automaton_transition* t1_1_2	= automaton_transition_create(1, 2);
	automaton_transition* t1_2_1	= automaton_transition_create(2, 1);
	automaton_transition_add_signal_event(t1_1_2, ctx, in);
	automaton_transition_add_signal_event(t1_2_1, ctx, tau);
	automaton_automaton_add_transition(automaton_1, t1_1_2);
	automaton_automaton_add_transition(automaton_1, t1_2_1);
	automaton_automaton_add_initial_state(automaton_1, 1);
	automaton_automaton* automaton_2= automaton_automaton_create("Automaton 2", ctx, 2, local_alphabet_2, false, false);
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
	automaton_automaton* composition	= automaton_automata_compose(automata, 2, CONCURRENT, false);
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
void run_report_tests(){
	automaton_signal_event *x1 = automaton_signal_event_create("x1.on", INPUT_SIG);
	automaton_signal_event *x2 = automaton_signal_event_create("x1.off", INPUT_SIG);
	automaton_signal_event *y1 = automaton_signal_event_create("y1.on", OUTPUT_SIG);
	automaton_signal_event *y2 = automaton_signal_event_create("y1.off", OUTPUT_SIG);
	automaton_alphabet *alphabet = automaton_alphabet_create();
	automaton_alphabet_add_signal_event(alphabet, x1);
	automaton_alphabet_add_signal_event(alphabet, x2);
	automaton_alphabet_add_signal_event(alphabet, y1);
	automaton_alphabet_add_signal_event(alphabet, y2);
	automaton_fluent *f1 = automaton_fluent_create("test fluent", true);
	automaton_fluent_add_starting_signal(f1, alphabet, x1);
	automaton_fluent_add_ending_signal(f1, alphabet, y1);
	automaton_fluent_add_ending_signal(f1, alphabet, y2);

	automaton_fluent **fluents	= malloc(sizeof(automaton_fluent*) * 1);
	fluents[0]					= f1;
	obdd **liveness_valuations	= malloc(sizeof(obdd*) * 2);
	obdd_mgr* mgr				= obdd_mgr_create();
	liveness_valuations[0]	= obdd_mgr_var(mgr, "x1");
	liveness_valuations[1]	= obdd_mgr_var(mgr, "y1");
	char** liveness_valuations_names	= malloc(sizeof(char*) * 2);
	liveness_valuations_names[0]		= "ass_1";
	liveness_valuations_names[1]		= "goal_1";
	automaton_automata_context *ctx		= automaton_automata_context_create("CTX", alphabet, 1, fluents, 2, liveness_valuations, liveness_valuations_names);
	automaton_transition *t1	= automaton_transition_create(0, 1);
	automaton_transition_add_signal_event(t1, ctx, x1);
	automaton_transition_add_signal_event(t1, ctx, y1);

	uint32_t *local_alphabet				= malloc(sizeof(uint32_t) * 4);
	uint32_t i;
	for(i = 0; i < 4; i++)local_alphabet[i]	= i;

	automaton_automaton *automaton		= automaton_automaton_create("automaton", ctx, 4, local_alphabet, true, true);
	automaton_automaton_add_initial_state(automaton, 0);
	automaton_automaton_add_transition(automaton, t1);
	automaton_automaton_update_valuation(automaton);


	obdd_destroy(liveness_valuations[0]);
	obdd_destroy(liveness_valuations[1]);

	free(local_alphabet);
	free(fluents);
	free(liveness_valuations);
	free(liveness_valuations_names);

	printf("FLUENT:");
	automaton_fluent_serialize_report(stdout, f1);
	printf("\n");
	printf("ALPHABET:");
	automaton_alphabet_serialize_report(stdout, alphabet);
	printf("\n");
	printf("CTX:");
	automaton_automata_context_serialize_report(stdout, ctx);
	printf("\n");
	printf("Transition:");
	automaton_transition_serialize_report(stdout, t1);
	printf("\n");
	printf("Automaton:");
	automaton_automaton_serialize_report(stdout, automaton);
	printf("\n");

	automaton_transition_destroy(t1, true);
	automaton_fluent_destroy(f1, true);
	automaton_alphabet_destroy(alphabet);
	automaton_signal_event_destroy(x1, true);
	automaton_signal_event_destroy(x2, true);
	automaton_signal_event_destroy(y1, true);
	automaton_signal_event_destroy(y2, true);
}

typedef struct test_item_pool_str{
	uint32_t a; char name[250];
}test_item_pool;

#define MAX_POOL_COUNT	1000000

void run_fast_pool_tests(){
	size_t item_size	= sizeof(test_item_pool);
	test_item_pool** pool_ref1	= malloc(MAX_POOL_COUNT * sizeof(test_item_pool*));
	test_item_pool** pool_ref2	= malloc(MAX_POOL_COUNT * sizeof(test_item_pool*));
	test_item_pool** pool_frag1	= malloc(MAX_POOL_COUNT * sizeof(uint32_t));
	test_item_pool** pool_frag2	= malloc(MAX_POOL_COUNT * sizeof(uint32_t));



	automaton_fast_pool* pool1	= automaton_fast_pool_create(item_size, 100, 100);
	automaton_fast_pool* pool2	= automaton_fast_pool_create(item_size, 5, 2);

	uint32_t i;
	for(i = 0; i < MAX_POOL_COUNT/2; i++){
		test_item_pool* item	= automaton_fast_pool_get_instance(pool1, &(pool_frag1[i]));
		item->a		= i;
		item->name[0]	= 'h';item->name[1]	= 'i';item->name[2]	= '!';item->name[3]	= '\0';
		pool_ref1[i]	= item;
		item	= automaton_fast_pool_get_instance(pool2, &(pool_frag2[i]));
		item->a		= i;
		item->name[0]	= 'h';item->name[1]	= 'i';item->name[2]	= '!';item->name[3]	= '\0';
		pool_ref2[i]	= item;
	}
	printf("10 100 pool \n");
	for(i = 0; i < MAX_POOL_COUNT/2; i++){
		if(i % 999 == 0)
			printf("%d%s ", pool_ref1[i]->a, pool_ref1[i]->name);
	}
	printf("\n");
	printf("5 2 pool \n");
	for(i = 0; i < MAX_POOL_COUNT/2; i++){
		if(i % 999 == 0)
			printf("%d%s ", pool_ref2[i]->a, pool_ref2[i]->name);
	}
	printf("\n");
	for(i = 0; i < MAX_POOL_COUNT/2; i++){
		automaton_fast_pool_release_instance(pool1, pool_frag1[i]);
		automaton_fast_pool_release_instance(pool2, pool_frag2[i]);
	}
	for(i = 0; i < MAX_POOL_COUNT; i++){
		test_item_pool* item	= automaton_fast_pool_get_instance(pool1, &(pool_frag1[i]));
		item->a		= i;
		item->name[0]	= 'h';item->name[1]	= 'i';item->name[2]	= '!';item->name[3]	= '\0';
		pool_ref1[i]	= item;
		item	= automaton_fast_pool_get_instance(pool2, &(pool_frag2[i]));
		item->a		= i;
		item->name[0]	= 'h';item->name[1]	= 'i';item->name[2]	= '!';item->name[3]	= '\0';
		pool_ref2[i]	= item;
		if(i % 999 == 0)
			printf("%d%s ", item->a, item->name);
	}
	printf("\n");
	automaton_fast_pool_destroy(pool1);
	automaton_fast_pool_destroy(pool2);
	free(pool_frag1);free(pool_frag2);
	free(pool_ref1);free(pool_ref2);
}
void run_all_tests(){
	run_tree_tests();
	run_automaton_tests();
	run_fast_pool_tests();
	run_fsp_tests(5);
}

int main (void){
	//chdir("/Users/ezequiel/henos/henos-automata/src");
	/*
	run_all_tests();
	*/
	//run_obdd_tree_tests();
	//run_small_obdd_tests();
	//run_obdd_tests();
	//run_obdd_valuations();
	//run_concrete_bucket_list_tests();
	//run_ordered_list_tests();
	//run_max_heap_tests();
	//run_report_tests();
	//run_fast_pool_tests();

	//run_fsp_tests(18);
	//run_parse_test("tests/test5.fsp",  "test5");
	//run_parse_test("tests/test18.fsp",  "test18");
	//run_parse_test("tests/test21.fsp", "test21");
	//run_parse_test("tests/test23.fsp", "test23");
	//run_parse_test("tests/test26.fsp", "test26");
	//run_parse_test("tests/test27.fsp", "test27");
	//run_parse_test("tests/test28.fsp", "mixed model 3 signals 2 labels");//mixed model 3 signals 2 labels
	//run_parse_test("tests/test29.fsp", "lift 3 floors");//lift 3 floors
	//run_parse_test("tests/test30.fsp", "lift 2 floors");//lift 2 floors
	run_parse_test("tests/test31.fsp", "GenBuf 4 sndrs");//GENBUF 4 sndrs
	//run_parse_test("tests/test32.fsp", "test32");
	//run_parse_test("tests/test34.fsp", "test34");
	//run_parse_test("tests/test35.fsp", "GenBuf 2 sndrs");//GENBUF 2 Sndrs
	//run_parse_test("tests/test36.fsp", "lift 5 floors");//lift 5 floors
	return 0;    
}


