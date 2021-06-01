#ifndef AUTOMATON_PARSER_UTILS_H
#define AUTOMATON_PARSER_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "automaton_utils.h"
//#include "automaton.h"
#include "obdd.h"

#define SIGNAL_ON_SUFFIX	".on"
#define SIGNAL_OFF_SUFFIX	".off"
#define SIGNAL_PRIME_SUFFIX	"_p"
/****************
==== SHARED VARS ====
****************/
extern uint32_t* parser_primed_variables;
extern uint32_t* parser_primed_original_variables;
extern uint32_t parser_primed_variables_size;
extern uint32_t parser_primed_variables_count;
/****************
==== ENUMS ====
****************/
typedef enum {
	RANGE_DEF_TYPE_AUT,
	RANGE_TYPE_AUT,
	CONST_TYPE_AUT,
	BINARY_TYPE_AUT,
	PARENTHESIS_TYPE_AUT,
	IDENT_TERMINAL_TYPE_AUT,
	INTEGER_TERMINAL_TYPE_AUT,
	UPPER_IDENT_TERMINAL_TYPE_AUT
} automaton_expression_type_syntax;
typedef enum {
	NOP_AUT = 0,		// nop
	RANGE_OP_AUT,		// ..
	ASSIGN_OP_AUT,		// =
	EQ_OP_AUT,			// ==
	NEQ_OP_AUT,			// !=
	GE_OP_AUT,			// >=
	LE_OP_AUT,			// <=
	GT_OP_AUT,			// >
	LT_OP_AUT,			// <
	PLUS_OP_AUT,		// +
	MINUS_OP_AUT,		// -
	MUL_OP_AUT,			// *
	DIV_OP_AUT,			// /
	MOD_OP_AUT,			// %
	AND_OP_AUT,			// &
	OR_OP_AUT,			// |
	NOT_OP_AUT,			// !
	XOR_OP_AUT,			// ^
	DIAMOND_OP_AUT,		// <>
	SQUARE_OP_AUT		// []
} automaton_expression_operator_syntax;
typedef enum {
	IMPORT_AUT,
	EXPORT_AUT,
	MENU_AUT,
	CONST_AUT,
	RANGE_AUT,
	FLUENT_AUT,
	ASSERTION_AUT,
	SET_AUT,
	COMPOSITION_AUT,
	GR_1_AUT,
	GOAL_AUT,
	LTL_RULE_AUT,
	LTL_FLUENT_AUT,
	EQUIV_CHECK_AUT,
	VSTATES_FLUENT_AUT,
	LTS_SEQ_AUT
} automaton_statement_type_syntax;
typedef enum {
	ASYNCH_AUT,
	SYNCH_AUT,
	CONCURRENT_AUT
} automaton_synchronization_type_syntax;
/****************
==== STRUCTS ====
*/
typedef struct automaton_expression_syntax_str{
	automaton_expression_type_syntax	type;
	struct automaton_expression_syntax_str* first;
	struct automaton_expression_syntax_str* second;
	char* string_terminal;
	int32_t integer_terminal;
	automaton_expression_operator_syntax op;
}automaton_expression_syntax;
typedef struct automaton_label_syntax_str{
	bool	is_set;
	bool	is_concurrent;
	struct automaton_indexes_syntax_str* indexes;
	struct automaton_set_syntax_str* set;
	char* string_terminal;
}automaton_label_syntax;
typedef struct automaton_set_syntax_str{
	bool is_ident;
	uint32_t count;
	uint32_t* labels_count;
	struct automaton_label_syntax_str*** labels;
	char* string_terminal;
}automaton_set_syntax;
typedef struct automaton_set_def_syntax_str{
	struct automaton_set_syntax_str* set;
	char* name;
}automaton_set_def_syntax;
typedef struct automaton_fluent_syntax_str{
	struct automaton_set_syntax_str* initiating_set;
	struct automaton_set_syntax_str* finishing_set;
	uint32_t initial_value;
	char* name;
}automaton_fluent_syntax;
typedef struct automaton_index_syntax_str{
	bool is_expr;
	bool is_range;
	struct automaton_expression_syntax_str* expr;
	char* lower_ident;
	char* upper_ident;
}automaton_index_syntax;
typedef struct automaton_indexes_syntax_str{
	uint32_t count;
	struct automaton_index_syntax_str** indexes;
}automaton_indexes_syntax;
typedef struct automaton_state_label_syntax_str{
	char* name;
	struct automaton_indexes_syntax_str* indexes;
}automaton_state_label_syntax;
typedef struct automaton_state_syntax_str{
	bool is_ref;
	struct automaton_state_label_syntax_str* label;
	struct automaton_state_label_syntax_str* ref;
	uint32_t transitions_count;
	struct automaton_transition_syntax_str** transitions;
}automaton_state_syntax;
typedef struct automaton_states_syntax_str{
	uint32_t count;
	struct automaton_state_syntax_str** states;
}automaton_states_syntax;
typedef struct automaton_transition_syntax_str{
	uint32_t count;
	struct automaton_expression_syntax_str* condition;
	struct automaton_trace_label_syntax_str** labels;
	struct automaton_state_label_syntax_str* to_state;
}automaton_transition_syntax;
typedef struct automaton_trace_label_syntax_str{
	uint32_t count;
	struct automaton_trace_label_atom_syntax_str** atoms;
}automaton_trace_label_syntax;
typedef struct automaton_trace_label_atom_syntax_str{
	struct automaton_label_syntax_str* label;
	struct automaton_indexes_syntax_str* indexes;
}automaton_trace_label_atom_syntax;
typedef struct automaton_transitions_syntax_str{
	uint32_t count;
	struct automaton_transition_syntax_str** transitions;
}automaton_transitions_syntax;
typedef struct automaton_component_syntax_str{
	char* ident;
	char* prefix;
	automaton_index_syntax* index;
	automaton_indexes_syntax* indexes;
	automaton_synchronization_type_syntax synch_type;
}automaton_component_syntax;
typedef struct automaton_components_syntax_str{
	uint32_t count;
	struct automaton_component_syntax_str** components;
}automaton_components_syntax;
typedef struct automaton_composition_syntax_str{
	char* name;
	uint32_t count;
	struct automaton_state_syntax_str** states;
	struct automaton_component_syntax_str** components;
	bool is_game;
}automaton_composition_syntax;
typedef struct automaton_equivalence_check_syntax_str{
	char* name;
	char* left;
	char* right;
}automaton_equivalence_check_syntax;
typedef struct automaton_gr1_game_syntax_str{
	char* name;
	struct automaton_set_syntax_str* assumptions;
	struct automaton_set_syntax_str* guarantees;
	char* composition_name;
}automaton_gr1_game_syntax;
typedef struct automaton_statement_syntax_str{
	automaton_statement_type_syntax type;
	struct automaton_composition_syntax_str* composition_def;
	struct automaton_expression_syntax_str* range_def;
	struct automaton_expression_syntax_str* const_def;
	struct automaton_fluent_syntax_str* fluent_def;
	struct automaton_set_def_syntax_str* set_def;
	struct automaton_gr1_game_syntax_str* gr1_game_def;
	struct ltl_rule_syntax_str* ltl_rule_def;
	struct ltl_fluent_syntax_str* ltl_fluent_def;
	struct automaton_equivalence_check_syntax_str* equivalence_check;
	struct automaton_import_syntax_str* import_def;
	struct automaton_vstates_fluent_syntax_str* vstates_syntax;
	struct automaton_serialization_syntax_str* serialization_syntax;
}automaton_statement_syntax;
typedef struct automaton_program_syntax_str{
	uint32_t count;
	struct automaton_statement_syntax_str** statements;
}automaton_program_syntax;
typedef struct automaton_vstates_syntax_str{
	uint32_t count;
	struct automaton_state_label_syntax_str** list;
}automaton_vstates_syntax;
typedef struct automaton_vstates_fluent_syntax_str{
	char* name;
	char* automaton_name;
	struct automaton_vstates_syntax_str* vstates;
}automaton_vstates_fluent_syntax;
typedef struct automaton_serialization_syntax_str{
	char* name;
	char* automaton_name;
	bool sequential;
	bool has_ticks;
}automaton_serialization_syntax;
typedef struct ltl_rule_syntax_str{
	bool is_theta;
	bool is_env;
	char* name;
	char* game_structure_name;
	obdd* obdd;
}ltl_rule_syntax;
typedef struct ltl_fluent_syntax_str{
	char* name;
	obdd* obdd;
}ltl_fluent_syntax;
typedef struct automaton_import_syntax_str{
	char *name;
	char *filename;
}automaton_import_syntax;
/****************
==== FUNCTIONS ====
*/
void automaton_program_syntax_destroy(automaton_program_syntax* program);
void automaton_statement_syntax_destroy(automaton_statement_syntax* statement);
void automaton_components_syntax_destroy(automaton_components_syntax* components);
void automaton_component_syntax_destroy(automaton_component_syntax* component);
void automaton_composition_syntax_destroy(automaton_composition_syntax* composition);
void automaton_states_syntax_destroy(automaton_states_syntax* states);
void automaton_state_syntax_destroy(automaton_state_syntax* state);
void automaton_state_label_syntax_destroy(automaton_state_label_syntax* state_label);
void automaton_transitions_syntax_destroy(automaton_transitions_syntax* transitions);
void automaton_transition_syntax_destroy(automaton_transition_syntax* transition);
void automaton_trace_label_syntax_destroy(automaton_trace_label_syntax* trace_label);
void automaton_trace_label_atom_syntax_destroy(automaton_trace_label_atom_syntax* trace_label_atom);
void automaton_label_syntax_destroy(automaton_label_syntax* label);
void automaton_indexes_syntax_destroy(automaton_indexes_syntax* indexes);
void automaton_index_syntax_destroy(automaton_index_syntax* index);
void automaton_fluent_syntax_destroy(automaton_fluent_syntax* fluent);
void automaton_set_syntax_destroy(automaton_set_syntax* set);
void automaton_set_def_syntax_destroy(automaton_set_def_syntax* set_def);
void automaton_expression_syntax_destroy(automaton_expression_syntax* expr);
void automaton_gr1_game_syntax_destroy(automaton_gr1_game_syntax* gr1_game);
void automaton_equivalence_check_syntax_destroy(automaton_equivalence_check_syntax* check);
void automaton_import_syntax_destroy(automaton_import_syntax* import_syntax);
void ltl_rule_syntax_destroy(ltl_rule_syntax* ltl_rule);
void ltl_fluent_syntax_destroy(ltl_fluent_syntax* ltl_fluent);
automaton_expression_syntax* automaton_expression_syntax_create(automaton_expression_type_syntax type, automaton_expression_syntax* first
		, automaton_expression_syntax* second, char* string_terminal, int32_t integer_terminal, automaton_expression_operator_syntax op);
automaton_set_syntax* automaton_set_syntax_create(bool is_ident, uint32_t count, uint32_t* labels_count,
		automaton_label_syntax*** labels, char* string_terminal);
void automaton_program_add_obdd_primed_variables();
automaton_set_syntax* automaton_set_syntax_create_concurrent(char* string_terminal, automaton_indexes_syntax* indexes);
automaton_set_syntax* automaton_set_syntax_concat_concurrent(automaton_set_syntax* set, char* string_terminal, automaton_indexes_syntax* indexes);
automaton_set_syntax* automaton_set_syntax_create_from_label(automaton_label_syntax* label);
automaton_set_syntax* automaton_set_syntax_create_from_ident(char* ident);
automaton_set_syntax* automaton_set_syntax_concat_labels(automaton_set_syntax* set, automaton_label_syntax* label);
automaton_label_syntax* automaton_label_syntax_create(bool is_set, bool is_concurrent, automaton_set_syntax* set, char* string_terminal, automaton_indexes_syntax* indexes);
automaton_label_syntax* automaton_label_syntax_create_empty();
automaton_set_def_syntax* automaton_set_def_syntax_create(automaton_set_syntax* set, char* name);
automaton_fluent_syntax* automaton_fluent_syntax_create(char* name, automaton_set_syntax* initiating_set, automaton_set_syntax* finishing_set, uint32_t initial_value);
automaton_index_syntax* automaton_index_syntax_create(bool is_expr, bool is_range, automaton_expression_syntax* expr, char* lower_ident
		, char* upper_ident);
automaton_indexes_syntax* automaton_indexes_syntax_create(automaton_index_syntax* first_index);
automaton_indexes_syntax* automaton_indexes_syntax_add_index(automaton_indexes_syntax* indexes, automaton_index_syntax* index);
automaton_trace_label_atom_syntax* automaton_trace_label_atom_syntax_create_from_index(automaton_index_syntax* first_index);
automaton_trace_label_atom_syntax* automaton_trace_label_atom_syntax_create(automaton_label_syntax* label, automaton_indexes_syntax* indexes);
automaton_trace_label_syntax* automaton_trace_label_syntax_create(automaton_trace_label_atom_syntax* first_atom);
automaton_trace_label_syntax* automaton_trace_label_syntax_add_atom(automaton_trace_label_syntax* trace_label, automaton_trace_label_atom_syntax* atom);
automaton_transition_syntax* automaton_transition_syntax_finish(automaton_expression_syntax* condition, automaton_transition_syntax* trace
		, automaton_state_label_syntax* state);
automaton_transition_syntax* automaton_transition_syntax_create_from_trace(automaton_trace_label_syntax* trace);
automaton_transition_syntax* automaton_transition_syntax_add_trace(automaton_transition_syntax* transition, automaton_trace_label_syntax* trace);
automaton_transitions_syntax* automaton_transitions_syntax_create(automaton_transition_syntax* transition);
automaton_transitions_syntax* automaton_transitions_syntax_add_transition(automaton_transitions_syntax* transitions, automaton_transition_syntax* transition);
automaton_state_syntax* automaton_state_syntax_create(bool is_ref, automaton_state_label_syntax* label, automaton_state_label_syntax* ref
		,automaton_transitions_syntax* transitions);
automaton_state_label_syntax* automaton_state_label_syntax_create(char* name, automaton_indexes_syntax* indexes);
automaton_states_syntax* automaton_states_syntax_create(automaton_state_syntax* state);
automaton_states_syntax* automaton_states_syntax_add_state(automaton_states_syntax* states, automaton_state_syntax* state);
automaton_equivalence_check_syntax* automaton_equality_check_syntax_create(char* name, char* left,char* right);
automaton_composition_syntax* automaton_composition_syntax_create_from_states(automaton_states_syntax* states);
automaton_composition_syntax* automaton_composition_syntax_create_from_ref(char* name, automaton_components_syntax* components, bool is_game);
automaton_gr1_game_syntax* automaton_gr1_game_syntax_create(char* name, char* composition_name, automaton_set_syntax* assumptions, automaton_set_syntax* goals);
automaton_components_syntax* automaton_components_syntax_create(automaton_component_syntax* component);
automaton_components_syntax* automaton_components_syntax_add_component(automaton_components_syntax* components, automaton_component_syntax* component, automaton_synchronization_type_syntax type);
automaton_component_syntax* automaton_component_syntax_create(char* ident, char* prefix, automaton_index_syntax* index, automaton_indexes_syntax* indexes);
automaton_program_syntax* automaton_program_syntax_create(automaton_statement_syntax* first_statement);
automaton_program_syntax* automaton_program_syntax_add_statement(automaton_program_syntax* program, automaton_statement_syntax* statement);
automaton_statement_syntax* automaton_statement_syntax_create(automaton_statement_type_syntax type, automaton_composition_syntax* composition_def,
		automaton_expression_syntax* range_def, automaton_expression_syntax* const_def, automaton_fluent_syntax* fluent_def,
		automaton_set_def_syntax* set_def, automaton_gr1_game_syntax* gr1_game_def, ltl_rule_syntax* ltl_rule_def, ltl_fluent_syntax* ltl_fluent_def,
		automaton_equivalence_check_syntax* equivalence_check, automaton_import_syntax* import_syntax,
		automaton_vstates_fluent_syntax* vstates_fluent_syntax, automaton_serialization_syntax* serialization_syntax);
automaton_import_syntax *automaton_import_syntax_create(char *name, char *filename);
bool automaton_syntax_is_reserved(char* token);
automaton_vstates_syntax* automaton_vstates_syntax_concat_state(automaton_vstates_syntax* vstates, automaton_state_label_syntax* state);
automaton_vstates_syntax* automaton_vstates_syntax_create_from_state(automaton_state_label_syntax* state);
void automaton_vstates_syntax_destroy(automaton_vstates_syntax* vstates);
automaton_vstates_fluent_syntax* automaton_vstates_fluent_syntax_create(char* name, char* automaton_name, automaton_vstates_syntax* states);
void automaton_vstates_fluent_syntax_destroy(automaton_vstates_fluent_syntax* vstates_fluent);
automaton_serialization_syntax* automaton_serialization_syntax_create_from_ref(char *name, char *automaton_name, bool sequential, bool has_ticks);
void automaton_serialization_syntax_destroy(automaton_serialization_syntax* automaton_serialization);
ltl_rule_syntax* ltl_rule_syntax_create(bool is_theta, bool is_env, char* name, char* game_structure_name, obdd* obdd);
ltl_fluent_syntax* automaton_ltl_fluent_syntax_create(char* name, obdd* obdd);
obdd_mgr* parser_get_obdd_mgr();
void parser_reset_obdd_mgr();
uint32_t* parser_get_primed_variables();
void parser_add_primed_variables(uint32_t primed_variable, uint32_t original_var_ID);
#endif
