#include "automaton_composite_hash_table.h"

automaton_composite_hash_table *automaton_composite_hash_table_create(uint32_t automata_count, uint32_t *automata_state_count){
	automaton_composite_hash_table *hash_table	= calloc(1, sizeof(automaton_composite_hash_table));

	uint32_t i;
	hash_table->automata_count	= automata_count;
	hash_table->slots	= 8;
	while (hash_table->slots < COMPOSITE_TABLE_SLOTS)hash_table->slots <<= 1;
	uint32_t log_slot = 1;
	uint32_t log_value = 0;
	while(log_slot < hash_table->slots[i]){
		log_slot <<= 1;
		log_value++;
	}
	hash_table->shift = sizeof(uint32_t) * 8 - log_value;
	hash_table->composite_count	= 0;
	hash_table->max_keys = hash_table->slots * COMPOSITE_TABLE_MAX_DENSITY;
	hash_table->levels	= calloc(hash_table->slots, sizeof(automaton_composite_hash_table_entry));
	//get max order
	uint32_t max_order = 1;
	for(i = 0; i < automata_count; i++){
		if(automata_state_count[i] == 0){printf("Automaton state count was zero\n"); exit(-1);}
		max_order *= i+1;
	}
	log_slot	= 1;
	//get max order length in bits
	hash_table->order_bits	= 0;
	while(log_slot < max_order){
		log_slot <<= 1;
		hash_table->order_bits++;
	}
	//check if available bits are enough for the rest of the hash
	log_value = 0;
	for(i = 0; i < automata_count; i++){
		log_slot = 1;
		while(log_slot < automata_state_count[i]){
			log_slot <<= 1;
			log_value++;
		}
	}
	if(log_value > (128 - hash_table->order_bits)){
		printf("Not enough bits in hash key to map composite states\n");
		exit(-1);
	}
#if OBDD_USE_POOL
	hash_table->entries_pool	= automaton_fast_pool_create(sizeof(automaton_composite_hash_table_entry),
			CT_FRAGMENTS_SIZE, CT_FRAGMENT_SIZE);
#endif
	hash_table->log_sizes	= calloc(automata_count, sizeof(uint32_t));
	hash_table->previous_order	= NULL;

	return hash_table;
}

void automaton_composite_hash_table_resize(automaton_composite_hash_table* table){
	uint32_t old_slots	= table->slots;
	uint32_t old_shift	= table->shift;
	automaton_composite_hash_table_entry **old_level	= table->levels;

	/* Compute the new size of the subtable. */
	table->slots = old_slots << 1;
	table->shift = old_shift - 1;
	table->max_keys = table->slots * COMPOSITE_TABLE_MAX_DENSITY;
	table->levels	= calloc(table->slots, sizeof(automaton_composite_hash_table_entry*));

	uint32_t j, pos;
	automaton_composite_hash_table_entry **even_node, **odd_node, *current_node, *next_node;
	/* Move the nodes from the old table to the new table.
	** This code depends on the type of hash function.
	** It assumes that the effect of doubling the size of the table
	** is to retain one more bit of the 32-bit hash value.
	** The additional bit is the LSB. */
	for (j = 0; j < old_slots; j++) {
	    current_node = old_level[j];
	    even_node = &(table->levels[j<<1]);
	    odd_node = &(table->levels[(j<<1)+1]);
	    while (current_node != NULL) {
		next_node = current_node->next;
		pos = current_node->original_key >> table->shift;
		if (pos & 1) {
		    *odd_node = current_node;
		    odd_node = &(current_node->next);
		} else {
		    *even_node = current_node;
		    even_node = &(current_node->next);
		}
		current_node = next_node;
	    }
	    *even_node = *odd_node = NULL;
	}
	free(old_level);
}
/*
typedef struct automaton_composite_hash_entry_str{
	uint128_t key;
	uint32_t state;
	struct automaton_composite_hash_entry_str *next;
}automaton_composite_hash_entry;
typedef struct automaton_composite_hash_table_str{
	uint32_t automata_count;
	uint32_t *max_states;
	uint32_t order_bits;
	uint32_t slots;
	uint32_t shift;
	uint32_t max_keys;
	automaton_composite_hash_entry **levels;
}automaton_composite_hash_table;
*/
uint32_t automaton_composite_hash_table_get_state(automaton_composite_hash_table *table, uint32_t *composite_states){
	//get first hash
	uint32_t i, j, pos = 0, original_key = 0;
	for(i = 0; i < table->automata_count; i++){
		original_key += composite_states[i] * CT_P1;
	}
	pos = original_key >> table->shift;

	//check if order was kept between the last call and this one
	bool ordered = true;
	if(table->previous_order == NULL){
		ordered	= false;
		table->previous_order	= calloc(table->automata_count, sizeof(uint32_t));
	}else{
		uint32_t previous_state = composite_states[table->previous_order[0]];
		for(i = 1; i < table->automata_count; i++){
			if(previous_state < composite_states[table->previous_order[i]]){
				ordered = false;
				break;
			}
		}
	}
	//if order was kept, then there is no need to recompute the order key
	//otherwise compute and update order and key
	uint32_t current_order_key, order_fact;
	if(ordered){
		current_order_key	= table->previous_order_key;
	}else{
		uint32_t previous_max_value = 0, max_value = 0, max_order = 0;
		table->previous_order_key	= 0;
		order_fact = 1;
		for(j = 0; j < table->automata_count; j++){
			order_fact *= j + 1;
			for(i = 0; i < table->automata_count; i++){
				if(composite_states[i] > max_value && (composite_states[i] < previous_max_value || previous_max_value == 0)){
					max_value = composite_states[i];
					max_order = i;
				}
			}
			previous_max_value = max_value;
			table->previous_order[j]	= max_order;
		}
		//get index of current ordering within n!
		for(j = 0; j < table->automata_count; j++){
			order_fact /= (table->automata_count - j);
			table->previous_order_key += table->previous_order[j] * order_fact;
		}
	}
	//compute compound key (concatenation of states plus order key)
	uint128_t compound_key	= 0;
	uint32_t current_value;
	for(i =1 ; i < table->automata_count; i++){
		table->log_sizes[i]	= 0;
		current_value	= composite_states[i];
		while(current_value > 0){
			table->log_sizes[i]++;
			current_value >>= 1;
		}
	}
	//build compound key
	for(i = 0; i < table->automata_count; i++){
		if(i > 0)compound_key <<= table->log_sizes[i];
		compound_key |= composite_states[i];
	}
	compound_key <<= table->order_bits;
	compound_key |= table->previous_order_key;

	//check if node exists

#if CT_USE_POOL
	uint32_t fragment_ID;
	automaton_composite_hash_table_entry* new_entry	= automaton_fast_pool_get_instance(table->entries_pool, &fragment_ID);
	new_entry->fragment_ID	= fragment_ID;
#else
	automaton_composite_hash_table_entry* new_entry	= calloc(1, sizeof(automaton_composite_hash_table_entry));
#endif
	new_entry->original_key	= original_key;

	return 0;
}

void automaton_composite_hash_table_destroy(automaton_composite_hash_table *table){
	free(table->max_states);
	uint32_t i;
	automaton_composite_hash_table_entry *current_entry = NULL, *next_entry = NULL;
	for(i = 0; i < table->slots; i++){
		current_entry	= table->levels[i];
		while(current_entry != NULL){
			next_entry	= current_entry->next;
#if CT_USE_POOL
			automaton_fast_pool_release_instance(table->entries_pool, current_entry->fragment_ID);
#else
			free(next_entry);
#endif
			next_entry->next	= NULL;
			current_entry	= next_entry;
		}
	}
	if(table->previous_order != NULL){
		free(table->previous_order);
	}
	free(table->log_sizes);
	free(table->levels);
	free(table);
}
