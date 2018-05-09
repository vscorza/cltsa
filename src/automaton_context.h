/*
 * automaton_context.h
 *
 *  Created on: Apr 19, 2018
 *      Author: mariano
 */

#ifndef AUTOMATON_CONTEXT_H_
#define AUTOMATON_CONTEXT_H_

#include "automaton.h"
#include "parser_utils.h"

#define GLOBAL_ALPHABET_NAME_AUT		"Global_Alphabet"
#define CONTROLLABLE_ALPHABET_NAME_AUT	"Controllable_Alphabet"
/****************
==== STRUCTS ====
*/
typedef enum{
	LABEL_ENTRY_AUT,
	SET_ENTRY_AUT,
	FLUENT_ENTRY_AUT,
	RANGE_ENTRY_AUT,
	CONST_ENTRY_AUT,
	AUTOMATON_ENTRY_AUT,
	COMPOSITION_ENTRY_AUT
}automaton_parsing_table_entry_type;
typedef struct automaton_parsing_table_entry_str{
	automaton_parsing_table_entry_type type;
	char* key;
	int32_t index;
	void* value;
	bool solved;
	int32_t valuation_count;
	union{
		char** labels_value;
		int32_t int_value;
		automaton_automaton* automaton_value;
	}valuation;
}automaton_parsing_table_entry;
typedef struct automaton_parsing_tables_str{
	uint32_t label_count;
	struct automaton_parsing_table_entry_str** label_entries;
	uint32_t set_count;
	struct automaton_parsing_table_entry_str** set_entries;
	uint32_t fluent_count;
	struct automaton_parsing_table_entry_str** fluent_entries;
	uint32_t range_count;
	struct automaton_parsing_table_entry_str** range_entries;
	uint32_t const_count;
	struct automaton_parsing_table_entry_str** const_entries;
	uint32_t automaton_count;
	struct automaton_parsing_table_entry_str** automaton_entries;
	uint32_t composition_count;
	struct automaton_parsing_table_entry_str** composition_entries;
}automaton_parsing_tables;
/****************
==== FUNCTIONS ====
*/
automaton_parsing_tables* automaton_parsing_tables_create();
void automaton_parsing_tables_destroy(automaton_parsing_tables* tables);
automaton_parsing_table_entry* automaton_parsing_table_entry_create(automaton_parsing_table_entry_type type, char* key, void* value, int32_t index);
void automaton_parsing_table_entry_destroy(automaton_parsing_table_entry* entry);
int32_t automaton_parsing_tables_get_entry_index(automaton_parsing_tables* tables, automaton_parsing_table_entry_type type, char* value);
int32_t automaton_parsing_tables_add_entry(automaton_parsing_tables* tables, automaton_parsing_table_entry_type type, char* key, void* value);
automaton_automata_context* automaton_automata_context_create_from_syntax(automaton_program_syntax* program, char* ctx_name);

void automaton_statement_syntax_to_table(automaton_statement_syntax* statement, automaton_parsing_tables* tables);
int32_t automaton_expression_syntax_evaluate(automaton_parsing_tables* tables, automaton_expression_syntax* expr);
char** automaton_set_syntax_evaluate(automaton_parsing_tables* tables, automaton_set_syntax* set, int32_t *count);
automaton_alphabet* automaton_parsing_tables_get_global_alphabet(automaton_parsing_tables* tables);
bool automaton_statement_syntax_to_automaton(automaton_automata_context* ctx, automaton_composition_syntax* composition_syntax
		, automaton_parsing_tables* tables);
#endif /* AUTOMATON_CONTEXT_H_ */
