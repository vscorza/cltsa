/*
 * bucket_count.h
 *
 *  Created on: May 23, 2018
 *      Author: mariano
 */

#ifndef BUCKET_LIST_H_
#define BUCKET_LIST_H_

#include "automaton_utils.h"

typedef struct automaton_bucket_list_str{
	uint32_t composite_count;
	uint32_t count;
	uint32_t *bucket_count;
	uint32_t *bucket_size;
	uint32_t **buckets;
}automaton_bucket_list;

automaton_bucket_list* automaton_bucket_list_create(uint32_t count);
bool automaton_bucket_has_entry(automaton_bucket_list* list, uint32_t entry);
bool automaton_bucket_add_entry(automaton_bucket_list* list, uint32_t entry);
bool automaton_bucket_remove_entry(automaton_bucket_list* list, uint32_t entry);
void automaton_bucket_destroy(automaton_bucket_list* list);

typedef uint32_t (*automaton_ptr_bucket_list_key_extractor)(void*);

typedef struct automaton_ptr_bucket_list_str{
	uint32_t composite_count;
	uint32_t count;
	uint32_t *bucket_count;
	uint32_t *bucket_size;
	void ***buckets;
	uint32_t last_added_bucket;
	bool has_last_index;
}automaton_ptr_bucket_list;

automaton_ptr_bucket_list* automaton_ptr_bucket_list_create(uint32_t count);
void automaton_ptr_bucket_list_initialize(automaton_ptr_bucket_list* bucket, uint32_t count);
automaton_ptr_bucket_list* automaton_ptr_bucket_list_clone(automaton_ptr_bucket_list* source);
void automaton_ptr_bucket_list_copy(automaton_ptr_bucket_list* target, automaton_ptr_bucket_list* source);
void automaton_ptr_bucket_list_merge(automaton_ptr_bucket_list* list, automaton_ptr_bucket_list* new_elements
		, automaton_ptr_bucket_list_key_extractor extractor);
void automaton_ptr_bucket_list_intersect(automaton_ptr_bucket_list* list, automaton_ptr_bucket_list* new_elements
		, automaton_ptr_bucket_list_key_extractor extractor);
bool automaton_ptr_bucket_has_key(automaton_ptr_bucket_list* list, uint32_t key, automaton_ptr_bucket_list_key_extractor extractor);
bool automaton_ptr_bucket_has_entry(automaton_ptr_bucket_list* list, void* entry, uint32_t key);
void* automaton_ptr_bucket_get_entry(automaton_ptr_bucket_list* list, uint32_t key, automaton_ptr_bucket_list_key_extractor extractor);
void* automaton_ptr_bucket_pop_entry(automaton_ptr_bucket_list* list);
bool automaton_ptr_bucket_add_entry(automaton_ptr_bucket_list* list, void* entry, uint32_t key);
bool automaton_ptr_bucket_remove_entry(automaton_ptr_bucket_list* list, void* entry, uint32_t key);
void automaton_ptr_bucket_reset(automaton_ptr_bucket_list* list);
void automaton_ptr_bucket_destroy(automaton_ptr_bucket_list* list);


#endif /* BUCKET_LIST_H_ */
