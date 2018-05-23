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

#endif /* BUCKET_LIST_H_ */
