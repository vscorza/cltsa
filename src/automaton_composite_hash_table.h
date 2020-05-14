/*
 * automaton_composite_hash_table.h
 *
 *  Created on: May 13, 2020
 *      Author: mariano
 */

#ifndef AUTOMATON_COMPOSITE_HASH_TABLE_H_
#define AUTOMATON_COMPOSITE_HASH_TABLE_H_

#include "automaton_utils.h"
#include "fast_pool.h"

#define COMPOSITE_TABLE_SLOTS		256
#define COMPOSITE_TABLE_MAX_DENSITY	4
#define CT_USE_POOL 1

/* Primes for cache hash functions. */
#define CT_P1			12582917
#define CT_P2			4256249
#define CT_P3			741457
#define CT_P4			1618033999
#define CT_FRAGMENT_SIZE	100
#define CT_FRAGMENTS_SIZE	100
/** COMPOSITE HASH TABLE **/
typedef struct automaton_composite_hash_table_entry_str{
	uint128_t key;
	uint32_t state;
	uint32_t fragment_ID;
	uint32_t original_key;
	struct automaton_composite_hash_table_entry_str *next;
}automaton_composite_hash_table_entry;
typedef struct automaton_composite_hash_table_str{
	uint32_t automata_count;
	uint32_t *max_states;
	uint32_t order_bits;
	uint32_t slots;
	uint32_t shift;
	uint32_t *log_sizes;
	uint32_t composite_count;
	uint32_t max_keys;
	automaton_composite_hash_table_entry **levels;
	uint32_t max_state;
	uint32_t *previous_order;//assume orders are mostly preserved between calls
	uint32_t previous_order_key;
#if CT_USE_POOL
	automaton_fast_pool*	entries_pool;
#endif
}automaton_composite_hash_table;

automaton_composite_hash_table *automaton_composite_hash_table_create(uint32_t automata_count, uint32_t *automata_state_count);
void automaton_composite_hash_table_resize(automaton_composite_hash_table* table);
uint32_t automaton_composite_hash_table_get_state(automaton_composite_hash_table *table, uint32_t *composite_states);
void automaton_composite_hash_table_destroy(automaton_composite_hash_table *table);
#endif /* AUTOMATON_COMPOSITE_HASH_TABLE_H_ */
