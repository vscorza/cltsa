#ifndef OBDD_H
#define OBDD_H

#include "automaton_utils.h"
#include "dictionary.h"
#include "fast_pool.h"

#define TRUE_VAR			"1"
#define FALSE_VAR			"0"

#define VAR_NEXT_SUFFIX		"_p"

#define DEBUG_OBDD	0
#define DEBUG_OBDD_VALUATIONS	1
#define DEBUG_OBDD_STATE_TREE	0

#define OBDD_FRAGMENT_SIZE		100
#define OBDD_FRAGMENTS_SIZE		100
#define OBDD_NODE_FRAGMENT_SIZE	100
#define OBDD_NODE_FRAGMENTS_SIZE	100
#define OBDD_NODE_LIST_SIZE		64

#define OBDD_CACHE_DEBUG		0
#define OBDD_CACHE_SIZE			16//262144
#define OBDD_CACHE_MAX_SIZE		2262144//0
#define OBDD_USE_POOL			1
#define OBDD_MERGE_NODES		1
#define OBDD_TABLE_SLOTS		256
#define OBDD_TABLE_MAX_DENSITY	5

typedef uintptr_t ptruint;

typedef uint16_t obdd_var_size_t;
typedef uint32_t obdd_ref_count_size_t;

/* Primes for cache hash functions. */
#define DD_P1			12582917
#define DD_P2			4256249
#define DD_P3			741457
#define DD_P4			1618033999

#if SIZEOF_VOID_P == 8 && SIZEOF_INT == 4
#define ddHash(f,g,s) \
((((unsigned)(ptruint)(f) * DD_P1 + \
   (unsigned)(ptruint)(g)) * DD_P2) >> (s))
#else
#define ddHash(f,g,s) \
((((unsigned)(f) * DD_P1 + (unsigned)(g)) * DD_P2) >> (s))
#endif

//	Hash function for the cache.
#if SIZEOF_VOID_P == 8 && SIZEOF_INT == 4
#define ddCHash(o,f,g,h,s) \
((((((unsigned)(ptruint)(f) + (unsigned)(ptruint)(o)) * DD_P1 + \
    (unsigned)(ptruint)(g)) * DD_P2 + \
   (unsigned)(ptruint)(h)) * DD_P3) >> (s))
#else
#define ddCHash(o,f,g,h,s) \
((((((unsigned)(f) + (unsigned)(o)) * DD_P1 + (unsigned)(g)) * DD_P2 + \
   (unsigned)(h)) * DD_P3) >> (s))
#endif

//	Hash function for the cache for functions with two operands.
#if SIZEOF_VOID_P == 8 && SIZEOF_INT == 4
#define ddCHash2(o,f,g,s) \
(((((unsigned)(ptruint)(f) + (unsigned)(ptruint)(o)) * DD_P1 + \
   (unsigned)(ptruint)(g)) * DD_P2) >> (s))
#else
#define ddCHash2(o,f,g,s) \
(((((uint64_t)(f) + (uint64_t)(o)) * DD_P1 + (uint64_t)(g)) * DD_P2) >> (s))
#endif

#define GET_VAR_INDEX(variable_count, valuation_index, variable_index) ((((variable_count) * (valuation_index) + (variable_index)) * sizeof(bool)))
#define GET_VAR_IN_VALUATION(arr, variable_count, valuation_index, variable_index)	(arr[GET_VAR_INDEX(variable_count, valuation_index, variable_index)])

extern uint32_t* parser_primed_variables;
extern uint32_t* parser_primed_original_variables;
extern uint32_t parser_primed_variables_size;
extern uint32_t parser_primed_variables_count;

/** STRUCTS **/
/** OBDD COMPOSITE STATE**/
typedef struct obdd_composite_state_str{
	uint32_t state;
	bool valuation[0];
}obdd_composite_state;
/** BINARY MAP TREE **/
typedef struct obdd_state_tree_entry_str{
	bool is_leaf;
	uint32_t leaf_value;
	int32_t low_index;
	int32_t high_index;
} obdd_state_tree_entry;
typedef struct obdd_state_tree_str{
	uint32_t entries_size;
	uint32_t entries_count;
	obdd_state_tree_entry* entries_pool;
	uint32_t key_length;
	uint32_t max_value;
	int32_t first_entry_index;
} obdd_state_tree;
/** OBDD **/
typedef struct obdd_mgr_t {
	uint32_t		ID;
	uint32_t		greatest_node_ID;
	uint32_t		greatest_var_ID;
	struct obdd_t*			true_obdd;
	struct obdd_t*			false_obdd;
	dictionary*		vars_dict;
#if OBDD_USE_POOL
	automaton_fast_pool*	obdd_pool;
	automaton_fast_pool*	nodes_pool;
#endif
	struct obdd_cache_t*	cache;
	struct obdd_table_t*	table;
}obdd_mgr;

typedef struct obdd_node_t{
	obdd_var_size_t 		var_ID;
	//uint32_t		node_ID;
	obdd_ref_count_size_t		ref_count;
	struct obdd_node_t*	high_obdd;
	struct obdd_node_t*	low_obdd;
	struct obdd_node_t *next;
	uint32_t 		fragment_ID;
}obdd_node;

typedef struct obdd_t{
	struct obdd_mgr_t*	mgr;
	struct obdd_node_t*	root_obdd;
	struct obdd_node_t* true_obdd;
	struct obdd_node_t* false_obdd;
	uint32_t 		fragment_ID;
}obdd;

typedef struct obdd_cache_item_t{
	obdd_node *f, *g;
	uintptr_t h;
	obdd_node *data;
}obdd_cache_item;

typedef struct obdd_cache_t{
	obdd_mgr *mgr;
	obdd_cache_item *cache_items;
	uint32_t cache_slots;
	int32_t cache_shift;
	double cache_misses;
	double cache_hits;
	double cache_collisions;
	double cache_inserts;
	double min_hits;
	int32_t cache_slack;
	uint32_t max_cache_hard;

	uint32_t vars_size;
	obdd_node **cache_vars, **cache_neg_vars;
}obdd_cache;

typedef struct obdd_table_t{
	obdd_mgr *mgr;
	uint32_t size;
	uint32_t *slots;
	uint32_t *shift;
	uint32_t *max_keys;
	uint32_t *min_keys;
	obdd_node ***levels;
	uint64_t *levels_composite_counts;
	uint64_t max_live_fast_nodes;
	uint64_t live_fast_nodes;
	uint64_t fast_hits;
	uint64_t fast_misses;
}obdd_table;

/** OBDD COMPOSITE STATE **/
obdd_composite_state* obdd_composite_state_create(uint32_t state, uint32_t valuation_count);
uint32_t obdd_composite_state_extractor(void* value);
int32_t obdd_composite_state_compare(void* left_state, void* right_state);
/** MAP TREE **/
obdd_state_tree* obdd_state_tree_create(uint32_t key_length);
int32_t obdd_state_tree_entry_get_from_pool(obdd_state_tree* tree);
uint32_t obdd_state_tree_get_key(obdd_state_tree* tree, bool* valuation, int32_t key_length);
void obdd_state_tree_destroy(obdd_state_tree* tree);
void obdd_state_tree_entry_print(obdd_state_tree* tree, obdd_state_tree_entry* entry, char *buff, uint32_t buff_size);
void obdd_state_tree_print(obdd_state_tree* tree, char *buff, uint32_t buff_size);
/** GLOBAL **/
uint32_t get_next_mgr_ID();
/** OBDD  MGR**/
obdd_mgr*	obdd_mgr_create();
void obdd_mgr_destroy(obdd_mgr* mgr);
void obdd_mgr_print(obdd_mgr* mgr, char *buff);
obdd* 	obdd_mgr_true(obdd_mgr* mgr);
obdd* 	obdd_mgr_false(obdd_mgr* mgr);
obdd*	obdd_mgr_not_var(obdd_mgr* mgr, char* name);
obdd*	obdd_mgr_not_var_ID(obdd_mgr* mgr, obdd_var_size_t var_ID);
obdd*	obdd_mgr_var(obdd_mgr* mgr, char* name);
obdd*	obdd_mgr_var_ID(obdd_mgr* mgr, obdd_var_size_t var_ID);
bool obdd_mgr_equals(obdd_mgr* mgr, obdd* left, obdd* right);

uint32_t obdd_mgr_get_next_node_ID(obdd_mgr* mgr);
obdd_node* obdd_mgr_mk_node(obdd_mgr* mgr, char* var, obdd_node* high, obdd_node* low);
obdd_node* obdd_mgr_mk_node_ID(obdd_mgr* mgr, obdd_var_size_t var_ID, obdd_node* high, obdd_node* low);
void obdd_node_destroy(obdd_mgr* mgr, obdd_node* root);
/** OBDD **/
obdd_node* obdd_node_get_false_node(obdd_mgr* mgr, obdd_node* node);
obdd_node* obdd_node_get_true_node(obdd_mgr* mgr, obdd_node* node);
obdd* obdd_create(obdd_mgr* mgr, obdd_node* root);
obdd_node* obdd_node_clone(obdd_mgr* mgr, obdd_node* root);
obdd* obdd_clone(obdd* root);
void obdd_destroy(obdd* root);
void obdd_add_high_successor(obdd_node* src, obdd_node* dst);
void obdd_add_low_successor(obdd_node* src, obdd_node* dst);
void obdd_remove_high_successor(obdd_node* src, obdd_node* dst);
void obdd_remove_low_successor(obdd_node* src, obdd_node* dst);

/** CORE COMPUTATION **/
obdd* obdd_exists_vector(obdd* root, uint32_t* var_ids, uint32_t count);
obdd_node* obdd_exists_vector_node(obdd_mgr* mgr, obdd_node* root, uint32_t* var_ids, uint32_t count);
obdd* obdd_restrict(obdd* root, char* var, bool value);
obdd* obdd_restrict_ID(obdd* root, uint32_t var_ID, bool value);
obdd_node* obdd_node_restrict(obdd_mgr* mgr, obdd_node* root, uint32_t var_ID, bool value);
bool obdd_satisfies_vector(obdd* root, uint32_t* var_ids, bool* values, uint32_t count, uint32_t* x_y_order);
obdd* obdd_restrict_vector(obdd* root, uint32_t* var_ids, bool* values, uint32_t count);
obdd_node* obdd_node_restrict_vector(obdd_mgr* mgr, obdd_node* root, uint32_t* var_ids, bool* values, uint32_t current_index, uint32_t count);
obdd* obdd_exists(obdd* root, char* var);					//apply reduction based on shannon
obdd* obdd_forall(obdd* root, char* var);					//apply reduction based on shannon
void obdd_print(obdd* root, char *buff, uint32_t buff_size);
void obdd_node_print(obdd_mgr* mgr, obdd_node* root, uint32_t spaces, char *buff, uint32_t buff_size);

/** REACHABLE STATES **/
obdd* obdd_reachable_states(obdd* theta, obdd* rho);
obdd* obdd_img(obdd *root, uint32_t *primed_vars, uint32_t *original_vars, uint32_t var_count);
obdd* obdd_swap_vars(obdd* root, uint32_t *primed_vars, uint32_t *original_vars, uint32_t var_count);

bool obdd_apply_equals_fkt(bool left, bool right);
bool obdd_apply_xor_fkt(bool left, bool right);
bool obdd_apply_and_fkt(bool left, bool right);
bool obdd_apply_or_fkt(bool left, bool right);

obdd* obdd_apply_not(obdd* value);
obdd_node* obdd_node_apply_next(obdd_mgr* mgr, obdd_node* value);
obdd* obdd_apply_next(obdd* value);

obdd* obdd_apply_equals(obdd* left, obdd* right);
obdd* obdd_apply_xor(obdd* left, obdd* right);
obdd* obdd_apply_and(obdd* left, obdd* right);
obdd* obdd_apply_or(obdd* left, obdd* right);
obdd* obdd_apply(bool (*apply_fkt)(bool,bool), obdd *left, obdd* right);	//if not canonical results should be reduced
obdd_node* obdd_node_apply(bool (*apply_fkt)(bool,bool), obdd_mgr* mgr, obdd_node* left_node, obdd_node* right_node, bool first_call);
void obdd_remove_duplicated_terminals(obdd_mgr* mgr, obdd_node* root, obdd_node** true_node, obdd_node** false_node);
obdd_node* obdd_merge_redundant_nodes(obdd_mgr* mgr, obdd_node* root);
void obdd_reduce(obdd* root);
bool obdd_is_true(obdd_mgr* mgr, obdd_node* root);
bool obdd_is_constant(obdd_mgr* mgr, obdd_node* root);						//checks if representation is constant
bool obdd_is_tautology(obdd_mgr* mgr, obdd_node* root);						//checks if representation is always true
bool obdd_is_sat(obdd_mgr* mgr, obdd_node* root);							//checks if representation is satisfiable
void obdd_print_valuations_names(obdd_mgr* mgr, bool* valuations, uint32_t valuations_count, uint32_t* valuation_img, uint32_t img_count);
void obdd_print_valuations_stdout(obdd_mgr* mgr, bool* valuations, uint32_t valuations_count, uint32_t* valuation_img, uint32_t img_count);
void obdd_print_valuations(obdd_mgr* mgr, bool* valuations, uint32_t valuations_count, uint32_t* valuation_img, uint32_t img_count, char *buff);
void obdd_get_valuations(obdd_mgr* mgr, obdd* root, bool** valuations, uint32_t* valuations_size, uint32_t* valuations_count, uint32_t* valuation_img, uint32_t img_count
		, bool* dont_care_list, bool* partial_valuation, bool* initialized_values, bool* valuation_set, obdd_node** last_nodes);
void obdd_node_get_obdd_nodes(obdd_mgr* mgr, obdd_node* root, obdd_node*** nodes, uint32_t* nodes_count, uint32_t* nodes_size);
obdd_node** obdd_get_obdd_nodes(obdd_mgr* mgr, obdd* root, uint32_t* nodes_count);

/** CACHE **/
obdd_cache* obdd_cache_create(obdd_mgr *mgr, uint32_t cache_size, uint32_t cache_max_size);
void obdd_cache_insert(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g, obdd_node *h, obdd_node *data);
void obdd_cache_insert2(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g, obdd_node *data);
void obdd_cache_insert1(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *data);
obdd_node* obdd_cache_insert_var(obdd_cache *cache, obdd_var_size_t var_id);
obdd_node* obdd_cache_insert_neg_var(obdd_cache *cache, obdd_var_size_t var_id);
obdd_node* obdd_cache_lookup(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g, obdd_node *h);
obdd_node* obdd_cache_lookup2(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g);
obdd_node* obdd_cache_lookup1(obdd_cache *cache, uintptr_t op, obdd_node *f);
obdd_node* obdd_cache_constant_lookup(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g, obdd_node *h);
obdd_node* obdd_cache_lookup_var(obdd_cache *cache, uint32_t var_id);
obdd_node* obdd_cache_lookup_neg_var(obdd_cache *cache, uint32_t var_id);
void obdd_cache_resize(obdd_cache *cache);
void obdd_cache_flush(obdd_cache *cache);
void obdd_cache_destroy(obdd_cache *cache);

/** OBDD CACHE TABLE **/
obdd_table* obdd_table_create(obdd_mgr *mgr);
obdd_node* obdd_table_search_node_ID(obdd_table* table, obdd_var_size_t var_ID, obdd_node* high, obdd_node* low);
obdd_node* obdd_table_mk_node_ID(obdd_table* table, obdd_var_size_t var_ID, obdd_node* high, obdd_node* low);
void obdd_table_node_destroy(obdd_table* table, obdd_node *node);
void obdd_table_resize(obdd_table* table, uint32_t level);
void obdd_table_node_add(obdd_table* table, obdd_node *node);
void obdd_table_destroy(obdd_table *table);
void obdd_table_print_fast_lists(obdd_table* table);
#endif
