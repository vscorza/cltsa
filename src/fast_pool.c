/*
 * fast_pool.c
 *
 *  Created on: Nov 15, 2018
 *      Author: mariano
 */
#include "fast_pool.h"

automaton_fast_pool* automaton_fast_pool_create(uint32_t sizeof_element, uint32_t initial_size, uint32_t fragment_size){
	automaton_fast_pool *pool	= malloc(sizeof(automaton_fast_pool));
	pool->size					= initial_size;
	pool->fragment_size			= fragment_size;
	pool->sizeof_element		= sizeof_element;
	pool->fragments_count		= calloc(pool->size, sizeof(uint32_t));
	pool->fragments_last_index	= calloc(pool->size, sizeof(uint32_t));
	pool->fragments				= calloc(pool->size, sizeof(void*));
	uint32_t i;
	for(i = 0; i < pool->size; i++){
		pool->fragments[i]		= calloc(pool->fragment_size, pool->sizeof_element);
	}
	pool->next_free_element		= &(pool->fragments[0][0]);
	pool->next_free_element_fragment_index	= 0;
	return pool;
}
void automaton_fast_pool_destroy(automaton_fast_pool* pool){
	uint32_t i;
	for(i = 0; i < pool->size; i++){
		free(pool->fragments[i]);
	}
	free(pool->fragments_count);
	free(pool->fragments_last_index);
	free(pool->fragments);
	free(pool);
}


