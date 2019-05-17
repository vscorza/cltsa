#include "obdd.h"
#include "assert.h"

uint32_t obdd_mgr_greatest_ID = 0;
/** OBDD COMPOSITE STATE **/
obdd_composite_state* obdd_composite_state_create(uint32_t state, uint32_t valuation_count){
	uint32_t alignment_offset	= 4;
	obdd_composite_state* composite_state = malloc(sizeof(uint32_t) + alignment_offset + sizeof(bool) * (valuation_count));
	composite_state->state	= state;
	uint32_t i;
	for(i = 0; i < valuation_count; i++)
		composite_state->valuation[i]	= false;
	return composite_state;
}
uint32_t obdd_composite_state_extractor(void* value){
	return ((obdd_composite_state*)value)->state;
}
int32_t obdd_composite_state_compare(void* left_state, void* right_state){
	uint32_t left_value	= ((obdd_composite_state*)left_state)->state;
	uint32_t right_value	= ((obdd_composite_state*)right_state)->state;

	if(left_value < right_value) return -1;
	if(left_value == right_value) return 0;
	return 1;
}

/** MAP TREE **/
void obdd_state_tree_entry_print(obdd_state_tree* tree, obdd_state_tree_entry* entry){
	if(entry->is_leaf)
		printf("[%d]\n", entry->leaf_value);
	else{
		if(entry->low_index != NULL){
			printf("0");
			obdd_state_tree_entry_print(tree, &(tree->entries_pool[entry->low_index]));
		}
		if(entry->high_index != NULL){
			printf("1");
			obdd_state_tree_entry_print(tree, &(tree->entries_pool[entry->high_index]));
		}
	}
}

void obdd_state_tree_print(obdd_state_tree* tree){
	printf("Binary Map Tree.\n");
	if(tree->first_entry_index != NULL)
		obdd_state_tree_entry_print(tree, tree->first_entry_index);
}

obdd_state_tree* obdd_state_tree_create(uint32_t key_length){
	obdd_state_tree* tree= malloc(sizeof(obdd_state_tree));
	tree->key_length	= key_length;
	tree->max_value		= 1;
	tree->first_entry_index	= -1;
	tree->entries_count	= 0;
	tree->entries_size	= LIST_INITIAL_SIZE;
	tree->entries_pool	= malloc(sizeof(obdd_state_tree_entry) * tree->entries_size);
	return tree;
}
int32_t obdd_state_tree_entry_get_from_pool(obdd_state_tree* tree){
	if(tree->entries_count >= tree->entries_size){

		uint32_t new_size			= tree->entries_size * LIST_INCREASE_FACTOR;
		obdd_state_tree_entry* ptr	= realloc(tree->entries_pool, sizeof(obdd_state_tree_entry) * new_size);
		if(ptr == NULL){
			printf("Could not allocate memory\n");
			exit(-1);
		}else{
			tree->entries_pool	= ptr;
		}
		tree->entries_size			= new_size;
	}
	obdd_state_tree_entry* entry	= &(tree->entries_pool[tree->entries_count++]);
	entry->high_index						= -1;
	entry->low_index						= -1;
	entry->is_leaf					= false;
	entry->leaf_value				= 0;
	return (tree->entries_count - 1);
}
uint32_t obdd_state_tree_get_key(obdd_state_tree* tree, bool* valuation, int32_t key_length){
	uint32_t i,j;
	int32_t current_entry_index, last_entry_index;
	obdd_state_tree_entry* current_entry 	= -1;
	obdd_state_tree_entry* last_entry		= -1;
	if(tree->first_entry_index == -1){
		current_entry_index		= obdd_state_tree_entry_get_from_pool(tree);
		tree->first_entry_index	= current_entry_index;
	}
	last_entry_index				= tree->first_entry_index;
	last_entry						= &(tree->entries_pool[last_entry_index]);
	bool next_is_high;
	uint32_t current_length			= key_length < 1 ? tree->key_length : key_length;
#if DEBUG_OBDD_STATE_TREE
	printf("obdd map[");
	for(i = 0; i < current_length; i++)
		printf("%s", valuation[i]? "1" : "0");
	printf("]:");
#endif
	for(i = 0; i < current_length; i++){
		next_is_high		= valuation[i];

		if((next_is_high && tree->entries_pool[last_entry_index].high_index == -1) || (!next_is_high && tree->entries_pool[last_entry_index].low_index == -1)){
			current_entry_index			= obdd_state_tree_entry_get_from_pool(tree);
			current_entry				= &(tree->entries_pool[current_entry_index]);
			current_entry->high_index	= -1;
			current_entry->low_index	= -1;
			if(next_is_high)
				tree->entries_pool[last_entry_index].high_index = current_entry_index;
			else
				tree->entries_pool[last_entry_index].low_index = current_entry_index;
			if(i == (current_length -1)){
				current_entry->is_leaf		= true;
				current_entry->leaf_value	= tree->max_value++;
#if DEBUG_OBDD_STATE_TREE
				printf("%d\n", current_entry->leaf_value);
#endif
				return current_entry->leaf_value;
			}else{
				current_entry->is_leaf	= false;
			}
		}else{
			current_entry_index	= next_is_high ? tree->entries_pool[last_entry_index].high_index : tree->entries_pool[last_entry_index].low_index;
			current_entry				= &(tree->entries_pool[current_entry_index]);
			if(i == (current_length -1)){
				if(!current_entry->is_leaf){
					current_entry->is_leaf	= true;
					current_entry->leaf_value	= tree->max_value++;
				}
#if DEBUG_OBDD_STATE_TREE
				printf("%d\n", current_entry->leaf_value);
#endif
				return current_entry->leaf_value;
			}
		}
		last_entry_index = current_entry_index;
	}
#if DEBUG_OBDD_STATE_TREE
	printf("NOT FOUND\n");
#endif
	return 0;
}
void obdd_state_tree_destroy(obdd_state_tree* tree){
	tree->max_value 	= 0;
	tree->key_length	= 0;
	tree->entries_size				= 0;
	tree->entries_count				= 0;
	free(tree->entries_pool);
	tree->entries_pool				= NULL;
	free(tree);
}
/** MGR FUNCTIONS **/
uint32_t get_new_mgr_ID(){
	uint32_t previous_ID	= obdd_mgr_greatest_ID;
	obdd_mgr_greatest_ID++;
	return previous_ID;
}

obdd_mgr*	obdd_mgr_create(){
	//instantiate manager
	obdd_mgr* new_mgr	= malloc(sizeof(obdd_mgr));
	new_mgr->ID			= get_new_mgr_ID();
	//create pools
	new_mgr->obdd_pool	= automaton_fast_pool_create(sizeof(obdd), OBDD_FRAGMENTS_SIZE, OBDD_FRAGMENT_SIZE);
	new_mgr->nodes_pool	= automaton_fast_pool_create(sizeof(obdd_node), OBDD_NODE_FRAGMENTS_SIZE, OBDD_NODE_FRAGMENT_SIZE);
	//is initialized in 1 so that we can later check for already deleted nodes
	new_mgr->greatest_node_ID	= 1;
	new_mgr->greatest_var_ID	= 0;
	
	//create variables dict
	new_mgr->vars_dict		= dictionary_create();
	
	//create constant obdds for true and false values
	uint32_t fragment_ID;
	obdd* true_obdd		= automaton_fast_pool_get_instance(new_mgr->obdd_pool, &fragment_ID);//malloc(sizeof(obdd));
	true_obdd->root_obdd= obdd_mgr_mk_node(new_mgr, TRUE_VAR, NULL, NULL);
	true_obdd->root_obdd->ref_count++;
	true_obdd->mgr		= new_mgr;
	true_obdd->true_obdd	= NULL;
	true_obdd->false_obdd	= NULL;
	true_obdd->fragment_ID	= fragment_ID;
	new_mgr->true_obdd	= true_obdd;
	obdd* false_obdd	= automaton_fast_pool_get_instance(new_mgr->obdd_pool, &fragment_ID);//malloc(sizeof(obdd));
	false_obdd->root_obdd= obdd_mgr_mk_node(new_mgr, FALSE_VAR, NULL, NULL);
	false_obdd->root_obdd->ref_count++;
	false_obdd->mgr		= new_mgr;
	false_obdd->true_obdd	= NULL;
	false_obdd->false_obdd	= NULL;
	false_obdd->fragment_ID	= fragment_ID;
	new_mgr->false_obdd	= false_obdd;
	new_mgr->cache		= obdd_cache_create(new_mgr, OBDD_CACHE_SIZE, OBDD_CACHE_MAX_SIZE);
	return new_mgr;
}

void obdd_mgr_destroy(obdd_mgr* mgr){
	if(mgr->vars_dict != NULL){
		dictionary_destroy(mgr->vars_dict);
		mgr->vars_dict 	= NULL;
	}
	mgr->true_obdd->root_obdd->ref_count--;
	obdd_destroy(mgr->true_obdd);
	mgr->true_obdd	= NULL;
	mgr->false_obdd->root_obdd->ref_count--;
	obdd_destroy(mgr->false_obdd);
	mgr->false_obdd	= NULL;
	obdd_cache_destroy(mgr->cache);
	free(mgr->cache);
	mgr->cache		= NULL;
	automaton_fast_pool_destroy(mgr->obdd_pool);
	automaton_fast_pool_destroy(mgr->nodes_pool);
	mgr->obdd_pool	= NULL;
	mgr->nodes_pool	= NULL;
	free(mgr);	
}

void obdd_mgr_print(obdd_mgr* mgr){
	printf("[OBDD MANAGER]\nMgr: %d\n", mgr->ID);
	printf("Mgr.Dict:\n");
	uint32_t i;
	dictionary* dict	= mgr->vars_dict;
	for(i = 0; i < dict->size; i++){
		printf("[%s]:%d\n", dict->entries[i].key, dict->entries[i].value);
	}
}

uint32_t obdd_mgr_get_next_node_ID(obdd_mgr* mgr){
	uint32_t previous_ID	= mgr->greatest_node_ID;
	mgr->greatest_node_ID++;
	return previous_ID;
}

obdd_node* obdd_mgr_mk_node(obdd_mgr* mgr, char* var, obdd_node* high, obdd_node* low){
	uint32_t var_ID		= dictionary_add_entry(mgr->vars_dict, var);
	return obdd_mgr_mk_node_ID(mgr, var_ID, high, low);
}

obdd_node* obdd_mgr_mk_node_ID(obdd_mgr* mgr, obdd_var_size_t var_ID, obdd_node* high, obdd_node* low){
	//obdd_node* new_node	= malloc(sizeof(obdd_node));
	uint32_t fragment_ID;
	obdd_node* new_node	= automaton_fast_pool_get_instance(mgr->nodes_pool, &fragment_ID);
	new_node->var_ID	= var_ID;
	new_node->node_ID	= obdd_mgr_get_next_node_ID(mgr);
	obdd_add_high_successor(new_node, high);
	obdd_add_low_successor(new_node, low);
	new_node->ref_count	= 0;
	new_node->fragment_ID = fragment_ID;
#if DEBUG_OBDD
		printf("(create)[%d]%p <%s>\n",new_node->var_ID, (void*)new_node, var);
#endif
	return new_node;
}

obdd*	obdd_mgr_var(obdd_mgr* mgr, char* name){
	uint32_t var_ID		= dictionary_add_entry(mgr->vars_dict, name);
	return obdd_mgr_var_ID(mgr, var_ID);
}

obdd*	obdd_mgr_var_ID(obdd_mgr* mgr, obdd_var_size_t var_ID){
	obdd* var_obdd	= obdd_create(mgr, NULL);
	if(mgr->cache->cache_vars[var_ID] != NULL){
		var_obdd->root_obdd = mgr->cache->cache_vars[var_ID];
	}else{
		var_obdd->root_obdd = obdd_cache_insert_var(mgr->cache, var_ID);
	}
	var_obdd->root_obdd->ref_count++;
	return var_obdd;	
}

obdd*	obdd_mgr_not_var(obdd_mgr* mgr, char* name){
	uint32_t var_ID		= dictionary_add_entry(mgr->vars_dict, name);
	return obdd_mgr_not_var_ID(mgr, var_ID);
}

obdd*	obdd_mgr_not_var_ID(obdd_mgr* mgr, obdd_var_size_t var_ID){
	obdd* var_obdd	= obdd_create(mgr, NULL);
	if(mgr->cache->cache_neg_vars[var_ID] != NULL){
		var_obdd->root_obdd = mgr->cache->cache_neg_vars[var_ID];
	}else{
		var_obdd->root_obdd = obdd_cache_insert_neg_var(mgr->cache, var_ID);
	}
	var_obdd->root_obdd->ref_count++;
	return var_obdd;
}

obdd*	obdd_mgr_true(obdd_mgr* mgr){ return mgr->true_obdd; }
obdd*	obdd_mgr_false(obdd_mgr* mgr){ return mgr->false_obdd; }

/** OBDD FUNCTIONS **/
obdd* obdd_create(obdd_mgr* mgr, obdd_node* root){
	//obdd* new_obdd		= malloc(sizeof(obdd));
	uint32_t fragment_ID;
	obdd* new_obdd		= automaton_fast_pool_get_instance(mgr->obdd_pool, &fragment_ID);
	new_obdd->mgr		= mgr;
	new_obdd->root_obdd	= root;
	if(root!= NULL)
		root->ref_count++;
	new_obdd->true_obdd	= mgr->true_obdd->root_obdd;
	new_obdd->false_obdd= mgr->false_obdd->root_obdd;
	new_obdd->fragment_ID	= fragment_ID;
	return new_obdd;
}

obdd_node* obdd_node_clone(obdd_mgr* mgr, obdd_node* root){
	if(obdd_is_constant(mgr, root))
		return root;
	obdd_node* clone	= obdd_mgr_mk_node(mgr, dictionary_key_for_value(mgr->vars_dict, root->var_ID)
			, obdd_node_clone(mgr, root->high_obdd), obdd_node_clone(mgr, root->low_obdd));
	return clone;
}

obdd* obdd_clone(obdd* root){
	//obdd* clone	= malloc(sizeof(obdd));
	uint32_t fragment_ID;
	obdd* clone			= automaton_fast_pool_get_instance(root->mgr->obdd_pool, &fragment_ID);
	clone->false_obdd	= root->false_obdd;
	clone->true_obdd	= root->true_obdd;
	clone->mgr			= root->mgr;
	clone->root_obdd	= obdd_node_clone(root->mgr, root->root_obdd);
	clone->fragment_ID	= fragment_ID;
	obdd_reduce(clone);
	return clone;
}

void obdd_destroy(obdd* root){
	if(root->root_obdd != NULL){
		root->root_obdd->ref_count--;
		obdd_node_destroy(root->mgr, root->root_obdd);
		root->root_obdd		= NULL;
	}
	obdd_mgr* mgr		= root->mgr;
	//must not delete true and false obdd since they are being destroyed by recursive call on root
	root->true_obdd	= NULL;
	root->false_obdd	= NULL;
	root->mgr			= NULL;
	//free(root);
	automaton_fast_pool_release_instance(mgr->obdd_pool, root->fragment_ID);
}

void obdd_add_high_successor(obdd_node* src, obdd_node* dst){
	src->high_obdd	= dst;
	if(dst != NULL){
		dst->ref_count++;
#if DEBUG_OBDD
		printf("(++)[%d]%p -{1}-> [%d]%p (ref:%d)\n", src->var_ID, (void*)src, dst->var_ID,(void*)dst, dst->ref_count);
#endif
	}else{
#if DEBUG_OBDD
		printf("High successor is null\n");
#endif
	}
}
void obdd_add_low_successor(obdd_node* src, obdd_node* dst){
	src->low_obdd	= dst;
	if(dst != NULL){
		dst->ref_count++;
#if DEBUG_OBDD
		printf("(++)[%d]%p -{0}-> [%d]%p (ref:%d)\n", src->var_ID, (void*)src, dst->var_ID,(void*)dst, dst->ref_count);
#endif
	}else{
#if DEBUG_OBDD
		printf("Low successor is null\n");
#endif
	}
}
void obdd_remove_high_successor(obdd_node* src, obdd_node* dst){
	src->high_obdd	= NULL;
	if(dst != NULL){
		dst->ref_count--;
#if DEBUG_OBDD
		printf("(--)[%d]%p -{1}-> [%d]%p (ref:%d)\n", src->var_ID, (void*)src, dst->var_ID, (void*)dst, dst->ref_count);
#endif
	}
}
void obdd_remove_low_successor(obdd_node* src, obdd_node* dst){
	src->low_obdd	= NULL;
	if(dst != NULL){
		dst->ref_count--;
#if DEBUG_OBDD
		printf("(--)[%d]%p -{0}-> [%d]%p (ref:%d)\n", src->var_ID, (void*)src, dst->var_ID, (void*)dst, dst->ref_count);
#endif
	}
}


bool obdd_apply_equals_fkt(bool left, bool right)	{ 	return left == right;	}
bool obdd_apply_xor_fkt(bool left, bool right)	{	return left ^ right;	}
bool obdd_apply_and_fkt(bool left, bool right)	{	return left && right;	}
bool obdd_apply_or_fkt(bool left, bool right)	{	return left || right;	}

obdd* obdd_apply_not(obdd* value){
	return obdd_apply_xor(value->mgr->true_obdd, value);
}

obdd_node* obdd_node_apply_next(obdd_mgr* mgr, obdd_node* value){
	dictionary* dict	= mgr->vars_dict;
	char var_next[200];
	strcpy(var_next, dictionary_key_for_value(dict, value->var_ID));
	strcat(var_next, VAR_NEXT_SUFFIX);

	obdd_node *cached_node	= obdd_cache_lookup1(mgr->cache, obdd_node_apply_next, value);

	if(cached_node != NULL)
		return cached_node;

	uint32_t var_ID		= dictionary_add_entry(dict,  var_next);
	obdd_node *high_value	= (value->high_obdd != NULL && !obdd_is_constant(mgr, value->high_obdd))?
				obdd_node_apply_next(mgr, value->high_obdd) : value->high_obdd;
	obdd_node *low_value	= (value->low_obdd != NULL && !obdd_is_constant(mgr, value->low_obdd))?
					obdd_node_apply_next(mgr, value->low_obdd) : value->low_obdd;
	obdd_node *next_value	= 	obdd_mgr_mk_node(mgr, var_next
			, high_value, low_value);

	obdd_cache_insert1(mgr->cache, obdd_node_apply_next, value, next_value);

	return value;
}


obdd* obdd_apply_next(obdd* value){
	obdd_node_apply_next(value->mgr, value->root_obdd);
	return value;
}

obdd* obdd_apply_equals(obdd* left, obdd* right){
	return obdd_apply(&obdd_apply_equals_fkt, left, right);
}

obdd* obdd_apply_xor(obdd* left, obdd* right){
	return obdd_apply(&obdd_apply_xor_fkt, left, right);
}

obdd* obdd_apply_and(obdd* left, obdd* right){
	return obdd_apply(&obdd_apply_and_fkt, left, right);
}

obdd* obdd_apply_or(obdd* left, obdd* right){
	return obdd_apply(&obdd_apply_or_fkt, left, right);
}

void obdd_remove_duplicated_terminals(obdd_mgr* mgr, obdd_node* root, obdd_node** true_node, obdd_node** false_node){
	if(obdd_is_constant(mgr, root))
		return;
	if(obdd_is_constant(mgr, root->high_obdd)){
		if(obdd_is_true(mgr, root->high_obdd)){
			if(*true_node == NULL){
				*true_node	= root->high_obdd;
			}else{
				obdd_remove_high_successor(root, root->high_obdd);
				obdd_add_high_successor(root, *(true_node));
			}
		}else{
			if(*false_node == NULL){
				*false_node	= root->high_obdd;
			}else{
				obdd_remove_high_successor(root, root->high_obdd);
				obdd_add_high_successor(root, *(false_node));
			}
		}
	}else{
		obdd_remove_duplicated_terminals(mgr, root->high_obdd, true_node, false_node);
	}
	if(obdd_is_constant(mgr, root->low_obdd)){
		if(obdd_is_true(mgr, root->low_obdd)){
			if(*true_node == NULL){
				*true_node	= root->low_obdd;
			}else{
				obdd_remove_low_successor(root, root->low_obdd);
				obdd_add_low_successor(root, *(true_node));
			}
		}else{
			if(*false_node == NULL){
				*false_node	= root->low_obdd;
			}else{
				obdd_remove_low_successor(root, root->low_obdd);
				obdd_add_low_successor(root, *(false_node));
			}
		}
	}else{
		obdd_remove_duplicated_terminals(mgr, root->low_obdd, true_node, false_node);
	}

}

void obdd_merge_redundant_nodes(obdd_mgr* mgr, obdd_node* root){
	if(obdd_is_constant(mgr, root))
		return;
	obdd_merge_redundant_nodes(mgr, root->high_obdd);
	obdd_merge_redundant_nodes(mgr, root->low_obdd);
	int32_t i;
	if(!obdd_is_constant(mgr, root->high_obdd)){
		if(root->high_obdd->high_obdd->node_ID == root->high_obdd->low_obdd->node_ID){
			obdd_node* to_remove	= root->high_obdd;
			obdd_node* to_add		= root->high_obdd->high_obdd;
			obdd_remove_high_successor(root->high_obdd, root->high_obdd->high_obdd);
			obdd_remove_low_successor(root->high_obdd, root->high_obdd->low_obdd);
			obdd_remove_high_successor(root, to_remove);
			obdd_add_high_successor(root, to_add);
			obdd_node_destroy(mgr, to_remove);
		}
	}
	if(!obdd_is_constant(mgr, root->low_obdd)){
		if(root->low_obdd->high_obdd->node_ID == root->low_obdd->low_obdd->node_ID){
			obdd_node* to_remove	= root->low_obdd;
			obdd_node* to_add		= root->low_obdd->low_obdd;
			obdd_remove_high_successor(root->low_obdd, root->low_obdd->high_obdd);
			obdd_remove_low_successor(root->low_obdd, root->low_obdd->low_obdd);
			obdd_remove_low_successor(root, to_remove);
			obdd_add_low_successor(root, to_add);
			obdd_node_destroy(mgr, to_remove);
		}
	}
}

void obdd_reduce(obdd* root){
	obdd_node* true_node	= NULL;
	obdd_node* false_node	= NULL;
	obdd_remove_duplicated_terminals(root->mgr, root->root_obdd, &true_node, &false_node);
	obdd_merge_redundant_nodes(root->mgr, root->root_obdd);
}

obdd* obdd_apply(bool (*apply_fkt)(bool,bool), obdd *left, obdd* right){
	if(left->mgr != right->mgr)
		return NULL;
#if DEBUG_OBDD
		printf("(apply)%p (%p) %p\n", (void*)left, (void*)apply_fkt, (void*)right);
#endif
	obdd_node* cached_node	= obdd_cache_lookup2(left->mgr->cache, apply_fkt, left->root_obdd, right->root_obdd);

	if(cached_node != NULL)
		return obdd_create(left->mgr,cached_node);

	obdd* applied_obdd	= obdd_create(left->mgr, obdd_node_apply(apply_fkt, left->mgr, left->root_obdd, right->root_obdd));
	obdd_reduce(applied_obdd);

	obdd_cache_insert2(left->mgr->cache, apply_fkt, left->root_obdd, right->root_obdd, applied_obdd->root_obdd);

	return applied_obdd;
}	

obdd_node* obdd_node_apply(bool (*apply_fkt)(bool,bool), obdd_mgr* mgr, obdd_node* left_node, obdd_node* right_node){

	obdd_var_size_t left_var_ID	=  left_node->var_ID;
	obdd_var_size_t right_var_ID	=  right_node->var_ID;

	bool is_left_constant		= obdd_is_constant(mgr, left_node);
	bool is_right_constant		= obdd_is_constant(mgr, right_node);

	if(is_left_constant && is_right_constant){
		if((*apply_fkt)(obdd_is_true(mgr, left_node), obdd_is_true(mgr, right_node))){
			return mgr->true_obdd->root_obdd;
		}else{
			return mgr->false_obdd->root_obdd;
		}
	}

	obdd_node* applied_node;

	if(is_left_constant){
		applied_node 	= obdd_mgr_mk_node_ID(mgr, right_var_ID,
			obdd_node_apply(apply_fkt, mgr, left_node, right_node->high_obdd), 
			obdd_node_apply(apply_fkt, mgr, left_node, right_node->low_obdd));
	}else if(is_right_constant){
		applied_node 	= obdd_mgr_mk_node_ID(mgr, left_var_ID,
			obdd_node_apply(apply_fkt, mgr, left_node->high_obdd, right_node), 
			obdd_node_apply(apply_fkt, mgr, left_node->low_obdd, right_node));
	}else if(left_var_ID == right_var_ID){
		applied_node 	= obdd_mgr_mk_node_ID(mgr, left_var_ID,
			obdd_node_apply(apply_fkt, mgr, left_node->high_obdd, right_node->high_obdd), 
			obdd_node_apply(apply_fkt, mgr, left_node->low_obdd, right_node->low_obdd));
	}else if(left_var_ID < right_var_ID){
		applied_node 	= obdd_mgr_mk_node_ID(mgr, left_var_ID,
			obdd_node_apply(apply_fkt, mgr, left_node->high_obdd, right_node), 
			obdd_node_apply(apply_fkt, mgr, left_node->low_obdd, right_node));
	}else{
		applied_node 	= obdd_mgr_mk_node_ID(mgr, right_var_ID,
			obdd_node_apply(apply_fkt, mgr, left_node, right_node->high_obdd), 
			obdd_node_apply(apply_fkt, mgr, left_node, right_node->low_obdd));
	}

	return applied_node;	
}
obdd* obdd_exists_vector(obdd* root, uint32_t* var_ids, uint32_t count){
	return obdd_create(root->mgr, obdd_exists_vector_node(root->mgr, root->root_obdd, var_ids, count));
}

obdd_node* obdd_exists_vector_node(obdd_mgr* mgr, obdd_node* root, uint32_t* var_ids, uint32_t count){
	int32_t i;
	obdd_node* true_node;
	obdd_node* false_node;

	obdd_node* current_node = root;
	obdd_node* tmp_node		= NULL;

	for(i = 0; i < count; i++){
		true_node	= obdd_node_restrict(mgr, current_node, 0, true);
		false_node	= obdd_node_restrict(mgr, current_node, 0, false);

		tmp_node	= current_node;

		current_node	= obdd_node_apply(&obdd_apply_or_fkt, mgr, true_node, false_node);

		if(tmp_node != NULL && tmp_node != root) obdd_node_destroy(mgr, tmp_node);
		obdd_node_destroy(mgr, true_node);
		obdd_node_destroy(mgr, false_node);
	}

	return current_node;
}
obdd* obdd_restrict(obdd* root, char* var, bool value){
	uint32_t var_ID	=  dictionary_value_for_key(root->mgr->vars_dict, var);
	return obdd_restrict_ID(root, var_ID, value);
}

obdd* obdd_restrict_ID(obdd* root, uint32_t var_ID, bool value){
	obdd_node* restricted_node	= obdd_node_restrict(root->mgr, root->root_obdd, var_ID, value);

	return (obdd_create(root->mgr, restricted_node));
}

obdd_node* obdd_node_restrict(obdd_mgr* mgr, obdd_node* root, uint32_t var_ID, bool value){
	bool is_root_constant	= obdd_is_constant(mgr, root);
	uint32_t root_var_ID	= root->var_ID;
	char* root_var		= dictionary_key_for_value(mgr->vars_dict,root_var_ID);

	if(is_root_constant){
		return root;
	}

	uint32_t low_var_ID	=  root->low_obdd->var_ID;
	uint32_t high_var_ID	=  root->high_obdd->var_ID;

	bool is_low_constant		= obdd_is_constant(mgr, root->low_obdd);
	bool is_high_constant		= obdd_is_constant(mgr, root->high_obdd);

	obdd_node* applied_node;

	if(root_var_ID == var_ID){
		if(value){
			applied_node 	= obdd_mgr_mk_node_ID(mgr, high_var_ID
				, root->high_obdd->high_obdd, root->high_obdd->low_obdd); 
		}else{
			applied_node 	= obdd_mgr_mk_node_ID(mgr, low_var_ID
				, root->low_obdd->high_obdd, root->low_obdd->low_obdd);  
		}
	}else{
		applied_node 	= obdd_mgr_mk_node_ID(mgr, root_var_ID
			, obdd_node_restrict(mgr, root->high_obdd, var_ID, value)
			, obdd_node_restrict(mgr, root->low_obdd, var_ID, value));
	}
	return applied_node;	
}

obdd* obdd_restrict_vector(obdd* root, uint32_t* var_ids, bool* values, uint32_t count){
	obdd *acum_obdd = NULL, *new_obdd = NULL, *tmp_obdd = NULL;
	uint32_t i;
	for(i = 0; i < count; i++){
		new_obdd	= (values[i]) ? obdd_mgr_var(root->mgr, dictionary_key_for_value(root->mgr->vars_dict, var_ids[i])) : obdd_mgr_not_var(root->mgr, dictionary_key_for_value(root->mgr->vars_dict, var_ids[i]));
		if(acum_obdd == NULL){
			acum_obdd	= new_obdd;
		}else{
			tmp_obdd	= obdd_apply_and(acum_obdd, new_obdd);
			obdd_destroy(acum_obdd);
			obdd_destroy(new_obdd);
			acum_obdd	= tmp_obdd;
		}
	}
	obdd* return_obdd	=  obdd_apply_and(root, acum_obdd);
	obdd_destroy(acum_obdd);
	return return_obdd;
	/*
	obdd_node* restricted_node	= obdd_node_restrict_vector(root->mgr, root->root_obdd, var_ids, values, 0, count);
	obdd* restricted_obdd = obdd_apply_and(root->mgr, restricted_node);
	obdd_reduce(restricted_obdd);
	return restricted_obdd;
	*/
}
/*
obdd_node* obdd_node_restrict_vector(obdd_mgr* mgr, obdd_node* root, uint32_t* var_ids, bool* values, uint32_t current_index, uint32_t count){
	bool is_root_constant	= obdd_is_constant(mgr, root);

	if(is_root_constant){
		return root;
	}

	obdd_node* applied_node;
	if(root->var_ID == var_ids[current_index]){//current var should be restricted
		if(values[current_index]){
			current_index++;
			applied_node 	= obdd_mgr_mk_node(mgr, dictionary_key_for_value(mgr->vars_dict,root->var_ID)
				, obdd_node_restrict_vector(mgr, root->high_obdd, var_ids, values, current_index, count)
				, mgr->false_obdd->root_obdd);
		}else{
			current_index++;
			applied_node 	= obdd_mgr_mk_node(mgr, dictionary_key_for_value(mgr->vars_dict,root->var_ID)
				, mgr->false_obdd->root_obdd
				, obdd_node_restrict_vector(mgr, root->low_obdd, var_ids, values, current_index, count));
		}
	}else if(root->var_ID > var_ids[current_index]){//current var was not present, add node for current restriction
		uint32_t added_var_ID	= var_ids[current_index];
		if(values[current_index]){
			current_index++;
			applied_node 	= obdd_mgr_mk_node(mgr, dictionary_key_for_value(mgr->vars_dict,added_var_ID)
				, obdd_node_restrict_vector(mgr, root, var_ids, values, current_index, count)
				, mgr->false_obdd->root_obdd);
		}else{
			current_index++;
			applied_node 	= obdd_mgr_mk_node(mgr, dictionary_key_for_value(mgr->vars_dict,added_var_ID)
				, mgr->false_obdd->root_obdd
				, obdd_node_restrict_vector(mgr, root, var_ids, values, current_index, count));
		}
	}else{//variables prior to current restriction
		applied_node 	= obdd_mgr_mk_node(mgr, dictionary_key_for_value(mgr->vars_dict,root->var_ID)
			, obdd_node_restrict_vector(mgr, root->high_obdd, var_ids, values, current_index, count)
			, obdd_node_restrict_vector(mgr, root->low_obdd, var_ids, values, current_index, count));
	}
	return applied_node;
}
*/
obdd* obdd_exists(obdd* root, char* var){ 
	obdd* restrict_true_obdd	= obdd_restrict(root, var, true);
	obdd* restrict_false_obdd	= obdd_restrict(root, var, false);

	obdd* var_or_no_var_obdd	= obdd_apply_or(restrict_true_obdd, restrict_false_obdd);

	obdd_destroy(restrict_true_obdd);
	obdd_destroy(restrict_false_obdd);

	return var_or_no_var_obdd;
}

obdd* obdd_forall(obdd* root, char* var){ 
	obdd* restrict_true_obdd	= obdd_restrict(root, var, true);
	obdd* restrict_false_obdd	= obdd_restrict(root, var, false);

	obdd* var_and_no_var_obdd	= obdd_apply_and(restrict_true_obdd, restrict_false_obdd);

	obdd_destroy(restrict_true_obdd);
	obdd_destroy(restrict_false_obdd);

	return var_and_no_var_obdd;
}

void obdd_print(obdd* root){
	printf("[OBDD]\nMgr_ID:%d\nValue:", root->mgr->ID);
	obdd_node_print(root->mgr, root->root_obdd, 0);
	printf("\n");
}

void obdd_node_print(obdd_mgr* mgr, obdd_node* root, uint32_t spaces){
	char* var			= dictionary_key_for_value(mgr->vars_dict,root->var_ID);
	uint32_t i;

	if(obdd_is_constant(mgr, root)){
		printf("->%s", var);
		return;
	}else if(spaces > 0){
		printf("^");
	}
	printf("\n");
	for(i = 0; i < spaces; i++)
		printf("\t");
	bool has_left_side = false;
/*
	if(root->var_ID != mgr->true_obdd->root_obdd->var_ID && root->var_ID != mgr->false_obdd->root_obdd->var_ID){
		if(root->high_predecessors_count > 0){
			printf("[1 pred:");
			for(i = 0; i < root->high_predecessors_count; i++)
				printf("%s%s", dictionary_key_for_value(mgr->vars_dict,root->high_predecessors[i]->var_ID), i < (root->high_predecessors_count -1) ? "," : "");
			printf("]\n");
		}
		if(root->low_predecessors_count > 0){
			printf("[0 pred:");
			for(i = 0; i < root->low_predecessors_count; i++)
				printf("%s%s", dictionary_key_for_value(mgr->vars_dict,root->low_predecessors[i]->var_ID), i < (root->low_predecessors_count -1) ? "," : "");
			printf("]\n");
		}
	}
	for(i = 0; i < spaces; i++)
		printf("\t");
		*/
	if(root->high_obdd != NULL){
		printf("%s", var);
		obdd_node_print(mgr, root->high_obdd, spaces + 1);
		has_left_side	= true;
	}	
	if(root->low_obdd != NULL){
		if(has_left_side){
			printf("\n");
			for(i = 0; i < spaces; i++)
				printf("\t");
			printf("v");
		}
		printf("(!%s)", var);
		obdd_node_print(mgr, root->low_obdd, spaces + 1);
	}	
}

bool obdd_is_true(obdd_mgr* mgr, obdd_node* root){
	return (root->var_ID == mgr->true_obdd->root_obdd->var_ID);
}
bool obdd_is_constant(obdd_mgr* mgr, obdd_node* root){
	return root->var_ID < 2;
	/*
	return ((root->var_ID == mgr->true_obdd->root_obdd->var_ID) 
		|| (root->var_ID == mgr->false_obdd->root_obdd->var_ID));*/
}

bool obdd_is_tautology(obdd_mgr* mgr, obdd_node* root){
	if(obdd_is_constant(mgr, root)){
		return obdd_is_true(mgr, root);
	}else{
		return obdd_is_tautology(mgr, root->high_obdd) && obdd_is_tautology(mgr, root->low_obdd);
	}
}

bool obdd_is_sat(obdd_mgr* mgr, obdd_node* root){
	if(obdd_is_constant(mgr, root)){
		return obdd_is_true(mgr, root);
	}else{
		return obdd_is_sat(mgr, root->high_obdd) || obdd_is_sat(mgr, root->low_obdd);
	}
}

void obdd_print_valuations(obdd_mgr* mgr, bool* valuations, uint32_t valuations_count, uint32_t* valuation_img, uint32_t img_count){
	uint32_t i, j;
	printf(ANSI_COLOR_GREEN);
	for(i = 0; i < img_count; i++){
		printf("%s\t", mgr->vars_dict->entries[valuation_img[i]].key);
	}
	printf("\n");
	for(i = 0; i < valuations_count; i++){
		for(j = 0; j < (img_count); j++){
			int value = GET_VAR_IN_VALUATION(valuations, img_count, i, j);
			//printf("%s\t",  value > 1 ? "X" : (value != 0 ? "1" : "0"));
			if(value > 1)
				printf("%d[%d %d]\t", value,  i, j);
			else
				printf("%d\t",  value);
		}
		printf("\n");
	}
	printf(ANSI_COLOR_RESET);
}

void obdd_node_get_obdd_nodes(obdd_mgr* mgr, obdd_node* root, obdd_node*** nodes, uint32_t* nodes_count, uint32_t* nodes_size){
	uint32_t i;
	bool found	= false;
	for(i = 0; i < *nodes_count; i++)
		if((*nodes)[i] == root || root == mgr->false_obdd->root_obdd || root == mgr->true_obdd->root_obdd){
			found = true;
			break;
		}
	if(!found){
		if(*nodes_count >= *nodes_size){
			*nodes_size	= (*nodes_size) * LIST_INCREASE_FACTOR;
			obdd_node** ptr	= realloc(*nodes, sizeof(obdd_node*) * (*nodes_size));
			if(ptr == NULL){
				printf("Realloc failed while getting obdd nodes\n");
				exit(-1);
			}
			*nodes		= ptr;
		}
		(*nodes)[*nodes_count]	= root;
		*nodes_count			= *nodes_count + 1;
	}
	if(root->high_obdd != NULL)
		obdd_node_get_obdd_nodes(mgr, root->high_obdd, nodes, nodes_count, nodes_size);
	if(root->high_obdd != NULL)
		obdd_node_get_obdd_nodes(mgr, root->low_obdd, nodes, nodes_count, nodes_size);
}

obdd_node** obdd_get_obdd_nodes(obdd_mgr* mgr, obdd* root, uint32_t* nodes_count){
	uint32_t nodes_size	= LIST_INITIAL_SIZE;
	obdd_node** nodes	= malloc(sizeof(obdd_node*) * nodes_size);
	*nodes_count		= 0;
#if DEBUG_OBDD_VALUATIONS
	printf("Getting OBDD nodes\n");
#endif
	obdd_node_get_obdd_nodes(mgr, root->root_obdd, &nodes, nodes_count, &nodes_size);
#if DEBUG_OBDD_VALUATIONS
	uint32_t i;
	for(i = 0; i < *nodes_count; i++)
		printf("<%p>\n", (void*)nodes[i]);
	printf("\n");
#endif

	return nodes;
}
void obdd_get_valuations(obdd_mgr* mgr, obdd* root, bool** valuations, uint32_t* valuations_size, uint32_t* valuations_count, uint32_t* valuation_img, uint32_t img_count
		, bool* dont_care_list, bool* partial_valuation, bool* initialized_values, bool* valuation_set, obdd_node** last_nodes, int32_t* last_succ_index){
	int32_t i, j, dont_cares_count, variable_index;
	uint32_t nodes_count;
	*valuations_count			= 0;
	uint32_t variables_count	= mgr->vars_dict->size - 2;
	int32_t last_bit_index		= -1;
	for( i = 0; i < (int32_t)variables_count; i++){
		dont_care_list[i]		= true;
		partial_valuation[i]	= false;
		initialized_values[i]	= false;
	}
	int32_t last_node_index	= 0;
	//starts from the TRUE leaf and keeps backtracking
	obdd_node* current_node		= root->root_obdd;
	obdd_node* last_node;

	int32_t current_index		= 0;
	int32_t next_index			= 0;
	bool taking_high			= false;
	bool found_node_to_expand	= false;
#if DEBUG_OBDD_VALUATIONS
	printf(ANSI_COLOR_GREEN);
#endif
	//update the current branch list
	last_nodes[current_index]	= current_node;

	while(partial_valuation[current_index] != true){
		if(!initialized_values[current_index]){
			initialized_values[current_index]	= true;
			taking_high	= false;
		}else{
			partial_valuation[current_index]	= true;
			taking_high	= true;
		}
		dont_care_list[current_index]	= false;

		if(taking_high)current_node		= current_node->high_obdd;
		else	current_node			= current_node->low_obdd;

		if(current_node->var_ID > 1){
			next_index					= current_node->var_ID - 2;
			//update range of variables not fixed by current branch
			for(i = (current_index + 1); i < (next_index - 1); i++)
				dont_care_list[i]		= true;
			current_index				= next_index;
			last_nodes[current_index]	= current_node;
		}else{
			//update range of variables not fixed by current branch
			for(i = (current_index + 1); i < (int32_t)variables_count; i++)
				dont_care_list[i]		= true;
			if(current_node->var_ID == mgr->false_obdd->root_obdd->var_ID){//wrong branch
				//?
			}else if(current_node->var_ID == mgr->true_obdd->root_obdd->var_ID){//found terminal
				//add valuations
				dont_cares_count	= 1;
				//count dont cares to get number of new valuations (only for variables belonging to the image)
				for(i = 0; i < (int32_t)variables_count; i++){
					if(dont_care_list[i]){
						variable_index	= -1;
						for(j = 0; j < (int32_t)img_count; j++)
							if(valuation_img[j] == (uint32_t)i + 2){
								variable_index	= valuation_img[j] - 2;
								break;
							}
						if(variable_index >= 0)
							dont_cares_count *= 2;
					}
				}
				//get new valuations according to dont care list
				uint32_t new_size	= *valuations_count + (dont_cares_count);
				if((new_size * mgr->vars_dict->size * 2) >= *valuations_size){
					bool* ptr	= realloc(*valuations, sizeof(bool) * (new_size * mgr->vars_dict->size * 2));
					if(ptr == NULL){
						printf("Could not reallocate valuations array\n");
						exit(-1);
					}
					*valuations	= ptr;
					*valuations_size	= (new_size * mgr->vars_dict->size * 2);
				}
				uint32_t modulo	= dont_cares_count;

				#if DEBUG_OBDD_VALUATIONS
								printf("[T]erminals on node: %d (%d:%s) :\n", last_node_index, last_nodes[current_index]->var_ID,dictionary_key_for_value(mgr->vars_dict,last_nodes[current_index]->var_ID));

								printf("last_succ_index[%d]:%d\t<", last_node_index, last_succ_index[current_index]);
								for(i = 0; i <= (int32_t)current_index; i++)
									printf("%s", dont_care_list[i]? "?" : (partial_valuation[i]? "1" : "0"));
								for(i = variables_count - 1; i > last_node_index; i--)
									printf("x");
								printf(">\n");
								printf("Partial Valuation\t<");
								for(i = 0; i < (int32_t)variables_count; i++)
									printf("%s", partial_valuation[i]? "1" : "0");
								printf(">\n");
								printf("Dont care list\t<");
								for(i = 0; i < (int32_t)variables_count; i++){
									variable_index	= -1;
									for(j = 0; j < (int32_t)img_count; j++){
										if(valuation_img[j] == (uint32_t)i + 2){
											variable_index	= valuation_img[j] - 2;
											break;
										}
									}
									printf("%s", dont_care_list[i]? (variable_index > -1 ? "?" : "_") : "0");
								}
								printf("> count:%d\n", dont_cares_count);
				#endif

				uint32_t k;
				uint32_t img_index;
				for(k = 0; k < (uint32_t)dont_cares_count; k++){
					for(i = 0; i < (int32_t)img_count; i++)
						valuation_set[i] = false;
					last_bit_index = -1;
					for(i = 0; i < (int32_t)variables_count; i++){
						variable_index	= -1;
						//only setting odd variables
						for(j = 0; j < (int32_t)img_count && variable_index < 0; j++){
							if(valuation_img[j] == (uint32_t)i + 2){
								variable_index	= valuation_img[j] - 2;
								img_index		= j;
								valuation_set[j]	= true;
							}
						}

						if(variable_index >= 0){
							if(dont_care_list[variable_index]){
								last_bit_index++;
								//set according to modulo division if current position was set to dont care
								GET_VAR_IN_VALUATION((*valuations), img_count, *valuations_count + k, img_index)	= (k & (0x1 << last_bit_index)) != 0;
							}else{
								//set to predefined value for this search
								GET_VAR_IN_VALUATION((*valuations), img_count, *valuations_count + k, img_index)	= partial_valuation[variable_index];
							}
						}
					}
					for(j = 0; j < (int32_t)img_count; j++){
						if(!valuation_set[j]){
							printf("Value not set for %s on valuation %d\n", dictionary_key_for_value(mgr->vars_dict, valuation_img[j]), *valuations_count + k );
							exit(-1);
						}
					}
#if DEBUG_OBDD_VALUATIONS
					printf("[");
					for(i = 0; i < (int32_t)img_count; i++){
						bool value = GET_VAR_IN_VALUATION((*valuations), img_count, *valuations_count + k, i);
						bool care_for_value = dont_care_list[valuation_img[i] - 2];
						printf("%s", value ? (care_for_value ? "i" : "1") : (care_for_value ? "o" : "0"));
					}
					printf("]\n");
#endif
				}


				*valuations_count	+= dont_cares_count;
			}//ADD VALUATIONS

			//fire backtrack, check if a node needs to be expanded
			uint32_t previous_index	= current_index;
			while(current_index > 0 && (partial_valuation[current_index] == true || dont_care_list[current_index])){
				current_index--;
			}
			current_node				= last_nodes[current_index];
			//reset backtracked values
			for(i = current_index + 1; i <= previous_index; i++){
				dont_care_list[i]		= false;
				partial_valuation[i]	= false;
				initialized_values[i]	= false;
			}
		}
	}
#if DEBUG_OBDD_VALUATIONS
	printf(ANSI_COLOR_RESET);
#endif
}
/*
bool* obdd_get_valuations(obdd_mgr* mgr, obdd* root, uint32_t* valuations_count, uint32_t* valuation_img, uint32_t img_count){
	int32_t i, j, dont_cares_count;
	uint32_t nodes_count;
	//build predecessors if needed
	//gets a list of the obdd nodes
	obdd_node** nodes	= obdd_get_obdd_nodes(mgr, root, &nodes_count);
	*valuations_count			= 0;
	uint32_t valuations_size	= LIST_INITIAL_SIZE;
	uint32_t variables_count	= mgr->vars_dict->size - 2;
	int32_t last_bit_index		= -1;
	bool* valuations			= calloc(img_count * valuations_size, sizeof(bool));
	bool* dont_care_list		= malloc(sizeof(bool) * variables_count);
	for( i = 0; i < (int32_t)variables_count; i++)
		dont_care_list[i]		= true;
	bool* partial_valuation		= malloc(sizeof(bool) * variables_count);
	for( i = 0; i < (int32_t)variables_count; i++)
			partial_valuation[i]= false;
	bool* valuation_set			= malloc(sizeof(bool) * img_count);
	//keeps a stack of visited nodes
	obdd_node** last_nodes		= malloc(sizeof(obdd_node*) * variables_count);
	//keeps a stack of predecessors as track of the path taken
	int32_t* last_succ_index	= malloc(sizeof(int32_t) * variables_count);
	int32_t last_node_index	= 0;
	int32_t max_pred_index	= 0;
	int32_t min_var_ID		= mgr->vars_dict->size - 3;
	//starts from the TRUE leaf and keeps backtracking
	obdd_node* current_node		= mgr->true_obdd->root_obdd;
	obdd_node* current_predecessor;
	obdd_node* last_node;

	last_nodes[last_node_index]	= current_node;
	last_succ_index[last_node_index]	= 0;
	bool belongs, through_high, has_no_pred;
	bool at_least_one_node_expanded		= true;
#if DEBUG_OBDD_VALUATIONS
	printf("Getting OBDD valuations\n");
	obdd_print(root);
	printf("Projected over:[");
	for(i = 0; i < (int32_t)img_count; i++)
		printf("%d %s %s", valuation_img[i], dictionary_key_for_value(mgr->vars_dict, valuation_img[i]), i == ((int32_t)img_count -1) ? "]\n": ",");

	printf("[N]ow on node: %d (%d:%s) \n", last_node_index, last_nodes[last_node_index]->var_ID,dictionary_key_for_value(mgr->vars_dict,last_nodes[last_node_index]->var_ID));
	printf("High pred. count: %d \tLow pred.count: %d\n", last_nodes[last_node_index]->high_predecessors_count, last_nodes[last_node_index]->low_predecessors_count);
#endif
	//perform dfs exploration of obdd
	while(last_succ_index[last_node_index] != -1){
		last_node	= last_nodes[last_node_index];
		if((last_node->var_ID - 2) < (int32_t)min_var_ID){
					min_var_ID	= last_node->var_ID - 2;
#if DEBUG_OBDD_VALUATIONS
							printf("New [m]in var ID: %d\n", min_var_ID);
#endif
		}
		has_no_pred	= false;
		if((int32_t)(last_node->high_predecessors_count + last_node->low_predecessors_count) == 0){
			has_no_pred	= true;
		}
		if(has_no_pred || last_succ_index[last_node_index] <
				(int32_t)(last_node->high_predecessors_count + last_node->low_predecessors_count)){//check if we can still explore the current node
			//get current predecessor
			//if this is the last node we should check that only current-obdd predecessors are take into account, since many obdds share TRUE/FALSE leaves
			if(!has_no_pred){
				if(last_node_index == 0){
					belongs	= false;
					while(!belongs){
						if(last_succ_index[last_node_index] < (int32_t)last_node->high_predecessors_count){
							//backtrack through high pred.
							current_predecessor	= last_node->high_predecessors[last_succ_index[last_node_index]];
							through_high		= true;
						}else if(last_succ_index[last_node_index] <
								(int32_t)(last_node->high_predecessors_count + last_node->low_predecessors_count)){
							//backtrack through low pred.
							current_predecessor	= last_node->low_predecessors[last_succ_index[last_node_index]  - last_node->high_predecessors_count];
							through_high		= false;
						}else{
							//should only happen after exhaustive backtrack
							last_succ_index[last_node_index]	= -1;
	#if DEBUG_OBDD_VALUATIONS
								printf("NF.exit\n");
	#endif
							break;//breaks while
						}
						for(i = 0; i < (int32_t)nodes_count; i++){
							if(nodes[i] == current_predecessor){
								belongs	= true;
	#if DEBUG_OBDD_VALUATIONS
								printf("F.exit\n");
	#endif
								break;
							}
						}
						//keep looking for proper pred.
						if(!belongs){
							last_succ_index[last_node_index]++;
	#if DEBUG_OBDD_VALUATIONS
							printf("NF.");
	#endif
						}
					}
					//should only happen after exhaustive backtrack
					if(!belongs)break;
				}else{
					if(last_succ_index[last_node_index] < (int32_t)last_node->high_predecessors_count){
						//backtrack through high pred.
						current_predecessor	= last_node->high_predecessors[last_succ_index[last_node_index]];
						through_high		= true;
					}else if(last_succ_index[last_node_index] <
							(int32_t)(last_node->high_predecessors_count + last_node->low_predecessors_count)){
						//backtrack through low pred.
						current_predecessor	= last_node->low_predecessors[last_succ_index[last_node_index]  - last_node->high_predecessors_count];
						through_high		= false;
					}
				}
				//updates structures and keep exploring
				if(last_succ_index[last_node_index] > -1){
					partial_valuation[current_predecessor->var_ID - 2]	= through_high;
					dont_care_list[current_predecessor->var_ID - 2]		= false;
					if((current_predecessor->var_ID - 2) < (int32_t)min_var_ID){
						min_var_ID	= current_predecessor->var_ID - 2;
#if DEBUG_OBDD_VALUATIONS
						printf("New [m]in var ID: %d\n", min_var_ID);
#endif
					}
					//update dont care list for nodes skipped due to obdd reduction
					for(i = (int32_t)(current_predecessor->var_ID - 2)+ 1; i < (int32_t)(last_node->var_ID - 2); i++)
						dont_care_list[i]	= true;
					last_succ_index[last_node_index]++;
					at_least_one_node_expanded		= true;
#if DEBUG_OBDD_VALUATIONS
					printf("E[X]panded %d\n", last_node_index);
#endif
				}
			}else{//had no pred
				last_succ_index[last_node_index]	= -1;
				current_predecessor	= last_node;
			}
			int32_t variable_index;

#if DEBUG_OBDD_VALUATIONS
			printf("last_pred_index[%d]:%d\t<", last_node_index, last_succ_index[last_node_index]);
			for(i = 0; i <= (int32_t)last_node_index; i++)
				printf("%s", dont_care_list[i]? "?" : (partial_valuation[i]? "1" : "0"));
			for(i = variables_count - 1; i > last_node_index; i--)
				printf("x");
			printf(">\n");
			printf("Partial Valuation\t<");
			for(i = 0; i < (int32_t)variables_count; i++)
				printf("%s", partial_valuation[i]? "1" : "0");
			printf(">\n");
			printf("Dont care list\t<");
			for(i = 0; i < (int32_t)variables_count; i++){
				variable_index	= -1;
				for(j = 0; j < (int32_t)img_count; j++){
					if(valuation_img[j] == (uint32_t)i + 2){
						variable_index	= valuation_img[j] - 2;
						break;
					}
				}
				printf("%s", dont_care_list[i]? (variable_index > -1 ? "?" : "_") : "0");
			}

			printf(">\n");
#endif
			//terminal case reached
			if(current_predecessor->high_predecessors_count == 0 && current_predecessor->low_predecessors_count == 0
					&& at_least_one_node_expanded){//terminal case, add valuation
				dont_cares_count	= 1;
				//set unexplored variables as dont care
				if(min_var_ID > 0){
					for(i = min_var_ID - 1; i >= 0; i--)
							dont_care_list[i]	= true;
				}
				//count dont cares to get number of new valuations (only for variables belonging to the image)
				for(i = 0; i < (int32_t)variables_count; i++){
					if(dont_care_list[i]){
						variable_index	= -1;
						for(j = 0; j < (int32_t)img_count; j++)
							if(valuation_img[j] == (uint32_t)i + 2){
								variable_index	= valuation_img[j] - 2;
								break;
							}
						if(variable_index >= 0)
							dont_cares_count *= 2;
					}
				}
				//get new valuations according to dont care list
				uint32_t new_size	= *valuations_count + dont_cares_count;
				if(new_size >= valuations_size){
					bool* ptr	= realloc(valuations, sizeof(bool) * img_count * new_size);
					if(ptr == NULL){
						printf("Could not reallocate valuations array\n");
						exit(-1);
					}
					valuations	= ptr;
					valuations_size	= new_size;
				}
				uint32_t modulo	= dont_cares_count;

#if DEBUG_OBDD_VALUATIONS
				printf("[T]erminals on node: %d (%d:%s) :\n", last_node_index, last_nodes[last_node_index]->var_ID,dictionary_key_for_value(mgr->vars_dict,last_nodes[last_node_index]->var_ID));

				printf("last_pred_index[%d]:%d\t<", last_node_index, last_succ_index[last_node_index]);
				for(i = 0; i <= (int32_t)last_node_index; i++)
					printf("%s", dont_care_list[i]? "?" : (partial_valuation[i]? "1" : "0"));
				for(i = variables_count - 1; i > last_node_index; i--)
					printf("x");
				printf(">\n");
				printf("Partial Valuation\t<");
				for(i = 0; i < (int32_t)variables_count; i++)
					printf("%s", partial_valuation[i]? "1" : "0");
				printf(">\n");
				printf("Dont care list\t<");
				for(i = 0; i < (int32_t)variables_count; i++){
					variable_index	= -1;
					for(j = 0; j < (int32_t)img_count; j++){
						if(valuation_img[j] == (uint32_t)i + 2){
							variable_index	= valuation_img[j] - 2;
							break;
						}
					}
					printf("%s", dont_care_list[i]? (variable_index > -1 ? "?" : "_") : "0");
				}
				printf("> count:%d\n", dont_cares_count);
#endif

				uint32_t k;
				uint32_t img_index;
				for(k = 0; k < (uint32_t)dont_cares_count; k++){
					for(i = 0; i < (int32_t)img_count; i++)
						valuation_set[i] = false;
					last_bit_index = -1;
					for(i = 0; i < (int32_t)variables_count; i++){
						variable_index	= -1;
						//only setting odd variables
						for(j = 0; j < (int32_t)img_count && variable_index < 0; j++){
							if(valuation_img[j] == (uint32_t)i + 2){
								variable_index	= valuation_img[j] - 2;
								img_index		= j;
								valuation_set[j]	= true;
							}
						}

						if(variable_index >= 0){
							if(dont_care_list[variable_index]){
								last_bit_index++;
								//set according to modulo division if current position was set to dont care
								GET_VAR_IN_VALUATION(valuations, img_count, *valuations_count + k, img_index)	= (k & (0x1 << last_bit_index)) != 0;
							}else{
								//set to predefined value for this search
								GET_VAR_IN_VALUATION(valuations, img_count, *valuations_count + k, img_index)	= partial_valuation[variable_index];
							}
						}
					}
					for(j = 0; j < (int32_t)img_count; j++){
						if(!valuation_set[j]){
							printf("Value not set for %s on valuation %d\n", dictionary_key_for_value(mgr->vars_dict, valuation_img[j]), *valuations_count + k );
							exit(-1);
						}
					}
#if DEBUG_OBDD_VALUATIONS
					printf("[");
					for(i = 0; i < (int32_t)img_count; i++){
						bool value = GET_VAR_IN_VALUATION(valuations, img_count, *valuations_count + k, i);
						bool care_for_value = dont_care_list[valuation_img[i] - 2];
						printf("%s", value ? (care_for_value ? "i" : "1") : (care_for_value ? "o" : "0"));
					}
					printf("]\n");
#endif
				}


				*valuations_count	+= dont_cares_count;
			}
			//all options have been taken for last_node
			if(last_succ_index[last_node_index] >=
					(int32_t)(last_node->high_predecessors_count + last_node->low_predecessors_count)){
				last_succ_index[last_node_index]	= -1;
				at_least_one_node_expanded			= false;
#if DEBUG_OBDD_VALUATIONS
				printf("[E]xhausted node: %d (%s) -1 \n", last_node_index, dictionary_key_for_value(mgr->vars_dict,last_nodes[last_node_index]->var_ID));
#endif
			}
			if(last_node_index < (int32_t)(variables_count - 1)){
				last_node_index++;
				if(last_node->var_ID > 1){
					if((last_node->var_ID - 2) < (int32_t)min_var_ID){
								min_var_ID	= last_node->var_ID - 2;
#if DEBUG_OBDD_VALUATIONS
							printf("New [m]in var ID: %d\n", min_var_ID);
#endif
					}
					if((last_node->var_ID - 2) > (int32_t)max_pred_index){
								max_pred_index	= last_node->var_ID - 2;
#if DEBUG_OBDD_VALUATIONS
							printf("New [M]ax index: %d\n", max_pred_index);
#endif
					}
				}
				last_nodes[last_node_index]			= current_predecessor;
				last_succ_index[last_node_index]	= 0;
#if DEBUG_OBDD_VALUATIONS
				printf("[N]ow on node: %d (%d:%s) \n", last_node_index, last_nodes[last_node_index]->var_ID,dictionary_key_for_value(mgr->vars_dict,last_nodes[last_node_index]->var_ID));
				printf("High pred. count: %d \tLow pred.count: %d\n", last_nodes[last_node_index]->high_predecessors_count, last_nodes[last_node_index]->low_predecessors_count);
#endif

			}else{//start backtracking until one node exists with unexplored preds.
				at_least_one_node_expanded		= false;
				dont_care_list[last_node_index]	= true;
				while(--last_node_index > 0){//firing backtrack can avoid terminals
					dont_care_list[last_node_index]	= true;
					if(last_succ_index[last_node_index] != -1)
						break;
				}
				max_pred_index	= 0;
				min_var_ID		= mgr->vars_dict->size - 3;
#if DEBUG_OBDD_VALUATIONS
	printf("[B]acktracked on exploration to %d\n", last_node_index);
#endif
			}
		}else{
			printf("Should not have reached this point\n");
			printf("[W]as on node: %d (%s) %d\n", last_node_index, dictionary_key_for_value(mgr->vars_dict,last_nodes[last_node_index]->var_ID)
					,last_succ_index[last_node_index]);
			break;
		}
	}
	int32_t variable_index;
	//if it is projecting, remove duplicates

	if(img_count != variables_count && *valuations_count > 0){
		uint32_t new_count			= 0;
		uint32_t k;
		bool* new_valuations	= calloc((*valuations_count) * img_count, sizeof(bool) );
		bool has_valuation, one_found;
		bool* current_valuation	= calloc(img_count, sizeof(bool));
		for(i = 0; i < (int32_t)*valuations_count; i++){
			for(j = 0; j < (int32_t)img_count; j++){
				variable_index	= -1;
				for(k = 0; k < variables_count; k++)
					if(valuation_img[j] == (uint32_t)(k + 2)){
						variable_index	= valuation_img[j] -2;
						current_valuation[j] = GET_VAR_IN_VALUATION(valuations, img_count, i, j);
						break;
					}
			}
			has_valuation = false;
			one_found = false;
			int32_t l;
			for(k = 0; k < new_count; k++){
				has_valuation		= true;
				for(j = 0; j < (int32_t)img_count; j++){
					variable_index = -1;
					for(l = 0; l < (int32_t)variables_count; l++)
						if(valuation_img[j] == (uint32_t)(l + 2)){
							variable_index	= valuation_img[j] - 2;
							if(current_valuation[j] != GET_VAR_IN_VALUATION(new_valuations, img_count, k, j)){
								has_valuation = false;
							}
							break;
						}
					if(variable_index == -1){
						exit(-1);
					}

				}
				one_found = one_found || has_valuation;
				if(one_found)break;
			}
			if(!one_found){
				for(j = 0; j < (int32_t)img_count; j++){
					variable_index = -1;
					for(l = 0; l < (int32_t)variables_count; l++)
						if(valuation_img[j] == (uint32_t)(l + 2)){
							variable_index	= valuation_img[j] - 2;
							GET_VAR_IN_VALUATION(new_valuations, img_count, new_count, j)	= current_valuation[j];
							break;
						}
					if(variable_index == -1){
						exit(-1);
					}

				}
				new_count++;
			}
		}
		free(valuations);
		free(current_valuation);
		valuations			= new_valuations;
		*valuations_count	= new_count;
	}
	free(valuation_set);
	free(nodes);
	free(last_nodes);
	free(last_succ_index);
	free(dont_care_list);
	free(partial_valuation);
	return valuations;
}
*/

/** OBDD NODE FUNCTIONS **/
void obdd_node_destroy(obdd_mgr* mgr, obdd_node* node){
#if DEBUG_OBDD
		printf("(destroy)[%d]%p (ref:%d)",node->var_ID, (void*)node, node->ref_count);
	if(node->ref_count > 0)
		printf("\n");
#endif
	if(node->ref_count == 0){
#if DEBUG_OBDD
		printf(ANSI_COLOR_RED"[XX]\n"ANSI_COLOR_RESET);
#endif
		if(node->high_obdd != NULL){
			obdd_node* to_remove = node->high_obdd;
			obdd_remove_high_successor(node, to_remove);
			obdd_node_destroy(mgr, to_remove);
			node->high_obdd = NULL;
		}
		if(node->low_obdd != NULL){
			obdd_node* to_remove = node->low_obdd;
			obdd_remove_low_successor(node, to_remove);
			obdd_node_destroy(mgr, to_remove);
			node->low_obdd = NULL;
		}
		node->var_ID	= 0;
		node->node_ID	= 0;
		//free(node);
		automaton_fast_pool_release_instance(mgr->nodes_pool, node->fragment_ID);
	}
}
