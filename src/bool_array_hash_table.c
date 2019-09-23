/*
 * bool_array_hash_table.c
 *
 *  Created on: Sep 22, 2019
 *      Author: mariano
 */
#include "bool_array_hash_table.h"

/**
 * Creates a new hash table for boolean arrays
 * IMPORTANT: all arrays are removed as a they are replaced by new entries
 * @param variable_count the lenght of each boolean array to be stored
 * @return an empty hash table structure
 */
automaton_bool_array_hash_table* automaton_bool_array_hash_table_create(uint32_t variable_count){
	automaton_bool_array_hash_table *table	= calloc(1, sizeof(automaton_bool_array_hash_table));
	table->variable_count	= variable_count;
	table->entries			= calloc((2 << BOOL_ARRAY_HASH_TABLE_SIZE), sizeof(bool***));
	return table;
}

/**
 * Destroys the boolean array hash table provided as input
 * @param table the hash table to be destroyed
 */
void automaton_bool_array_hash_table_destroy(automaton_bool_array_hash_table* table){
	uint32_t i,j, k;
	for(i = 0; i < (2 << BOOL_ARRAY_HASH_TABLE_SIZE); i++){
		if(table->entries[i] != NULL){
			for(j = 0; j < (2 << BOOL_ARRAY_HASH_TABLE_SIZE); j++){
				if(table->entries[i][j] != NULL){
					for(k = 0; k < BOOL_ARRAY_HASH_TABLE_SIZE; k++){
						if(table->entries[i][j][k] != NULL){
							free(table->entries[i][j][k]);
							table->entries[i][j][k] = NULL;
						}
					}
					free(table->entries[i][j]);
					table->entries[i][j] = NULL;
				}
			}
			free(table->entries[i]);
			table->entries[i]	= NULL;
		}
	}
	free(table->entries);
	table->entries	= NULL;
	free(table);
}
/**
 * Returns the equivalent entry for the provided boolean array if one exists, if none exists it adds it
 * IMPORTANT: all arrays are removed as a they are replaced by new entries, clone entry if persistent use is expected
 * @param table the hash table
 * @param entry the boolean array to be searched
 * @param copy_entry whether the new entry should be cloned or not
 * @return the boolean array already stored that is equivalent to the one provided
 */
bool *automaton_bool_array_hash_table_add_or_get_entry(automaton_bool_array_hash_table* table, bool *entry, bool copy_entry){
	bool* current_entry	= NULL;
	current_entry	= automaton_bool_array_hash_table_get_entry(table, entry);
	if(current_entry != NULL)return current_entry;

	automaton_bool_array_hash_table_add_entry(table, entry, copy_entry);
	return entry;
}
/**
 * Returns the equivalent entry for the provided boolean array if one exists
 * IMPORTANT: all arrays are removed as a they are replaced by new entries, clone entry if persistent use is expected
 * @param table the hash table
 * @param entry the boolean array to be searched
 * @return the boolean array already stored that is equivalent to the one provided
 */
bool *automaton_bool_array_hash_table_get_entry(automaton_bool_array_hash_table* table, bool *entry){
	//build index entries for first and second level
	uint32_t first_level_index, second_level_index, i, j;
	automaton_bool_array_hash_table_get_entry_indexes(table, entry, &first_level_index, &second_level_index);
	if(table->entries[first_level_index] == NULL)return false;
	if(table->entries[first_level_index][second_level_index] == NULL)return false;

	bool **inner_entries	= table->entries[first_level_index][second_level_index];
	bool found = true;

	for(i = 0; i < BOOL_ARRAY_HASH_TABLE_SIZE; i++){
		if(inner_entries[i] == NULL)break;
		found = true;
		for(j = 0; j < table->variable_count; j++){
			if(inner_entries[i][j] != entry[j]){
				found = false;
				break;
			}
		}
		if(found){
			table->hits++;
			return inner_entries[i];
		}
	}
	table->misses++;
	return NULL;
}

/**
 * Adds a copy of the boolean array as an entry into the hash table following a FIFO scheme
 * @param table the hash table
 * @param entry the boolean array to be added
 * @param copy_entry whether the entry should be stored as is or cloned
 */
void automaton_bool_array_hash_table_add_entry(automaton_bool_array_hash_table* table, bool *entry, bool copy_entry){
	uint32_t first_level_index, second_level_index;
	int32_t i;
	automaton_bool_array_hash_table_get_entry_indexes(table, entry, &first_level_index, &second_level_index);
	//inner entries will be created if needed
	if(table->entries[first_level_index] == NULL){
		table->entries[first_level_index]	= calloc((2 << BOOL_ARRAY_HASH_TABLE_SIZE),sizeof(bool**));
	}
	if(table->entries[first_level_index][second_level_index] == NULL){
		table->entries[first_level_index][second_level_index] = calloc(BOOL_ARRAY_HASH_TABLE_SIZE, sizeof(bool*));
	}
	//existing internal array is full shift all entries one position up and delete the last
	bool **current_entry	= table->entries[first_level_index][second_level_index];
	if(current_entry[BOOL_ARRAY_HASH_TABLE_SIZE - 1] != NULL){
		free(current_entry[BOOL_ARRAY_HASH_TABLE_SIZE - 1]);
	}
	for(i = BOOL_ARRAY_HASH_TABLE_SIZE - 2; i >= 0; i--){
		current_entry[i + 1] = current_entry[i];
	}
	//copy entry
	if(copy_entry){
		bool *new_entry	= malloc(table->variable_count * sizeof(bool));
		for(i = 0; i < table->variable_count; i++)
			new_entry[i]	= entry[i];
		current_entry[0]	= new_entry;
	}else	current_entry[0]	= entry;
}

/**
 * Gets the first and second level indexes for a given entry
 * @param table the hash table
 * @param entry the entry to be evaluated, first BOOL_SIZE values will be interpreted  as an int, same for the second part
 * @param first_level_index placeholder for the first level index
 * @param scond_level_index placeholder for the second level index
 */
void automaton_bool_array_hash_table_get_entry_indexes(automaton_bool_array_hash_table* table, bool *entry
		, uint32_t *first_level_index, uint32_t *second_level_index){
	*first_level_index = 0;	*second_level_index = 0;
	//adjust evaluation limit according to variable count
	uint32_t i, j, limit	= BOOL_ARRAY_HASH_TABLE_SIZE > table->variable_count ? table->variable_count : BOOL_ARRAY_HASH_TABLE_SIZE;
	//convert partial boolean array to int
	for(i = 0; i < limit; i++){
		*first_level_index <<= 1;
		if(entry[i])*first_level_index |= 0x1;
	}
	//convert partial boolean array to int
	if(table->variable_count > BOOL_ARRAY_HASH_TABLE_SIZE){
		limit = BOOL_ARRAY_HASH_TABLE_SIZE > (table->variable_count - BOOL_ARRAY_HASH_TABLE_SIZE) ? (table->variable_count - BOOL_ARRAY_HASH_TABLE_SIZE) : BOOL_ARRAY_HASH_TABLE_SIZE;
		for(i = 0; i < limit; i++){
			*second_level_index <<= 1;
			if(entry[i + BOOL_ARRAY_HASH_TABLE_SIZE])*second_level_index |= 0x1;
		}
	}
}


