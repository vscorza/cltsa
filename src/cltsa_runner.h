/*
 * cltsa_runner.h
 *
 *  Created on: Apr 16, 2021
 *      Author: mariano
 */

#ifndef SRC_CLTSA_RUNNER_H_
#define SRC_CLTSA_RUNNER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "automaton_context.h"
//#include "minuint.h"

#define PRINT_TEST_OUTPUT 0
#define STRING_LIST_SIZE	8
#define FAST_LIST_VAR_COUNT	100
#define COMPOSITE_TEST_STATE_COUNT	100
#define COMPOSITE_TEST_AUTOMATA_COUNT	2
#define MAX_POOL_COUNT	1000

typedef struct test_item_bucket_str{
	uint32_t a; uint32_t b;
}test_item_bucket;
typedef struct test_item_pool_str{
	uint32_t a; char name[250];
}test_item_pool;


char* test_get_output_content(char *filename);
void test_set_expected_content(char* content, char* filename);
void print_test_result(bool passed, char* name, char* description);
void run_parse_test_local(char* test_file, char* test_name, char* result_name, char* steps_name,
		diagnosis_search_method diagnosis_method, bool append_results);
void run_parse_test(char* test_file, char* test_name);
void run_parse_test_linear(char* test_file, char* test_name);
void run_diagnosis(char* test_file, char* test_name, bool append_results);
void run_automaton_export_test();
void run_automaton_import_test();
void run_obdd_exists();
void run_obdd_valuations();
void run_small_obdd_tests();
void run_next_obdd_tests();
void run_obdd_tests();

void run_automaton_string_list_test();
uint32_t test_item_compare(void* left, void* right);
uint32_t test_item_extractor(void* entry);
void run_max_heap_tests();
void run_max_heap_tests_2();
void run_concrete_bucket_list_tests();
void run_ordered_list_tests();
void run_fsp_tests(uint32_t test_count);
void run_small_obdd_tree_tests();
void run_obdd_tree_tests();
void run_obdd_cache_tests();
void run_obdd_fast_lists_tests();
void run_tree_tests();
void run_automaton_tests();
void run_report_tests();
void run_fast_pool_tests();
void run_bool_array_hash_table_tests();
void run_automaton_composite_hash_table_small_tests();
void run_automaton_composite_hash_table_tests();
void run_functional_tests();
void run_load_tests();
void run_all_tests();

#endif /* SRC_CLTSA_RUNNER_H_ */
