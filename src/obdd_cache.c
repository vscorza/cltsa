/*
 * obdd_cache.c
 *
 *  Created on: Mar 28, 2019
 *      Author: mariano
 */
#include "obdd_cache.h"

obdd_cache* obdd_cache_create(uint32_t cache_size, uint32_t cache_max_size){
	obdd_cache* cache	= calloc(1, sizeof(obdd_cache));
	uint32_t log_size	= 0;
	uint32_t size_copy	= cache_size;
	while(size_copy > 1){log_size++; size_copy >>= 1;}
	//fix cache_size to power of 2
	cache_size			= 1U << log_size;
	cache->cache_items	= calloc(cache_size + 1, sizeof(obdd_cache_item));
	cache->cache_slots	= cache_size;
	cache->cache_shift	= sizeof(int32_t) * 8 - log_size;
	cache->max_cache_hard	= cache_max_size;
	cache->cache_slack	= cache_max_size;
	cache->cache_misses	= (double)(int32_t)(cache_size * .3f + 1);
	cache->cache_hits	= 0;
	cache->cache_collisions	= 0;
	cache->cache_inserts	= 0;
	return cache;
}

void obdd_cache_insert(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g, obdd_node *h, obdd_node *data){
	int32_t pos;
	obdd_cache_item *item;
	uintptr_t uf, ug, uh;

	uf	= (uintptr_t) f | (op & 0xe);
	ug	= (uintptr_t) g | (op >> 4);
	uh	= (uintptr_t) h;

	cache->cache_collisions	+= item->data != NULL;
	cache->cache_inserts++;

	pos	= ddCHash2(uh, uf, ug, cache->cache_shift);
	item= &(cache->cache_items[pos]);

	item->f	= (obdd_node*) uf;
	item->g	= (obdd_node*) ug;
	item->h	= uh;
	item->data	= data;
}
void obdd_cache_insert2(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g, obdd_node *data){
	int32_t pos;
	obdd_cache_item *item;
	uintptr_t uf, ug;

	cache->cache_collisions	+= item->data != NULL;
	cache->cache_inserts++;

	pos	= ddCHash2(op, f, g, cache->cache_shift);
	item= &(cache->cache_items[pos]);

	item->f	= (obdd_node*) uf;
	item->g	= (obdd_node*) ug;
	item->h	= op;
	item->data	= data;
}

void obdd_cache_insert1(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *data){
	int32_t pos;
	obdd_cache_item *item;

	cache->cache_collisions	+= item->data != NULL;
	cache->cache_inserts++;

	pos	= ddCHash2(op, f, f, cache->cache_shift);
	item= &(cache->cache_items[pos]);

	item->f	= f;
	item->g	= f;
	item->h	= op;
	item->data	= data;
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
	int32_t pos;
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
void obdd_cache_resize(obdd_cache *cache){
	obdd_cache_item *items_old, *current_item, *new_item;
	int32_t i, pos, shift = --(cache->cache_shift);
	uint32_t cache_slots, cache_old_slots;
	double offset;
	int32_t moved	= 0;
	items_old		= cache->cache_items;
	cache_old_slots	= cache->cache_slots;
	cache_slots		= cache->cache_slots	= cache_old_slots << 1;

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
	cache->cache_misses	= cache->cache_misses - ((double)(int32_t)(cache_slots * .3f + 1));
	cache->cache_hits	= 0;
}
void obdd_cache_flush(obdd_cache *cache){
	int32_t i	= 0;
	for(i = 0; i < cache->cache_slots; i++)cache->cache_items[i].data	= NULL;
}
void obdd_cache_destroy(obdd_cache *cache){
	obdd_cache_flush(cache);
	if(cache->cache_items != NULL)
		free(cache->cache_items);
	cache->cache_items	= NULL;
}


