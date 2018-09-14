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
*/

automaton_max_heap* automaton_max_heap_create(uint32_t sizeof_element, automaton_max_heap_compare_func compare_func){
	automaton_max_heap* max_heap	= malloc(sizeof(automaton_max_heap));
	max_heap->cmp_func				= compare_func;
	max_heap->sizeof_element		= sizeof_element;
	max_heap->count					= 0;
	max_heap->size					= INITIAL_HEAP_SIZE;
	max_heap->values				= malloc(max_heap->sizeof_element * max_heap->size);
	max_heap->tmp_value				= malloc(max_heap->sizeof_element);
	return max_heap;
}

void automaton_max_heap_swap(automaton_max_heap* heap, void* n1, void* n2) {
	if(n1 == n2)return;
	memcpy(heap->tmp_value, n1, heap->sizeof_element);
	memcpy(n1, n2, heap->sizeof_element);
	memcpy(n2, heap->tmp_value, heap->sizeof_element);
}


/*
    Heapify function is used to make sure that the heap property is never violated
    In case of deletion of a node, or creating a max heap from an array, heap property
    may be violated. In such cases, heapify function can be called to make sure that
    heap property is never violated
*/
void automaton_max_heap_heapify(automaton_max_heap* heap, uint32_t i) {
	uint32_t largest = (MAX_HEAP_L_INDEX(i) < heap->count && heap->cmp_func(GET_MAX_HEAP_L_CHILD(heap, i),GET_MAX_HEAP_ENTRY(heap, i)) > 0) ? MAX_HEAP_L_INDEX(i) : i;
	if(MAX_HEAP_R_INDEX(i) < heap->count && heap->cmp_func(GET_MAX_HEAP_R_CHILD(heap, i),GET_MAX_HEAP_ENTRY(heap, largest)) > 0)
		largest = MAX_HEAP_R_INDEX(i);
	if(largest != i){
		automaton_max_heap_swap(heap, GET_MAX_HEAP_ENTRY(heap, i), GET_MAX_HEAP_ENTRY(heap, largest));
		automaton_max_heap_heapify(heap, largest);
	}
}

/*
    Build a Max Heap given an array of numbers
    Instead of using insertNode() function n times for total complexity of O(nlogn),
    we can use the buildMaxHeap() function to build the heap in O(n) time
*/
automaton_max_heap* automaton_max_heap_create_from_array(uint32_t sizeof_element, automaton_max_heap_compare_func compare_func,void *arr, uint32_t count) {
	automaton_max_heap* max_heap	= malloc(sizeof(automaton_max_heap));
	max_heap->cmp_func				= compare_func;
	max_heap->sizeof_element		= sizeof_element;
	max_heap->count					= count;
	max_heap->size					= count;
	max_heap->values				= malloc(max_heap->sizeof_element * max_heap->size);
	max_heap->tmp_value				= malloc(max_heap->sizeof_element);
	int32_t i;

    // Insertion into the heap without violating the shape property
    for(i = 0; i < (int32_t)count; i++)
        memcpy(GET_MAX_HEAP_ENTRY(max_heap, (uint32_t)i), GET_MAX_HEAP_ARRAY_ENTRY(max_heap, arr, (uint32_t)i), max_heap->sizeof_element);
    // Making sure that heap property is also satisfied
    for(i = (int32_t)(count - 1) / 2; i >= 0; i--)
    	automaton_max_heap_heapify(max_heap, (uint32_t)i) ;
    return max_heap;
}

/*
    Function to insert a node into the max heap, by allocating space for that node in the
    heap and also making sure that the heap property and shape propety are never violated.
*/
void automaton_max_heap_add_entry(automaton_max_heap* heap, void* entry) {
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
	memcpy(GET_MAX_HEAP_ENTRY(heap, i), entry, heap->sizeof_element);
}

/*
    Function to delete a node from the max heap
    It shall remove the root node, and place the last node in its place
    and then call heapify function to make sure that the heap property
    is never violated
*/
void automaton_max_heap_pop_entry(automaton_max_heap* heap, void* target) {
    if(heap->count > 0) {
    	memcpy(target, GET_MAX_HEAP_ENTRY(heap, 0), heap->sizeof_element);
        automaton_max_heap_swap(heap, GET_MAX_HEAP_ENTRY(heap, 0), GET_MAX_HEAP_ENTRY(heap, --(heap->count)));
        automaton_max_heap_heapify(heap, 0) ;
    } else {
        return;
    }
}

/*
    Function to get minimum node from a max heap
    The minimum node shall always be one of the leaf nodes. So we shall recursively
    move through both left and right child, until we find their minimum nodes, and
    compare which is smaller. It shall be done recursively until we get the minimum
    node
*/
uint32_t automaton_max_heap_get_min_entry_index(automaton_max_heap* heap, void* target, uint32_t i) {
    if(MAX_HEAP_L_INDEX(i) >= heap->count)
    	memcpy(target, GET_MAX_HEAP_ENTRY(heap, i), heap->sizeof_element);

    uint32_t l = automaton_max_heap_get_min_entry_index(heap, target, MAX_HEAP_L_INDEX(i));
    uint32_t r = automaton_max_heap_get_min_entry_index(heap, target, MAX_HEAP_R_INDEX(i));
    if(l <= r) return l;
    else return r;
}
void automaton_max_heap_destroy(automaton_max_heap* heap) {
	free(heap->values);
	free(heap->tmp_value);
    free(heap);
}
