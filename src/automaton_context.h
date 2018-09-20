/*
 * automaton_context.h
 *
 *  Created on: Apr 19, 2018
 *      Author: mariano
 */

#ifndef AUTOMATON_CONTEXT_H_
#define AUTOMATON_CONTEXT_H_

#include "automaton.h"
#include "automaton_mu_calculus.h"
#include "parser_utils.h"

#define GLOBAL_ALPHABET_NAME_AUT		"Global_Alphabet"
#define CONTROLLABLE_ALPHABET_NAME_AUT	"Controllable_Alphabet"
#define GLOBAL_SIGNALS_NAME_AUT			"Global_Signals"
#define OUTPUT_SIGNALS_NAME_AUT			"Output_Signals"

#define DEBUG_PARSE_STATES 0
#define DEBUG_LTL_AUTOMATON 0
#define LTL_BUCKET_SIZE	10000
#if DEBUG_PARSE_STATES
#define aut_context_log(format, ...) printf(format, ##__VA_ARGS__ )
#else
#define aut_context_log(format, ...)
#endif
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
	COMPOSITION_ENTRY_AUT,
	LTL_RULE_ENTRY_AUT
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
		automaton_range* range_value;
		automaton_fluent* fluent_value;
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
automaton_automata_context* automaton_automata_context_create_from_syntax(automaton_program_syntax* program, char* ctx_name
		, bool is_synchronous, bool print_fsp);
automaton_indexes_valuation* automaton_indexes_valuation_create_from_indexes(automaton_parsing_tables* tables, automaton_indexes_syntax* indexes);
void automaton_indexes_valuation_increase(automaton_indexes_valuation* valuation);
bool automaton_indexes_valuation_has_next(automaton_indexes_valuation* valuation);
void automaton_indexes_valuation_set_label(automaton_indexes_valuation* valuation, char* label, char* target);
void automaton_indexes_valuation_set_to_label(automaton_parsing_tables* tables,automaton_indexes_valuation* valuation, automaton_indexes_syntax* from_indexes, automaton_indexes_syntax* to_indexes, char* label, char* target);
void automaton_statement_syntax_to_table(automaton_statement_syntax* statement, automaton_parsing_tables* tables);
int32_t automaton_expression_syntax_evaluate(automaton_parsing_tables* tables, automaton_expression_syntax* expr, automaton_indexes_valuation* indexes_valuation);
char** automaton_set_syntax_evaluate(automaton_parsing_tables* tables, automaton_set_syntax* set, int32_t *count, char* set_def_key);
automaton_alphabet* automaton_parsing_tables_get_global_alphabet(automaton_parsing_tables* tables);
bool automaton_statement_syntax_to_automaton(automaton_automata_context* ctx, automaton_composition_syntax* composition_syntax
		, automaton_parsing_tables* tables, bool is_synchronous);
automaton_range* automaton_range_syntax_evaluate(automaton_parsing_tables *tables, char* name, automaton_expression_syntax *range_def_syntax);
bool automaton_statement_syntax_to_constant(automaton_automata_context* ctx, automaton_expression_syntax* const_def_syntax
		, automaton_parsing_tables* tables);
bool automaton_statement_syntax_to_range(automaton_automata_context* ctx, automaton_expression_syntax* range_def_syntax
		, automaton_parsing_tables* tables);
bool automaton_statement_syntax_to_fluent(automaton_automata_context* ctx, automaton_fluent_syntax* fluent_def_syntax
		, automaton_parsing_tables* tables, automaton_alphabet* global_alphabet);
automaton_fluent* automaton_fluent_create_from_syntax(automaton_parsing_tables* tables, automaton_fluent_syntax* fluent_def_syntax, automaton_alphabet* global_alphabet);
void automaton_index_syntax_get_range(automaton_parsing_tables* tables, automaton_index_syntax* index, int32_t *lower_index, int32_t *upper_index);
void automaton_indexes_syntax_eval_strings(automaton_parsing_tables* tables, automaton_indexes_valuation* valuation, char*** a, int32_t* a_count, automaton_indexes_syntax* indexes);
bool automaton_add_transition_from_valuations(obdd_mgr* mgr, automaton_automaton* automaton, uint32_t from_state, uint32_t to_state, bool* from_valuation,
		bool* to_valuation, bool is_initial, bool is_input, uint32_t x_count, uint32_t y_count, char** obdd_on_indexes, char** obdd_off_indexes
		,uint32_t* x_y_alphabet, uint32_t* x_y_x_p_alphabet);
void automaton_set_composed_valuation(bool* valuation, bool* partial_valuation, uint32_t valuation_offset, bool is_initial, bool is_input, uint32_t* var_ids
		, uint32_t x_count, uint32_t y_count);
automaton_automaton* automaton_build_automaton_from_obdd(automaton_automata_context* ctx, char* name, obdd** env_theta_obdd, uint32_t env_theta_count, obdd** sys_theta_obdd, uint32_t sys_theta_count
		, obdd** env_rho_obdd, uint32_t env_rho_count, obdd** sys_rho_obdd, uint32_t sys_rho_count, automaton_parsing_tables* tables);
#endif /* AUTOMATON_CONTEXT_H_ */
