/*
 * automaton_utils.c
 *
 *  Created on: Apr 20, 2018
 *      Author: mariano
 */
#include "automaton_utils.h"
/** UTILS FUNCTIONS **/
void aut_add_ptr_list(void*** list, void* element, uint32_t* count){
	void** new_list	= malloc(sizeof(void*) * (*count+1));
	uint32_t i;
	for(i = 0; i < *count; i++) new_list[i]	= *list[i];
	new_list[*count]	= element;
	if(*count > 0)
		free(*list);
	*count	= *count + 1;
	list	= &new_list;
}
void aut_add_incr_ptr_list(void*** list, void* element, uint32_t* size, uint32_t* count){
	if(*count < *size){
		*list[*count]	= element;
		*count = *count + 1;
	}else{
		uint32_t new_size	= *size*LIST_INCREASE_FACTOR;
		void** new_list	= malloc(sizeof(void*) * (new_size));
		uint32_t i;
		for(i = 0; i < *count; i++) new_list[i]	= *list[i];
		new_list[*count]	= element;
		*count	= *count + 1;
		*size	= new_size;
		free(*list);
		list	= &new_list;
	}
}
void aut_free_ptr_list(void*** list, uint32_t* count){
	uint32_t i;
	for(i = 0; i < *count; i++){
		if(*list[i] != NULL){free(*list[i]); *list[i] = NULL;}
	}
	*count = 0;
	free(*list);list = NULL;
}
void aut_dupstr(char** dst, char* src){
	if(src != NULL){
		*dst = malloc(strlen(src) + 1);
		strcpy(*dst, src);
	}else{*dst = NULL;}
}


