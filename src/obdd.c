#include "obdd.h"
#include "assert.h"

uint32_t obdd_mgr_greatest_ID = 0;

/** MAP TREE **/
void obdd_state_tree_entry_print(obdd_state_tree_entry* entry){
	if(entry->is_leaf)
		printf("[%d]\n", entry->leaf_value);
	else{
		if(entry->low != NULL){
			printf("0");
			obdd_state_tree_entry_print(entry->low);
		}
		if(entry->high != NULL){
			printf("1");
			obdd_state_tree_entry_print(entry->high);
		}
	}
}

void obdd_state_tree_print(obdd_state_tree* tree){
	printf("Binary Map Tree.\n");
	if(tree->first_entry != NULL)
		obdd_state_tree_entry_print(tree->first_entry);
}

obdd_state_tree* obdd_state_tree_create(uint32_t key_length){
	obdd_state_tree* tree= malloc(sizeof(obdd_state_tree));
	tree->key_length	= key_length;
	tree->max_value		= 0;
	tree->first_entry	= NULL;
	tree->entries_count	= 0;
	tree->entries_size	= LIST_INITIAL_SIZE;
	tree->entries_pool	= malloc(sizeof(obdd_state_tree_entry) * tree->entries_size);
	return tree;
}
obdd_state_tree_entry* obdd_state_tree_entry_get_from_pool(obdd_state_tree* tree){
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
	entry->high						= NULL;
	entry->low						= NULL;
	entry->is_leaf					= false;
	entry->leaf_value				= 0;
	return entry;
}
uint32_t obdd_state_tree_get_key(obdd_state_tree* tree, bool* valuation){
	uint32_t i,j;
	obdd_state_tree_entry* current_entry 	= NULL;
	obdd_state_tree_entry* last_entry		= NULL;
	if(tree->first_entry == NULL){
		current_entry		= obdd_state_tree_entry_get_from_pool(tree);
		tree->first_entry	= current_entry;
	}
	last_entry				= tree->first_entry;
	bool next_is_high;
	for(i = 0; i < tree->key_length; i++){
		next_is_high		= valuation[i];

		if((next_is_high && last_entry->high == NULL) || (!next_is_high && last_entry->low == NULL)){
			current_entry		= obdd_state_tree_entry_get_from_pool(tree);
			current_entry->high	= NULL;
			current_entry->low	= NULL;
			if(i == (tree->key_length -1)){
				current_entry->is_leaf		= true;
				current_entry->leaf_value	= tree->max_value++;
				return current_entry->leaf_value;
			}else{
				current_entry->is_leaf	= false;
			}
			if(next_is_high)
				last_entry->high = current_entry;
			else
				last_entry->low = current_entry;
		}else{
			current_entry	= next_is_high ? last_entry->high : last_entry->low;
			if(i == (tree->key_length -1))
				return current_entry->leaf_value;
		}
		last_entry = current_entry;
	}
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
	//is initialized in 1 so that we can later check for already deleted nodes
	new_mgr->greatest_node_ID	= 1;
	new_mgr->greatest_var_ID	= 0;
	
	//create variables dict
	new_mgr->vars_dict		= dictionary_create();
	
	//create constant obdds for true and false values
	obdd* true_obdd		= malloc(sizeof(obdd));
	true_obdd->root_obdd= obdd_mgr_mk_node(new_mgr, TRUE_VAR, NULL, NULL);
	true_obdd->root_obdd->ref_count++;
	true_obdd->mgr		= new_mgr;
	true_obdd->true_obdd	= NULL;
	true_obdd->false_obdd	= NULL;
	new_mgr->true_obdd	= true_obdd;
	obdd* false_obdd	= malloc(sizeof(obdd));
	false_obdd->root_obdd= obdd_mgr_mk_node(new_mgr, FALSE_VAR, NULL, NULL);
	false_obdd->root_obdd->ref_count++;
	false_obdd->mgr		= new_mgr;
	false_obdd->true_obdd	= NULL;
	false_obdd->false_obdd	= NULL;
	new_mgr->false_obdd	= false_obdd;
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

void obdd_add_high_succesor(obdd_node* src, obdd_node* dst){
	src->high_obdd	= dst;
	if(dst != NULL){
		if(dst->high_predecessors == NULL)		dst->high_predecessors		= malloc(sizeof(obdd_node*));
		else{
			obdd_node** ptr	= realloc(dst->high_predecessors, sizeof(obdd_node*) * (dst->high_predecessors_count + 1));
			if(ptr == NULL){
				printf("Could not allocate memory\n");
				exit(-1);
			}else{
				dst->high_predecessors	= ptr;
			}
		}
		dst->high_predecessors[dst->high_predecessors_count++]	= src;
		dst->ref_count++;
#if DEBUG_OBDD
		printf("(++)%p -[1]-> %p (ref:%d)\n", (void*)src, (void*)dst, dst->ref_count);
#endif
	}
}
void obdd_add_low_succesor(obdd_node* src, obdd_node* dst){
	src->low_obdd	= dst;
	if(dst != NULL){
		if(dst->low_predecessors == NULL)		dst->low_predecessors		= malloc(sizeof(obdd_node*));
		else{
			obdd_node** ptr	= realloc(dst->low_predecessors, sizeof(obdd_node*) * (dst->low_predecessors_count + 1));
			if(ptr == NULL){
				printf("Could not allocate memory\n");
				exit(-1);
			}else{
				dst->low_predecessors	= ptr;
			}
		}
		dst->low_predecessors[dst->low_predecessors_count++]	= src;
		dst->ref_count++;
#if DEBUG_OBDD
		printf("(++)%p -[0]-> %p (ref:%d)\n", src, dst, dst->ref_count);
#endif
	}
}
void obdd_remove_high_succesor(obdd_node* src, obdd_node* dst){
	int32_t index	= -1;
	int32_t i;
	src->high_obdd	= NULL;
	if(dst != NULL){
		for(i = 0; i < (int32_t)dst->high_predecessors_count; i++)
			if(dst->high_predecessors[i] == src){
				index = i;
				break;
			}
		for(i = index; i < (int32_t)(dst->high_predecessors_count - 1); i++){
			dst->high_predecessors[i]	= dst->high_predecessors[i+ 1];
		}
		if(dst->high_predecessors_count == 1){
			free(dst->high_predecessors);
			dst->high_predecessors	= NULL;
		}else{
			obdd_node** ptr	= realloc(dst->high_predecessors, sizeof(obdd_node*) * (dst->high_predecessors_count - 1));
			if(ptr == NULL){
				printf("Could not allocate memory\n");
				exit(-1);
			}else{
				dst->high_predecessors	= ptr;
			}
		}
		dst->high_predecessors_count--;
		dst->ref_count--;
#if DEBUG_OBDD
		printf("(--)%p -[1]-> %p (ref:%d)\n", src, dst, dst->ref_count);
#endif
	}
}
void obdd_remove_low_succesor(obdd_node* src, obdd_node* dst){
	int32_t index	= -1;
	int32_t i;
	src->low_obdd	= NULL;
	if(dst != NULL){
		for(i = 0; i < (int32_t)dst->low_predecessors_count; i++)
			if(dst->low_predecessors[i] == src){
				index = i;
				break;
			}
		for(i = index; i < (int32_t)(dst->low_predecessors_count - 1); i++){
			dst->low_predecessors[i]	= dst->low_predecessors[i+ 1];
		}
		if(dst->low_predecessors_count == 1){
			free(dst->low_predecessors);
			dst->low_predecessors	= NULL;
		}else{
			obdd_node** ptr	= realloc(dst->low_predecessors, sizeof(obdd_node*) * (dst->low_predecessors_count - 1));
			if(ptr == NULL){
				printf("Could not allocate memory\n");
				exit(-1);
			}else{
				dst->low_predecessors	= ptr;
			}
		}
		dst->low_predecessors_count--;
		dst->ref_count--;
#if DEBUG_OBDD
		printf("(--)%p -[0]-> %p (ref:%d)\n", src, dst, dst->ref_count);
#endif
	}
}
obdd_node* obdd_mgr_mk_node(obdd_mgr* mgr, char* var, obdd_node* high, obdd_node* low){
	uint32_t var_ID		= dictionary_add_entry(mgr->vars_dict, var);
	obdd_node* new_node	= malloc(sizeof(obdd_node));
	new_node->var_ID	= var_ID;
	new_node->node_ID	= obdd_mgr_get_next_node_ID(mgr);
	new_node->high_predecessors_count	= 0;
	new_node->high_predecessors			= NULL;
	new_node->low_predecessors_count	= 0;
	new_node->low_predecessors			= NULL;
	obdd_add_high_succesor(new_node, high);
	obdd_add_low_succesor(new_node, low);
	new_node->ref_count	= 0;
	return new_node;
}

obdd*	obdd_mgr_var(obdd_mgr* mgr, char* name){
	obdd* var_obdd	= obdd_create(mgr, NULL);
	var_obdd->root_obdd= obdd_mgr_mk_node(mgr, name
		, var_obdd->true_obdd, var_obdd->false_obdd);
	return var_obdd;	
}

obdd*	obdd_mgr_true(obdd_mgr* mgr){ return mgr->true_obdd; }
obdd*	obdd_mgr_false(obdd_mgr* mgr){ return mgr->false_obdd; }

/** OBDD FUNCTIONS **/
obdd* obdd_create(obdd_mgr* mgr, obdd_node* root){
	obdd* new_obdd		= malloc(sizeof(obdd));
	new_obdd->mgr		= mgr;
	new_obdd->root_obdd	= root;
	new_obdd->true_obdd	= mgr->true_obdd->root_obdd;
	new_obdd->false_obdd= mgr->false_obdd->root_obdd;
	return new_obdd;
}

void obdd_destroy(obdd* root){
	if(root->root_obdd != NULL){
		obdd_node_destroy(root->root_obdd);
		root->root_obdd		= NULL;
	}
	//must not delete true and false obdd since they are being destroyed by recursive call on root
	root->true_obdd	= NULL;
	root->false_obdd	= NULL;
	root->mgr			= NULL;
	free(root);
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
	uint32_t var_ID		= dictionary_add_entry(dict,  var_next);
	value->var_ID		= var_ID;
	if(value->high_obdd != NULL)
		obdd_node_apply_next(mgr, value->high_obdd);
	if(value->low_obdd != NULL)
		obdd_node_apply_next(mgr, value->low_obdd);
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
				obdd_remove_high_succesor(root, root->high_obdd);
				obdd_add_high_succesor(root, *(true_node));
			}
		}else{
			if(*false_node == NULL){
				*false_node	= root->high_obdd;
			}else{
				obdd_remove_high_succesor(root, root->high_obdd);
				obdd_add_high_succesor(root, *(false_node));
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
				obdd_remove_low_succesor(root, root->low_obdd);
				obdd_add_low_succesor(root, *(true_node));
			}
		}else{
			if(*false_node == NULL){
				*false_node	= root->low_obdd;
			}else{
				obdd_remove_low_succesor(root, root->low_obdd);
				obdd_add_low_succesor(root, *(false_node));
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
	if(!obdd_is_constant(mgr, root->high_obdd)){
		if(root->high_obdd->high_obdd->node_ID == root->high_obdd->low_obdd->node_ID){
			obdd_node* to_remove	= root->high_obdd;
			obdd_node* to_add		= root->high_obdd->high_obdd;
			obdd_remove_high_succesor(root->high_obdd, root->high_obdd->high_obdd);
			obdd_remove_low_succesor(root->high_obdd, root->high_obdd->low_obdd);
			obdd_add_high_succesor(root, to_add);
			obdd_node_destroy(to_remove);
		}
	}
	if(!obdd_is_constant(mgr, root->low_obdd)){
		if(root->low_obdd->high_obdd->node_ID == root->low_obdd->low_obdd->node_ID){
			obdd_node* to_remove	= root->low_obdd;
			obdd_node* to_add		= root->low_obdd->low_obdd;
			obdd_remove_high_succesor(root->low_obdd, root->low_obdd->high_obdd);
			obdd_remove_low_succesor(root->low_obdd, root->low_obdd->low_obdd);
			obdd_add_low_succesor(root, to_add);
			obdd_node_destroy(to_remove);
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
	
	obdd* applied_obdd	= obdd_create(left->mgr, obdd_node_apply(apply_fkt, left->mgr, left->root_obdd, right->root_obdd));
	obdd_reduce(applied_obdd);
	return applied_obdd;
}	

obdd_node* obdd_node_apply(bool (*apply_fkt)(bool,bool), obdd_mgr* mgr, obdd_node* left_node, obdd_node* right_node){

	uint32_t left_var_ID	=  left_node->var_ID;
	uint32_t right_var_ID	=  right_node->var_ID;

	char* left_var			= dictionary_key_for_value(mgr->vars_dict,left_var_ID);
	char* right_var			= dictionary_key_for_value(mgr->vars_dict,right_var_ID);

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
		applied_node 	= obdd_mgr_mk_node(mgr, right_var, 
			obdd_node_apply(apply_fkt, mgr, left_node, right_node->high_obdd), 
			obdd_node_apply(apply_fkt, mgr, left_node, right_node->low_obdd));
	}else if(is_right_constant){
		applied_node 	= obdd_mgr_mk_node(mgr, left_var, 
			obdd_node_apply(apply_fkt, mgr, left_node->high_obdd, right_node), 
			obdd_node_apply(apply_fkt, mgr, left_node->low_obdd, right_node));
	}else if(left_var_ID == right_var_ID){
		applied_node 	= obdd_mgr_mk_node(mgr, left_var, 
			obdd_node_apply(apply_fkt, mgr, left_node->high_obdd, right_node->high_obdd), 
			obdd_node_apply(apply_fkt, mgr, left_node->low_obdd, right_node->low_obdd));
	}else if(left_var_ID < right_var_ID){
		applied_node 	= obdd_mgr_mk_node(mgr, left_var, 
			obdd_node_apply(apply_fkt, mgr, left_node->high_obdd, right_node), 
			obdd_node_apply(apply_fkt, mgr, left_node->low_obdd, right_node));
	}else{
		applied_node 	= obdd_mgr_mk_node(mgr, right_var, 
			obdd_node_apply(apply_fkt, mgr, left_node, right_node->high_obdd), 
			obdd_node_apply(apply_fkt, mgr, left_node, right_node->low_obdd));
	}

	return applied_node;	
}

obdd* obdd_restrict(obdd* root, char* var, bool value){
	uint32_t var_ID	=  dictionary_value_for_key(root->mgr->vars_dict, var);
	obdd_node* restricted_node	= obdd_node_restrict(root->mgr, root->root_obdd, var, var_ID, value);

	return (obdd_create(root->mgr, restricted_node));
}

obdd_node* obdd_node_restrict(obdd_mgr* mgr, obdd_node* root, char* var, uint32_t var_ID, bool value){
	bool is_root_constant	= obdd_is_constant(mgr, root);
	uint32_t root_var_ID	= root->var_ID;
	char* root_var		= dictionary_key_for_value(mgr->vars_dict,root_var_ID);

	if(is_root_constant){
		return root;
	}

	uint32_t low_var_ID	=  root->low_obdd->var_ID;
	uint32_t high_var_ID	=  root->high_obdd->var_ID;

	char* low_var			= dictionary_key_for_value(mgr->vars_dict,low_var_ID);
	char* high_var			= dictionary_key_for_value(mgr->vars_dict,high_var_ID);

	bool is_low_constant		= obdd_is_constant(mgr, root->low_obdd);
	bool is_high_constant		= obdd_is_constant(mgr, root->high_obdd);

	obdd_node* applied_node;

	if(root_var_ID == var_ID){
		if(value){
			applied_node 	= obdd_mgr_mk_node(mgr, high_var
				, root->high_obdd->high_obdd, root->high_obdd->low_obdd); 
		}else{
			applied_node 	= obdd_mgr_mk_node(mgr, low_var
				, root->low_obdd->high_obdd, root->low_obdd->low_obdd);  
		}
	}else{
		applied_node 	= obdd_mgr_mk_node(mgr, root_var
			, obdd_node_restrict(mgr, root->high_obdd, var, var_ID, value)
			, obdd_node_restrict(mgr, root->low_obdd, var, var_ID, value));  	
	}
	return applied_node;	
}

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
	if(obdd_is_constant(mgr, root)){
		printf("->%s", var);
		return;
	}else if(spaces > 0){
		printf(" &");
	}
	uint32_t i;
	printf("\n");
	for(i = 0; i < spaces; i++)
		printf(" ");
	bool has_left_side = false;
	if(root->high_obdd != NULL){
		printf("%s", var);
		obdd_node_print(mgr, root->high_obdd, spaces + 1);
		has_left_side	= true;
	}	
	if(root->low_obdd != NULL){
		if(has_left_side){
			printf("\n");
			for(i = 0; i < spaces; i++)
				printf(" ");
			printf("|");
			printf("\n");
			for(i = 0; i < spaces; i++)
				printf(" ");
		}
		printf("(!%s)", var);
		obdd_node_print(mgr, root->low_obdd, spaces + 1);
	}	
}

bool obdd_is_true(obdd_mgr* mgr, obdd_node* root){
	return (root->var_ID == mgr->true_obdd->root_obdd->var_ID);
}
bool obdd_is_constant(obdd_mgr* mgr, obdd_node* root){
	return ((root->var_ID == mgr->true_obdd->root_obdd->var_ID) 
		|| (root->var_ID == mgr->false_obdd->root_obdd->var_ID));
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

bool** obdd_get_valuations(obdd_mgr* mgr, obdd_node* root){
	exit(-1);
}

/** OBDD NODE FUNCTIONS **/

void obdd_node_destroy(obdd_node* node){
	if(node->ref_count == 0){
		if(node->high_obdd != NULL){
			obdd_node* to_remove = node->high_obdd;
			obdd_remove_high_succesor(node, to_remove);
			obdd_node_destroy(to_remove);
		}
		if(node->low_obdd != NULL){
			obdd_node* to_remove = node->low_obdd;
			obdd_remove_low_succesor(node, to_remove);
			obdd_node_destroy(to_remove);
		}
		node->var_ID	= 0;
		node->node_ID	= 0;
		free(node);
	}
}
