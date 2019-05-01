/*
 * obdd_cache.h
 *
 *  Created on: Mar 28, 2019
 *      Author: mariano
 */

#ifndef SRC_OBDD_CACHE_H_
#define SRC_OBDD_CACHE_H_

#include "obdd.h"

typedef uintptr_t ptruint;
/* Primes for cache hash functions. */
#define DD_P1			12582917
#define DD_P2			4256249
#define DD_P3			741457
#define DD_P4			1618033999
#define DD_MAX_CACHE_TO_SLOTS_RATIO 4

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
(((((unsigned)(f) + (unsigned)(o)) * DD_P1 + (unsigned)(g)) * DD_P2) >> (s))
#endif

typedef struct obdd_cache_item_str{
	obdd_node *f, *g;
	uintptr_t h;
	obdd_node *data;
}obdd_cache_item;

typedef struct obdd_cache_str{
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
}obdd_cache;

obdd_cache* obdd_cache_create(uint32_t cache_size, uint32_t cache_max_size);
void obdd_cache_insert(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g, obdd_node *h, obdd_node *data);
void obdd_cache_insert2(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g, obdd_node *data);
void obdd_cache_insert1(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *data);
obdd_node* obdd_cache_lookup(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g, obdd_node *h);
obdd_node* obdd_cache_lookup2(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g);
obdd_node* obdd_cache_lookup1(obdd_cache *cache, uintptr_t op, obdd_node *f);
obdd_node* obdd_cache_constant_lookup(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g, obdd_node *h);
void obdd_cache_resize(obdd_cache *cache);
void obdd_cache_flush(obdd_cache *cache);
void obdd_cache_destroy(obdd_cache *cache);

#endif /* SRC_OBDD_CACHE_H_ */
