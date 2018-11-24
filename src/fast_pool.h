/*
 * fast_pool.h
 *
 *  Created on: Nov 15, 2018
 *      Author: mariano
 */

#ifndef SRC_FAST_POOL_H_
#define SRC_FAST_POOL_H_

#define DEBUG_POOL 0

#include "automaton_utils.h"

typedef struct automaton_fast_pool_str {
	uintptr_t* next_free_element;
	uint32_t next_free_element_fragment_index;
    uint32_t size ;
    uint32_t fragment_size;
    uintptr_t* fragments;
	uint32_t* fragments_count;
	uint32_t* fragments_last_index;
	uintptr_t* fragments_last_free_element;
	size_t sizeof_element;
	uint32_t composite_count;
}automaton_fast_pool;

automaton_fast_pool* automaton_fast_pool_create(size_t sizeof_element, uint32_t initial_size, uint32_t fragment_size);
void automaton_fast_pool_destroy(automaton_fast_pool* pool);
void* automaton_fast_pool_get_instance(automaton_fast_pool* pool, uint32_t *fragment_ID);
bool automaton_fast_pool_release_instance(automaton_fast_pool* pool, uint32_t fragment_ID);

#endif /* SRC_FAST_POOL_H_ */
