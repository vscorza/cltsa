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
	for(i = 0; i < *count; i++) new_list[i]	= (*list)[i];
	new_list[*count]	= element;
	if(*count > 0)
		free(*list);
	*count	= *count + 1;
	*list	= new_list;
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
		*list	= new_list;
	}
}
void aut_free_ptr_list(void*** list, uint32_t* count){
	uint32_t i;
	for(i = 0; i < *count; i++){
		if((*list)[i] != NULL){free((*list)[i]); (*list)[i] = NULL;}
	}
	*count = 0;
	if(*list != NULL)
		free(*list);
	list = NULL;
}
void aut_dupstr(char** dst, char* src){
	if(src != NULL){
		*dst = malloc(strlen(src) + 1);
		strcpy(*dst, src);
	}else{*dst = NULL;}
}

void aut_merge_string_lists(char*** a, int32_t* a_count, char** b, int32_t b_count, bool ordered, bool repeat_values){
	if(*a_count == 0 && b_count == 0) return;
	//a = a+b
	int32_t diff_count	= 0;
	int32_t i, j, a_index, b_index, a_b_cmp;
	bool diff	= true;
	if(!repeat_values){
		for(i = 0; i < b_count; i++){
			diff	= true;
			for(j = 0; j < *a_count; j++){
				if(strcmp((*a)[j], b[i]) == 0)
					diff= false;
			}
			if(diff)diff_count++;
		}
	}else{
		diff_count		= b_count;
	}
	int32_t new_count	= *a_count + diff_count;
	char** new_list		= malloc(sizeof(char*) * new_count);
	a_index	= b_index	= 0;
	if(*a_count == 0){
		for(i = 0; i < new_count; i++){
			aut_dupstr(&(new_list[i]), b[b_index++]);
		}
	}else{
		if(ordered){
			for(i = 0; i < new_count; i++){
				if(b_index >= b_count){
					aut_dupstr(&(new_list[i]), (*a)[a_index++]);
				}else if(a_index >= *a_count){
					aut_dupstr(&(new_list[i]), b[b_index++]);
				}else{
					a_b_cmp		= strcmp((*a)[a_index], b[b_index]);
					if(a_b_cmp < 0){
						aut_dupstr(&(new_list[i]), (*a)[a_index++]);
					}else if(a_b_cmp == 0){
						aut_dupstr(&(new_list[i]), (*a)[a_index++]);
						if(!repeat_values){
							b_index++;
						}
					}else{
						aut_dupstr(&(new_list[i]), b[b_index++]);
					}
				}
			}
		}else{
			a_index	= 0;
			for(i = 0; i < *a_count; i++){
				for(j = 0; j < b_count; j++){
					a_b_cmp		= strcmp((*a)[i], b[j]);
					if(a_b_cmp < 0){
						aut_dupstr(&(new_list[a_index++]), (*a)[i]);
					}else if(a_b_cmp == 0){
						aut_dupstr(&(new_list[a_index++]), (*a)[i]);
						if(repeat_values){		
							aut_dupstr(&(new_list[a_index++]), (*a)[i]);
						}
					}else{
						aut_dupstr(&(new_list[a_index++]), b[j]);
					}
				}
			}
		}
	}
	for(i = 0; i < *a_count; i++) free((*a)[i]);
	if(*a != NULL)
		free(*a);
	*a					= new_list;
	*a_count			= new_count;
}

bool aut_push_string_to_list(char*** list, int32_t* list_count, char* element, int32_t* position, bool ordered, bool repeat_values){
	int32_t i;
	int32_t a_b_cmp;
	*position	= -1;
	for(i = 0; i < *list_count; i++){
		a_b_cmp	= strcmp((*list)[i], element);
		if(a_b_cmp == 0){
			*position = i;
			if(!repeat_values){
				return false;
			}
		}else if(a_b_cmp > 0){
			*position		= i;
			break;
		}
	}
	if(!ordered)*position = -1;

	if(*position == -1){
		*position	= *list_count;
	}
	int32_t new_count	= *list_count + 1;
	char** new_list		= malloc(sizeof(char*) * new_count);
	for(i = 0; i < *list_count; i++){
		if(i < *position){
			aut_dupstr(&(new_list[i]), (*list)[i]);
		}else{
			aut_dupstr(&(new_list[i+1]), (*list)[i]);
		}
	}
	aut_dupstr(&(new_list[*position]), element);
	for(i = 0; i < *list_count; i++) free((*list)[i]);
	if(*list != NULL)
		free(*list);
	*list					= new_list;
	*list_count			= new_count;
	return true;
}


int32_t aut_string_list_index_of(char** list, int32_t list_count, char* element){
	int32_t i;
	for(i = 0; i < list_count; i++)
		if(strcmp(list[i], element) == 0) return i;
	return -1;
}
