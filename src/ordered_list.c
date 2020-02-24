/*
 * ordered_list.c
 *
 *  Created on: Jul 30, 2018
 *      Author: mariano
 */
#include "ordered_list.h"
#include "automaton.h"
/*************************
 * AUTOMATON_ORDERED_LIST
 *************************/

automaton_ordered_list* automaton_ordered_list_create(uint32_t temporary_size, automaton_ordered_list_key_extractor_func extractor_func, uint32_t sizeof_element){
	automaton_ordered_list* list	= malloc(sizeof(automaton_ordered_list));
	automaton_ordered_list_initialize(list, temporary_size, extractor_func, sizeof_element);
	return list;
}

void automaton_ordered_list_initialize(automaton_ordered_list* list, uint32_t temporary_size
		, automaton_ordered_list_key_extractor_func extractor_func, uint32_t sizeof_element){
	uint32_t i;
	list->sizeof_element		= sizeof_element;
	list->extractor_func		= extractor_func;
	list->count					= 0;
	list->size					= 0;
	list->values				= NULL;
	list->temporary_size		= temporary_size;
	list->temporary_count		= 0;
	list->temporary				= malloc(list->sizeof_element * list->temporary_size);
}

automaton_ordered_list* automaton_ordered_list_clone(automaton_ordered_list* source){
	automaton_ordered_list* target	= malloc(sizeof(automaton_ordered_list));
	uint32_t i;
	target->count					= source->count;
	target->extractor_func			= source->extractor_func;
	target->sizeof_element			= source->sizeof_element;
	target->size					= source->size;
	target->count					= source->count;
	target->values					= malloc(target->sizeof_element * target->size);
	for(i = 0; i < target->count; i++)
		memcpy(GET_ORDERED_LIST_SINGLE_ENTRY(target, target->values, i), GET_ORDERED_LIST_SINGLE_ENTRY(source, source->values, i), target->sizeof_element);
	target->temporary_count			= source->temporary_count;
	target->temporary_size			= source->temporary_size;
	target->temporary				= malloc(target->sizeof_element * target->temporary_size);
	for(i = 0; i < target->temporary_count; i++)
		memcpy(GET_ORDERED_LIST_SINGLE_ENTRY(target, target->temporary, i), GET_ORDERED_LIST_SINGLE_ENTRY(source, source->temporary, i), target->sizeof_element);
	return target;
}

void automaton_ordered_list_copy(automaton_ordered_list* target, automaton_ordered_list* source){
	if(target->count != source->count || target->sizeof_element != source->sizeof_element || target->extractor_func != source->extractor_func
			|| target->temporary_size != source->temporary_size){
		automaton_ordered_list_destroy(target);
		automaton_ordered_list_initialize(target, source->temporary_size, source->extractor_func, source->sizeof_element);
	}
	uint32_t i;
	if(target->size < source->size){
		free(target->values);
		target->values	= malloc(source->sizeof_element * source->size);
		target->size	= source->size;
	}
	for(i = 0; i < target->count; i++)
		memcpy(GET_ORDERED_LIST_SINGLE_ENTRY(target, target->values, i), GET_ORDERED_LIST_SINGLE_ENTRY(source, source->values, i), target->sizeof_element);
	target->count		= source->count;
	target->temporary_count			= source->temporary_count;
	for(i = 0; i < target->temporary_count; i++)
		memcpy(GET_ORDERED_LIST_SINGLE_ENTRY(target, target->temporary, i), GET_ORDERED_LIST_SINGLE_ENTRY(source, source->temporary, i), target->sizeof_element);
}

int32_t automaton_ordered_list_binary_search(automaton_ordered_list* list, void* values, uint32_t low, uint32_t high, uint32_t key){
	if (high >= low){
		uint32_t mid		= low + (high - low)/2;
		uint32_t value	= list->extractor_func(GET_ORDERED_LIST_SINGLE_ENTRY(list, values, mid));
		if (value == key)
			return mid;
		if(low == high || low == mid || mid == high)
			return -1;
		if (value > key)
			return automaton_ordered_list_binary_search(list, values, low, mid-1, key);
		return automaton_ordered_list_binary_search(list, values, mid+1, high, key);
	}
	return -1;
}

bool automaton_ordered_list_has_key(automaton_ordered_list* list, uint32_t key){
	bool found = false;
	if(automaton_ordered_list_binary_search(list, list->temporary, 0, list->temporary_count -1, key) != -1)
		return true;
	return automaton_ordered_list_binary_search(list, list->values, 0, list->count -1, key) != -1;
}

bool automaton_ordered_list_has_entry(automaton_ordered_list* list, void* entry){
	return (list->count + list->temporary_count) > 0 && automaton_ordered_list_get_entry(list, list->extractor_func(entry)) != NULL;
}

void* automaton_ordered_list_get_entry(automaton_ordered_list* list, uint32_t key){
	int32_t index	= -1;
	if(list->temporary_count > 0){
		index	=  automaton_ordered_list_binary_search(list, list->temporary, 0, list->temporary_count -1, key);
		if(index != -1){
			return GET_ORDERED_LIST_SINGLE_ENTRY(list, list->temporary, index);
		}
	}
	if(list->count > 0){
		index	=  automaton_ordered_list_binary_search(list, list->values, 0, list->count -1, key);
		if(index != -1){
			return GET_ORDERED_LIST_SINGLE_ENTRY(list, list->values, index);
		}
	}
	return NULL;
}

void automaton_ordered_list_pop_entry(automaton_ordered_list* list, void* target){
	if(list->temporary_count > 0){
		if((list->count == 0) || (list->extractor_func(GET_ORDERED_LIST_SINGLE_ENTRY(list, list->temporary, (list->temporary_count - 1)))
				> list->extractor_func(GET_ORDERED_LIST_SINGLE_ENTRY(list, list->values, (list->count - 1)))))
				memcpy(target, GET_ORDERED_LIST_SINGLE_ENTRY(list, list->temporary, --(list->temporary_count)), list->sizeof_element);
		else if(list->count > 0)
			memcpy(target, GET_ORDERED_LIST_SINGLE_ENTRY(list, list->values, --(list->count)), list->sizeof_element);
		else{
			printf("FATAL ERROR TRYING TO POP FROM EMPTY LIST");
			exit(-1);
		}
	}else if(list->count > 0)
		memcpy(target, GET_ORDERED_LIST_SINGLE_ENTRY(list, list->values, --(list->count)), list->sizeof_element);
	else{
		printf("FATAL ERROR TRYING TO POP FROM EMPTY LIST");
		exit(-1);
	}
}

bool automaton_ordered_list_add_entry(automaton_ordered_list* list, void* entry){
	if(automaton_ordered_list_has_entry(list, entry))
		return true;
	int32_t i;
	//if it can not be added to the temporary list then a merge should be triggered
	if(list->temporary_count == list->temporary_size){
		void* new_values;
		uint32_t new_size;
		//check if general list needs initialization
		if(list->values == NULL){
			new_size	= list->temporary_size;
		}else{
			new_size	= list->count * LIST_INCREASE_FACTOR;
			if(new_size <= list->count)
					new_size	= list->count * 2;
		}
		new_values		= malloc(list->sizeof_element * new_size);

		if(list->values == NULL){//if values list is not initialized copy directly
#if DEBUG_ORDERED_LIST
			printf("INITIALIAZING ORDERED LIST(%d)\n", list->temporary_count);
#endif
			for(i = 0; i < list->temporary_count; i++){
				memcpy(GET_ORDERED_LIST_SINGLE_ENTRY(list, new_values, (uint32_t)i), GET_ORDERED_LIST_SINGLE_ENTRY(list, list->temporary, (uint32_t)i), list->sizeof_element);
#if DEBUG_ORDERED_LIST
			printf("%d,", list->extractor_func(GET_ORDERED_LIST_SINGLE_ENTRY(list, new_values, (uint32_t)i)));
#endif
			}
#if DEBUG_ORDERED_LIST
			printf("\n");
#endif
		}else{//else copy in order from the correct source (values or temporary)
#if DEBUG_ORDERED_LIST
			printf("COPYING FROM TEMPORARY LIST\n");
			for(i = 0; i < list->temporary_count; i++){
				printf("%d,",list->extractor_func(GET_ORDERED_LIST_SINGLE_ENTRY(list, list->temporary, i)));
			}
			printf("\n");
#endif
			uint32_t values_index = 0, temporary_index = 0;
			for(i = 0; i < (int32_t)(list->temporary_count + list->count); i++){
#if DEBUG_ORDERED_LIST
				printf("Comparing: %d > %d\n", list->extractor_func(GET_ORDERED_LIST_SINGLE_ENTRY(list, list->temporary, temporary_index)),
								list->extractor_func(GET_ORDERED_LIST_SINGLE_ENTRY(list, list->values,values_index)));
#endif
				if(temporary_index >= list->temporary_count ||
					(values_index < (list->count) && list->extractor_func(GET_ORDERED_LIST_SINGLE_ENTRY(list, list->temporary, temporary_index)) >
					list->extractor_func(GET_ORDERED_LIST_SINGLE_ENTRY(list, list->values,values_index)))){
					memcpy(GET_ORDERED_LIST_SINGLE_ENTRY(list, new_values, (uint32_t)i), GET_ORDERED_LIST_SINGLE_ENTRY(list, list->values, values_index++), list->sizeof_element);
#if DEBUG_ORDERED_LIST
					printf("%d(%d),", list->extractor_func(GET_ORDERED_LIST_SINGLE_ENTRY(list, new_values, (uint32_t)i)), values_index - 1);
#endif
				}else{
					memcpy(GET_ORDERED_LIST_SINGLE_ENTRY(list, new_values, (uint32_t)i), GET_ORDERED_LIST_SINGLE_ENTRY(list, list->temporary, temporary_index++), list->sizeof_element);
#if DEBUG_ORDERED_LIST
					printf("%d*(%d),", list->extractor_func(GET_ORDERED_LIST_SINGLE_ENTRY(list, new_values, (uint32_t)i)), temporary_index - 1);
#endif
				}

			}
#if DEBUG_ORDERED_LIST
			printf("\n");
#endif
		}
		if(list->values != NULL)
			free(list->values);
		list->values	= new_values;
		list->count		= list->count + list->temporary_count;
		list->size		= new_size;
		list->temporary_count	= 0;
	}
	//add in the proper order within the temporary list
	bool found	= false;
	if(list->temporary_count > 0){
		for(i = (int32_t)list->temporary_count - 1; i >= 0;  i--){
			if(list->extractor_func(GET_ORDERED_LIST_SINGLE_ENTRY(list, list->temporary, (uint32_t)i)) >
			list->extractor_func(entry)){
				memcpy(GET_ORDERED_LIST_SINGLE_ENTRY(list, list->temporary, (uint32_t)(i+1)),GET_ORDERED_LIST_SINGLE_ENTRY(list, list->temporary, (uint32_t)i), list->sizeof_element);
			}else{
				found = true;
				memcpy(GET_ORDERED_LIST_SINGLE_ENTRY(list, list->temporary, (uint32_t)(i+1)),entry, list->sizeof_element);
				break;
			}
		}
	}
	//if it was not found then add it at the beginning of the list
	if(!found){
		memcpy(GET_ORDERED_LIST_SINGLE_ENTRY(list, list->temporary, 0),entry, list->sizeof_element);
	}
	list->temporary_count++;
#if DEBUG_ORDERED_LIST
	printf("AFTER PUSH(%d)\n", list->temporary_count);
	for(i = 0; i < (int32_t)list->temporary_count; i++){
		printf("%d,",list->extractor_func(GET_ORDERED_LIST_SINGLE_ENTRY(list, list->temporary, i)));
	}
	printf("\n");
#endif
	return true;
}


void automaton_ordered_list_reset(automaton_ordered_list* list){
	list->count				= 0;
	list->temporary_count	= 0;
}

void automaton_ordered_list_destroy(automaton_ordered_list* list){
	free(list->temporary);
	if(list->values != NULL)
		free(list->values);
	free(list);
}
