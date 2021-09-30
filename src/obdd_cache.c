/*
 * obdd_cache.c
 *
 *  Created on: Mar 28, 2019
 *      Author: mariano
 */
#include "obdd.h"

obdd_cache *obdd_cache_create(obdd_mgr *mgr, uint32_t cache_size, uint32_t cache_max_size) {
  obdd_cache *cache = calloc(1, sizeof(obdd_cache));
  cache->mgr = mgr;
  uint32_t log_size = 0;
  uint32_t size_copy = cache_size;
  while (size_copy > 1) {
    log_size++;
    size_copy >>= 1;
  }
  // fix cache_size to power of 2
  cache_size = 1U << log_size;
  cache->cache_items = calloc(cache_size + 1, sizeof(obdd_cache_item));
  cache->cache_slots = cache_size;
  cache->cache_shift = sizeof(int64_t) * 8 - log_size;
  cache->max_cache_hard = cache_max_size;
  cache->cache_slack = cache_max_size;
  cache->cache_misses = (double)(int32_t)(cache_size * .3f + 1);
  cache->cache_hits = 0;
  cache->min_hits = .3f;
  cache->cache_collisions = 0;
  cache->cache_inserts = 0;
  cache->vars_size = LIST_INITIAL_SIZE;
  cache->cache_vars = calloc(cache->vars_size, sizeof(obdd_node *));
  cache->cache_neg_vars = calloc(cache->vars_size, sizeof(obdd_node *));
  return cache;
}

void obdd_cache_insert(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g, obdd_node *h, obdd_node *data) {
  int32_t pos;
  obdd_cache_item *item;
  uintptr_t uf, ug, uh;

  uf = (uintptr_t)f | (op & 0xe);
  ug = (uintptr_t)g | (op >> 4);
  uh = (uintptr_t)h;

  pos = ddCHash2(uh, uf, ug, cache->cache_shift);
  item = &(cache->cache_items[pos]);

  cache->cache_collisions += item->data != NULL;
  cache->cache_inserts++;

  if (item->data != NULL) {
    item->data->ref_count--;

    if (item->data->ref_count == 0) {
      // obdd_node_destroy(cache->mgr, item->data);
    }
  }

  item->f = (obdd_node *)uf;
  item->g = (obdd_node *)ug;
  item->h = uh;
  item->data = data;
  item->data->ref_count++;
}
void obdd_cache_insert2(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g, obdd_node *data) {
  int32_t pos;
  obdd_cache_item *item;
  uintptr_t uf, ug;

  pos = ddCHash2(op, f, g, cache->cache_shift);
  item = &(cache->cache_items[pos]);

  cache->cache_collisions += item->data != NULL;
  cache->cache_inserts++;

  if (item->data != NULL) {
    item->data->ref_count--;

    if (item->data->ref_count == 0) {
      // obdd_node_destroy(cache->mgr, item->data);
    }
  }

  item->f = f;
  item->g = g;
  item->h = op;
  item->data = data;
  item->data->ref_count++;
}

void obdd_cache_insert1(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *data) {
  int32_t pos;
  obdd_cache_item *item;

  pos = ddCHash2(op, f, f, cache->cache_shift);
  item = &(cache->cache_items[pos]);

  cache->cache_collisions += item->data != NULL;
  cache->cache_inserts++;

  if (item->data != NULL) {
    item->data->ref_count--;
    if (item->data->ref_count == 0) {
      // obdd_node_destroy(cache->mgr, item->data);
    }
  }

  item->f = f;
  item->g = f;
  item->h = op;
  item->data = data;
  data->ref_count++;
}

obdd_node *obdd_cache_insert_var(obdd_cache *cache, obdd_var_size_t var_id) {
  if ((var_id + 1) > cache->vars_size) {
    uint32_t i, old_size = cache->vars_size;
    cache->vars_size = var_id * LIST_INCREASE_FACTOR;
    obdd_node **ptr = realloc(cache->cache_vars, sizeof(obdd_node *) * (cache->vars_size));
    if (ptr == NULL) {
      printf("Could not allocate memory[obdd_cache_insert_var:1]\n");
      exit(-1);
    } else {
      cache->cache_vars = ptr;
      for (i = old_size; i < cache->vars_size; i++) {
        cache->cache_vars[i] = NULL;
      }
    }
    ptr = realloc(cache->cache_neg_vars, sizeof(obdd_node *) * (cache->vars_size));
    if (ptr == NULL) {
      printf("Could not allocate memory[obdd_cache_insert_var:2]\n");
      exit(-1);
    } else {
      cache->cache_neg_vars = ptr;
      for (i = old_size; i < cache->vars_size; i++) {
        cache->cache_neg_vars[i] = NULL;
      }
    }
  }
  if (cache->cache_vars[var_id] == NULL) {
    cache->cache_vars[var_id] =
        obdd_mgr_mk_node_ID(cache->mgr, var_id, cache->mgr->true_obdd->root_obdd, cache->mgr->false_obdd->root_obdd);
    cache->cache_vars[var_id]->ref_count++;
  }
  return cache->cache_vars[var_id];
}

obdd_node *obdd_cache_insert_neg_var(obdd_cache *cache, obdd_var_size_t var_id) {
  if ((var_id + 1) > cache->vars_size) {
    uint32_t i, old_size = cache->vars_size;
    cache->vars_size = var_id * LIST_INCREASE_FACTOR;
    obdd_node **ptr = realloc(cache->cache_vars, sizeof(obdd_node *) * (cache->vars_size));
    if (ptr == NULL) {
      printf("Could not allocate memory[obdd_cache_insert_neg_var:1]\n");
      exit(-1);
    } else {
      cache->cache_vars = ptr;
      for (i = old_size; i < cache->vars_size; i++) {
        cache->cache_vars[i] = NULL;
      }
    }
    ptr = realloc(cache->cache_neg_vars, sizeof(obdd_node *) * (cache->vars_size));
    if (ptr == NULL) {
      printf("Could not allocate memory[obdd_cache_insert_neg_var:2]\n");
      exit(-1);
    } else {
      cache->cache_neg_vars = ptr;
      for (i = old_size; i < cache->vars_size; i++) {
        cache->cache_neg_vars[i] = NULL;
      }
    }
  }
  if (cache->cache_neg_vars[var_id] == NULL) {
    cache->cache_neg_vars[var_id] =
        obdd_mgr_mk_node_ID(cache->mgr, var_id, cache->mgr->false_obdd->root_obdd, cache->mgr->true_obdd->root_obdd);
    cache->cache_neg_vars[var_id]->ref_count++;
  }
  return cache->cache_neg_vars[var_id];
}

obdd_node *obdd_cache_lookup(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g, obdd_node *h) {
  obdd_cache_item *item;
  obdd_node *data;
  int32_t pos;
  uintptr_t uf, ug, uh;

  uf = (uintptr_t)f | (op & 0xe);
  ug = (uintptr_t)g | (op >> 4);
  uh = (uintptr_t)h;

  pos = ddCHash2(uh, uf, ug, cache->cache_shift);
  item = &(cache->cache_items[pos]);

  if (item->data != NULL && item->f == (obdd_node *)uf && item->g == (obdd_node *)ug && item->h == uh) {
    data = item->data;
    cache->cache_hits++;
    return data;
  }

  cache->cache_misses++;
  if (cache->cache_slack >= 0 && cache->cache_hits > cache->cache_misses * cache->min_hits)
    obdd_cache_resize(cache);
  return NULL;
}
obdd_node *obdd_cache_lookup2(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g) {
  obdd_cache_item *item;
  obdd_node *data;
  uint64_t pos;
  uintptr_t uf, ug, uh;

  pos = ddCHash2(op, f, g, cache->cache_shift);

  item = &(cache->cache_items[pos]);

  if (item->data != NULL && item->f == f && item->g == g && item->h == op) {
    data = item->data;
    cache->cache_hits++;
    return data;
  }

  cache->cache_misses++;
  if (cache->cache_slack >= 0 && cache->cache_hits > cache->cache_misses * cache->min_hits)
    obdd_cache_resize(cache);
  return NULL;
}
obdd_node *obdd_cache_lookup1(obdd_cache *cache, uintptr_t op, obdd_node *f) {
  obdd_cache_item *item;
  obdd_node *data;
  int32_t pos;
  uintptr_t uf, ug, uh;

  pos = ddCHash2(op, f, f, cache->cache_shift);
  item = &(cache->cache_items[pos]);

  if (item->data != NULL && item->f == f && item->h == op) {
    data = item->data;
    cache->cache_hits++;
    return data;
  }

  cache->cache_misses++;
  if (cache->cache_slack >= 0 && cache->cache_hits > cache->cache_misses * cache->min_hits)
    obdd_cache_resize(cache);
  return NULL;
}
obdd_node *obdd_cache_constant_lookup(obdd_cache *cache, uintptr_t op, obdd_node *f, obdd_node *g, obdd_node *h) {
  obdd_cache_item *item;
  obdd_node *data;
  int32_t pos;
  uintptr_t uf, ug, uh;

  uf = (uintptr_t)f | (op & 0xe);
  ug = (uintptr_t)g | (op >> 4);
  uh = (uintptr_t)h;

  pos = ddCHash2(uh, uf, ug, cache->cache_shift);
  item = &(cache->cache_items[pos]);

  // no reclaim

  if (item->data != NULL && item->f == (obdd_node *)uf && item->g == (obdd_node *)ug && item->h == uh) {
    data = item->data;
    cache->cache_hits++;
    return data;
  }

  cache->cache_misses++;
  if (cache->cache_slack >= 0 && cache->cache_hits > cache->cache_misses * cache->min_hits)
    obdd_cache_resize(cache);
  return NULL;
}

obdd_node *obdd_cache_lookup_var(obdd_cache *cache, uint32_t var_id) {
  if ((var_id + 1) > cache->vars_size) {
    printf("[OBDD_CACHE] var out of range");
    exit(-1);
  }
  return cache->cache_vars[var_id];
}
obdd_node *obdd_cache_lookup_neg_var(obdd_cache *cache, uint32_t var_id) {
  if ((var_id + 1) > cache->vars_size) {
    printf("[OBDD_CACHE] var out of range");
    exit(-1);
  }
  return cache->cache_neg_vars[var_id];
}

void obdd_cache_resize(obdd_cache *cache) {

  obdd_cache_item *items_old, *current_item, *new_item;
  int32_t i, pos, shift = --(cache->cache_shift);
  uint32_t cache_slots, cache_old_slots;
  double offset;
  int32_t moved = 0;
  items_old = cache->cache_items;
  cache_old_slots = cache->cache_slots;
  cache_slots = cache->cache_slots = cache_old_slots << 1;
#if OBDD_CACHE_DEBUG
  printf("Resizing cache from %d to %d slots\n", cache_old_slots, cache_slots);
#endif

  cache->cache_items = calloc(cache->cache_slots + 1, sizeof(obdd_cache_item));

  for (i = 0; (uint32_t)i < cache_old_slots; i++) {
    current_item = &(items_old[i]);
    if (current_item->data != NULL) {
      pos = ddCHash2(current_item->h, current_item->f, current_item->g, shift);
      new_item = &(cache->cache_items[pos]);
      new_item->f = current_item->f;
      new_item->g = current_item->g;
      new_item->h = current_item->h;
      new_item->data = current_item->data;
    }
  }

  free(items_old);
  cache->cache_misses = ((double)(int32_t)(cache_slots * cache->min_hits + 1));
  cache->cache_hits = 0;
}
void obdd_cache_flush(obdd_cache *cache) {
  int32_t i = 0;
  for (i = 0; (uint32_t)i < cache->cache_slots; i++) {
    if (cache->cache_items[i].data != NULL) {
      obdd_node *current_node = cache->cache_items[i].data;
      current_node->ref_count--;
      if (cache->cache_items[i].data->ref_count == 0) {
        obdd_node_destroy(cache->mgr, cache->cache_items[i].data);
      }
    }
    cache->cache_items[i].data = NULL;
  }
  for (i = 0; (uint32_t)i < cache->vars_size; i++) {
    if (cache->cache_vars[i] != NULL) {
      cache->cache_vars[i]->ref_count--;
      if (cache->cache_vars[i]->ref_count == 0)
        obdd_node_destroy(cache->mgr, cache->cache_vars[i]);
    }
    if (cache->cache_neg_vars[i] != NULL) {
      cache->cache_neg_vars[i]->ref_count--;
      if (cache->cache_neg_vars[i]->ref_count == 0)
        obdd_node_destroy(cache->mgr, cache->cache_neg_vars[i]);
    }
    cache->cache_vars[i] = NULL;
    cache->cache_neg_vars[i] = NULL;
  }
}
void obdd_cache_destroy(obdd_cache *cache) {
  obdd_cache_flush(cache);
  if (cache->cache_items != NULL)
    free(cache->cache_items);
  cache->cache_items = NULL;
  free(cache->cache_vars);
  cache->cache_vars = NULL;
  free(cache->cache_neg_vars);
  cache->cache_neg_vars = NULL;
  cache->mgr = NULL;
}

/** OBDD TABLE CACHE **/

obdd_table *obdd_table_create(obdd_mgr *mgr) {
  uint32_t i, j;
  uint32_t log_slot = 1, log_value = 0;
  obdd_table *new_table = calloc(1, sizeof(obdd_table));
  new_table->size = LIST_INITIAL_SIZE;
  new_table->slots = calloc(new_table->size, sizeof(uint32_t));
  new_table->shift = calloc(new_table->size, sizeof(uint32_t));
  new_table->max_keys = calloc(new_table->size, sizeof(uint32_t));
  for (i = 0; i < new_table->size; i++) {
    new_table->slots[i] = 8;
    while (new_table->slots[i] < OBDD_TABLE_SLOTS)
      new_table->slots[i] <<= 1;
    log_slot = 1;
    log_value = 0;
    while (log_slot < new_table->slots[i]) {
      log_slot <<= 1;
      log_value++;
    }
    new_table->shift[i] = sizeof(uint32_t) * 8 - log_value;
    new_table->max_keys[i] = new_table->slots[i] * OBDD_TABLE_MAX_DENSITY;
  }

  new_table->levels = calloc(new_table->size, sizeof(obdd_node **));
  for (i = 0; i < new_table->size; i++) {
    new_table->levels[i] = calloc(new_table->slots[i], sizeof(obdd_node *));
  }
  new_table->levels_composite_counts = calloc(new_table->size, sizeof(uint64_t));
  new_table->mgr = mgr;
  new_table->max_live_fast_nodes = 0;
  new_table->live_fast_nodes = 0;
  new_table->fast_hits = 0;
  new_table->fast_misses = 0;
  return new_table;
}
/*
 typedef struct obdd_table_t{
        obdd_mgr *mgr;
        uint32_t size;
        uint32_t *slots;
        uint32_t *shift;
        uint32_t *max_keys;
        obdd_fast_node **levels;
        uint64_t *levels_composite_counts;
        uint64_t max_live_fast_nodes;
        uint64_t live_fast_nodes;
        uint64_t fast_hits;
        uint64_t fast_misses;
}obdd_table;
 * */
void obdd_table_resize(obdd_table *table, uint32_t level) {
  uint32_t old_slots = table->slots[level];
  uint32_t old_shift = table->shift[level];
  obdd_node **old_level = table->levels[level];

  /* Compute the new size of the subtable. */
  table->slots[level] = old_slots << 1;
  table->shift[level] = old_shift - 1;
  table->max_keys[level] = table->slots[level] * OBDD_TABLE_MAX_DENSITY;
  table->levels[level] = calloc(table->slots[level], sizeof(obdd_node *));

  uint32_t j, pos;
  obdd_node **even_node, **odd_node, *current_node, *next_node;
  /* Move the nodes from the old table to the new table.
  ** This code depends on the type of hash function.
  ** It assumes that the effect of doubling the size of the table
  ** is to retain one more bit of the 32-bit hash value.
  ** The additional bit is the LSB. */
  for (j = 0; j < old_slots; j++) {
    current_node = old_level[j];
    even_node = &(table->levels[level][j << 1]);
    odd_node = &(table->levels[level][(j << 1) + 1]);
    while (current_node != NULL) {
      next_node = current_node->next;
      pos = ddHash(current_node->high_obdd, current_node->low_obdd, table->shift[level]);
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

void obdd_table_node_add(obdd_table *table, obdd_node *node) {
  obdd_node *current_node = NULL, *last_node = NULL;
  obdd_node *current_fast_node = NULL, *previous_fast_node = NULL;
  uint32_t i, fragment_ID;
  // resize if needed
  if (node->var_ID >= (table->size - 1)) {
    uint32_t old_size = table->size;
    while ((table->size - 1) <= node->var_ID)
      table->size *= 2;
    obdd_node ***ptr = realloc(table->levels, sizeof(obdd_node **) * (table->size));
    if (ptr == NULL) {
      printf("Could not allocate memory[obdd_table_node_add:1]\n");
      exit(-1);
    } else
      table->levels = ptr;
    uint64_t *uint_ptr = realloc(table->levels_composite_counts, sizeof(uint64_t) * (table->size));
    if (uint_ptr == NULL) {
      printf("Could not allocate memory[obdd_table_node_add:2]\n");
      exit(-1);
    } else
      table->levels_composite_counts = uint_ptr;

    uint32_t *uint32_ptr = realloc(table->slots, sizeof(uint32_t) * table->size);
    if (uint32_ptr == NULL) {
      printf("Could not allocate memory[obdd_table_node_add:3]\n");
      exit(-1);
    } else
      table->slots = uint32_ptr;
    uint32_ptr = realloc(table->shift, sizeof(uint32_t) * table->size);
    if (uint32_ptr == NULL) {
      printf("Could not allocate memory[obdd_table_node_add:4]\n");
      exit(-1);
    } else
      table->shift = uint32_ptr;
    uint32_ptr = realloc(table->max_keys, sizeof(uint32_t) * table->size);
    if (uint32_ptr == NULL) {
      printf("Could not allocate memory[obdd_table_node_add:5]\n");
      exit(-1);
    } else
      table->max_keys = uint32_ptr;
    uint32_t log_slot, log_value;
    for (i = old_size; i < table->size; i++) {
      table->slots[i] = 8;
      while (table->slots[i] < OBDD_TABLE_SLOTS)
        table->slots[i] <<= 1;
      log_slot = 1;
      log_value = 0;
      while (log_slot < table->slots[i]) {
        log_slot <<= 1;
        log_value++;
      }
      table->shift[i] = sizeof(uint32_t) * 8 - log_value;
      table->max_keys[i] = table->slots[i] * OBDD_TABLE_MAX_DENSITY;
      table->levels[i] = calloc(table->slots[i], sizeof(obdd_node *));
      table->levels_composite_counts[i] = 0;
    }
  }
  // pick entry level
  uint32_t pos = ddHash(node->high_obdd, node->low_obdd, table->shift[node->var_ID]);

  if (table->levels[node->var_ID][pos] == NULL) {
    table->live_fast_nodes++;
    if (table->live_fast_nodes > table->max_live_fast_nodes)
      table->max_live_fast_nodes = table->live_fast_nodes;
    table->levels_composite_counts[node->var_ID]++;
    node->next = NULL;
    table->levels[node->var_ID][pos] = node;
  } else {
    current_fast_node = table->levels[node->var_ID][pos];
    previous_fast_node = NULL;
    while (current_fast_node != NULL) {
      if ((current_fast_node->next != NULL) &&
          ((current_fast_node->low_obdd < node->low_obdd) ||
           ((current_fast_node->low_obdd == node->low_obdd) && (current_fast_node->high_obdd < node->high_obdd)))) {
        previous_fast_node = current_fast_node;
        current_fast_node = current_fast_node->next;
      } else if ((current_fast_node->low_obdd == node->low_obdd) && (current_fast_node->high_obdd == node->high_obdd)) {
        return;
      } else {
        table->live_fast_nodes++;
        if (table->live_fast_nodes > table->max_live_fast_nodes)
          table->max_live_fast_nodes = table->live_fast_nodes;
        table->levels_composite_counts[node->var_ID]++;
        // current_fast_node = node;
        node->next = current_fast_node;
        if (previous_fast_node != NULL) {
          previous_fast_node->next = node;
        } else {
          table->levels[node->var_ID][pos] = node;
        }
        current_fast_node = node;
        break;
      }
    }
  }
  if (table->levels_composite_counts[node->var_ID] > table->max_keys[node->var_ID]) {
    obdd_table_resize(table, node->var_ID);
  }
}

void obdd_table_node_destroy(obdd_table *table, obdd_node *node) {
  obdd_node *current_fast_node = NULL, *last_fast_node = NULL, *to_remove = NULL;
  uint32_t pos = ddHash(node->high_obdd, node->low_obdd, table->shift[node->var_ID]);
  if (node->var_ID >= (table->size))
    return;
  if (table->levels[node->var_ID][pos] == NULL)
    return;

  uint32_t current_level = 0;
  bool found = false;
  current_fast_node = table->levels[node->var_ID][pos];
  while (current_fast_node != NULL) {
    if ((current_fast_node->low_obdd < node->low_obdd) ||
        ((current_fast_node->low_obdd == node->low_obdd) && (current_fast_node->high_obdd < node->high_obdd))) {
      last_fast_node = current_fast_node;
      if (current_fast_node->next != NULL) {
        current_fast_node = current_fast_node->next;
      } else {
        break;
      }
    } else if ((current_fast_node->low_obdd == node->low_obdd) && (current_fast_node->high_obdd == node->high_obdd)) {
      to_remove = current_fast_node;
      if (last_fast_node != NULL) {
        last_fast_node->next = current_fast_node->next;
      } else {
        table->levels[node->var_ID][pos] = current_fast_node->next;
      }
      last_fast_node = NULL;
      table->live_fast_nodes--;
      table->levels_composite_counts[node->var_ID]--;
      current_level++;
      found = true;
      break;
    } else {
      break;
    }
  }
  if (found)
    node->ref_count--;
}

obdd_node *obdd_table_search_node_ID(obdd_table *table, obdd_var_size_t var_ID, obdd_node *high, obdd_node *low) {
  uint32_t current_level = 0;
  obdd_node *current_node = NULL, *last_node = NULL, *tmp_node = NULL;
  if (var_ID >= (table->size)) {
    return NULL;
  }
  uint32_t pos = ddHash(high, low, table->shift[var_ID]);

  current_node = table->levels[var_ID][pos];
  while (current_node != NULL) {
    if ((current_node->low_obdd < low) || ((current_node->low_obdd == low) && (current_node->high_obdd < high))) {
      last_node = current_node;
      current_node = current_node->next;
    } else if ((current_node->low_obdd == low) && (current_node->high_obdd == high)) {
      table->fast_hits++;
      return current_node;
    } else {
      break;
    }
  }
  table->fast_misses++;
  return NULL;
}

obdd_node *obdd_table_mk_node_ID(obdd_table *table, obdd_var_size_t var_ID, obdd_node *high, obdd_node *low) {
  // if node is trivially redundant, solve redundancy in situ
  if (high == low) {
    return high;
  }
  // check if single var
  if (high->var_ID == 0 && low->var_ID == 1 && table->mgr->cache->cache_vars[var_ID] != NULL) {
    table->mgr->cache->cache_vars[var_ID]->ref_count++;
    return table->mgr->cache->cache_vars[var_ID];
  }
  if (high->var_ID == 1 && low->var_ID == 0 && table->mgr->cache->cache_neg_vars[var_ID] != NULL) {
    table->mgr->cache->cache_neg_vars[var_ID]->ref_count++;
    return table->mgr->cache->cache_neg_vars[var_ID];
  }
  // check if node exists
  obdd_node *current_node = NULL, *last_node = NULL;
  obdd_node *current_fast_node = NULL, *last_fast_node = NULL, *new_fast_node = NULL;
  current_fast_node = obdd_table_search_node_ID(table, var_ID, high, low);
  if (current_fast_node != NULL) {
    current_fast_node->ref_count++;
    return current_fast_node;
  }
  // return new node
#if OBDD_USE_POOL
  uint32_t fragment_ID;
  obdd_node *new_node = automaton_fast_pool_get_instance(table->mgr->nodes_pool, &fragment_ID);
#else
  obdd_node *new_node = calloc(1, sizeof(obdd_node));
#endif
  new_node->var_ID = var_ID;
  obdd_add_high_successor(new_node, high);
  obdd_add_low_successor(new_node, low);
  new_node->ref_count = 0;
#if OBDD_USE_POOL
  new_node->fragment_ID = fragment_ID;
#endif
#if DEBUG_OBDD
  printf("(create)[%d]%p <%s>\n", new_node->var_ID, (void *)new_node, var);
#endif
  new_node->ref_count++;

  obdd_table_node_add(table, new_node);

  return new_node;
}
void obdd_table_destroy(obdd_table *table) {
  uint32_t i, j;
  obdd_node *current_fast_node, *next_fast_node;
  for (i = 0; i < table->size; i++) {
    for (j = 0; j < table->slots[i]; j++) {
      current_fast_node = table->levels[i][j];
      while (current_fast_node != NULL) {
        next_fast_node = current_fast_node->next;
        current_fast_node->ref_count--;
        if (current_fast_node->ref_count == 0)
          obdd_node_destroy(table->mgr, current_fast_node);
        current_fast_node = next_fast_node;
      }
    }
  }
  for (i = 0; i < table->size; i++) {
    free(table->levels[i]);
  }
  free(table->levels_composite_counts);
  free(table->slots);
  free(table->shift);
  free(table->max_keys);
  free(table->levels);
  free(table);
}

void obdd_table_print_fast_lists(obdd_table *table) {
  printf("Obdd chace table fast lists, nodes reported as "
         "[curr[19:4]:next[19:4]]\n");
  uint32_t i, j;
  obdd_node *current_fast_node;
  for (i = 0; i < table->mgr->vars_dict->size; i++) {
    printf("\tList for var %s\n", table->mgr->vars_dict->entries[i].key);
    for (j = 0; j < table->slots[i]; j++) {
      if (table->levels[i][j] == NULL)
        continue;
      current_fast_node = table->levels[i][j];
      while (current_fast_node != NULL) {
        printf("[0x%x:0x%x]", (((uint64_t)current_fast_node) >> 4) % 0xFFFF, (((uint64_t)current_fast_node->next) >> 4) % 0xFFFF);
        current_fast_node = current_fast_node->next;
      }
      printf("\n");
    }
  }
}
