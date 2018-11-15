/*
 * fast_pool.h
 *
 *  Created on: Nov 15, 2018
 *      Author: mariano
 */

#ifndef SRC_FAST_POOL_H_
#define SRC_FAST_POOL_H_

#include "automaton_utils.h"

typedef struct automaton_fast_pool_str {
	void* next_free_element;
	uint32_t next_free_element_fragment_index;
    uint32_t size ;
    uint32_t fragment_size;
    void** fragments;
	uint32_t* fragments_count;
	uint32_t* fragments_last_index;
	uint32_t sizeof_element;
}automaton_fast_pool;

automaton_fast_pool* automaton_fast_pool_create(uint32_t sizeof_element, uint32_t initial_size, uint32_t fragment_size);
void automaton_fast_pool_destroy(automaton_fast_pool* pool);

void* automaton_fast_pool_get_instance(automaton_fast_pool* pool){
	//check if pool is full
	uint32_t i;
	void* value	= NULL;
	if(pool->next_free_element == NULL){
		uint32_t new_size	= ++(pool->size);
		void** ptr	= realloc(pool->fragments, new_size * sizeof(void*));
		if(ptr!= NULL){
			pool->size	= new_size;
			pool->fragments[pool->size - 1]	= calloc(pool->fragment_size, pool->sizeof_element);
		}else{
			printf("Could not get new instance from pool\n"); exit(-1);
		}
		uint32_t* ptr_uint	= realloc(pool->fragments_count, new_size * sizeof(uint32_t));
		if(ptr_uint != NULL){
			pool->fragments_count	= ptr_uint; pool->fragments_count[pool->size - 1]	= 0;
		}else{
			printf("Could not get new instance from pool\n"); exit(-1);
		}
		ptr_uint	= realloc(pool->fragments_last_index, new_size * sizeof(uint32_t));
		if(ptr_uint != NULL){
			pool->fragments_last_index	= ptr_uint; pool->fragments_last_index[pool->size - 1]	= 0;
		}else{
			printf("Could not get new instance from pool\n"); exit(-1);
		}
		pool->next_free_element	= &(pool->fragments[pool->size - 1][0]);
		pool->next_free_element_fragment_index	= pool->size - 1;
	}
	value	= pool->next_free_element;
	//update structures
	pool->fragments_count[pool->next_free_element_fragment_index]++;
	pool->fragments_last_index[pool->next_free_element_fragment_index]++;
	if(pool->fragments_count[pool->next_free_element_fragment_index] < (pool->fragment_size - 1)){
		pool->next_free_element	= &(pool->fragments[pool->next_free_element_fragment_index][pool->fragments_last_index[pool->next_free_element_fragment_index]]);
	}else{
		//check if there is an empty fragment somewhere
		bool found	= false;
		for(i = 0; i < pool->size; i++){
			if(pool->fragments_last_index[i] < (pool->fragment_size - 1)){
				pool->next_free_element_fragment_index	= i;
				pool->next_free_element	= &(pool->fragments[pool->next_free_element_fragment_index][pool->fragments_last_index[pool->next_free_element_fragment_index]]);
				found	= true;
				break;
			}
		}
		if(!found){
			pool->next_free_element = NULL;
		}
	}

	return value;
}
void automaton_fast_pool_release_instance(automaton_fast_pool* pool, void* instance){
	//check void as address in range of fragment, if so decrement count, if count == 0 set last_index to 0
}


#endif /* SRC_FAST_POOL_H_ */
