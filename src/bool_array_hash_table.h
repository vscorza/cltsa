/*
 * bool_array_hash_table.h
 *
 *  Created on: Sep 22, 2019
 *      Author: mariano
 */

#ifndef SRC_BOOL_ARRAY_HASH_TABLE_H_
#define SRC_BOOL_ARRAY_HASH_TABLE_H_

#define BOOL_ARRAY_HASH_TABLE_SIZE 3

#include "automaton_utils.h"

typedef struct automaton_bool_array_hash_table_str {
  uint32_t variable_count;
  bool ****entries;
  uint64_t hits;
  uint64_t misses;
} automaton_bool_array_hash_table;

automaton_bool_array_hash_table *
automaton_bool_array_hash_table_create(uint32_t variable_count);
void automaton_bool_array_hash_table_destroy(
    automaton_bool_array_hash_table *table);
bool *automaton_bool_array_hash_table_get_entry(
    automaton_bool_array_hash_table *table, bool *entry);
void automaton_bool_array_hash_table_add_entry(
    automaton_bool_array_hash_table *table, bool *entry, bool copy_entry);
void automaton_bool_array_hash_table_get_entry_indexes(
    automaton_bool_array_hash_table *table, bool *entry,
    uint32_t *first_level_index, uint32_t *second_level_index);
bool *automaton_bool_array_hash_table_add_or_get_entry(
    automaton_bool_array_hash_table *table, bool *entry, bool copy_entry);
#endif /* SRC_BOOL_ARRAY_HASH_TABLE_H_ */
