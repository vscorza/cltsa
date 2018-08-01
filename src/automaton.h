#ifndef AUTOMATON_H
#define AUTOMATON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include "automaton_utils.h"
#include "parser_utils.h"
#include "bucket_list.h"
#include "ordered_list.h"
#include "max_heap.h"
#include "y.tab.h"

#define COMPOSE_SYNCH	1
#define PARTIAL_SHARE	2
#define FIXED_SIGNALS_COUNT	4
#define SIGNALS_INCREASE_FACTOR 2
#define TRANSITIONS_INITIAL_SIZE 2

#define RANKING_INFINITY	-1
#define INITIAL_RANKING_VALUE 0

#define DEBUG_COMPOSITION 0
#define DEBUG_SYNTHESIS 0
#define DEBUG_COMPOSITE_TREE 0

#define BUCKET_SIZE		1000000
#define FLUENT_BUCKET_SIZE 10000
#define RANKING_BUCKET_SIZE 10000
#define PRINT_PARTIAL_COMPOSITION 1

#define PRINT_PARTIAL_SYNTHESIS 1

#define AUT_TAU_CONSTANT	"__tau__"

#define FLUENT_ENTRY_SIZE	32
#define GET_FLUENTS_ARR_SIZE(fluent_count, trans_size)	(1 + ((fluent_count * trans_size) / FLUENT_ENTRY_SIZE))
#define GET_STATE_FLUENT_INDEX(fluent_count, state_index, fluent_index)	((state_index * fluent_count) + fluent_index)
#define SET_FLUENT_BIT(arr,index)     ( arr[(index/FLUENT_ENTRY_SIZE)] |= (1 << (index%FLUENT_ENTRY_SIZE)) )
#define CLEAR_FLUENT_BIT(arr,index)   ( arr[(index/FLUENT_ENTRY_SIZE)] &= ~(1 << (index%FLUENT_ENTRY_SIZE)) )
#define TEST_FLUENT_BIT(arr,index)    ( arr[(index/FLUENT_ENTRY_SIZE)] & (1 << (index%FLUENT_ENTRY_SIZE)) )

typedef uint16_t signal_t;
typedef uint8_t fluent_count_t;
/****************
==== ENUMS ==== 
****************/

typedef enum aumaton_signal_type_enum{
	INPUT_SIG 	= 0,
	OUTPUT_SIG	= 1,
	INTERNAL_SIG	= 2
} automaton_signal_type;

typedef enum aumaton_synchronization_type_enum{
	CONCURRENT 	= 0,
	INTERLEAVED	= 1,
	SYNCHRONOUS = 2
} automaton_synchronization_type;

/****************
==== STRUCTS ==== 
****************/
/** COMPOSITE STATE TREE **/
typedef struct automaton_composite_tree_entry_str{
	uint32_t value;
	struct automaton_composite_tree_entry_str* next;
	struct automaton_composite_tree_entry_str* succ;
} automaton_composite_tree_entry;
typedef struct automaton_composite_tree_str{
	uint32_t entries_size_count;
	uint32_t entries_composite_size;
	uint32_t* entries_size;
	uint32_t* entries_count;
	uint32_t entries_composite_count;
	automaton_composite_tree_entry** entries_pool;
	uint32_t key_length;
	uint32_t max_value;
	automaton_composite_tree_entry* first_entry;
} automaton_composite_tree;
/** AUTOMATON **/
typedef struct automaton_indexes_valuation_str{
	uint32_t count;
	int32_t	*current_values;
	struct automaton_range_str **ranges;
	int32_t total_combinations;
	int32_t current_combination;
}automaton_indexes_valuation;
typedef struct automaton_range_str{
	char*		name;
	int32_t 	lower_value;
	int32_t 	upper_value;
} automaton_range;
typedef struct automaton_signal_str {
	char*					name;	//signal name
	automaton_signal_type	type;	//signal type, input, output, internal
} automaton_signal_event;

typedef struct automaton_alphabet_str{
	uint32_t				count;	//alphabet element count
	uint32_t				size;
	automaton_signal_event*	list;	//alphabet list
} automaton_alphabet;

typedef struct automaton_transition_str{
	uint32_t	state_from;
	uint32_t	state_to;
	uint32_t	signals_count;
	uint32_t	signals_size;
	signal_t	signals[FIXED_SIGNALS_COUNT];
	signal_t*	other_signals;
	bool 		is_input;
} automaton_transition;

typedef struct automaton_fluent_str{
	char*		name;
	uint32_t	starting_signals_count;
	signal_t*	starting_signals;
	uint32_t	ending_signals_count;
	signal_t*	ending_signals;
	bool		initial_valuation;
} automaton_fluent;

typedef struct automaton_valuation_str{
	uint32_t	state;
	uint32_t	active_fluents_count;
	uint32_t*	active_fluents;
} automaton_valuation;

typedef struct automata_context_str{
	char*				name;
	automaton_alphabet*	global_alphabet;
	uint32_t			global_fluents_count;
	automaton_fluent*	global_fluents;
} automaton_automata_context;
typedef struct automaton_transitions_pool_str{
	uint32_t entries_size_count;
	uint32_t entries_composite_size;
	uint32_t* entries_size;
	uint32_t* entries_count;
	uint32_t entries_composite_count;
	automaton_transition** entries_pool;
}automaton_transitions_pool;
typedef struct automaton_str{
	char*					name;
	automaton_automata_context*		context;
	uint32_t				local_alphabet_count;
	uint32_t*				local_alphabet;
	uint32_t				transitions_size;
	uint32_t				transitions_count;
	uint32_t				transitions_composite_count;
	uint32_t				max_out_degree;
	uint32_t				max_concurrent_degree;
	bool*					is_controllable;
	uint32_t*				out_degree;
	uint32_t*				out_size;
	automaton_transition**	transitions;			// S -> list of transitions (s,s')
	uint32_t*				in_degree;
	uint32_t*				in_size;
	automaton_transition**	inverted_transitions;
	uint32_t				initial_states_count;
	uint32_t*				initial_states;
	bool					is_game;
	uint32_t				valuations_size;
	uint32_t*				valuations;
	automaton_transitions_pool*	transitions_pool;
	automaton_bucket_list**	inverted_valuations;
} automaton_automaton;

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
} automaton_automata;

/** AUX **/
typedef struct automaton_ranking_str{
	uint32_t state;
	int32_t value;
	int32_t assumption_to_satisfy;
} automaton_ranking;
typedef struct automaton_pending_state_str{
	uint32_t state;
	int32_t goal_to_satisfy;
	int32_t value;
	uint32_t timestamp;
} automaton_pending_state;
typedef struct dictionary_entry_str {
    char *key;
    uint32_t value;
}  dictionary_entry;

typedef struct dictionary_str{
	uint32_t size;
	uint32_t max_size;
	dictionary_entry* entries;
}  dictionary;

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
automaton_range* automaton_range_clone(automaton_range* source);
automaton_indexes_valuation* automaton_indexes_valuation_clone(automaton_indexes_valuation* source);
/** COPYING FUNCTIONS **/
void automaton_signal_event_copy(automaton_signal_event* source,automaton_signal_event* target);
void automaton_alphabet_copy(automaton_alphabet* source,automaton_alphabet* target);
void automaton_transition_copy(automaton_transition* source,automaton_transition* target);
void automaton_fluent_copy(automaton_fluent* source,automaton_fluent* target);
void automaton_valuation_copy(automaton_valuation* source,automaton_valuation* target);
void automaton_automata_context_copy(automaton_automata_context* source,automaton_automata_context* target);
void automaton_automaton_copy(automaton_automaton* source,automaton_automaton* target);
void automaton_automata_copy(automaton_automata* source,automaton_automata* target);
void automaton_range_copy(automaton_range* source, automaton_range* target);
void automaton_indexes_valuation_copy(automaton_indexes_valuation* source, automaton_indexes_valuation* target);
/** CREATE FUNCTIONS **/
automaton_signal_event* automaton_signal_event_create(char* name, automaton_signal_type type);
automaton_alphabet* automaton_alphabet_create();
automaton_transition* automaton_transition_create(uint32_t from_state, uint32_t to_state);
automaton_fluent* automaton_fluent_create(char* name, bool initial_valuation);
automaton_valuation* automaton_valuation_create(uint32_t state);
automaton_automata_context* automaton_automata_context_create(char* name, automaton_alphabet* alphabet, uint32_t fluents_count, automaton_fluent** fluents);
automaton_automaton* automaton_automaton_create(char* name, automaton_automata_context* ctx, uint32_t local_alphabet_count, uint32_t* local_alphabet, bool is_game);
automaton_range* automaton_range_create(char* name, uint32_t lower_value, uint32_t upper_value);
automaton_indexes_valuation* automaton_indexes_valuation_create();
/** INIT FUNCTIONS **/
void automaton_signal_event_initialize(automaton_signal_event* signal_event, char* name, automaton_signal_type type);
void automaton_alphabet_initialize(automaton_alphabet* alphabet);
void automaton_transition_initialize(automaton_transition* transition, uint32_t from_state, uint32_t to_state);
void automaton_fluent_initialize(automaton_fluent* fluent, char* name, bool initial_valuation);
void automaton_valuation_initialize(automaton_valuation* valuation, uint32_t state);
void automaton_automata_context_initialize(automaton_automata_context* ctx, char* name, automaton_alphabet* alphabet, uint32_t fluents_count, automaton_fluent** fluents);
void automaton_automaton_initialize(automaton_automaton* automaton, char* name, automaton_automata_context* ctx, uint32_t local_alphabet_count, uint32_t* local_alphabet, bool is_game);
void automaton_range_initialize(automaton_range* range, char* name, uint32_t lower_value, uint32_t upper_value);
/** DESTROY FUNCTIONS **/
void automaton_signal_event_destroy(automaton_signal_event* signal_event, bool freeBase);
void automaton_alphabet_destroy(automaton_alphabet* alphabet);
void automaton_transition_destroy(automaton_transition* transition, bool freeBase);
void automaton_fluent_destroy(automaton_fluent* fluent, bool freeBase);
void automaton_valuation_destroy(automaton_valuation* valuation);
void automaton_automata_context_destroy(automaton_automata_context* ctx);
void automaton_automaton_destroy(automaton_automaton* automaton);
void automaton_range_destroy(automaton_range* range);
void automaton_indexes_valuation_destroy(automaton_indexes_valuation* valuation);
/** PRINTING FUNCTIONS **/
void automaton_signal_type_print(automaton_signal_type type, char* prefix, char* suffix);
void automaton_signal_event_print(automaton_signal_event* signal_event, char* prefix, char* suffix);
void automaton_alphabet_print(automaton_alphabet* alphabet, char* prefix, char* suffix);
void automaton_transition_print(automaton_transition* transition, automaton_automata_context* ctx, char* prefix, char* suffix);
void automaton_fluent_print(automaton_fluent* fluent, automaton_automata_context* ctx, char* prefix, char* suffix);
void automaton_valuation_print(automaton_valuation* valuation, automaton_automata_context* ctx, char* prefix, char* suffix);
void automaton_automata_context_print(automaton_automata_context* ctx, char* prefix, char* suffix);
void automaton_automaton_print(automaton_automaton* current_automaton, bool print_ctx, bool print_alphabet, bool print_valuations, char* prefix, char* suffix);
bool automaton_automaton_print_fsp(automaton_automaton* current_automaton, char* filename);
void automaton_range_print(automaton_range* range, char* prefix, char* suffix);
void automaton_indexes_valuation_print(automaton_indexes_valuation* valuation, char* prefix, char* suffix);
/** INDEXES VALUATION **/
bool automaton_indexes_valuation_has_range(automaton_indexes_valuation* valuation, automaton_range* range);
bool automaton_indexes_valuation_add_range(automaton_indexes_valuation* valuation, automaton_range* range);
int32_t automaton_indexes_valuation_get_value(automaton_indexes_valuation* valuation, char* range_name);
void automaton_indexes_valuation_set_value(automaton_indexes_valuation* valuation, char* range_name, int32_t value);
/** ALPHABET **/
bool automaton_alphabet_has_signal_event(automaton_alphabet* alphabet, automaton_signal_event* signal_event);
bool automaton_alphabet_add_signal_event(automaton_alphabet* alphabet, automaton_signal_event* signal_event);
int32_t automaton_alphabet_get_value_index(automaton_alphabet* alphabet, char* signal_name);
signal_t automaton_alphabet_get_signal_index(automaton_alphabet* alphabet, automaton_signal_event* signal_event);
/** TRANSITION **/
bool automaton_transition_has_signal_event(automaton_transition* transition, automaton_automata_context* ctx, automaton_signal_event* signal_event);
bool automaton_transition_add_signal_event(automaton_transition* transition, automaton_automata_context* ctx, automaton_signal_event* signal_event);
/** FLUENT **/
bool automaton_fluent_has_starting_signal(automaton_fluent* fluent, automaton_alphabet* alphabet, automaton_signal_event* signal_event);
bool automaton_fluent_add_starting_signal(automaton_fluent* fluent, automaton_alphabet* alphabet, automaton_signal_event* signal_event);
bool automaton_fluent_has_ending_signal(automaton_fluent* fluent, automaton_alphabet* alphabet, automaton_signal_event* signal_event);
bool automaton_fluent_add_ending_signal(automaton_fluent* fluent, automaton_alphabet* alphabet, automaton_signal_event* signal_event);
automaton_automaton* automaton_fluent_build_automaton(automaton_automata_context* ctx, uint32_t fluent_index);
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
automaton_automaton* automaton_get_gr1_strategy(automaton_automaton* game_automaton, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count);
/** AUTOMATA OPERATIONS **/
uint32_t automaton_automata_get_composite_state(uint32_t states_count, uint32_t* states);
automaton_automaton* automaton_automata_compose(automaton_automaton** automata, uint32_t automata_count, automaton_synchronization_type type, bool is_game);
bool automaton_automaton_check_invariant(automaton_automaton* current_automaton);
bool automaton_automaton_update_valuation(automaton_automaton* current_automaton);
void automaton_automaton_minimize(automaton_automaton* current_automaton);
bool automaton_automaton_check_reachability(automaton_automaton* current_automaton, automaton_valuation target);
bool automaton_automaton_check_liveness(automaton_automaton* current_automaton, automaton_valuation target);
/** AUTOMATON RANKING AND PENDING **/
uint32_t automaton_ranking_key_extractor(void* ranking);
int32_t automaton_pending_state_compare(void* left_pending_state, void* right_pending_state);
void automaton_pending_state_copy(void* target_pending_state, void* source_pending_state);
automaton_ranking* automaton_ranking_create_infinity(uint32_t current_state, int32_t assumption_to_satisfy);
automaton_ranking* automaton_ranking_create(uint32_t current_state, int32_t assumption_to_satisfy);
void automaton_ranking_destroy(automaton_ranking*  ranking);
uint32_t automaton_pending_state_extractor(void* pending_state);
uint32_t automaton_pending_state_ranking_extractor(void* pending_state);
automaton_pending_state* automaton_pending_state_create(uint32_t current_state, int32_t goal_to_satisfy, int32_t value, uint32_t timestamp);
void automaton_pending_state_destroy(automaton_pending_state*  pending_state);
automaton_ranking* automaton_state_best_successor_ranking(automaton_automaton* game_automaton, uint32_t state, automaton_concrete_bucket_list** ranking
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t* guarantees_indexes);
bool automaton_ranking_gt(automaton_ranking* left, automaton_ranking* right);
bool automaton_ranking_eq(automaton_ranking* left, automaton_ranking* right);
bool automaton_ranking_lt(automaton_ranking* left, automaton_ranking* right);
bool automaton_state_is_stable(automaton_automaton* game_automaton, uint32_t state, automaton_concrete_bucket_list** ranking
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t assumptions_count
		, uint32_t* guarantees_indexes, uint32_t* assumptions_indexes, int32_t first_assumption_index);
void automaton_add_unstable_predecessors(automaton_automaton* game_automaton, automaton_max_heap* pending_list, automaton_concrete_bucket_list* key_list
		, uint32_t state, automaton_concrete_bucket_list** ranking
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t assumptions_count
		, uint32_t* guarantees_indexes, uint32_t* assumptions_indexes, int32_t first_assumption_index, uint32_t timestamp);
void automaton_ranking_increment(automaton_automaton* game_automaton, automaton_concrete_bucket_list** ranking, automaton_ranking* current_ranking, uint32_t ref_state, uint32_t* max_delta
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t assumptions_count
		, uint32_t* guarantees_indexes, uint32_t* assumptions_indexes, uint32_t first_assumption_index, automaton_ranking* target_ranking);
void automaton_ranking_update(automaton_automaton* game_automaton, automaton_concrete_bucket_list** ranking, automaton_ranking* current_ranking, uint32_t* max_delta
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t assumptions_count
		, uint32_t* guarantees_indexes, uint32_t* assumptions_indexes, uint32_t first_assumption_index);
/** COMPOSITE TREE **/
automaton_composite_tree* automaton_composite_tree_create(uint32_t key_length);
automaton_composite_tree_entry* automaton_composite_tree_entry_get_from_pool(automaton_composite_tree* tree);
uint32_t automaton_composite_tree_get_key(automaton_composite_tree* tree, uint32_t* composite_key);
void automaton_composite_tree_destroy(automaton_composite_tree* tree);
void automaton_composite_tree_entry_print(automaton_composite_tree_entry* entry, bool comes_from_next, uint32_t* tabs);
void automaton_composite_tree_print(automaton_composite_tree* tree);
/** AUTOMATA **/

#endif
