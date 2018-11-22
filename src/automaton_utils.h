/*
 * automaton_utils.h
 *
 *  Created on: Apr 20, 2018
 *      Author: mariano
 */

#ifndef AUTOMATON_UTILS_H_
#define AUTOMATON_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define LIST_INITIAL_SIZE		32
#define LIST_INCREASE_FACTOR	2

#define TRUE_VAR			"1"
#define FALSE_VAR			"0"

#define SET_BIT(A,k)     ( A[(k/32)] |= (1 << (k%32)) )
#define CLEAR_BIT(A,k)   ( A[(k/32)] &= ~(1 << (k%32)) )
#define TEST_BIT(A,k)    ( A[(k/32)] & (1 << (k%32)) )

/** CONSOLE COLORS **/
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/** LIST FUNCTIONS **/
void aut_add_ptr_list(void*** list, void* element, uint32_t* count);
void aut_add_incr_ptr_list(void*** list, void* element, uint32_t* size, uint32_t* count);
void aut_free_ptr_list(void*** list, uint32_t* count);
/** STRING FUNCTIONS **/
void aut_dupstr(char** dst, char* src);
void aut_merge_string_lists(char*** a, int32_t* a_count, char** b, int32_t b_count, bool sorted, bool repeat_values);
bool aut_push_string_to_list(char*** list, int32_t* list_count, char* element, int32_t* position, bool sorted, bool repeat_values);
int32_t aut_string_list_index_of(char** list, int32_t list_count, char* element);
#endif /* AUTOMATON_UTILS_H_ */
