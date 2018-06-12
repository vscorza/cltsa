/*
 * bucket_list.c
 *
 *  Created on: May 23, 2018
 *      Author: mariano
 */
#include "bucket_list.h"
#include "automaton.h"
/*************************
 * AUTOMATON_BUCKET_LIST
 *************************/

automaton_bucket_list* automaton_bucket_list_create(uint32_t count){
	automaton_bucket_list* bucket	= malloc(sizeof(automaton_bucket_list));
	uint32_t i;
	bucket->count					= count;
	bucket->bucket_count			= malloc(sizeof(uint32_t) * count);
	bucket->bucket_size				= malloc(sizeof(uint32_t) * count);
	bucket->buckets					= malloc(sizeof(uint32_t*) * count);
	for(i = 0; i < count; i++){
		bucket->bucket_count[i]		= 0;
		bucket->bucket_size[i]		= 0;
		bucket->buckets[i]			= NULL;
	}
	return bucket;
}
void automaton_ptr_bucket_list_merge(automaton_ptr_bucket_list* list, automaton_ptr_bucket_list* new_elements
		, automaton_ptr_bucket_list_key_extractor extractor){
	uint32_t i, j;
	for(i = 0; i < new_elements->count; i++){
		for(j = 0; j < new_elements->bucket_count[i]; j++){
			automaton_ptr_bucket_add_entry(list, new_elements->buckets[i][j], extractor(new_elements->buckets[i][j]));
		}
	}
}

bool automaton_bucket_has_entry(automaton_bucket_list* list, uint32_t entry){
	uint32_t index		= entry % list->count;
	uint32_t* bucket	= list->buckets[index];
	bool found			= false;
	uint32_t i;
	for(i = 0; i < list->bucket_count[index]; i++){
		if(bucket[i] == entry)
			return true;
	}
	return false;
}
bool automaton_bucket_add_entry(automaton_bucket_list* list, uint32_t entry){
	if(automaton_bucket_has_entry(list, entry))
		return true;
	uint32_t index		= entry % list->count;
	uint32_t* bucket	= list->buckets[index];
	uint32_t i;

	if(bucket == NULL){
		list->bucket_size[index]	= 2;
		list->buckets[index]		= malloc(sizeof(uint32_t) * list->bucket_size[index]);
		bucket						= list->buckets[index];
	}

	if(list->bucket_count[index] >= (list->bucket_size[index] - 1)){
		uint32_t new_size	= list->bucket_size[index] * LIST_INCREASE_FACTOR;
		uint32_t* new_bucket= malloc(sizeof(uint32_t) * new_size);
		for(i = 0; i < list->bucket_count[index]; i++){
			new_bucket[i]	= bucket[i];
		}
		list->bucket_size[index]	= new_size;
		free(bucket);
		list->buckets[index]		= new_bucket;
		bucket						= new_bucket;
	}

	bucket[(list->bucket_count[index])++]	= entry;
	return false;
}
bool automaton_bucket_remove_entry(automaton_bucket_list* list, uint32_t entry){
	if(!automaton_bucket_has_entry(list, entry))
		return false;
	uint32_t index		= entry % list->count;
	uint32_t* bucket	= list->buckets[index];
	uint32_t i;
	bool found	= false;
	for(i = 0; i < list->bucket_count[index]; i++){
		if(bucket[i] == entry){
			found = true;
		}
		if(found){
			bucket[i]	= bucket[i + 1];
		}
	}
	list->bucket_count[index]--;
	return true;
}

void automaton_bucket_destroy(automaton_bucket_list* list){
	uint32_t i;
	for(i = 0; i < list->count; i++){
		free(list->buckets[i]);
	}
	free(list->buckets);
	free(list->bucket_count);
	free(list->bucket_size);
	free(list);
}

/*************************
 * AUTOMATON_PTR_BUCKET_LIST
 *************************/

automaton_ptr_bucket_list* automaton_ptr_bucket_list_create(uint32_t count){
	automaton_ptr_bucket_list* bucket	= malloc(sizeof(automaton_ptr_bucket_list));
	uint32_t i;
	bucket->count					= count;
	bucket->bucket_count			= malloc(sizeof(uint32_t) * count);
	bucket->bucket_size				= malloc(sizeof(uint32_t) * count);
	bucket->buckets					= malloc(sizeof(void**) * count);
	for(i = 0; i < count; i++){
		bucket->bucket_count[i]		= 0;
		bucket->bucket_size[i]		= 0;
		bucket->buckets[i]			= NULL;
	}
	return bucket;
}
bool automaton_ptr_bucket_has_key(automaton_ptr_bucket_list* list, uint32_t key, automaton_ptr_bucket_list_key_extractor extractor){
	uint32_t index		= key % list->count;
	void** bucket	= list->buckets[index];
	bool found			= false;
	uint32_t i;
	for(i = 0; i < list->bucket_count[index]; i++){
		if(extractor(bucket[i]) == key)
			return true;
	}
	return false;
}
bool automaton_ptr_bucket_has_entry(automaton_ptr_bucket_list* list, void* entry, uint32_t key){
	uint32_t index		= key % list->count;
	void** bucket	= list->buckets[index];
	bool found			= false;
	uint32_t i;
	for(i = 0; i < list->bucket_count[index]; i++){
		if(bucket[i] == entry)
			return true;
	}
	return false;
}
bool automaton_ptr_bucket_add_entry(automaton_ptr_bucket_list* list, void* entry, uint32_t key){
	if(automaton_ptr_bucket_has_entry(list, entry, key))
		return true;
	uint32_t index		= key % list->count;
	void** bucket	= list->buckets[index];
	uint32_t i;

	if(bucket == NULL){
		list->bucket_size[index]	= 2;
		list->buckets[index]		= malloc(sizeof(void*) * list->bucket_size[index]);
		bucket						= list->buckets[index];
	}

	if(list->bucket_count[index] >= (list->bucket_size[index] - 1)){
		uint32_t new_size	= list->bucket_size[index] * LIST_INCREASE_FACTOR;
		void** new_bucket= malloc(sizeof(void*) * new_size);
		for(i = 0; i < list->bucket_count[index]; i++){
			new_bucket[i]	= bucket[i];
		}
		list->bucket_size[index]	= new_size;
		free(bucket);
		list->buckets[index]		= new_bucket;
		bucket						= new_bucket;
	}

	bucket[(list->bucket_count[index])++]	= entry;
	return false;
}
bool automaton_ptr_bucket_remove_entry(automaton_ptr_bucket_list* list, void* entry, uint32_t key){
	if(!automaton_ptr_bucket_has_entry(list, entry, key))
		return false;
	uint32_t index		= key % list->count;
	void** bucket	= list->buckets[index];
	uint32_t i;
	bool found	= false;
	for(i = 0; i < list->bucket_count[index]; i++){
		if(bucket[i] == entry){
			found = true;
		}
		if(found){
			bucket[i]	= bucket[i + 1];
		}
	}
	list->bucket_count[index]--;
	return true;
}

void automaton_ptr_bucket_reset(automaton_ptr_bucket_list* list){
	uint32_t i;
	for(i = 0; i < list->count; i++){
		list->bucket_size[i]	= 0;
	}
}

void automaton_ptr_bucket_destroy(automaton_ptr_bucket_list* list){
	uint32_t i;
	for(i = 0; i < list->count; i++){
		free(list->buckets[i]);
	}
	free(list->buckets);
	free(list->bucket_count);
	free(list->bucket_size);
	free(list);
}
