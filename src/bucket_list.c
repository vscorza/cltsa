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
	bucket->composite_count			= 0;
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

	list->composite_count++;

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

	list->composite_count--;

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
	automaton_ptr_bucket_list_initialize(bucket, count);
	return bucket;
}
void automaton_ptr_bucket_list_initialize(automaton_ptr_bucket_list* bucket, uint32_t count){
	uint32_t i;
	bucket->has_last_index			= false;
	bucket->composite_count			= 0;
	bucket->count					= count;
	bucket->bucket_count			= malloc(sizeof(uint32_t) * count);
	bucket->bucket_size				= malloc(sizeof(uint32_t) * count);
	bucket->buckets					= malloc(sizeof(void**) * count);
	for(i = 0; i < count; i++){
		bucket->bucket_count[i]		= 0;
		bucket->bucket_size[i]		= 0;
		bucket->buckets[i]			= NULL;
	}
}
void automaton_ptr_bucket_list_copy(automaton_ptr_bucket_list* target, automaton_ptr_bucket_list* source){
	if(target->count != source->count){
		automaton_ptr_bucket_destroy(target);
		automaton_ptr_bucket_list_initialize(target, source->count);
	}
	uint32_t i, j;
	for(i = 0; i < target->count; i++){
		if(target->bucket_size[i] < source->bucket_size[i]){
			free(target->buckets[i]);
			target->buckets[i]	= malloc(sizeof(void*) * source->bucket_size[i]);
			target->bucket_size[i]	= source->bucket_size[i];
		}
		for(j = 0; j < target->bucket_count[i]; j++){
			target->buckets[i][j]	= source->buckets[i][j];
		}
		target->bucket_count[i]		= source->bucket_count[i];
	}
	target->has_last_index			= source->has_last_index;
	target->last_added_bucket		= source->last_added_bucket;
	target->last_added_index		= source->last_added_index;
}
automaton_ptr_bucket_list* automaton_ptr_bucket_list_clone(automaton_ptr_bucket_list* source){
	automaton_ptr_bucket_list* target	= malloc(sizeof(automaton_ptr_bucket_list));
	uint32_t i, j;
	target->composite_count			= source->composite_count;
	target->count					= source->count;
	target->bucket_count			= malloc(sizeof(uint32_t) * target->count);
	target->bucket_size				= malloc(sizeof(uint32_t) * target->count);
	target->buckets					= malloc(sizeof(void**) * target->count);
	for(i = 0; i < target->count; i++){
		target->bucket_count[i]		= source->bucket_count[i];
		target->bucket_size[i]		= source->bucket_size[i];
		target->buckets[i]			= malloc(sizeof(void*) * target->bucket_size[i]);
		for(j =0; j < target->bucket_count[i]; j++){
			target->buckets[i][j]	= source->buckets[i][j];
		}
	}
	target->has_last_index			= source->has_last_index;
	target->last_added_bucket		= source->last_added_bucket;
	target->last_added_index		= source->last_added_index;
	return target;
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

void automaton_ptr_bucket_list_intersect(automaton_ptr_bucket_list* list, automaton_ptr_bucket_list* new_elements
		, automaton_ptr_bucket_list_key_extractor extractor){
	uint32_t i, j, current_key;
	void *current_entry;
	for(i = 0; i < new_elements->count; i++){
		for(j = 0; j < new_elements->bucket_count[i]; j++){
			current_entry	= new_elements->buckets[i][j];
			current_key		= extractor(current_entry);
			if(!automaton_ptr_bucket_has_entry(list, current_entry, current_key))
				automaton_ptr_bucket_remove_entry(list, current_entry, current_key);
		}
	}
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
void* automaton_ptr_bucket_get_entry(automaton_ptr_bucket_list* list, uint32_t key, automaton_ptr_bucket_list_key_extractor extractor){
	uint32_t index		= key % list->count;
		void** bucket	= list->buckets[index];
		bool found			= false;
		uint32_t i;
		for(i = 0; i < list->bucket_count[index]; i++){
			if(extractor(bucket[i]) == key)
				return bucket[i];
		}
		return NULL;
}
void* automaton_ptr_bucket_pop_entry(automaton_ptr_bucket_list* list){
	if(!list->has_last_index)return NULL;
	void* entry	= list->buckets[list->last_added_bucket][list->last_added_index];
	list->composite_count--;

	if(list->last_added_index > 0){
		list->bucket_count[list->last_added_bucket]--;
		list->last_added_index--;

	}else{
		uint32_t i;
		bool found	= false;
		for(i = 0; i < list->count; i++){
			if(list->bucket_count[i] > 0){
				list->last_added_bucket	= i;
				list->last_added_index	= list->bucket_count[i] - 1;
				found	= true;
				break;
			}
		}
		if(!found)
			list->has_last_index	= false;
	}
	return entry;
}
bool automaton_ptr_bucket_add_entry(automaton_ptr_bucket_list* list, void* entry, uint32_t key){
	if(automaton_ptr_bucket_has_entry(list, entry, key))
		return true;
	uint32_t index		= key % list->count;
	void** bucket	= list->buckets[index];
	uint32_t i;

	list->composite_count++;

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
	list->has_last_index			= true;
	list->last_added_bucket			= index;
	list->last_added_index			= list->bucket_count[index];
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
			if(i == list->last_added_index && index == list->last_added_bucket){
				automaton_ptr_bucket_pop_entry(list);
				return true;
			}
		}
		if(found){
			bucket[i]	= bucket[i + 1];
		}
	}
	list->composite_count--;
	list->bucket_count[index]--;
	return true;
}

void automaton_ptr_bucket_reset(automaton_ptr_bucket_list* list){
	uint32_t i;
	for(i = 0; i < list->count; i++){
		list->bucket_size[i]	= 0;
	}
	list->composite_count		= 0;
	list->has_last_index		= false;
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
