/*
 * ordered_list.h
 *
 *  Created on: Jul 30, 2018
 *      Author: mariano
 */

#ifndef ORDERED_LIST
#define ORDERED_LIST

#include "automaton_utils.h"

#define DEBUG_ORDERED_LIST 0

typedef uint32_t (*automaton_ordered_list_key_extractor_func)(void*);
typedef void (*automaton_ordered_list_copy_func)(void*, void*);

#define GET_ORDERED_LIST_SINGLE_ENTRY(list, array, index)	((void*)(((char*)array) + (index * list->sizeof_element)))

typedef struct automaton_ordered_list_str{
	uint32_t count;
	uint32_t size;
	uint32_t temporary_count;
	uint32_t temporary_size;
	void *values;
	void *temporary;
	uint32_t sizeof_element;
	automaton_ordered_list_key_extractor_func extractor_func;
}automaton_ordered_list;

automaton_ordered_list* automaton_ordered_list_create(uint32_t temporary_size, automaton_ordered_list_key_extractor_func extractor_func, uint32_t sizeof_element);
void automaton_ordered_list_initialize(automaton_ordered_list* bucket, uint32_t temporary_size
		, automaton_ordered_list_key_extractor_func extractor_func, uint32_t sizeof_element);
automaton_ordered_list* automaton_ordered_list_clone(automaton_ordered_list* source);
void automaton_ordered_list_copy(automaton_ordered_list* target, automaton_ordered_list* source);
int32_t automaton_ordered_list_binary_search(automaton_ordered_list* list, void* values, uint32_t low, uint32_t high, uint32_t key);
bool automaton_ordered_list_has_key(automaton_ordered_list* list, uint32_t key);
bool automaton_ordered_list_has_entry(automaton_ordered_list* list, void* entry);
void* automaton_ordered_list_get_entry(automaton_ordered_list* list, uint32_t key);
void automaton_ordered_list_pop_entry(automaton_ordered_list* list, void* target);
bool automaton_ordered_list_add_entry(automaton_ordered_list* list, void* entry);
void automaton_ordered_list_reset(automaton_ordered_list* list);
void automaton_ordered_list_destroy(automaton_ordered_list* list);

#endif /* ORDERED_LIST */
