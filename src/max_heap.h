/*
 * ordered_list.h
 *
 *  Created on: Jul 30, 2018
 *      Author: mariano
 */

#ifndef MAX_HEAP_H
#define MAX_HEAP_H

#define INITIAL_HEAP_SIZE	64

#include "automaton_utils.h"

#define MAX_HEAP_L_INDEX(x) (2 * x + 1)
#define MAX_HEAP_R_INDEX(x) (2 * x + 2)
#define MAX_HEAP_PARENT_INDEX(x) ((x - 1)/2)
#define GET_MAX_HEAP_ARRAY_ENTRY(heap, array, index)	((void*)(((char*)array) + (index * heap->sizeof_element)))
#define GET_MAX_HEAP_ENTRY(heap, index)	((void*)(((char*)heap->values) + (index * heap->sizeof_element)))
#define GET_MAX_HEAP_L_CHILD(heap, x) GET_MAX_HEAP_ENTRY(heap,MAX_HEAP_L_INDEX(x))
#define GET_MAX_HEAP_R_CHILD(heap, x) GET_MAX_HEAP_ENTRY(heap,MAX_HEAP_R_INDEX(x))
#define GET_MAX_HEAP_PARENT(heap, x) GET_MAX_HEAP_ENTRY(heap,MAX_HEAP_PARENT_INDEX(x))

//it should return > 0 if the first is greater than second, 0 if they are equal, < 0 otherwise
typedef int32_t (*automaton_max_heap_compare_func)(void*, void*);
typedef void (*automaton_max_heap_copy_func)(void*, void*);
typedef uint32_t (*automaton_max_heap_extractor_func)(void*);

typedef struct automaton_max_heap_str {
    uint32_t size ;
    void* values ;
	uint32_t count;
	uint32_t sizeof_element;
	automaton_max_heap_compare_func cmp_func;
	automaton_max_heap_extractor_func extractor_func;
	void* tmp_value;
}automaton_max_heap;

automaton_max_heap* automaton_max_heap_create(uint32_t sizeof_element, automaton_max_heap_compare_func compare_func);
void automaton_max_heap_swap(automaton_max_heap* heap, void* n1, void* n2);
void automaton_max_heap_heapify(automaton_max_heap* heap, uint32_t i) ;
automaton_max_heap* automaton_max_heap_create_from_array(uint32_t sizeof_element, automaton_max_heap_compare_func compare_func,void *arr, uint32_t count);
void automaton_max_heap_add_entry(automaton_max_heap* heap, void* entry);
void automaton_max_heap_pop_entry(automaton_max_heap* heap, void* target);
uint32_t automaton_max_heap_get_min_entry_index(automaton_max_heap* heap, void* target, uint32_t i);
void automaton_max_heap_destroy(automaton_max_heap* heap);

#endif
