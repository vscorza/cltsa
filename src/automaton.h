#ifndef AUTOMATON_H
#define AUTOMATON_H


#include "automaton_common_structs.h"
#include "automaton_utils.h"
#include "parser_utils.h"
#include "bucket_list.h"
#include "ordered_list.h"
#include "max_heap.h"
#include "dictionary.h"
#include "obdd.h"
#include "bool_array_hash_table.h"
#include "automaton_composite_hash_table.h"
#include "y.tab.h"

/****************************
 * DEBUG FLAGS
 ****************************/
#define VERBOSE 0
#define DEBUG_COMPOSITION 0
#define DEBUG_SYNTHESIS 0
#define DEBUG_STRATEGY_BUILD 0
#define DEBUG_COMPOSITE_TREE 0
#define DEBUG_UNREAL 0
#define DEBUG_DD 0
#define PRINT_PARTIAL_SYNTHESIS 0
#define PRINT_HTML 0


/****************************
 * FUNCTIONAL FLAGS
 ****************************/

#define COMPOSE_SYNCH	1
#define PARTIAL_SHARE	2
#define SIGNALS_INCREASE_FACTOR (1.5f)//2
#define TRANSITIONS_INITIAL_SIZE 2

#define RANKING_INFINITY	-1
#define INITIAL_RANKING_VALUE 0

#define USE_COMPOSITE_HASH_TABLE 1

#define BUCKET_SIZE		10000//1000000
#define FLUENT_BUCKET_SIZE 1000//10000
#define RANKING_BUCKET_SIZE 10000
#define DISTANCE_BUCKET_SIZE 1000
#define PRINT_PARTIAL_COMPOSITION 0

#define AUT_TAU_CONSTANT	"<>"

#define FLUENT_ENTRY_SIZE	32
#define GET_FLUENTS_ARR_SIZE(fluent_count, trans_size)	(1 + ((fluent_count * trans_size) / FLUENT_ENTRY_SIZE))
#define GET_STATE_FLUENT_INDEX(fluent_count, state_index, fluent_index)	((state_index * fluent_count) + fluent_index)
#define SET_FLUENT_BIT(arr,index)     ( arr[(index/FLUENT_ENTRY_SIZE)] |= (1 << (index%FLUENT_ENTRY_SIZE)) )
#define CLEAR_FLUENT_BIT(arr,index)   ( arr[(index/FLUENT_ENTRY_SIZE)] &= ~(1 << (index%FLUENT_ENTRY_SIZE)) )
#define TEST_FLUENT_BIT(arr,index)    ( arr[(index/FLUENT_ENTRY_SIZE)] & (1 << (index%FLUENT_ENTRY_SIZE)) )
//#define GET_TRANSITION_SIGNAL(t, i)  (((i) < FIXED_SIGNALS_COUNT ? (t)->signals[(i)] : (t)->other_signals[(i)-FIXED_SIGNALS_COUNT]))

#define BITVECTOR_ENTRY_SIZE 8
#define TEST_BITVECTOR_BIT(arr,index)    ( arr[(index/BITVECTOR_ENTRY_SIZE)] & (1 << (index%BITVECTOR_ENTRY_SIZE)) )
#define SET_BITVECTOR_BIT(arr,index)     ( arr[(index/BITVECTOR_ENTRY_SIZE)] |= (1 << (index%BITVECTOR_ENTRY_SIZE)) )
#define CLEAR_BITVECTOR_BIT(arr,index)   ( arr[(index/BITVECTOR_ENTRY_SIZE)] &= ~(1 << (index%BITVECTOR_ENTRY_SIZE)) )

#define TEST_TRANSITION_BIT(t,index)    ( (t->signals[((index+1)/TRANSITION_ENTRY_SIZE)]) & ((signal_bit_array_t)1 << ((index+1)%TRANSITION_ENTRY_SIZE)) )
#define SET_TRANSITION_BIT(t,index)     ( (t->signals[((index+1)/TRANSITION_ENTRY_SIZE)]) |= ((signal_bit_array_t)1 << ((index+1)%TRANSITION_ENTRY_SIZE)) )
#define SET_TRANSITION_MASK_BIT(m,index)     ( (m[((index+1)/TRANSITION_ENTRY_SIZE)]) |= ((signal_bit_array_t)1 << ((index+1)%TRANSITION_ENTRY_SIZE)) )
#define CLEAR_TRANSITION_BIT(t,index)   ( (t->signals[((index+1)/TRANSITION_ENTRY_SIZE)]) &= ~(((signal_bit_array_t)1 << ((index+1)%TRANSITION_ENTRY_SIZE))) )
#define TEST_SIGNAL_ARRAY_BIT(arr,index)    ( (arr[((index+1)/TRANSITION_ENTRY_SIZE)]) & ((signal_bit_array_t)1 << ((index+1)%TRANSITION_ENTRY_SIZE)) )
#define SET_SIGNAL_ARRAY_BIT(arr,index)     ( (arr[((index+1)/TRANSITION_ENTRY_SIZE)]) |= ((signal_bit_array_t)1 << ((index+1)%TRANSITION_ENTRY_SIZE)) )
#define CLEAR_SIGNAL_ARRAY_BIT(arr,index)   ( (arr[((index+1)/TRANSITION_ENTRY_SIZE)]) &= ~(((signal_bit_array_t)1 << ((index+1)%TRANSITION_ENTRY_SIZE))) )
#define GET_TRANSITION_SIGNAL_COUNT(transition) uint32_t signal_count = 0,_p_ = 0;for(_p_ = 0; _p_ < (TRANSITION_ENTRY_SIZE * FIXED_SIGNALS_COUNT) - 1; _p_++){if(TEST_TRANSITION_BIT((transition), _p_))signal_count++;}
#define TRANSITION_IS_INPUT(transition)	(((transition)->signals[0]&((signal_bit_array_t)0x1))==true)
#define TRANSITION_SET_INPUT(transition)	(transition)->signals[0] |= ((signal_bit_array_t)0x1);
#define TRANSITION_CLEAR_INPUT(transition)	(transition)->signals[0] &= ~((signal_bit_array_t)0x1);
#define TRANSITION_EQUALS(t1, t2, result) result = true; result &= t1->state_from == t2->state_from; result &= t1->state_to == t2->state_to; uint32_t _p_ = 0;  for(_p_ = 0; (_p_ < FIXED_SIGNALS_COUNT) && result; _p_++)result &= (t1->signals[_p_] == t2->signals[_p_]);

#define AUT_SER_OBJ_START "<"
#define AUT_SER_OBJ_END ">"
#define AUT_SER_ARRAY_START "["
#define AUT_SER_ARRAY_END "]"
#define AUT_SER_SEP ","

#define AUT_SER_OBJ_START_CHAR '<'
#define AUT_SER_OBJ_END_CHAR '>'
#define AUT_SER_ARRAY_START_CHAR '['
#define AUT_SER_ARRAY_END_CHAR ']'
#define AUT_SER_SEP_CHAR ','

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
automaton_automata_context* automaton_automata_context_create(char* name, automaton_alphabet* alphabet, uint32_t fluents_count, automaton_fluent** fluents, uint32_t liveness_valuations_count, obdd** liveness_valuations
		, char** liveness_valuations_names);
automaton_automaton* automaton_automaton_create(char* name, automaton_automata_context* ctx, uint32_t local_alphabet_count, uint32_t* local_alphabet, bool is_game, bool built_from_ltl);
automaton_range* automaton_range_create(char* name, uint32_t lower_value, uint32_t upper_value);
automaton_indexes_valuation* automaton_indexes_valuation_create();
/** INIT FUNCTIONS **/
void automaton_signal_event_initialize(automaton_signal_event* signal_event, char* name, automaton_signal_type type);
void automaton_alphabet_initialize(automaton_alphabet* alphabet);
void automaton_transition_initialize(automaton_transition* transition, uint32_t from_state, uint32_t to_state);
void automaton_fluent_initialize(automaton_fluent* fluent, char* name, bool initial_valuation);
void automaton_valuation_initialize(automaton_valuation* valuation, uint32_t state);
void automaton_automata_context_initialize(automaton_automata_context* ctx, char* name, automaton_alphabet* alphabet, uint32_t fluents_count, automaton_fluent** fluents, uint32_t liveness_valuations_count, obdd** liveness_valuations
		, char** liveness_valuations_names);
void automaton_automaton_initialize(automaton_automaton* automaton, char* name, automaton_automata_context* ctx, uint32_t local_alphabet_count, uint32_t* local_alphabet, bool is_game, bool built_from_ltl);
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
void automaton_transition_print(automaton_transition* transition, automaton_automata_context* ctx, char* prefix, char* suffix, int link_id);
void automaton_fluent_print(automaton_fluent* fluent, automaton_automata_context* ctx, char* prefix, char* suffix);
void automaton_valuation_print(automaton_valuation* valuation, automaton_automata_context* ctx, char* prefix, char* suffix);
void automaton_automata_context_print(automaton_automata_context* ctx, char* prefix, char* suffix);
void automaton_automaton_print(automaton_automaton* current_automaton, bool print_ctx, bool print_alphabet, bool print_valuations, char* prefix, char* suffix);
bool automaton_automaton_print_fsp(automaton_automaton* current_automaton, char* filename);
bool automaton_automaton_print_dot(automaton_automaton* current_automaton, char* filename);
void automaton_range_print(automaton_range* range, char* prefix, char* suffix);
void automaton_indexes_valuation_print(automaton_indexes_valuation* valuation, char* prefix, char* suffix);
/** SERIALIZING FUNCTIONS **/
void automaton_signal_event_serialize_report(FILE *f, automaton_signal_event *evt);
void automaton_fluent_serialize_report(FILE *f, automaton_fluent *fluent);
void automaton_automata_context_serialize_report(FILE *f, automaton_automata_context *ctx);
void automaton_alphabet_serialize_report(FILE *f, automaton_alphabet *alphabet);
void automaton_transition_serialize_report(FILE *f, automaton_transition *transition);
void automaton_automaton_serialize_report(FILE *f, automaton_automaton *automaton);
bool automaton_automaton_print_report(automaton_automaton *automaton, char *filename);
void automaton_ranking_alphabet_serialize_report(FILE *f, automaton_alphabet *alphabet, uint32_t max_delta);
void automaton_ranking_automata_context_serialize_report(FILE *f, automaton_automata_context *ctx, uint32_t max_delta);
void automaton_ranking_transition_serialize_report(FILE *f, automaton_transition *transition,uint32_t ranking_value);
bool automaton_ranking_print_report(automaton_automaton *automaton,
		automaton_concrete_bucket_list** ranking_list, uint32_t* max_delta, uint32_t guarantee_count,
		char **guarantees);
/** AUTOMATON IMPORTER **/
char *automaton_automaton_copy_string(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer);
char *automaton_automaton_load_string(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer);
uint32_t automaton_automaton_load_int(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer);
bool automaton_automaton_load_bool(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer);
void *automaton_automaton_load_array(FILE *f, void* (*load_function)(FILE*, char*, uint32_t, char**, uint32_t*, char*),
		char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count, automaton_import_type type) ;
void **automaton_automaton_load_array_array(FILE *f, void* (*load_function)(FILE*, char*, uint32_t, char**, uint32_t*, char*),
		char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count, uint32_t **inner_count, automaton_import_type type);
bool *automaton_automaton_load_bool_array(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count);
bool **automaton_automaton_load_bool_array_array(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count, uint32_t *inner_count);
uint32_t *automaton_automaton_load_int_array(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count);
uint32_t *automaton_automaton_load_int_array_array(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count, uint32_t *inner_count);
char **automaton_automaton_load_string_array(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count);
char ***automaton_automaton_load_string_array_array(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count, uint32_t *inner_count);
void automaton_automaton_check_alphabet(FILE *f, automaton_alphabet *alphabet, char **buf, uint32_t *buf_size,
		char *last_finalizer);
void automaton_automaton_check_load_context(FILE *f, automaton_automata_context *ctx, char **buf, uint32_t *buf_size,
		char *last_finalizer);
automaton_signal_event *automaton_automaton_load_signal_event(FILE *f, char **buf, uint32_t *buf_size,
		char *last_finalizer);
automaton_transition *automaton_automaton_load_transition(FILE *f, automaton_automata_context *ctx,
		uint32_t *local_alphabet, bool *input_alphabet,
		char **buf, uint32_t *buf_size, char *last_finalizer);
automaton_automaton *automaton_automaton_load_report(automaton_automata_context *ctx, char *filename);
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
void automaton_transition_set_from_to(automaton_transition* transition, uint32_t from_state, uint32_t to_state);
bool automaton_transition_has_signal_event(automaton_transition* transition, automaton_automata_context* ctx, automaton_signal_event* signal_event);
bool automaton_transition_has_signal_event_ID(automaton_transition* transition, automaton_automata_context* ctx, uint32_t signal_index);
bool automaton_transition_add_signal_event(automaton_transition* transition, automaton_automata_context* ctx, automaton_signal_event* signal_event);
bool automaton_transition_add_signal_event_ID(automaton_transition* transition, automaton_automata_context* ctx, uint32_t index, automaton_signal_type type);
/** FLUENT **/
bool automaton_fluent_has_starting_signals(automaton_fluent* fluent, automaton_alphabet* alphabet, uint32_t signal_count, automaton_signal_event** signal_events);
bool automaton_fluent_add_starting_signals(automaton_fluent* fluent, automaton_alphabet* alphabet, uint32_t signal_count, automaton_signal_event** signal_events);
bool automaton_fluent_has_ending_signals(automaton_fluent* fluent, automaton_alphabet* alphabet, uint32_t signal_count, automaton_signal_event** signal_events);
bool automaton_fluent_add_ending_signals(automaton_fluent* fluent, automaton_alphabet* alphabet, uint32_t signal_count, automaton_signal_event** signal_events);
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
bool automaton_automaton_transition_monitored_lt(automaton_automaton* current_automaton, automaton_transition* left, automaton_transition* right);
bool automaton_automaton_transition_monitored_eq(automaton_automaton* current_automaton, automaton_transition* left, automaton_transition* right);
bool automaton_automaton_transition_lt(automaton_automaton* current_automaton, automaton_transition* left, automaton_transition* right);
bool automaton_automaton_transition_eq(automaton_automaton* current_automaton, automaton_transition* left, automaton_transition* right);
void automaton_automaton_order_transitions(automaton_automaton* current_automaton);
bool automaton_automaton_add_transition(automaton_automaton* current_automaton, automaton_transition* transition);
bool automaton_automaton_has_transition(automaton_automaton* current_automaton, automaton_transition* transition);
bool automaton_automaton_remove_transition(automaton_automaton* current_automaton, automaton_transition* transition);
bool automaton_automaton_has_state(automaton_automaton* current_automaton, uint32_t state);
bool automaton_automaton_add_initial_state(automaton_automaton* current_automaton, uint32_t state);
uint32_t* automaton_compute_infinity(automaton_automaton* game_automaton, uint32_t assumptions_count,
		uint32_t guarantees_count, uint32_t* assumptions_indexes, uint32_t* guarantees_indexes);
void automaton_get_gr1_liveness_indexes(automaton_automaton* game_automaton, char** assumptions, uint32_t assumptions_count,
		 char** guarantees, uint32_t guarantees_count, uint32_t** assumptions_indexes, uint32_t** guarantees_indexes);
automaton_automaton* automaton_get_gr1_strategy(automaton_automaton* game_automaton, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count, bool print_ranking);
bool automaton_is_gr1_realizable(automaton_automaton* game_automaton, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count);
void automaton_minimization_adjust_steps_report(uint32_t *steps, uint32_t **steps_sizes,
		struct timeval **steps_times, uint32_t *steps_size);
automaton_automaton* automaton_get_gr1_unrealizable_minimization_dd(automaton_automaton* game_automaton, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count, uint32_t *steps, uint32_t **steps_sizes
		, struct timeval **steps_times, uint32_t *steps_size);
automaton_automaton* automaton_get_gr1_unrealizable_minimization_dd2_c_i_complement(automaton_automaton* master, automaton_automaton* minimized, automaton_automaton *inner_automaton
		, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count, uint8_t *partition_bit_vector, uint32_t transitions_kept_size
		, uint32_t partitions_count, uint32_t t_count, uint32_t t_size, uint32_t *t_states, uint32_t *t_indexes
		, bool start_with_complement, uint32_t last_partition
		, uint32_t *steps, uint32_t **steps_sizes, struct timeval **steps_times, uint32_t *steps_size);
automaton_automaton* automaton_get_gr1_unrealizable_minimization_dd2_c_i(automaton_automaton* master, automaton_automaton* minimized, automaton_automaton *inner_automaton
		, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count, uint8_t *partition_bit_vector, uint32_t transitions_kept_size
		, uint32_t partitions_count, uint32_t t_count, uint32_t t_size, uint32_t *t_states, uint32_t *t_indexes
		, bool start_with_complement, uint32_t last_partition
		, uint32_t *steps, uint32_t **steps_sizes, struct timeval **steps_times, uint32_t *steps_size);
automaton_automaton* automaton_get_gr1_unrealizable_minimization_dd2(automaton_automaton* master, automaton_automaton *minimized
		, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count, uint8_t *partition_bit_vector, uint32_t transitions_kept_size, uint32_t paritions_count
		, uint32_t t_count, uint32_t t_size, uint32_t *t_states, uint32_t *t_indexes, bool start_with_complement, uint32_t last_partition, uint32_t *steps, uint32_t **steps_sizes
		, struct timeval **steps_times, uint32_t *steps_size);
automaton_automaton* automaton_get_gr1_unrealizable_minimization(automaton_automaton* game_automaton, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count, uint32_t *steps, uint32_t **steps_sizes
		, struct timeval **steps_times, uint32_t *steps_size);
/** AUTOMATA OPERATIONS **/
uint32_t automaton_automata_get_composite_state(uint32_t states_count, uint32_t* states);
automaton_automaton* automaton_automata_compose(automaton_automaton** automata, automaton_synchronization_type* synch_type, uint32_t automata_count, bool is_game, char *composition_name);
bool automaton_automaton_check_invariant(automaton_automaton* current_automaton);
bool automaton_automaton_update_valuation(automaton_automaton* current_automaton);
automaton_automaton* automaton_automaton_determinize(automaton_automaton* left_automaton);
automaton_automaton* automaton_automaton_obs_minimize(automaton_automaton* left_automaton);
automaton_automaton* automaton_automaton_minimize(automaton_automaton* current_automaton);
bool automaton_automata_are_equivalent(automaton_automaton* left_automaton, automaton_automaton* right_automaton);
void automaton_automaton_print_traces_to_deadlock(automaton_automaton* automaton, uint32_t max_traces);
uint32_t** automaton_automaton_traces_to_deadlock(automaton_automaton* automaton, uint32_t initial_state, uint32_t *count, uint32_t **sizes);
bool automaton_automaton_check_reachability(automaton_automaton* current_automaton, automaton_valuation target);
bool automaton_automaton_check_liveness(automaton_automaton* current_automaton, automaton_valuation target);
uint32_t* automaton_automaton_distance_to_state(automaton_automaton* automaton, uint32_t state);
void automaton_automaton_remove_unreachable_states(automaton_automaton* automaton);
void automaton_automaton_remove_deadlocks(automaton_automaton* automaton);
void automaton_automaton_update_valuations(automaton_automaton* automaton);
//void automaton_compact_states(automaton_automaton* automaton);
/** AUTOMATON RANKING AND PENDING **/
uint32_t automaton_ranking_key_extractor(void* ranking);
void automaton_pending_state_copy(void* target_pending_state, void* source_pending_state);
automaton_ranking* automaton_ranking_create_infinity(uint32_t current_state, int32_t assumption_to_satisfy);
automaton_ranking* automaton_ranking_create(uint32_t current_state, int32_t assumption_to_satisfy);
void automaton_ranking_destroy(automaton_ranking*  ranking);
uint32_t automaton_pending_state_extractor(void* pending_state);
uint32_t automaton_pending_state_ranking_extractor(void* pending_state);
automaton_pending_state* automaton_pending_state_create(uint32_t current_state, int32_t goal_to_satisfy, int32_t assumption_to_satisfy,
		int32_t value, uint32_t timestamp);
void automaton_pending_state_destroy(automaton_pending_state*  pending_state);
automaton_ranking* automaton_state_best_successor_ranking(automaton_automaton* game_automaton, uint32_t state, automaton_concrete_bucket_list** ranking
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t* guarantees_indexes);
bool automaton_ranking_gt(automaton_ranking* left, automaton_ranking* right);
bool automaton_ranking_eq(automaton_ranking* left, automaton_ranking* right);
bool automaton_ranking_lt(automaton_ranking* left, automaton_ranking* right);
bool automaton_state_is_stable(automaton_automaton* game_automaton, uint32_t state, automaton_concrete_bucket_list** ranking
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t assumptions_count
		, uint32_t* guarantees_indexes, uint32_t* assumptions_indexes, int32_t first_assumption_index);
void automaton_add_unstable_predecessors(automaton_automaton* game_automaton, automaton_pending_state_max_heap* pending_list, automaton_concrete_bucket_list* key_list
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
/** INT AUX FUNCT **/
uint32_t automaton_int_extractor(void* value);
int32_t automaton_int_compare(void* left_int, void* right_int);
void automaton_int_copy(void* target_input, void* source_input);
void automaton_pending_state_destroy(automaton_pending_state*  pending_state);
/** AUTOMATA **/
#endif
