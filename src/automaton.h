#ifndef AUTOMATON_H
#define AUTOMATON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#define COMPOSE_SYNCH	1
#define PARTIAL_SHARE	2

#define LIST_INITIAL_SIZE		32
#define LIST_INCREASE_FACTOR	2

#define TRUE_VAR			"1"
#define FALSE_VAR			"0"

/****************
==== ENUMS ==== 
****************/
typedef enum { false, true } bool;

typedef enum aumaton_signal_type_enum{
	INPUT_SIG 	= 0,
	OUTPUT_SIG	= 1,
	INTERNAL_SIG	= 2
} automaton_signal_type;

typedef enum aumaton_synchronization_type_enum{
	SYNCHRONOUS 	= 0,
	ASYNCHRONOUS	= 1
} automaton_synchronization_type;

/****************
==== STRUCTS ==== 
****************/
/** COMPOSITE STATE TREE **/
typedef struct automaton_composite_tree_entry_str{
	uint32_t value;
	struct automaton_composite_tree_entry_str* next;
	struct automaton_composite_tree_entry_str* succ;
}__attribute__((__packed__)) automaton_composite_tree_entry;

typedef struct automaton_composite_tree_str{
	uint32_t key_length;
	uint32_t max_value;
	automaton_composite_tree_entry* first_entry;
}__attribute__((__packed__)) automaton_composite_tree;
/** AUTOMATON **/

typedef struct automaton_signal_str {
	char*					name;	//signal name
	automaton_signal_type	type;	//signal type, input, output, internal
}__attribute__((__packed__)) automaton_signal_event;

typedef struct automaton_alphabet_str{
	uint32_t				count;	//alphabet element count
	uint32_t				size;
	automaton_signal_event*	list;	//alphabet list
}__attribute__((__packed__)) automaton_alphabet;

typedef struct automaton_transition_str{
	uint32_t	state_from;
	uint32_t	state_to;
	uint32_t	signals_count;
	uint32_t*	signals; 
}__attribute__((__packed__)) automaton_transition;

typedef struct automaton_fluent_str{
	char*		name;
	uint32_t	starting_signals_count;
	uint32_t*	starting_signals;
	uint32_t	ending_signals_count;
	uint32_t*	ending_signals;
	bool		initial_valuation;
}__attribute__((__packed__)) automaton_fluent;

typedef struct automaton_valuation_str{
	uint32_t	state;
	uint32_t	active_fluents_count;
	uint32_t*	active_fluents;
}__attribute__((__packed__)) automaton_valuation;

typedef struct automata_context_str{
	char*				name;
	automaton_alphabet*	global_alphabet;
	uint32_t			global_fluents_count;
	automaton_fluent*	global_fluents;
}__attribute__((__packed__)) automaton_automata_context;

typedef struct automaton_str{
	char*					name;
	automaton_automata_context*		context;
	uint32_t				local_alphabet_count;
	uint32_t*				local_alphabet;
	uint32_t				states_count;
	uint32_t				transitions_size;
	uint32_t				transitions_count;
	uint32_t				max_out_degree;
	uint32_t*				out_degree;
	automaton_transition**	transitions;			// S -> list of transitions (s,s')
	uint32_t*				in_degree;
	automaton_transition**	inverted_transitions;
	uint32_t				initial_states_count;
	uint32_t*				initial_states;
	uint32_t				valuations_size;
	uint32_t				valuations_count;
	automaton_valuation*	valuations;
}__attribute__((__packed__)) automaton_automaton;

/** AUTOMATA **/
typedef struct automata_str{
	bool					forward_decomposition;
	uint32_t				automata_count;
	automaton_automaton*	automata_list;
	uint32_t				states_explored_count;
	uint32_t				states_explored_size;
	uint32_t**				states_explored;
	bool*					states_decomposed;
	automaton_valuation*	states_valuations;
}__attribute__((__packed__)) automaton_automata;

/** AUX **/

typedef struct dictionary_entry_str {
    char *key;
    uint32_t value;
} __attribute__((__packed__)) dictionary_entry;

typedef struct dictionary_str{
	uint32_t size;
	uint32_t max_size;
	dictionary_entry* entries;
} __attribute__((__packed__)) dictionary;

/****************
==== FUNCTIONS ==== 
****************/
/** CLONING FUNCTIONS **/
automaton_signal_event* automaton_signal_event_clone(automaton_signal_event* source);
automaton_alphabet* automaton_alphabet_clone(automaton_alphabet* source);
automaton_transition* automaton_transition_clone(automaton_transition* source);
automaton_fluent* automaton_fluent_clone(automaton_fluent* source);
automaton_valuation* automaton_valuation_clone(automaton_valuation* source);
automaton_automata_context* automaton_automata_context_clone(automaton_automata_context* source);
automaton_automaton* automaton_automaton_clone(automaton_automaton* source);
automaton_automata* automaton_automata_clone(automaton_automata* source);
/** COPYING FUNCTIONS **/
void automaton_signal_event_copy(automaton_signal_event* source,automaton_signal_event* target);
void automaton_alphabet_copy(automaton_alphabet* source,automaton_alphabet* target);
void automaton_transition_copy(automaton_transition* source,automaton_transition* target);
void automaton_fluent_copy(automaton_fluent* source,automaton_fluent* target);
void automaton_valuation_copy(automaton_valuation* source,automaton_valuation* target);
void automaton_automata_context_copy(automaton_automata_context* source,automaton_automata_context* target);
void automaton_automaton_copy(automaton_automaton* source,automaton_automaton* target);
void automaton_automata_copy(automaton_automata* source,automaton_automata* target);
/** CREATE FUNCTIONS **/
automaton_signal_event* automaton_signal_event_create(char* name, automaton_signal_type type);
automaton_alphabet* automaton_alphabet_create();
automaton_transition* automaton_transition_create(uint32_t from_state, uint32_t to_state);
automaton_fluent* automaton_fluent_create(char* name, bool initial_valuation);
automaton_valuation* automaton_valuation_create(uint32_t state);
automaton_automata_context* automaton_automata_context_create(char* name, automaton_alphabet* alphabet, uint32_t fluents_count, automaton_fluent** fluents);
automaton_automaton* automaton_automaton_create(char* name, automaton_automata_context* ctx, uint32_t local_alphabet_count, uint32_t* local_alphabet);
/** INIT FUNCTIONS **/
void automaton_signal_event_initialize(automaton_signal_event* signal_event, char* name, automaton_signal_type type);
void automaton_alphabet_initialize(automaton_alphabet* alphabet);
void automaton_transition_initialize(automaton_transition* transition, uint32_t from_state, uint32_t to_state);
void automaton_fluent_initialize(automaton_fluent* fluent, char* name, bool initial_valuation);
void automaton_valuation_initialize(automaton_valuation* valuation, uint32_t state);
void automaton_automata_context_initialize(automaton_automata_context* ctx, char* name, automaton_alphabet* alphabet, uint32_t fluents_count, automaton_fluent** fluents);
void automaton_automaton_initialize(automaton_automaton* automaton, char* name, automaton_automata_context* ctx, uint32_t local_alphabet_count, uint32_t* local_alphabet);
/** DESTROY FUNCTIONS **/
void automaton_signal_event_destroy(automaton_signal_event* signal_event, bool freeBase);
void automaton_alphabet_destroy(automaton_alphabet* alphabet);
void automaton_transition_destroy(automaton_transition* transition);
void automaton_fluent_destroy(automaton_fluent* fluent, bool freeBase);
void automaton_valuation_destroy(automaton_valuation* valuation);
void automaton_automata_context_destroy(automaton_automata_context* ctx);
void automaton_automaton_destroy(automaton_automaton* automaton);
/** PRINTING FUNCTIONS **/
void automaton_signal_type_print(automaton_signal_type type, char* prefix, char* suffix);
void automaton_signal_event_print(automaton_signal_event* signal_event, char* prefix, char* suffix);
void automaton_alphabet_print(automaton_alphabet* alphabet, char* prefix, char* suffix);
void automaton_transition_print(automaton_transition* transition, automaton_automata_context* ctx, char* prefix, char* suffix);
void automaton_fluent_print(automaton_fluent* fluent, automaton_automata_context* ctx, char* prefix, char* suffix);
void automaton_valuation_print(automaton_valuation* valuation, automaton_automata_context* ctx, char* prefix, char* suffix);
void automaton_automata_context_print(automaton_automata_context* ctx, char* prefix, char* suffix);
void automaton_automaton_print(automaton_automaton* current_automaton, bool print_ctx, bool print_alphabet, bool print_valuations, char* prefix, char* suffix);
/** ALPHABET **/
bool automaton_alphabet_has_signal_event(automaton_alphabet* alphabet, automaton_signal_event* signal_event);
bool automaton_alphabet_add_signal_event(automaton_alphabet* alphabet, automaton_signal_event* signal_event);
uint32_t automaton_alphabet_get_signal_index(automaton_alphabet* alphabet, automaton_signal_event* signal_event);
/** TRANSITION **/
bool automaton_transition_has_signal_event(automaton_transition* transition, automaton_automata_context* ctx, automaton_signal_event* signal_event);
bool automaton_transition_add_signal_event(automaton_transition* transition, automaton_automata_context* ctx, automaton_signal_event* signal_event);
/** FLUENT **/
bool automaton_fluent_has_starting_signal(automaton_fluent* fluent, automaton_alphabet* alphabet, automaton_signal_event* signal_event);
bool automaton_fluent_add_starting_signal(automaton_fluent* fluent, automaton_alphabet* alphabet, automaton_signal_event* signal_event);
bool automaton_fluent_has_ending_signal(automaton_fluent* fluent, automaton_alphabet* alphabet, automaton_signal_event* signal_event);
bool automaton_fluent_add_ending_signal(automaton_fluent* fluent, automaton_alphabet* alphabet, automaton_signal_event* signal_event);
/** VALUATION **/
bool automaton_valuation_has_fluent(automaton_valuation* valuation, automaton_automata_context* ctx, automaton_fluent* fluent);
bool automaton_valuation_add_fluent(automaton_valuation* valuation, automaton_automata_context* ctx, automaton_fluent* fluent);
/** AUTOMATA_CONTEXT **/
uint32_t automaton_automata_context_get_fluent_index(automaton_automata_context* ctx, automaton_fluent* fluent);
/** AUTOMATON OPERATIONS **/
uint32_t automaton_automaton_get_in_degree(automaton_automaton* current_automaton, uint32_t state);
uint32_t automaton_automaton_get_out_degree(automaton_automaton* current_automaton, uint32_t state);
void automaton_automaton_resize_to_state(automaton_automaton* current_automaton, uint32_t state);
automaton_transition* automaton_automaton_get_transitions(automaton_automaton* current_automaton, uint32_t state);
bool automaton_automaton_add_transition(automaton_automaton* current_automaton, automaton_transition* transition);
bool automaton_automaton_has_transition(automaton_automaton* current_automaton, automaton_transition* transition);
bool automaton_automaton_remove_transition(automaton_automaton* current_automaton, automaton_transition* transition);
bool automaton_automaton_has_state(automaton_automaton* current_automaton, uint32_t state);
bool automaton_automaton_add_initial_state(automaton_automaton* current_automaton, uint32_t state);
/** AUTOMATA OPERATIONS **/
uint32_t automaton_automata_get_composite_state(uint32_t states_count, uint32_t* states);
automaton_automaton* automaton_automata_compose(automaton_automaton** automata, uint32_t automata_count, automaton_synchronization_type type);
bool automaton_automaton_check_invariant(automaton_automaton* current_automaton);
bool automaton_automaton_update_valuation(automaton_automaton* current_automaton);
void automaton_automaton_minimize(automaton_automaton* current_automaton);
bool automaton_automaton_check_reachability(automaton_automaton* current_automaton, automaton_valuation target);
bool automaton_automaton_check_liveness(automaton_automaton* current_automaton, automaton_valuation target);
/** COMPOSITE TREE **/
automaton_composite_tree* automaton_composite_tree_create(uint32_t key_length);
uint32_t automaton_composite_tree_get_key(automaton_composite_tree* tree, uint32_t* composite_key);
void automaton_composite_tree_destroy(automaton_composite_tree* tree);
void automaton_composite_tree_entry_print(automaton_composite_tree_entry* entry, bool comes_from_next, uint32_t* tabs);
void automaton_composite_tree_print(automaton_composite_tree* tree);
/** AUTOMATA **/

#endif
