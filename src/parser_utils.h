#ifndef AUTOMATON_PARSER_UTILS_H
#define AUTOMATON_PARSER_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
/****************
==== ENUMS ====
****************/
typedef enum {
	RANGE_DEF_TYPE_AUT,
	RANGE_TYPE_AUT,
	CONST_TYPE_AUT,
	BINARY_TYPE_AUT,
	UNARY_TYPE_AUT,
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
}automaton_composition_syntax;
/****************
==== STRUCTS ====
*/
automaton_expression_syntax* automaton_expression_syntax_create(automaton_expression_type_syntax type, automaton_expression_syntax* first
		, automaton_expression_syntax* second, char* string_terminal, int32_t integer_terminal, automaton_expression_operator_syntax op);
automaton_set_syntax* automaton_set_syntax_create(bool is_ident, uint32_t count, uint32_t* labels_count,
		automaton_label_syntax*** labels, char* string_terminal);
automaton_set_syntax* automaton_set_syntax_create_concurrent(char* string_terminal);
automaton_set_syntax* automaton_set_syntax_concat_concurrent(automaton_set_syntax* set, char* string_terminal);
automaton_set_syntax* automaton_set_syntax_create_from_label(automaton_label_syntax* label);
automaton_set_syntax* automaton_set_syntax_create_from_ident(char* ident);
automaton_set_syntax* automaton_set_syntax_concat_labels(automaton_set_syntax* set, automaton_label_syntax* label);
automaton_label_syntax* automaton_label_syntax_create(bool is_set, automaton_set_syntax* set, char* string_terminal);
automaton_set_def_syntax* automaton_set_def_syntax_create(automaton_set_syntax* set, char* name);
automaton_fluent_syntax* automaton_fluent_syntax_create(char* name, automaton_set_syntax* initiating_set, automaton_set_syntax* finishing_set);
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
automaton_composition_syntax* automaton_composition_syntax_create_from_states(automaton_states_syntax* states);
automaton_composition_syntax* automaton_composition_syntax_create_from_ref(char* name, automaton_components_syntax* components);
automaton_components_syntax* automaton_components_syntax_create(automaton_component_syntax* component);
automaton_components_syntax* automaton_components_syntax_add_component(automaton_components_syntax* components, automaton_component_syntax* component);
automaton_component_syntax* automaton_component_syntax_create(char* ident, char* prefix, automaton_index_syntax* index, automaton_indexes_syntax* indexes);
#endif
