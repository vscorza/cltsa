/*
 * max_heap.c
 *
 *  Created on: Jul 31, 2018
 *      Author: mariano
 */
#include "max_heap.h"
#include "automaton.h"
/*
    Function to initialize the max heap with size = 0

#define MAX_HEAP_L_INDEX(x) (2 * x + 1)
#define MAX_HEAP_R_INDEX(x) (2 * x + 2)
#define MAX_HEAP_PARENT_INDEX(x) ((x - 1)/2)
#define GET_MAX_HEAP_ARRAY_ENTRY(heap, array, index)	((void*)(((char*)array) + (index * heap->sizeof_element)))
#define GET_MAX_HEAP_ENTRY(heap, index)	((void*)(((char*)heap->values) + (index * heap->sizeof_element)))
#define GET_MAX_HEAP_L_CHILD(heap, x) GET_MAX_HEAP_ENTRY(heap,MAX_HEAP_L_INDEX(x))
#define GET_MAX_HEAP_R_CHILD(heap, x) GET_MAX_HEAP_ENTRY(heap,MAX_HEAP_R_INDEX(x))
#define GET_MAX_HEAP_PARENT(heap, x) GET_MAX_HEAP_ENTRY(heap,MAX_HEAP_PARENT_INDEX(x))
*/

automaton_max_heap* automaton_max_heap_create(uint32_t sizeof_element, automaton_max_heap_compare_func compare_func){
	automaton_max_heap* max_heap	= malloc(sizeof(automaton_max_heap));
	max_heap->cmp_func				= compare_func;
	max_heap->sizeof_element		= sizeof_element;
	max_heap->count					= 0;
	max_heap->size					= INITIAL_HEAP_SIZE;
	max_heap->values				= malloc(max_heap->sizeof_element * max_heap->size);
	max_heap->values_ptrs			= malloc(max_heap->size * sizeof(uint32_t));
	uint32_t i;
	for(i = 0; i < max_heap->size; i++)max_heap->values_ptrs[i]	= i;
	return max_heap;
}

void automaton_max_heap_swap(automaton_max_heap* heap, uint32_t n1, uint32_t n2) {
	if(heap->values_ptrs[n1] == heap->values_ptrs[n2])return;
	heap->tmp_value	= heap->values_ptrs[n1];
	heap->values_ptrs[n1]	= heap->values_ptrs[n2];
	heap->values_ptrs[n2]	= heap->tmp_value;
}


/*
    Heapify function is used to make sure that the heap property is never violated
    In case of deletion of a node, or creating a max heap from an array, heap property
    may be violated. In such cases, heapify function can be called to make sure that
    heap property is never violated
*/
/*
 * void automaton_max_heap_heapify(automaton_max_heap* heap, uint32_t i) {
	uint32_t largest = (MAX_HEAP_L_INDEX(i) < heap->count && heap->cmp_func(GET_MAX_HEAP_L_CHILD(heap, i),GET_MAX_HEAP_ENTRY(heap, i)) > 0) ? MAX_HEAP_L_INDEX(i) : i;
	if(MAX_HEAP_R_INDEX(i) < heap->count && heap->cmp_func(GET_MAX_HEAP_R_CHILD(heap, i),GET_MAX_HEAP_ENTRY(heap, largest)) > 0)
		largest = MAX_HEAP_R_INDEX(i);
	if(largest != i){
		automaton_max_heap_swap(heap, GET_MAX_HEAP_ENTRY(heap, i), GET_MAX_HEAP_ENTRY(heap, largest));
		automaton_max_heap_heapify(heap, largest);
	}
}
*/
void automaton_max_heap_heapify(automaton_max_heap* heap, uint32_t i) {
	uint32_t largest = (MAX_HEAP_L_INDEX(i) < heap->count &&
			heap->cmp_func(GET_MAX_HEAP_ENTRY(heap, heap->values_ptrs[MAX_HEAP_L_INDEX(i)]),GET_MAX_HEAP_ENTRY(heap, heap->values_ptrs[i])) > 0) ? MAX_HEAP_L_INDEX(i) : i;
	if(MAX_HEAP_R_INDEX(i) < heap->count && heap->cmp_func(GET_MAX_HEAP_ENTRY(heap, heap->values_ptrs[MAX_HEAP_R_INDEX(i)]),GET_MAX_HEAP_ENTRY(heap, heap->values_ptrs[largest])) > 0)
		largest = MAX_HEAP_R_INDEX(i);
	if(largest != i){
		automaton_max_heap_swap(heap, i, largest);
		automaton_max_heap_heapify(heap, largest);
	}
}

/*
    Function to insert a node into the max heap, by allocating space for that node in the
    heap and also making sure that the heap property and shape propety are never violated.
*/
/*

void* automaton_max_heap_add_entry(automaton_max_heap* heap, void* entry) {
	//check if heap needs to be resized
	uint32_t i;
	if(heap->count == heap->size){
		uint32_t new_size	= heap->size * LIST_INCREASE_FACTOR;
		void* new_values	= malloc(heap->sizeof_element * new_size);
		for(i = 0; i < heap->count; i++)
			memcpy(GET_MAX_HEAP_ARRAY_ENTRY(heap, new_values, (uint32_t)i), GET_MAX_HEAP_ENTRY(heap, (uint32_t)i), heap->sizeof_element);
		free(heap->values);
		heap->values		= new_values;
		heap->size			= new_size;
	}
	//keep heap properties invariant
	i = heap->count++;
    while(i && heap->cmp_func(entry, GET_MAX_HEAP_PARENT(heap, i)) > 0) {
    	automaton_max_heap_swap(heap, GET_MAX_HEAP_ENTRY(heap, i), GET_MAX_HEAP_PARENT(heap, i));
        i = MAX_HEAP_PARENT_INDEX(i) ;
    }
	return memcpy(GET_MAX_HEAP_ENTRY(heap, i), entry, heap->sizeof_element);
}

 * */
void* automaton_max_heap_add_entry(automaton_max_heap* heap, void* entry) {
	//check if heap needs to be resized
	uint32_t i;
	if(heap->count == heap->size){
		uint32_t new_size	= heap->size * LIST_INCREASE_FACTOR;
		void* new_values	= realloc(heap->values, heap->sizeof_element * new_size);
		if(new_values == NULL){printf("Could not allocate memory[automaton_max_heap_add_entry:1]"); exit(-1);}
		heap->values	= new_values;
		uint32_t *new_ptrs	= realloc(heap->values_ptrs, sizeof(uint32_t) * new_size);
		if(new_ptrs == NULL){printf("Could not allocate memory[automaton_max_heap_add_entry:2]"); exit(-1);}
		heap->values_ptrs	= new_ptrs;
		for(i = heap->size; i < new_size; i++)heap->values_ptrs[i]	= i;
		heap->size			= new_size;
	}
	//keep heap properties invariant
	void *ret_value = memcpy(GET_MAX_HEAP_ENTRY(heap, heap->values_ptrs[heap->count]), entry, heap->sizeof_element);
	i = heap->count++;
    while(i && heap->cmp_func(entry, GET_MAX_HEAP_ENTRY(heap, heap->values_ptrs[MAX_HEAP_PARENT_INDEX(i)])) > 0) {
    	automaton_max_heap_swap(heap, i, MAX_HEAP_PARENT_INDEX(i));
        i = MAX_HEAP_PARENT_INDEX(i) ;
    }
	return ret_value;
}

void automaton_max_heap_pop_entry(automaton_max_heap* heap, void* target) {
    if(heap->count > 0) {
    	memcpy(target, GET_MAX_HEAP_ENTRY(heap, heap->values_ptrs[0]), heap->sizeof_element);
        automaton_max_heap_swap(heap, 0, --(heap->count));
        automaton_max_heap_heapify(heap, 0) ;
    } else {
    	exit(-1);
        return;
    }
}

void automaton_max_heap_destroy(automaton_max_heap* heap) {
	free(heap->values);
	free(heap->values_ptrs);
    free(heap);
}

/** CONCRETE PENDING STATE IMPL **/
int32_t automaton_pending_state_compare(void* left_pending_state, void* right_pending_state){
	automaton_pending_state* left	= (automaton_pending_state*)left_pending_state;
	automaton_pending_state* right= (automaton_pending_state*)right_pending_state;

	if(left->assumption_to_satisfy > right->assumption_to_satisfy){
		return 1;
	}else if(left->value > right->value){
		return 1;
	}else if(left->value < right->value){
		return -1;
	}else if(left->timestamp > right->timestamp){
		return 1;
	}else if(left->timestamp < right->timestamp){
		return -1;
	}else{
		return 0;
	}
}

automaton_pending_state_max_heap* automaton_pending_state_max_heap_create(){
	automaton_pending_state_max_heap* max_heap	= malloc(sizeof(automaton_pending_state_max_heap));
	max_heap->count					= 0;
	max_heap->size					= INITIAL_HEAP_SIZE;
	max_heap->values				= malloc(sizeof(automaton_pending_state) * max_heap->size);
	max_heap->values_ptrs			= malloc(max_heap->size * sizeof(uint32_t));
	uint32_t i;
	for(i = 0; i < max_heap->size; i++)max_heap->values_ptrs[i]	= i;
	return max_heap;
}



void automaton_pending_state_max_heap_heapify(automaton_pending_state_max_heap* heap, uint32_t i) {
	uint32_t largest = (MAX_HEAP_L_INDEX(i) < heap->count &&
			automaton_pending_state_compare(&(heap->values[heap->values_ptrs[MAX_HEAP_L_INDEX(i)]]),
					&(heap->values[heap->values_ptrs[i]])) > 0) ? MAX_HEAP_L_INDEX(i) : i;
	if(MAX_HEAP_R_INDEX(i) < heap->count && automaton_pending_state_compare(
			&(heap->values[heap->values_ptrs[MAX_HEAP_R_INDEX(i)]]),&(heap->values[heap->values_ptrs[largest]])) > 0)
		largest = MAX_HEAP_R_INDEX(i);
	if(largest != i){
		uint32_t tmp_value	= heap->values_ptrs[i];
		heap->values_ptrs[i]	= heap->values_ptrs[largest];
		heap->values_ptrs[largest]	= tmp_value;
		automaton_pending_state_max_heap_heapify(heap, largest);
	}
}


automaton_pending_state* automaton_pending_state_max_heap_add_entry(automaton_pending_state_max_heap* heap, automaton_pending_state* entry) {
	//check if heap needs to be resized
	uint32_t i;
	if(heap->count == heap->size){
		uint32_t new_size	= heap->size * LIST_INCREASE_FACTOR;
		void* new_values	= realloc(heap->values, sizeof(automaton_pending_state) * new_size);
		if(new_values == NULL){printf("Could not allocate memory[automaton_pending_state_max_heap_add_entry:1]"); exit(-1);}
		heap->values	= new_values;
		uint32_t *new_ptrs	= realloc(heap->values_ptrs, sizeof(uint32_t) * new_size);
		if(new_ptrs == NULL){printf("Could not allocate memory[automaton_pending_state_max_heap_add_entry:2]"); exit(-1);}
		heap->values_ptrs	= new_ptrs;
		for(i = heap->size; i < new_size; i++)heap->values_ptrs[i]	= i;
		heap->size			= new_size;
	}
	//keep heap properties invariant
	automaton_pending_state *ret_value = &(heap->values[heap->values_ptrs[heap->count]]);
	ret_value->assumption_to_satisfy	= entry->assumption_to_satisfy;
	ret_value->goal_to_satisfy			= entry->goal_to_satisfy;
	ret_value->state					= entry->state;
	ret_value->timestamp				= entry->timestamp;
	ret_value->value					= entry->value;
	i = heap->count++;
    while(i && automaton_pending_state_compare(entry, &(heap->values[heap->values_ptrs[MAX_HEAP_PARENT_INDEX(i)]])) > 0) {
		uint32_t tmp_value	= heap->values_ptrs[i];
		heap->values_ptrs[i]	= heap->values_ptrs[MAX_HEAP_PARENT_INDEX(i)];
		heap->values_ptrs[MAX_HEAP_PARENT_INDEX(i)]	= tmp_value;
        i = MAX_HEAP_PARENT_INDEX(i) ;
    }
	return ret_value;
}

void automaton_pending_state_max_heap_pop_entry(automaton_pending_state_max_heap* heap, automaton_pending_state *target) {
    if(heap->count > 0) {
    	automaton_pending_state *ret_value = &(heap->values[heap->values_ptrs[0]]);
    	target->assumption_to_satisfy	= ret_value->assumption_to_satisfy;
    	target->goal_to_satisfy			= ret_value->goal_to_satisfy;
    	target->state					= ret_value->state;
    	target->timestamp				= ret_value->timestamp;
    	target->value					= ret_value->value;
    	--(heap->count);
		uint32_t tmp_value	= heap->values_ptrs[0];
		heap->values_ptrs[0]	= heap->values_ptrs[heap->count];
		heap->values_ptrs[heap->count]	= tmp_value;
		automaton_pending_state_max_heap_heapify(heap, 0) ;
    } else {
    	exit(-1);
        return;
    }
}

void automaton_pending_state_max_heap_destroy(automaton_pending_state_max_heap* heap) {
	free(heap->values);
	free(heap->values_ptrs);
    free(heap);
}
