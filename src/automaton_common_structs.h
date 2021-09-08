/*
 * automaton_common_structs.h
 *
 *  Created on: May 24, 2020
 *      Author: mariano
 */

#ifndef AUTOMATON_COMMON_STRUCTS_H_
#define AUTOMATON_COMMON_STRUCTS_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>
#include <inttypes.h>
#include <math.h>
#include <sys/time.h>
#include "bucket_list.h"
#include "ordered_list.h"
#include "dictionary.h"
#include "obdd.h"

typedef uint8_t signal_t;
typedef uint8_t fluent_count_t;
typedef uint16_t signal_bit_array_t;
#define TRANSITION_ENTRY_SIZE (sizeof(signal_bit_array_t)*8)
#define FIXED_SIGNALS_COUNT	6//4

#define SYS_TICK	"■■■■sys■"
#define ENV_TICK	"■■■■env■"


/****************
==== ENUMS ====
****************/

typedef enum automaton_signal_type_enum{
	INPUT_SIG 	= 0,
	OUTPUT_SIG	= 1,
	INTERNAL_SIG	= 2
} automaton_signal_type;

typedef enum automaton_synchronization_type_enum{
	CONCURRENT 	= 0,
	ASYNCHRONOUS	= 1,
	SYNCHRONOUS = 2
} automaton_synchronization_type;

typedef enum automaton_import_type_enum{
	IMPORT_INT = 0,
	IMPORT_BOOL = 1,
	IMPORT_STRING = 2
}automaton_import_type;

typedef enum automaton_source_type_enum{
	SOURCE_GAME	= 0x1,
	SOURCE_LTL	= 0x2,
	SOURCE_STRAT= 0x4,
	SOURCE_DIAG	= 0x8,
	SOURCE_GAME_STRAT	= SOURCE_GAME | SOURCE_STRAT,
	SOURCE_GAME_DIAG	= SOURCE_GAME | SOURCE_DIAG,
	SOURCE_LTL_STRAT	= SOURCE_LTL | SOURCE_STRAT,
	SOURCE_LTL_DIAG		= SOURCE_LTL | SOURCE_DIAG
}automaton_source_type;

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
	signal_bit_array_t	signals[FIXED_SIGNALS_COUNT];
} automaton_transition;
typedef struct automaton_fluent_str{
	char*		name;
	uint32_t	starting_signals_count;
	uint32_t*	starting_signals_element_count;
	signal_t**	starting_signals;
	uint32_t	ending_signals_count;
	uint32_t*	ending_signals_element_count;
	signal_t**	ending_signals;
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
	uint32_t			liveness_valuations_count;
	obdd**				liveness_valuations;
	char**				liveness_valuations_names;
	uint32_t			state_valuations_count;
	char**				state_valuations_names;
} automaton_automata_context;
typedef struct automaton_transitions_pool_str{
	uint32_t entries_size_count;
	uint64_t entries_composite_size;
	uint32_t* entries_size;
	uint32_t* entries_count;
	uint64_t entries_composite_count;
	automaton_transition** entries_pool;
}automaton_transitions_pool;
typedef struct automaton_str{
	automaton_source_type	source_type;
	bool					is_game;
	char*					name;
	automaton_automata_context*		context;
	uint32_t				local_alphabet_count;
	uint32_t*				local_alphabet;
	uint64_t				transitions_size;
	uint64_t				transitions_count;
	uint64_t				transitions_composite_count;
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
	uint32_t				state_valuations_size;
	uint32_t*				state_valuations;
	uint32_t				state_valuations_declared_size;
	uint32_t*				state_valuations_declared;
	uint32_t				valuations_size;
	uint32_t*				valuations;
	uint32_t				liveness_valuations_size;
	uint32_t*				liveness_valuations;
	automaton_transitions_pool*	transitions_pool;
	automaton_bucket_list**	inverted_valuations;
	automaton_bucket_list**	liveness_inverted_valuations;
	automaton_bucket_list**	inverted_state_valuations;
	signal_bit_array_t		monitored_mask[FIXED_SIGNALS_COUNT];
	bool					ordered;
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
	int32_t assumption_to_satisfy;
	int64_t value;
} automaton_ranking;
typedef struct automaton_pending_state_str{
	uint32_t state;
	int32_t goal_to_satisfy;
	int32_t assumption_to_satisfy;
	int64_t value;
	uint64_t timestamp;
} automaton_pending_state;


#endif /* AUTOMATON_COMMON_STRUCTS_H_ */
