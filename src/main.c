#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include "automaton_context.h"
//#include "minuint.h"

#define PRINT_TEST_OUTPUT 0

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern int yyparse();
extern YY_BUFFER_STATE yy_scan_string(char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern FILE* yyin;
extern automaton_program_syntax* parsed_program;

char* automataFile  =  "automata.txt";

char* test_get_output_content(char *filename){
	char *buffer = NULL;
	size_t size = 0;
	FILE *fp = fopen(filename, "r");
	fseek(fp, 0, SEEK_END); /* Go to end of file */
	size = ftell(fp); /* How many bytes did we pass ? */
	rewind(fp);
	buffer = malloc((size + 1) * sizeof(*buffer)); /* size + 1 byte for the \0 */
	fread(buffer, size, 1, fp); /* Read 1 chunk of size bytes from fp into buffer */
	/* NULL-terminate the buffer */
	buffer[size] = '\0';
	/* Print it ! */
	return buffer;
}

void test_set_expected_content(char* content, char* filename){
	FILE *fp = fopen(filename, "w");

	int results = fputs(content, fp);
	if (results == EOF) {
	    // Failed to write do error code here.
	}
	fclose(fp);
}

void print_test_result(bool passed, char* name, char* description){
	if(!passed){
		printf(ANSI_COLOR_RED);
		printf("X");
		printf(ANSI_COLOR_RESET);
	}else{
		printf(ANSI_COLOR_GREEN);
		printf("■");
		printf(ANSI_COLOR_RESET);
	}
	printf("\t [%s] \t (%s)\n", name, description);
	fflush(stdout);
}

void run_parse_test_local(char* test_file, char* test_name, char* result_name){
	FILE *fd;
    if (!(yyin = fopen(test_file, "r")))
    {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        printf("Current working dir: %s\n", cwd);
        perror("Error: ");
        return;
    }
	parser_reset_obdd_mgr();
	yyparse();
    //printf("\n\n%d\n\n\n", parsed_program->count);

	bool PRINT_FSP				= true;

	automaton_automata_context* ctx		= automaton_automata_context_create_from_syntax(parsed_program, result_name, PRINT_FSP);
    automaton_automata_context_destroy(ctx);
    automaton_program_syntax_destroy(parsed_program);
    fclose(yyin);
}

void run_parse_test(char* test_file, char* test_name){
	char buf[255];
	snprintf(buf, sizeof(buf),"results/%s", test_name);
	run_parse_test_local(test_file, test_name, buf);
}

void run_obdd_exists(){
	obdd_mgr* new_mgr	= obdd_mgr_create();
	//compare x1 & !(x2 | x3) == x1 & !x2 & !x3
	obdd* x1_obdd		= obdd_mgr_var(new_mgr, "x1");
	obdd* x1_p_obdd		= obdd_mgr_var(new_mgr, "x1_p");
	obdd* x2_obdd		= obdd_mgr_var(new_mgr, "x2");
	obdd* x2_p_obdd		= obdd_mgr_var(new_mgr, "x2_p");
	obdd* x3_obdd		= obdd_mgr_var(new_mgr, "x3");
	obdd* not_x1_obdd	= obdd_apply_not(x1_obdd);
	obdd* not_x2_obdd	= obdd_apply_not(x2_obdd);
	obdd* not_x1_and_not_x2_obdd	= obdd_apply_and(not_x1_obdd, not_x2_obdd);
	obdd* true_obdd		= obdd_apply_or(x1_obdd, not_x1_obdd);
	obdd* x1_and_x2_obdd	= obdd_apply_and(x1_obdd, x2_obdd);
	obdd* x1_and_x2_and_x3_obdd	= obdd_apply_and(x1_and_x2_obdd, x3_obdd);
	uint32_t buff_size = 16384;
	char *buff = calloc(buff_size, sizeof(char));
	obdd_mgr_print(new_mgr, buff);
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

#if PRINT_TEST_OUTPUT
	printf("exists X1 (X1 && X2)");
#endif
	img_count	= 3;
	total_img	= malloc(sizeof(uint32_t) * img_count);
	total_img[0]= 2;
	total_img[1]= 4;
	total_img[2]= 6;
	obdd *exists_obdd	= obdd_exists(x1_and_x2_obdd, "x1");
	obdd_print(exists_obdd, buff, buff_size);
	obdd_get_valuations(new_mgr, exists_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count, buff);
	obdd_destroy(exists_obdd);
	free(total_img);
#if PRINT_TEST_OUTPUT
	printf("exists X2 (X1 && X2 && X3)");
#endif
	img_count	= 3;
	total_img	= malloc(sizeof(uint32_t) * img_count);
	total_img[0]= 2;
	total_img[1]= 4;
	total_img[2]= 6;
	exists_obdd	= obdd_exists(x1_and_x2_and_x3_obdd, "x2");
	obdd_print(exists_obdd, buff, buff_size);
	obdd_get_valuations(new_mgr, exists_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count, buff);
	obdd_destroy(exists_obdd);
	free(total_img);
#if PRINT_TEST_OUTPUT
	printf("exists X1, X2 (X1 && X2 && X3)");
#endif
	img_count	= 3;
	total_img	= malloc(sizeof(uint32_t) * img_count);
	total_img[0]= 2;
	total_img[1]= 4;
	total_img[2]= 6;
	exists_obdd	= obdd_exists_vector(x1_and_x2_and_x3_obdd, total_img, 2);
	obdd_print(exists_obdd, buff, buff_size);
	obdd_get_valuations(new_mgr, exists_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count, buff);
	obdd_destroy(exists_obdd);

	char * expected = test_get_output_content("tests/expected_output/run_obdd_exists_tests.exp");
	bool txt_cmp = strcmp(buff, expected) == 0;
	print_test_result(txt_cmp, "OBDD EXISTS", "obdd exists test");
	free(expected);


	free(valuations);
	free(total_img);
	free(initialized_values);
	free(valuation_set);
	free(last_nodes);
	free(dont_care_list);
	free(partial_valuation);
	free(buff);

	obdd_destroy(x1_obdd);
	obdd_destroy(x2_obdd);
	obdd_destroy(x3_obdd);
	obdd_destroy(x1_p_obdd);
	obdd_destroy(x2_p_obdd);
	obdd_destroy(not_x1_obdd);
	obdd_destroy(not_x2_obdd);
	obdd_destroy(not_x1_and_not_x2_obdd);
	obdd_destroy(x1_and_x2_obdd);
	obdd_destroy(x1_and_x2_and_x3_obdd);
	obdd_destroy(true_obdd);
	obdd_mgr_destroy(new_mgr);
}

void run_obdd_valuations(){
	obdd_mgr* new_mgr	= obdd_mgr_create();
	//compare x1 & !(x2 | x3) == x1 & !x2 & !x3
	obdd* x1_obdd		= obdd_mgr_var(new_mgr, "x1");
	obdd* x1_p_obdd		= obdd_mgr_var(new_mgr, "x1_p");
	obdd* x2_obdd		= obdd_mgr_var(new_mgr, "x2");
	obdd* x2_p_obdd		= obdd_mgr_var(new_mgr, "x2_p");
	obdd* x3_obdd		= obdd_mgr_var(new_mgr, "x3");
	obdd* not_x1_obdd	= obdd_apply_not(x1_obdd);
	obdd* not_x2_obdd	= obdd_apply_not(x2_obdd);
	obdd* not_x1_and_not_x2_obdd	= obdd_apply_and(not_x1_obdd, not_x2_obdd);
	obdd* true_obdd		= obdd_apply_or(x1_obdd, not_x1_obdd);

	uint32_t buff_size = 16384;
	char *buff = calloc(buff_size, sizeof(char));
	obdd_print(not_x1_and_not_x2_obdd, buff, buff_size);

	obdd_mgr_print(new_mgr, buff);
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
#if PRINT_TEST_OUTPUT
	printf("!X1 over x2\n");
#endif
	img_count	= 1;
	total_img	= malloc(sizeof(uint32_t) * img_count);
	total_img[0]	= 2 + 2;
	obdd_get_valuations(new_mgr, not_x1_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count, buff);
	free(total_img);
#if PRINT_TEST_OUTPUT
	printf("!X1 over x1, x2\n");
#endif
	img_count	= 2;
	total_img	= malloc(sizeof(uint32_t) * img_count);
	total_img[0]	= 2;
	total_img[1]	= 2 + 2;
	obdd_get_valuations(new_mgr, not_x1_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count, buff);
	free(total_img);
#if PRINT_TEST_OUTPUT
	printf("!X1 && !X2 over x1,x2\n");
#endif
	img_count	= (new_mgr->vars_dict->size - 2) / 2;
	total_img	= malloc(sizeof(uint32_t) * img_count);
	for(i = 0; i < img_count; i++)
		total_img[i]	= (i * 2) + 2;
	obdd_get_valuations(new_mgr, not_x1_and_not_x2_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count, buff);
	free(total_img);
#if PRINT_TEST_OUTPUT
	printf("!X1 && !X2 over x1\n");
#endif
	img_count	= 1;
	total_img	= malloc(sizeof(uint32_t) * img_count);
	total_img[0]= 2;
	obdd_get_valuations(new_mgr, not_x1_and_not_x2_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count, buff);
	obdd_print(not_x2_obdd, buff, buff_size);
	free(total_img);
#if PRINT_TEST_OUTPUT
	printf("!X2 over x1, x2, x3\n");
#endif
	img_count	= 3;
	total_img	= malloc(sizeof(uint32_t) * img_count);
	total_img[0]	= 2;
	total_img[1]	= 4;
	total_img[2]	= 6;
	obdd_get_valuations(new_mgr, not_x2_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count, buff);
	free(total_img);
#if PRINT_TEST_OUTPUT
	printf("!X1 && !X2 over x1, x2, x3\n");
#endif
	img_count	= 3;
	total_img	= malloc(sizeof(uint32_t) * img_count);
	total_img[0]	= 2;
	total_img[1]	= 4;
	total_img[2]	= 6;
	obdd_get_valuations(new_mgr, not_x1_and_not_x2_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count, buff);
	free(total_img);
#if PRINT_TEST_OUTPUT
	printf("true over x1, x2, x3\n");
#endif
	img_count	= 3;
	total_img	= malloc(sizeof(uint32_t) * img_count);
	total_img[0]	= 2;
	total_img[1]	= 4;
	total_img[2]	= 6;
	obdd_get_valuations(new_mgr, true_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count, buff);
	free(total_img);

	char * expected = test_get_output_content("tests/expected_output/run_obdd_valuations_tests.exp");
	bool txt_cmp = strcmp(buff, expected) == 0;
	print_test_result(txt_cmp, "OBDD VALUATIONS", "obdd valuations test");
	free(expected);

	free(valuations);
	free(initialized_values);
	free(valuation_set);
	free(last_nodes);
	free(dont_care_list);
	free(partial_valuation);
	free(buff);

	obdd_destroy(x1_obdd);
	obdd_destroy(x2_obdd);
	obdd_destroy(x3_obdd);
	obdd_destroy(x1_p_obdd);
	obdd_destroy(x2_p_obdd);
	obdd_destroy(not_x1_obdd);
	obdd_destroy(not_x2_obdd);
	obdd_destroy(not_x1_and_not_x2_obdd);
	obdd_destroy(true_obdd);
	obdd_mgr_destroy(new_mgr);

}
void run_small_obdd_tests(){
	obdd_mgr* new_mgr	= obdd_mgr_create();
	//compare x1 & !(x2 | x3) == x1 & !x2 & !x3
	obdd* x1_obdd		= obdd_mgr_var(new_mgr, "x1");
	obdd* not_x1_obdd	= obdd_apply_not(x1_obdd);
	obdd* x2_obdd		= obdd_mgr_var(new_mgr, "x2");
	obdd* x1_or_x2_obdd	= obdd_apply_or(x1_obdd, x2_obdd);
	obdd* x1_and_x2_obdd	= obdd_apply_and(x1_obdd, x2_obdd);
	obdd* x1_and_x2_obdd_bis= obdd_apply_and(x1_obdd, x2_obdd);
	obdd* x1_or_x1_and_x2_obdd	= obdd_apply_or(x1_obdd, x1_and_x2_obdd);
	obdd* x1_or_x1_and_x2_obdd_bis= obdd_apply_or(x1_obdd, x1_and_x2_obdd);

	obdd* x1_then_x2_obdd	= obdd_apply_or(not_x1_obdd, x2_obdd);
#if PRINT_TEST_OUTPUT
	printf("X1 -> X2\n");
#endif
	uint32_t buff_size = 16384;
	char *buff = calloc(buff_size, sizeof(char));
	obdd_print(x1_then_x2_obdd, buff, buff_size);


	char * expected = test_get_output_content("tests/expected_output/run_small_obdd_tests.exp");

	bool txt_cmp = strcmp(buff, expected) == 0;
	print_test_result(txt_cmp, "SMALL OBDD", "small obdd test");

	free(expected);

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

	for(i = 0; i < img_count; i++)
		total_img[i]	= i + 2;
	obdd_get_valuations(new_mgr, x1_then_x2_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count, buff);
	free(valuations);
	free(total_img);
	free(initialized_values);
	free(valuation_set);
	free(last_nodes);
	free(dont_care_list);
	free(partial_valuation);
	free(buff);
	obdd_destroy(x1_obdd);
	obdd_destroy(not_x1_obdd);
	obdd_destroy(x2_obdd);
	obdd_destroy(x1_or_x2_obdd);
	obdd_destroy(x1_and_x2_obdd);
	obdd_destroy(x1_and_x2_obdd_bis);
	obdd_destroy(x1_or_x1_and_x2_obdd);
	obdd_destroy(x1_or_x1_and_x2_obdd_bis);
	obdd_destroy(x1_then_x2_obdd);
	obdd_mgr_destroy(new_mgr);
}

void run_next_obdd_tests(){
	obdd_mgr* new_mgr	= obdd_mgr_create();
	//compare x1 & !(x2 | x3) == x1 & !x2 & !x3
	obdd* x1_obdd		= obdd_mgr_var(new_mgr, "x1");
	obdd* not_x1_obdd	= obdd_apply_not(x1_obdd);
	obdd* x2_obdd		= obdd_mgr_var(new_mgr, "x2");
	obdd* x1_or_x2_obdd	= obdd_apply_or(x1_obdd, x2_obdd);
	obdd* next_x1_obdd	= obdd_apply_next(x1_obdd);
	obdd* next_x2_obdd	= obdd_apply_next(x2_obdd);

	obdd* not_x1_then_next_x2_obdd	= obdd_apply_or(x1_obdd, next_x2_obdd);
#if PRINT_TEST_OUTPUT
	printf("!X1");
#endif
	uint32_t buff_size  = 16384;
	char *buff = calloc(buff_size, sizeof(char));
	obdd_print(not_x1_obdd, buff, buff_size);

#if PRINT_TEST_OUTPUT
	printf("X(X2)");
#endif
	obdd_print(next_x2_obdd, buff, buff_size);
#if PRINT_TEST_OUTPUT
	printf("!X1 -> X(X2)\n");
#endif
	obdd_print(not_x1_then_next_x2_obdd, buff, buff_size);
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

	for(i = 0; i < img_count; i++)
		total_img[i]	= i + 2;
	obdd_get_valuations(new_mgr, not_x1_then_next_x2_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count, buff);

	char * expected = test_get_output_content("tests/expected_output/run_next_obdd_tests.exp");
	bool txt_cmp = strcmp(buff, expected) == 0;
	print_test_result(txt_cmp, "NEXT OBDD", "next obdd test");

	free(expected);



	free(valuations);
	free(total_img);
	free(initialized_values);
	free(valuation_set);
	free(last_nodes);
	free(dont_care_list);
	free(partial_valuation);
	free(buff);
	obdd_destroy(x1_obdd);
	obdd_destroy(not_x1_obdd);
	obdd_destroy(x2_obdd);
	obdd_destroy(x1_or_x2_obdd);
	obdd_destroy(next_x1_obdd);
	obdd_destroy(next_x2_obdd);
	obdd_destroy(not_x1_then_next_x2_obdd);
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
	uint32_t buff_size = 16384;
	char *buff = calloc(buff_size, sizeof(char));
	obdd_print(x1_or_x2_obdd, buff, buff_size);
#if PRINT_TEST_OUTPUT
	printf("x1 || x2 sat? : %s \n", obdd_is_sat(new_mgr, x1_or_x2_obdd->root_obdd) ? "yes" : "no");
	printf("x1 || x2 taut? : %s \n", obdd_is_tautology(new_mgr, x1_or_x2_obdd->root_obdd) ? "yes" : "no");
#endif
	obdd_print(x1_and_x2_obdd, buff, buff_size);
#if PRINT_TEST_OUTPUT
	printf("x1 && x2 sat? : %s \n", obdd_is_sat(new_mgr, x1_and_x2_obdd->root_obdd) ? "yes" : "no");
	printf("x1 && x2 taut? : %s \n", obdd_is_tautology(new_mgr, x1_and_x2_obdd->root_obdd) ? "yes" : "no");
#endif
	obdd_print(x1_and_not_x1_obdd, buff, buff_size);
#if PRINT_TEST_OUTPUT
	printf("x1 && !x1 sat? : %s \n", obdd_is_sat(new_mgr, x1_and_not_x1_obdd->root_obdd) ? "yes" : "no");
	printf("x1 && !x1 taut? : %s \n", obdd_is_tautology(new_mgr, x1_and_not_x1_obdd->root_obdd) ? "yes" : "no");
#endif
	obdd_print(x1_and_x2_then_x1, buff, buff_size);
#if PRINT_TEST_OUTPUT
	printf("(x1 && x2)->x1 sat? : %s \n", obdd_is_sat(new_mgr, x1_and_x2_then_x1->root_obdd) ? "yes" : "no");
	printf("(x1 && x2)->x1 taut? : %s \n", obdd_is_tautology(new_mgr, x1_and_x2_then_x1->root_obdd) ? "yes" : "no");
#endif
	obdd_print(exists_obdd, buff, buff_size);
#if PRINT_TEST_OUTPUT
	printf("E x2.(x2 = (x1 && !x1)) sat? : %s \n", obdd_is_sat(new_mgr, exists_obdd->root_obdd) ? "yes" : "no");
	printf("E x2.(x2 = (x1 && !x1)) taut? : %s \n", obdd_is_tautology(new_mgr, exists_obdd->root_obdd) ? "yes" : "no");
#endif
	obdd_mgr_print(new_mgr, buff);
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
	uint32_t img_count	= new_mgr->vars_dict->size - 3;
	uint32_t* total_img	= malloc(sizeof(uint32_t) * img_count);
	uint32_t i;

	for(i = 0; i < img_count; i++)
		total_img[i]	= i + 2;
#if PRINT_TEST_OUTPUT
	printf("X1 && X2\n");
#endif
	obdd_print(x1_and_x2_obdd, buff, buff_size);
	obdd_get_valuations(new_mgr, x1_and_x2_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count, buff);
#if PRINT_TEST_OUTPUT
	printf("X1 & X2 && X3\n");
#endif
	obdd_print(x1_and_x2_and_x3_obdd, buff, buff_size);
	obdd_get_valuations(new_mgr, x1_and_x2_and_x3_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count, buff);
#if PRINT_TEST_OUTPUT
	printf("X1 || X2\n");
#endif
	obdd_print(x1_or_x2_obdd, buff, buff_size);
	obdd_get_valuations(new_mgr, x1_or_x2_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count, buff);
#if PRINT_TEST_OUTPUT
	printf("X1 || X2 || X3\n");
#endif
	obdd_print(x1_or_x2_or_x3_obdd, buff, buff_size);
	obdd_get_valuations(new_mgr, x1_or_x2_or_x3_obdd, &valuations, &valuations_size, &valuations_count, total_img, img_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
	obdd_print_valuations(new_mgr, valuations, valuations_count, total_img, img_count, buff);

	char * expected = test_get_output_content("tests/expected_output/run_obdd_tests.exp");
	bool txt_cmp = strcmp(buff, expected) == 0;
	print_test_result(txt_cmp, "OBDD", "obdd test");

	free(expected);
	free(valuations);
	free(total_img);
	free(initialized_values);
	free(valuation_set);
	free(last_nodes);
	free(dont_care_list);
	free(partial_valuation);
	free(buff);
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
	char *buff = calloc(150 * cycles, sizeof(char));
	for(i = 0; i < cycles; i++){
		current_item.b = i; current_item.a = i + cycles;
		automaton_max_heap_add_entry(heap, &current_item);
		snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff) - 1, "PUSHING ITEM <%d,%d>\n", current_item.a, current_item.b);
	}
	for(i = 0; i < cycles; i++){
		current_item.b = i; current_item.a = i;
		automaton_max_heap_add_entry(heap, &current_item);
		snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff) - 1, "PUSHING ITEM <%d,%d>\n", current_item.a, current_item.b);
	}
	for(i = 0; i < cycles * 2; i++){
		automaton_max_heap_pop_entry(heap, &current_item);
		snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff) - 1, "POPPING ITEM: <%d,%d>\n", i, current_item.a, current_item.b);
	}
	automaton_max_heap_destroy(heap);

	FILE *ftest = fopen("tests/expected_output/run_max_heap_tests.exp", "w");
	fprintf(ftest, "%s", buff);
	fclose(ftest);

	char * expected = test_get_output_content("tests/expected_output/run_max_heap_tests.exp");
	bool txt_cmp = strcmp(buff, expected) == 0;
	print_test_result(txt_cmp, "MAX HEAP", "max heap test");

	free(expected);
	free(buff);

}

void run_concrete_bucket_list_tests(){
	automaton_concrete_bucket_list* list	= automaton_concrete_bucket_list_create(RANKING_BUCKET_SIZE, test_item_extractor, sizeof(test_item_bucket));
	test_item_bucket current_item;
	uint32_t i;
	uint32_t cycles = 600;
	for(i = 0; i < cycles; i++){
		current_item.b = i; current_item.a = i % 2;
		automaton_concrete_bucket_add_entry(list, &current_item);
	}

	char *buff = calloc(100 * cycles, sizeof(char));


	for(i = 0; i < cycles; i++){
		automaton_concrete_bucket_pop_entry(list, &current_item);
			snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff) - 1, "ITEM at %d: <%d,%d>\n", i, current_item.a, current_item.b);
	}
	automaton_concrete_bucket_destroy(list);

	char * expected = test_get_output_content("tests/expected_output/run_concrete_bucket_list_tests.exp");
	bool txt_cmp = strcmp(buff, expected) == 0;
	print_test_result(txt_cmp, "CONCRETE BUCKET LIST", "concrete bucket list test");
	free(expected);
	free(buff);
}

void run_ordered_list_tests(){
	test_item_bucket current_item;
	uint32_t i;
	automaton_ordered_list* ordered_list	= automaton_ordered_list_create(2, test_item_extractor, sizeof(test_item_bucket));
	uint32_t cycles = 1000;
	char *buff = calloc(100 * cycles, sizeof(char));
	for(i = 0; i < cycles; i++){
		current_item.b = i; current_item.a = i + cycles;
		snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff) - 1, "PUSHING ITEM at %d: <%d,%d>\n", i, current_item.a, current_item.b);
		automaton_ordered_list_add_entry(ordered_list, &current_item);
	}
	for(i = 0; i < cycles; i++){
		current_item.b = i; current_item.a = i;
		snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff) - 1, "PUSHING ITEM at %d: <%d,%d>\n", i, current_item.a, current_item.b);
		automaton_ordered_list_add_entry(ordered_list, &current_item);
	}
	test_item_bucket* print_bucket;
	for(i = 0; i < ordered_list->temporary_count; i++){
		print_bucket = GET_ORDERED_LIST_SINGLE_ENTRY(ordered_list, ordered_list->temporary, i);
		snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff) - 1, "temporary item at %d: <%d,%d>\n", i, print_bucket->a, print_bucket->b);
	}
	for(i = 0; i < ordered_list->count; i++){
			print_bucket = GET_ORDERED_LIST_SINGLE_ENTRY(ordered_list, ordered_list->values, i);
			snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff) - 1, "values item at %d: <%d,%d>\n", i, print_bucket->a, print_bucket->b);
		}
	while(ordered_list->count > 0){
		automaton_ordered_list_pop_entry(ordered_list, &current_item);
		snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff) - 1, "POPPING ITEM at %d: <%d,%d>\n", i, current_item.a, current_item.b);
	}
	automaton_ordered_list_destroy(ordered_list);

	char * expected = test_get_output_content("tests/expected_output/run_ordered_list_tests.exp");
	bool txt_cmp = strcmp(buff, expected) == 0;
	print_test_result(txt_cmp, "OREDERED LIST", "ordered bucket list test");

	free(expected);
	free(buff);

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
	uint32_t buff_size	= 16384;
	char *buff = calloc(buff_size, sizeof(char));
	obdd_state_tree_print(tree, buff, buff_size);
	free(buff);
	obdd_state_tree_destroy(tree);
}
void run_obdd_tree_tests(){
	obdd_state_tree* tree	= obdd_state_tree_create(7);
	bool key1[7]				= {false, false, false, false, false, false, false};
	uint32_t i, j;
	bool passed = true;
	for(i = 0; i < 128; i++){
		for(j = 0; j < 7; j++){
			key1[j] = ((i >> j) & 0x1) == 0;
		}
#if PRINT_TEST_OUTPUT
		printf("%d:%d %s", i, obdd_state_tree_get_key(tree, key1, -1), i % 5 == 0 ? "\n" : "  ");
#endif
		passed = passed & ((i + 1) == obdd_state_tree_get_key(tree, key1, -1));
	}
#if PRINT_TEST_OUTPUT
	printf("\nREPEATING VALUES\n");
#endif
	for(i = 0; i < 128; i++){
		for(j = 0; j < 7; j++){
			key1[j] = ((i >> j) & 0x1) == 0;
		}
#if PRINT_TEST_OUTPUT
		printf("%d:%d %s", i, obdd_state_tree_get_key(tree, key1, -1), i % 5 == 0 ? "\n" : "  ");
#endif
		passed = passed & ((i + 1) == obdd_state_tree_get_key(tree, key1, -1));
	}
	print_test_result(passed, "OBDD TREE", "128 values tested");
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
	automaton_fluent_add_starting_signals(fluents[0], alphabet, 1, &in);
	automaton_fluent_add_ending_signals(fluents[0], alphabet, 1, &tau);
	automaton_fluent_add_starting_signals(fluents[1], alphabet, 1, &out);
	automaton_fluent_add_ending_signals(fluents[1], alphabet, 1, &in);

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
	automaton_automaton* composition	= automaton_automata_compose(automata, 2, CONCURRENT, false, "TEST");
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
	automaton_fluent_add_starting_signals(f1, alphabet, 1, &x1);
	automaton_fluent_add_ending_signals(f1, alphabet, 1, &y1);
	automaton_fluent_add_ending_signals(f1, alphabet, 1, &y2);

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

	free(local_alphabet);
	free(fluents);
	free(liveness_valuations);
	free(liveness_valuations_names);


	FILE *ftest = fopen("tests/expected_output/run_report_tests.test", "w");
	automaton_fluent_serialize_report(ftest, f1);
	automaton_alphabet_serialize_report(ftest, alphabet);
	automaton_automata_context_serialize_report(ftest, ctx);
	automaton_transition_serialize_report(ftest, t1);
	automaton_automaton_serialize_report(ftest, automaton);

	automaton_transition_destroy(t1, true);
	automaton_fluent_destroy(f1, true);
	automaton_alphabet_destroy(alphabet);
	automaton_signal_event_destroy(x1, true);
	automaton_signal_event_destroy(x2, true);
	automaton_signal_event_destroy(y1, true);
	automaton_signal_event_destroy(y2, true);

	automaton_automaton_destroy(automaton);
	automaton_automata_context_destroy(ctx);
	obdd_mgr_destroy(mgr);

	fclose(ftest);
	char * current = test_get_output_content("tests/expected_output/run_report_tests.test");
	char * expected = test_get_output_content("tests/expected_output/run_report_tests.exp");

	bool txt_cmp = strcmp(current, expected) == 0;
	print_test_result(txt_cmp, "REPORT", "automaton report test");

	free(expected);
	free(current);
}

typedef struct test_item_pool_str{
	uint32_t a; char name[250];
}test_item_pool;

#define MAX_POOL_COUNT	1000

void run_fast_pool_tests(){
	size_t item_size	= sizeof(test_item_pool);
	test_item_pool** pool_ref1	= malloc(MAX_POOL_COUNT * sizeof(test_item_pool*));
	test_item_pool** pool_ref2	= malloc(MAX_POOL_COUNT * sizeof(test_item_pool*));
	test_item_pool** pool_frag1	= malloc(MAX_POOL_COUNT * sizeof(uint32_t));
	test_item_pool** pool_frag2	= malloc(MAX_POOL_COUNT * sizeof(uint32_t));


	char *buff = calloc(MAX_POOL_COUNT * 200, sizeof(char));

	automaton_fast_pool* pool1	= automaton_fast_pool_create(item_size, 100, 100);
	automaton_fast_pool* pool2	= automaton_fast_pool_create(item_size, 5, 2);

	uint32_t i;
	for(i = 0; i < MAX_POOL_COUNT/2; i++){
		test_item_pool* item	= automaton_fast_pool_get_instance(pool1, (uint32_t*)&(pool_frag1[i]));
		item->a		= i;
		item->name[0]	= 'h';item->name[1]	= 'i';item->name[2]	= '!';item->name[3]	= '\0';
		pool_ref1[i]	= item;
		item	= automaton_fast_pool_get_instance(pool2, (uint32_t*)&(pool_frag2[i]));
		item->a		= i;
		item->name[0]	= 'h';item->name[1]	= 'i';item->name[2]	= '!';item->name[3]	= '\0';
		pool_ref2[i]	= item;
	}
	snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff) - 1, "10 100 pool \n");
	for(i = 0; i < MAX_POOL_COUNT/2; i++){
		snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff) - 1, "%d%s ", pool_ref1[i]->a, pool_ref1[i]->name);
	}
	snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff) - 1, "\n");
	snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff) - 1, "5 2 pool \n");
	for(i = 0; i < MAX_POOL_COUNT/2; i++){
		snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff) - 1, "%d%s ", pool_ref2[i]->a, pool_ref2[i]->name);
	}
	snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff) - 1, "\n");
	for(i = 0; i < MAX_POOL_COUNT/2; i++){
		automaton_fast_pool_release_instance(pool1, (uintptr_t)pool_frag1[i]);
		automaton_fast_pool_release_instance(pool2, (uintptr_t)pool_frag2[i]);
	}
	for(i = 0; i < MAX_POOL_COUNT/2; i++){
		test_item_pool* item	= automaton_fast_pool_get_instance(pool1, (uint32_t*)&(pool_frag1[i]));
		item->a		= i;
		item->name[0]	= 'h';item->name[1]	= 'i';item->name[2]	= '!';item->name[3]	= '\0';
		pool_ref1[i]	= item;
		item	= automaton_fast_pool_get_instance(pool2, (uint32_t*)&(pool_frag2[i]));
		item->a		= i;
		item->name[0]	= 'h';item->name[1]	= 'i';item->name[2]	= '!';item->name[3]	= '\0';
		pool_ref2[i]	= item;
		snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff) - 1, "%d%s ", item->a, item->name);
	}
	snprintf(buff + strlen(buff), sizeof(buff) - strlen(buff) - 1, "\n");
	automaton_fast_pool_destroy(pool1);
	automaton_fast_pool_destroy(pool2);
	free(pool_frag1);free(pool_frag2);
	free(pool_ref1);free(pool_ref2);

	char * expected = test_get_output_content("tests/expected_output/run_fast_pool_tests.exp");
	bool txt_cmp = strcmp(buff, expected) == 0;
	print_test_result(txt_cmp, "FAST POOL", "fast pool test");

	free(expected);
	free(buff);
}

void run_bool_array_hash_table_tests(){
	int32_t variable_count	= 40;
	automaton_bool_array_hash_table* table	= automaton_bool_array_hash_table_create(variable_count);
	int32_t i;
	bool **values	= malloc(variable_count * sizeof(bool*));
	bool test_cmp	= true;
	for(i = 0; i < variable_count; i++){
		values[i]	= calloc(variable_count, sizeof(bool));
		values[i][i]	= true;
		if(i % 4 == 0 && i > 0){
			values[i - 1][i]	= true;
			values[i][i - 1]	= true;
		}
	}
	for(i = 0; i < variable_count; i++){
		if(i % 4 == 0 && i > 0){
			test_cmp = test_cmp && automaton_bool_array_hash_table_get_entry(table, values[i]) != NULL;
		}
		if(automaton_bool_array_hash_table_get_entry(table, values[i]) == NULL){
			automaton_bool_array_hash_table_add_entry(table, values[i], true);
		}
	}
	automaton_bool_array_hash_table_destroy(table);
	for(i = 0; i < variable_count; i++){
		free(values[i]);
	}
	free(values);
	print_test_result(test_cmp, "BOOL_ARRAY_HASH", "bool array hash table");
}

void run_functional_tests(){
	//MODULE TESTING
	run_obdd_tree_tests();
	run_small_obdd_tests();
	run_next_obdd_tests();
	run_obdd_tests();
	run_obdd_valuations();
	run_obdd_exists();
	run_concrete_bucket_list_tests();
	run_ordered_list_tests();
	run_max_heap_tests();
	run_report_tests();
	run_fast_pool_tests();
	run_bool_array_hash_table_tests();
	//DRY TESTS
	run_parse_test("tests/composition_types.fsp", "compositions type");
	run_parse_test("tests/biscotti.fsp", "biscotti");
	run_parse_test("tests/ranges_1.fsp",  "range tests 1");
	run_parse_test("tests/ranges_2.fsp",  "range 2 test");
	run_parse_test("tests/ranges_3.fsp",  "range 3 test");
	run_parse_test("tests/mixed_3_signals_2_labels.fsp", "mixed model 3 signals 2 labels");//mixed model 3 signals 2 labels
	run_parse_test("tests/two_floors_lift.fsp", "lift 2 floors");//lift 2 floors
	run_parse_test("tests/concurrency_equiv_test.fsp",  "Asynch composition equiv. test");
	run_parse_test("tests/nonreal_test_1_realizable_version.fsp", "non realizable test 1 (realizable version)");
	run_parse_test("tests/nonreal_test_2_realizable_version.fsp", "non realizable test 2 (realizable version)");
	run_parse_test("tests/obdd_context_1.fsp", "OBDD NEXT IFF TEST");
	run_parse_test("tests/genbuf_2_sndrs_debug.fsp", "GenBuf 2 sndrs(debug version)");//GENBUF 2 Sndrs debug version
	run_parse_test("tests/nonreal_test_1.fsp", "non realizable test 1");
}

void run_load_tests(){
	run_parse_test("tests/k_10_100_graph.fsp",  "k 10, 100 graph tests");
	run_parse_test("tests/seven_floors_lift.fsp", "lift 7 floors");//lift 7 floors
	run_parse_test("tests/test37.fsp", "lts load test 1");
}

void run_all_tests(){
	run_functional_tests();
	run_load_tests();
}

void print_help(){
	printf("CLTS modeling and synthesis tool usage\n");
	printf("\t-h\tprints this help message\n");
	printf("\t-r filename\t inteprets specification located at [filename]\n");
	printf("\t--all-tests\t runs all tests\n");
	printf("\t--func-tests\t runs functional tests\n");
	printf("\t--load-tests\t runs load tests\n");
}

int main (int argc, char** argv){
	int32_t i;
	if(argc > 1){
		if(strcmp(argv[1], "-h") == 0)
			print_help();
		if(strcmp(argv[1], "--all-tests") == 0)
			run_all_tests();
		if(strcmp(argv[1], "--func-tests") == 0)
			run_functional_tests();
		if(strcmp(argv[1], "--load-tests") == 0)
			run_load_tests();
		char name_buff[700];
		if(argc > 2){
			if(strcmp(argv[1], "-r") == 0){
				char const *folder = getenv("TMPDIR");
				if (folder == 0)
					folder = "/tmp";
				char result_buff[255];
				char *result_name;
				for(i = 2; i < argc; i++){
					result_name = strrchr(argv[i], '/');
					if(result_name == NULL)result_name = argv[i];
					else result_name++;
					snprintf(result_buff, sizeof(result_buff),"%s/%s", folder, 	result_name);
					snprintf(name_buff, sizeof(name_buff), "Running:%s", argv[i]);
					run_parse_test_local(argv[i], name_buff, result_buff);
				}
			}
		}
	}else{
		//ONGOING
		//run_parse_test("tests/genbuf_2_sndrs_simplified.fsp", "GenBuf 2 sndrs (simplified)");//GENBUF 2 Sndrs
		//run_parse_test("tests/genbuf_2_sndrs.fsp", "GenBuf 2 sndrs");//GENBUF 2 Sndrs
		//run_parse_test("tests/genbuf_4_sndrs_no_automaton.fsp", "GenBuf 4 sndrs(no automaton)");//GENBUF 4 sndrs
		//run_parse_test("tests/genbuf_4_sndrs.fsp", "GenBuf 4 sndrs");//GENBUF 4 sndrs
		//run_parse_test("tests/two_floors_lift.fsp", "lift 2 floors");//lift 2 floors
		run_parse_test("tests/nonreal_test_1.fsp", "non realizable test 1");

		//GENERAL TESTS
		//run_all_tests();
		//run_functional_tests();

		//run_parse_test("tests/current_sut.fsp", "current_SUT");

		//TODO

		//run_parse_test("tests/half_adder_to_full_adder.fsp",  "half adder to full adder test");
		//run_parse_test("tests/two_full_adders.fsp",  "two full adders test");
		//run_parse_test("tests/automata_load_test_1.fsp",  "automata load test");
		//run_parse_test("tests/receiver_asynch_test.fsp",  "receiver asynch test");

		//run_parse_test("tests/test18.fsp",  "test18");
		//run_parse_test("tests/test26.fsp", "test26");
		//run_parse_test("tests/test27.fsp", "test27");
		//run_parse_test("tests/test32.fsp", "test32");
		//run_parse_test("tests/test34.fsp", "test34");
		//run_parse_test("tests/test40.fsp", "compositions type");

		//SHOWCASE
		//run_parse_test("tests/test21.fsp", "test21");


		//run_parse_test("tests/test29.fsp", "lift 3 floors");//lift 3 floors
		//run_parse_test("tests/test36.fsp", "lift 5 floors");//lift 5 floors

		//IN PROGRESS
		//run_parse_test("tests/test37.fsp", "lts load test 1");
		//run_parse_test("tests/test38.fsp", "lift 5 floors + 10 variables");//lift 5 floors + 10 variables

	}
	obdd_mgr* mgr	= parser_get_obdd_mgr();
	obdd_mgr_destroy(mgr);

	return 0;    
}


