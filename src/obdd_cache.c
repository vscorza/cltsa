/*
 * obdd_cache.c
 *
 *  Created on: Mar 28, 2019
 *      Author: mariano
 */
#include "obdd.h"

obdd_cache* obdd_cache_create(obdd_mgr *mgr, uint32_t cache_size, uint32_t cache_max_size){
	obdd_cache* cache	= calloc(1, sizeof(obdd_cache));
	cache->mgr			= mgr;
	uint32_t log_size	= 0;
	uint32_t size_copy	= cache_size;
	while(size_copy > 1){log_size++; size_copy >>= 1;}
	//fix cache_size to power of 2
	cache_size			= 1U << log_size;
	cache->cache_items	= calloc(cache_size + 1, sizeof(obdd_cache_item));
	cache->cache_slots	= cache_size;
	cache->cache_shift	= sizeof(int64_t) * 8 - log_size;
	cache->max_cache_hard	= cache_max_size;
	cache->cache_slack	= cache_max_size;
	cache->cache_misses	= (double)(int32_t)(cache_size * .3f + 1);
	cache->cache_hits	= 0;
	cache->min_hits		= .3f;
	cache->cache_collisions	= 0;
	cache->cache_inserts	= 0;
	cache->vars_size	= LIST_INITIAL_SIZE;
	cache->cache_vars	= calloc(cache->vars_size, sizeof(obdd_node*));
	cache->cache_neg_vars	= calloc(cache->vars_size, sizeof(obdd_node*));
	return cache;
}



void obdd_cache_insert(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g, obdd_node *h, obdd_node *data){
	int32_t pos;
	obdd_cache_item *item;
	uintptr_t uf, ug, uh;

	uf	= (uintptr_t) f | (op & 0xe);
	ug	= (uintptr_t) g | (op >> 4);
	uh	= (uintptr_t) h;


	pos	= ddCHash2(uh, uf, ug, cache->cache_shift);
	item= &(cache->cache_items[pos]);

	cache->cache_collisions	+= item->data != NULL;
	cache->cache_inserts++;

	if(item->data != NULL){
		item->data->ref_count--;

		 if(item->data->ref_count == 0){
			//obdd_node_destroy(cache->mgr, item->data);
		 }
	}

	item->f	= (obdd_node*) uf;
	item->g	= (obdd_node*) ug;
	item->h	= uh;
	item->data	= data;
	item->data->ref_count++;
}
void obdd_cache_insert2(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g, obdd_node *data){
	int32_t pos;
	obdd_cache_item *item;
	uintptr_t uf, ug;


	pos	= ddCHash2(op, f, g, cache->cache_shift);
	item= &(cache->cache_items[pos]);

	cache->cache_collisions	+= item->data != NULL;
	cache->cache_inserts++;

	if(item->data != NULL){
		item->data->ref_count--;

		 if(item->data->ref_count == 0){
			//obdd_node_destroy(cache->mgr, item->data);
		 }
	}

	item->f	= f;
	item->g	= g;
	item->h	= op;
	item->data	= data;
	item->data->ref_count++;
}

void obdd_cache_insert1(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *data){
	int32_t pos;
	obdd_cache_item *item;

	pos	= ddCHash2(op, f, f, cache->cache_shift);
	item= &(cache->cache_items[pos]);

	cache->cache_collisions	+= item->data != NULL;
	cache->cache_inserts++;

	if(item->data != NULL){
		item->data->ref_count--;
		 if(item->data->ref_count == 0){
			//obdd_node_destroy(cache->mgr, item->data);
		 }
	}

	item->f	= f;
	item->g	= f;
	item->h	= op;
	item->data	= data;
	data->ref_count++;
}

obdd_node* obdd_cache_insert_var(obdd_cache *cache, obdd_var_size_t var_id){
	if((var_id + 1) > cache->vars_size){
		uint32_t i, old_size	= cache->vars_size;
		cache->vars_size = var_id * LIST_INCREASE_FACTOR;
		obdd_node** ptr	= realloc(cache->cache_vars, sizeof(obdd_node*) * (cache->vars_size));
		if(ptr == NULL){
			printf("Could not allocate memory\n");
			exit(-1);
		}else{
			cache->cache_vars	= ptr;
			for(i = old_size; i < cache->vars_size; i++){
				cache->cache_vars[i]	= NULL;
			}
		}
		ptr	= realloc(cache->cache_neg_vars, sizeof(obdd_node*) * (cache->vars_size));
		if(ptr == NULL){
			printf("Could not allocate memory\n");
			exit(-1);
		}else{
			cache->cache_neg_vars	= ptr;
			for(i = old_size; i < cache->vars_size; i++){
				cache->cache_neg_vars[i]	= NULL;
			}
		}
	}
	if(cache->cache_vars[var_id] == NULL){
		cache->cache_vars[var_id] = obdd_mgr_mk_node_ID(cache->mgr, var_id, cache->mgr->true_obdd->root_obdd, cache->mgr->false_obdd->root_obdd);
		cache->cache_vars[var_id]->ref_count++;
	}
	return cache->cache_vars[var_id];
}

obdd_node* obdd_cache_insert_neg_var(obdd_cache *cache, obdd_var_size_t var_id){
	if((var_id + 1) > cache->vars_size){
		uint32_t i, old_size	= cache->vars_size;
		cache->vars_size = var_id  * LIST_INCREASE_FACTOR;
		obdd_node** ptr	= realloc(cache->cache_vars, sizeof(obdd_node*) * (cache->vars_size));
		if(ptr == NULL){
			printf("Could not allocate memory\n");
			exit(-1);
		}else{
			cache->cache_vars	= ptr;
			for(i = old_size; i < cache->vars_size; i++){
				cache->cache_vars[i]	= NULL;
			}
		}
		ptr	= realloc(cache->cache_neg_vars, sizeof(obdd_node*) * (cache->vars_size));
		if(ptr == NULL){
			printf("Could not allocate memory\n");
			exit(-1);
		}else{
			cache->cache_neg_vars	= ptr;
			for(i = old_size; i < cache->vars_size; i++){
				cache->cache_neg_vars[i]	= NULL;
			}
		}
	}
	if(cache->cache_neg_vars[var_id] == NULL){
		cache->cache_neg_vars[var_id] = obdd_mgr_mk_node_ID(cache->mgr, var_id, cache->mgr->false_obdd->root_obdd, cache->mgr->true_obdd->root_obdd);
		cache->cache_neg_vars[var_id]->ref_count++;
	}
	return cache->cache_neg_vars[var_id];
}

obdd_node* obdd_cache_lookup(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g, obdd_node *h){
	obdd_cache_item *item;
	obdd_node *data;
	int32_t pos;
	uintptr_t uf, ug, uh;

	uf	= (uintptr_t) f | (op & 0xe);
	ug	= (uintptr_t) g | (op >> 4);
	uh	= (uintptr_t) h;

	pos	= ddCHash2(uh, uf, ug, cache->cache_shift);
	item= &(cache->cache_items[pos]);

	if(item->data != NULL && item->f==(obdd_node*)uf && item->g==(obdd_node*)ug && item->h == uh){
		data	= item->data;
		cache->cache_hits++;
		return data;
	}

	cache->cache_misses++;
	if(cache->cache_slack >= 0 && cache->cache_hits > cache->cache_misses * cache->min_hits)
		obdd_cache_resize(cache);
	return NULL;
}
obdd_node* obdd_cache_lookup2(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g){
	obdd_cache_item *item;
	obdd_node *data;
	uint64_t pos;
	uintptr_t uf, ug, uh;

	pos	= ddCHash2(op, f, g, cache->cache_shift);

	item= &(cache->cache_items[pos]);

	if(item->data != NULL && item->f==f && item->g==g && item->h == op){
		data	= item->data;
		cache->cache_hits++;
		return data;
	}

	cache->cache_misses++;
	if(cache->cache_slack >= 0 && cache->cache_hits > cache->cache_misses * cache->min_hits)
		obdd_cache_resize(cache);
	return NULL;
}
obdd_node* obdd_cache_lookup1(obdd_cache *cache, uintptr_t op, obdd_node *f){
	obdd_cache_item *item;
	obdd_node *data;
	int32_t pos;
	uintptr_t uf, ug, uh;

	pos	= ddCHash2(op, f, f, cache->cache_shift);
	item= &(cache->cache_items[pos]);

	if(item->data != NULL && item->f==f && item->h == op){
		data	= item->data;
		cache->cache_hits++;
		return data;
	}

	cache->cache_misses++;
	if(cache->cache_slack >= 0 && cache->cache_hits > cache->cache_misses * cache->min_hits)
		obdd_cache_resize(cache);
	return NULL;
}
obdd_node* obdd_cache_constant_lookup(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g, obdd_node *h){
	obdd_cache_item *item;
	obdd_node *data;
	int32_t pos;
	uintptr_t uf, ug, uh;

	uf	= (uintptr_t) f | (op & 0xe);
	ug	= (uintptr_t) g | (op >> 4);
	uh	= (uintptr_t) h;

	pos	= ddCHash2(uh, uf, ug, cache->cache_shift);
	item= &(cache->cache_items[pos]);

	//no reclaim

	if(item->data != NULL && item->f==(obdd_node*)uf && item->g==(obdd_node*)ug && item->h == uh){
		data	= item->data;
		cache->cache_hits++;
		return data;
	}

	cache->cache_misses++;
	if(cache->cache_slack >= 0 && cache->cache_hits > cache->cache_misses * cache->min_hits)
		obdd_cache_resize(cache);
	return NULL;
}

obdd_node* obdd_cache_lookup_var(obdd_cache *cache, uint32_t var_id){
	if((var_id + 1) > cache->vars_size){
		printf("[OBDD_CACHE] var out of range");
		exit(-1);
	}
	return cache->cache_vars[var_id];
}
obdd_node* obdd_cache_lookup_neg_var(obdd_cache *cache, uint32_t var_id){
	if((var_id + 1) > cache->vars_size){
		printf("[OBDD_CACHE] var out of range");
		exit(-1);
	}
	return cache->cache_neg_vars[var_id];
}

void obdd_cache_resize(obdd_cache *cache){

	obdd_cache_item *items_old, *current_item, *new_item;
	int32_t i, pos, shift = --(cache->cache_shift);
	uint32_t cache_slots, cache_old_slots;
	double offset;
	int32_t moved	= 0;
	items_old		= cache->cache_items;
	cache_old_slots	= cache->cache_slots;
	cache_slots		= cache->cache_slots	= cache_old_slots << 1;
#if OBDD_CACHE_DEBUG
	printf("Resizing cache from %d to %d slots\n", cache_old_slots, cache_slots);
#endif


	cache->cache_items	= calloc(cache->cache_slots + 1, sizeof(obdd_cache_item));

	for(i = 0; (uint32_t) i <cache_old_slots; i++ ){
		current_item	= &(items_old[i]);
		if(current_item->data != NULL){
			pos	= ddCHash2(current_item->h, current_item->f, current_item->g, shift);
			new_item	= &(cache->cache_items[pos]);
			new_item->f	= current_item->f;
			new_item->g	= current_item->g;
			new_item->h	= current_item->h;
			new_item->data	= current_item->data;
		}
	}

	free(items_old);
	cache->cache_misses	= ((double)(int32_t)(cache_slots * cache->min_hits + 1));
	cache->cache_hits	= 0;
}
void obdd_cache_flush(obdd_cache *cache){
	int32_t i	= 0;
	for(i = 0; (uint32_t)i < cache->cache_slots; i++){
		if(cache->cache_items[i].data	!= NULL){
			obdd_node *current_node = cache->cache_items[i].data;
			current_node->ref_count--;
			if(cache->cache_items[i].data->ref_count == 0){
				obdd_node_destroy(cache->mgr, cache->cache_items[i].data);
			}
		}
		cache->cache_items[i].data	= NULL;
	}
	for(i = 0; (uint32_t)i < cache->vars_size; i++){
		if(cache->cache_vars[i] != NULL){
			cache->cache_vars[i]->ref_count--;
			if(cache->cache_vars[i]->ref_count == 0)
				obdd_node_destroy(cache->mgr, cache->cache_vars[i]);
		}
		if(cache->cache_neg_vars[i] != NULL){
			cache->cache_neg_vars[i]->ref_count--;
			if(cache->cache_neg_vars[i]->ref_count == 0)
				obdd_node_destroy(cache->mgr, cache->cache_neg_vars[i]);
		}
		cache->cache_vars[i]	= NULL;
		cache->cache_neg_vars[i]= NULL;
	}
}
void obdd_cache_destroy(obdd_cache *cache){
	obdd_cache_flush(cache);
	if(cache->cache_items != NULL)
		free(cache->cache_items);
	cache->cache_items	= NULL;
	free(cache->cache_vars);
	cache->cache_vars	= NULL;
	free(cache->cache_neg_vars);
	cache->cache_neg_vars	= NULL;
	cache->mgr			= NULL;
}

/** OBDD TABLE CACHE **/

obdd_table* obdd_table_create(obdd_mgr *mgr){
	obdd_table *new_table	= calloc(1, sizeof(obdd_table));
	new_table->size	= LIST_INITIAL_SIZE;
	new_table->levels	= calloc(new_table->size, sizeof(obdd_node*));
	new_table->dead_nodes	= calloc(new_table->size, sizeof(uint32_t));
	new_table->levels_counts	= calloc(new_table->size, sizeof(uint32_t));
	new_table->mgr	= mgr;
	return new_table;
}

void obdd_table_check_gc(obdd_table* table, obdd_var_size_t var_ID){
	obdd_node *current_node = NULL, *next_node = NULL, *last_node = NULL;
	//check if gc should be performed on dead nodes
	if(table->dead_nodes[var_ID] > table->levels_counts[var_ID] * .3){
		table->dead_nodes[var_ID]	= 0;
		table->levels_counts[var_ID]= 0;
		current_node = table->levels[var_ID];
		while(current_node != NULL){
			if(current_node->ref_count == 1){
				if(last_node == NULL){
					table->levels[var_ID]	= current_node->next;
				}else{
					last_node->next = current_node->next;
				}
				next_node = current_node->next;
				current_node->ref_count--;
				if(current_node->ref_count == 0){
#if DEBUG_OBDD
						printf(ANSI_COLOR_RED"[XX]\n"ANSI_COLOR_RESET);
#endif
						if(current_node->high_obdd != NULL){
							obdd_node* to_remove = current_node->high_obdd;
							obdd_remove_high_successor(current_node, to_remove);
							obdd_node_destroy(table->mgr, to_remove);
							current_node->high_obdd = NULL;
						}
						if(current_node->low_obdd != NULL){
							obdd_node* to_remove = current_node->low_obdd;
							obdd_remove_low_successor(current_node, to_remove);
							obdd_node_destroy(table->mgr, to_remove);
							current_node->low_obdd = NULL;
						}
						current_node->var_ID	= 0;
						current_node->next		= NULL;
						//node->node_ID	= 0;
						//free(node);
#if OBDD_USE_POOL
						automaton_fast_pool_release_instance(table->mgr->nodes_pool, current_node->fragment_ID);
#else
						free(node);
#endif
				}
				current_node	= next_node;
			}else{
				last_node = current_node;
				table->levels_counts[var_ID]++;
				current_node	= current_node->next;
			}
		}
	}
}

obdd_node* obdd_table_mk_node_ID(obdd_table* table, obdd_var_size_t var_ID, obdd_node* high, obdd_node* low){
	//resize if needed
	uint32_t i;
	if(var_ID >= table->size){
		uint32_t old_size	= table->size;
		while(table->size <= var_ID)table->size *= 2;
		obdd_node** ptr	= realloc(table->levels, sizeof(obdd_node*) * (table->size));
		if(ptr == NULL){
			printf("Could not allocate memory\n");
			exit(-1);
		}else table->levels	= ptr;
		uint32_t *uint_ptr	= realloc(table->dead_nodes, sizeof(uint32_t) * table->size);
		if(uint_ptr == NULL){
			printf("Could not allocate memory\n");
			exit(-1);
		}else table->dead_nodes	= uint_ptr;
		uint_ptr	= realloc(table->levels_counts, sizeof(uint32_t) * table->size);
		if(uint_ptr == NULL){
			printf("Could not allocate memory\n");
			exit(-1);
		}else table->levels_counts	= uint_ptr;
		for(i = old_size; i < table->size; i++){
			table->levels[i]		= NULL;
			table->dead_nodes[i]	= 0;
			table->levels_counts[i]	= 0;
		}
	}
	obdd_node *current_node, *last_node = NULL, *next_node;
	obdd_table_check_gc(table, var_ID);
	//check if node exists
	current_node = table->levels[var_ID];
	obdd_node *last_node	= NULL;
	while(current_node != NULL){
		if((current_node->low_obdd < low) || ((current_node->low_obdd == low) && (current_node->high_obdd < high))){
			last_node	= current_node;
			current_node	= current_node->next;
		}else if((current_node->low_obdd == low) && (current_node->high_obdd == high)){
			return current_node;
		}else{
			break;//no node found matching lexicographical order invariant
		}
	}


	//return new node
#if OBDD_USE_POOL
	uint32_t fragment_ID;
	obdd_node* new_node	= automaton_fast_pool_get_instance(table->mgr->nodes_pool, &fragment_ID);
#else
	obdd_node* new_node	= calloc(1, sizeof(obdd_node));
#endif
	new_node->var_ID	= var_ID;
	obdd_add_high_successor(new_node, high);
	obdd_add_low_successor(new_node, low);
	new_node->ref_count	= 0;
#if OBDD_USE_POOL
	new_node->fragment_ID = fragment_ID;
#endif
#if DEBUG_OBDD
		printf("(create)[%d]%p <%s>\n",new_node->var_ID, (void*)new_node, var);
#endif
	new_node->ref_count++;
	if(table->levels[var_ID] == NULL){
		table->levels[var_ID] = new_node;
	}else{
		if(last_node != NULL)last_node->next 	= new_node;
		new_node->next	= current_node;
	}
	return new_node;

	return NULL;
}
void obdd_table_destroy(obdd_table *table){
	uint32_t i,j;
	obdd_node *current_node, *next_node;
	for(i = 0; i < table->size; i++){
		current_node	= table->levels[i];
		while(current_node != NULL){
			current_node->ref_count--;
			next_node = current_node->next;
			if(current_node->ref_count == 0)obdd_node_destroy(current_node);
			current_node = next_node;
		}
	}
	free(table->levels);
	free(table->dead_nodes);
	free(table->levels_counts);
	free(table);
}

