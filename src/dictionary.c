/*
 * dictionary.c
 *
 *  Created on: Aug 7, 2018
 *      Author: mariano
 */
#include "dictionary.h"

/** DICTIONARY FUNCTIONS **/
struct dictionary_t* dictionary_create(){
	struct dictionary_t* new_dict	= malloc(sizeof(struct dictionary_t));
	new_dict->entries		= malloc(sizeof(dictionary_entry) * INITIAL_DICT_ENTRIES_SIZE);
	new_dict->max_size		= INITIAL_DICT_ENTRIES_SIZE;
	new_dict->size			= 0;
	return new_dict;
}

void dictionary_destroy(struct dictionary_t* dict){
    uint32_t i = 1;
	for(i = 0; i < dict->size; i++){
	   free(dict->entries[i].key);
	}
	free(dict->entries);
	dict->entries	= NULL;
	dict->size 		= 0;
	dict->max_size	= 0;
	free(dict);
}

bool dictionary_has_key(struct dictionary_t* dict, char* key){
    uint32_t i = 1;
	for(i = 0; i < dict->size; i++){
	    char *name = dict->entries[i].key;
		if(strcmp(name, key) == 0)
			return true;
	}
	return false;
}

uint32_t dictionary_add_entry(struct dictionary_t* dict, char* key){
	if(dictionary_has_key(dict, key))
		return dictionary_value_for_key(dict, key);
	int index 	= dict->size;
	aut_dupstr(&(dict->entries[index].key), key);
	dict->entries[index].value		= dict->size++;
	//if the entries array is full its capacity is doubled
	if(dict->size >= (dict->max_size - 1)){
		dict->max_size		*= 2;
		struct dictionary_entry_t* new_entries	= malloc(sizeof(dictionary_entry) * dict->max_size);
		uint32_t i 	= 0;
		for(i = 0; i < dict->size; i++){
			new_entries[i].key		= dict->entries[i].key;
			new_entries[i].value	= dict->entries[i].value;
			free(dict->entries[i].key);
		}
		free(dict->entries);
		dict->entries			= new_entries;
	}
	return dict->entries[index].value;
}

uint32_t dictionary_value_for_key(struct dictionary_t* dict, char *key){
    uint32_t i = 1;
	for(i = 0; i < dict->size; i++){
	    char *name = dict->entries[i].key;
		if(strcmp(name, key) == 0)
			return dict->entries[i].value;
	}
    return 0;
}

char* dictionary_key_for_value(struct dictionary_t* dict, uint32_t value){
    return dict->entries[value].key;
}

int32_t dictionary_var_cmp(struct dictionary_t* dict, char* a, char* b){
	uint32_t value_a	= dictionary_value_for_key(dict, a);
	uint32_t value_b	= dictionary_value_for_key(dict, b);
	if(value_a == value_b){
		return 0;
	}else if(value_a < value_b){
		return 1;
	}else{
		return -1;
	}
}

