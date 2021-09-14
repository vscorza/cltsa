/*
 * dictionary.h
 *
 *  Created on: Aug 7, 2018
 *      Author: mariano
 */

#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include "automaton_utils.h"

#define INITIAL_DICT_ENTRIES_SIZE 20
#define TRUE_VAR "1"
#define FALSE_VAR "0"

/** STRUCTS **/
typedef struct dictionary_entry_t {
  char *key;
  uint32_t value;
} dictionary_entry;

typedef struct dictionary_t {
  uint32_t size;
  uint32_t max_size;
  dictionary_entry *entries;
} dictionary;

/** DICTIONARY **/
dictionary *dictionary_create();
void dictionary_destroy(dictionary *dict);
bool dictionary_has_key(dictionary *dict, char *key);
uint32_t dictionary_add_entry(dictionary *dict, char *key);
uint32_t dictionary_value_for_key(dictionary *dict, char *key);
char *dictionary_key_for_value(dictionary *dict, uint32_t value);

#endif /* DICTIONARY_H_ */
