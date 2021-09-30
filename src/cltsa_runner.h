/*
 * cltsa_runner.h
 *
 *  Created on: Apr 16, 2021
 *      Author: mariano
 */

#ifndef SRC_CLTSA_RUNNER_H_
#define SRC_CLTSA_RUNNER_H_

#include "automaton_context.h"
#include <dirent.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include "minuint.h"

#define PRINT_TEST_OUTPUT 0
#define STRING_LIST_SIZE 8
#define FAST_LIST_VAR_COUNT 100
#define COMPOSITE_TEST_STATE_COUNT 100
#define COMPOSITE_TEST_AUTOMATA_COUNT 2
#define MAX_POOL_COUNT 1000
#define FAST_LIST_VAR_COUNT 100 // 10000
#define GR1_TEST_ASSUMPTION_COUNT 2
#define GR1_TEST_GOALS_COUNT 2

typedef struct test_item_bucket_str {
  uint32_t a;
  uint32_t b;
} test_item_bucket;
typedef struct test_item_pool_str {
  uint32_t a;
  char name[250];
} test_item_pool;

typedef enum automaton_test_type_enum {
  TEST_LOSE_DEADLOCK = 0,
  TEST_ALTERNATING_GOALS = 1,
  TEST_AVOID_SIGMA_TRAP = 2,
  TEST_LOSE_SIGMA_TRAP = 3,
  TEST_AVOID_SIGMA_NO_LABEL = 4
} automaton_test_type;

/** TEST UTILITIES **/
char *test_get_output_content(char *filename);
void test_set_expected_content(char *content, char *filename);
void print_test_result(bool passed, char *name, char *description);
void run_parse_test_local(char *test_file, char *test_name, char *result_name, char *steps_name, diagnosis_search_method diagnosis_method,
                          bool append_results);
void run_parse_test(char *test_file, char *test_name);
void run_parse_test_linear(char *test_file, char *test_name);
void run_diagnosis(char *test_file, char *test_name, bool append_results);
void run_automaton_export_test();
void run_automaton_import_test();

/** OBDD TESTS **/
void run_obdd_exists();
void run_obdd_valuations();
void run_small_obdd_tests();
void run_next_obdd_tests();
void run_obdd_tests();
void run_small_obdd_tree_tests();
void run_obdd_tree_tests();
void run_obdd_cache_tests();
void run_obdd_fast_lists_tests();

/** STRUCTS TESTS **/
void run_automaton_string_list_test();
uint32_t test_item_compare(void *left, void *right);
uint32_t test_item_extractor(void *entry);
void run_max_heap_tests();
void run_max_heap_tests_2();
void run_concrete_bucket_list_tests();
void run_ordered_list_tests();
void run_tree_tests();
void run_bool_array_hash_table_tests();
void run_automaton_composite_hash_table_small_tests();
void run_automaton_composite_hash_table_tests();
void run_functional_tests();

/** GR1 TESTS **/
void build_automaton_and_ranking_for_tests(uint32_t *assumptions_count, uint32_t *goals_count, uint32_t **assumptions_indexes,
                                           uint32_t **guarantees_indexes, char ***assumptions, char ***goals,
                                           automaton_concrete_bucket_list ***ranking_system, uint32_t **max_delta,
                                           automaton_automaton **game_automaton, automaton_test_type type);
void destroy_automaton_and_ranking_for_tests(automaton_automaton *game_automaton, uint32_t assumptions_count, uint32_t guarantees_count,
                                             uint32_t *assumptions_indexes, uint32_t *guarantees_indexes, char **assumptions, char **goals,
                                             automaton_concrete_bucket_list **ranking_system, uint32_t *max_delta);
void set_automaton_ranking_for_tests(automaton_automaton *game_automaton, uint32_t assumptions_count, uint32_t guarantees_count,
                                     uint32_t *assumptions_indexes, uint32_t *guarantees_indexes,
                                     automaton_concrete_bucket_list ***ranking_system);
void run_gr1_initialization_tests();
void run_ranking_arithmetic_tests();
void run_ranking_stabilization_tests();
void run_strategy_build_tests();
/** MACRO TESTS **/
void run_automaton_tests();
void run_report_tests();
void run_fast_pool_tests();
void run_all_tests();
void run_load_tests();
#endif /* SRC_CLTSA_RUNNER_H_ */
