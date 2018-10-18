#ifndef OBDD_H
#define OBDD_H

#include "automaton_utils.h"
#include "dictionary.h"

#define TRUE_VAR			"1"
#define FALSE_VAR			"0"

#define VAR_NEXT_SUFFIX		"_p"

#define DEBUG_OBDD	0
#define DEBUG_OBDD_VALUATIONS	0
#define DEBUG_OBDD_STATE_TREE	0

#define GET_VAR_INDEX(variable_count, valuation_index, variable_index) ((((variable_count) * (valuation_index) + (variable_index)) * sizeof(bool)))
#define GET_VAR_IN_VALUATION(arr, variable_count, valuation_index, variable_index)	(arr[GET_VAR_INDEX(variable_count, valuation_index, variable_index)])

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
}obdd_mgr;

typedef struct obdd_node_t{
	uint32_t 		var_ID;
	uint32_t		node_ID;
	uint32_t		ref_count;
	struct obdd_node_t*	high_obdd;
	struct obdd_node_t*	low_obdd;
	uint32_t high_predecessors_count;
	struct obdd_node_t** high_predecessors;
	uint32_t low_predecessors_count;
	struct obdd_node_t** low_predecessors;
}obdd_node;

typedef struct obdd_t{
	struct obdd_mgr_t*	mgr;
	struct obdd_node_t*	root_obdd;
	struct obdd_node_t* true_obdd;
	struct obdd_node_t* false_obdd;
}obdd;
/*
typedef struct obdd_partial_automaton_t{
	uint32_t			initial_state;
	obdd_state_tree*	map_tree;
	dictionary*			variables_dictionary;
	uint32_t			transitions_count;
	uint32_t			transitions_size;
	uint32_t*			out_degree;
	uint32_t*			out_size;
	uint32_t**			outgoing_transitions;
	uint32_t			valuations_size;
	uint32_t*			valuations;
}obdd_partial_automaton;*/
/** OBDD COMPOSITE STATE **/
obdd_composite_state* obdd_composite_state_create(uint32_t state, uint32_t valuation_count);
uint32_t obdd_composite_state_extractor(void* value);
int32_t obdd_composite_state_compare(void* left_state, void* right_state);
/** MAP TREE **/
obdd_state_tree* obdd_state_tree_create(uint32_t key_length);
int32_t obdd_state_tree_entry_get_from_pool(obdd_state_tree* tree);
uint32_t obdd_state_tree_get_key(obdd_state_tree* tree, bool* valuation, int32_t key_length);
void obdd_state_tree_destroy(obdd_state_tree* tree);
void obdd_state_tree_entry_print(obdd_state_tree* tree, obdd_state_tree_entry* entry);
void obdd_state_tree_print(obdd_state_tree* tree);
/** GLOBAL **/
uint32_t get_next_mgr_ID();
/** OBDD  MGR**/
obdd_mgr*	obdd_mgr_create();
void obdd_mgr_destroy(obdd_mgr* mgr);
void obdd_mgr_print(obdd_mgr* mgr);
obdd* 	obdd_mgr_true(obdd_mgr* mgr);
obdd* 	obdd_mgr_false(obdd_mgr* mgr);
obdd*	obdd_mgr_not_var(obdd_mgr* mgr, char* name);
obdd*	obdd_mgr_var(obdd_mgr* mgr, char* name);
bool obdd_mgr_equals(obdd_mgr* mgr, obdd* left, obdd* right);

uint32_t obdd_mgr_get_next_node_ID(obdd_mgr* mgr);
obdd_node* obdd_mgr_mk_node(obdd_mgr* mgr, char* var, obdd_node* high, obdd_node* low);
void obdd_node_destroy(obdd_node* root);
/** OBDD **/
obdd_node* obdd_node_get_false_node(obdd_mgr* mgr, obdd_node* node);
obdd_node* obdd_node_get_true_node(obdd_mgr* mgr, obdd_node* node);
obdd* obdd_create(obdd_mgr* mgr, obdd_node* root);
obdd_node* obdd_node_clone(obdd_mgr* mgr, obdd_node* root);
obdd* obdd_clone(obdd* root);
void obdd_destroy(obdd* root);
void obdd_add_high_succesor(obdd_node* src, obdd_node* dst);
void obdd_add_low_succesor(obdd_node* src, obdd_node* dst);
void obdd_remove_high_succesor(obdd_node* src, obdd_node* dst);
void obdd_remove_low_succesor(obdd_node* src, obdd_node* dst);

/** CORE COMPUTATION **/
obdd* obdd_restrict(obdd* root, char* var, bool value);
obdd_node* obdd_node_restrict(obdd_mgr* mgr, obdd_node* root, char* var, uint32_t var_ID, bool value);
obdd* obdd_restrict_vector(obdd* root, uint32_t* var_ids, bool* values, uint32_t count);
obdd_node* obdd_node_restrict_vector(obdd_mgr* mgr, obdd_node* root, uint32_t* var_ids, bool* values, uint32_t current_index, uint32_t count);
obdd* obdd_exists(obdd* root, char* var);					//apply reduction based on shannon
obdd* obdd_forall(obdd* root, char* var);					//apply reduction based on shannon
void obdd_print(obdd* root);
void obdd_node_print(obdd_mgr* mgr, obdd_node* root, uint32_t spaces);
void obdd_node_destroy(obdd_node* node);

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
obdd_node* obdd_node_apply(bool (*apply_fkt)(bool,bool), obdd_mgr* mgr, obdd_node* left_node, obdd_node* right_node);
void obdd_remove_duplicated_terminals(obdd_mgr* mgr, obdd_node* root, obdd_node** true_node, obdd_node** false_node);
void obdd_merge_redundant_nodes(obdd_mgr* mgr, obdd_node* root);
void obdd_reduce(obdd* root);
bool obdd_is_true(obdd_mgr* mgr, obdd_node* root);
bool obdd_is_constant(obdd_mgr* mgr, obdd_node* root);						//checks if representation is constant
bool obdd_is_tautology(obdd_mgr* mgr, obdd_node* root);						//checks if representation is always true
bool obdd_is_sat(obdd_mgr* mgr, obdd_node* root);							//checks if representation is satisfiable
void obdd_print_valuations(obdd_mgr* mgr, bool* valuations, uint32_t valuations_count, uint32_t* valuation_img, uint32_t img_count);
bool* obdd_get_valuations(obdd_mgr* mgr, obdd* root, uint32_t* valuations_count, uint32_t* valuation_img, uint32_t img_count);
void obdd_node_get_obdd_nodes(obdd_mgr* mgr, obdd_node* root, obdd_node*** nodes, uint32_t* nodes_count, uint32_t* nodes_size);
obdd_node** obdd_get_obdd_nodes(obdd_mgr* mgr, obdd* root, uint32_t* nodes_count);
#endif
