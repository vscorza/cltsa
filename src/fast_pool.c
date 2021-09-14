/*
 * fast_pool.c
 *
 *  Created on: Nov 15, 2018
 *      Author: mariano
 */
#include "fast_pool.h"

automaton_fast_pool *automaton_fast_pool_create(size_t sizeof_element,
                                                uint32_t initial_size,
                                                uint32_t fragment_size) {
  automaton_fast_pool *pool = malloc(sizeof(automaton_fast_pool));
  pool->fragment_size = fragment_size;
  pool->sizeof_element = sizeof_element;

  pool->size = initial_size;
  pool->fragments_count = calloc(pool->size, sizeof(uint32_t));
  pool->fragments_last_index = calloc(pool->size, sizeof(uint32_t));
  pool->fragments_last_free_element = calloc(pool->size, sizeof(uintptr_t));
  pool->fragments = calloc(pool->size, sizeof(uintptr_t));
  pool->composite_count = 0;

  uint32_t i;
  for (i = 0; i < pool->size; i++) {
    pool->fragments[i] =
        (uintptr_t)calloc(pool->fragment_size, pool->sizeof_element);
    pool->fragments_last_free_element[i] = pool->fragments[i];
  }
  pool->next_free_element = (uintptr_t)pool->fragments[0];
  pool->next_free_element_fragment_index = 0;
  return pool;
}

void automaton_fast_pool_destroy(automaton_fast_pool *pool) {
  uint32_t i;
  for (i = 0; i < pool->size; i++) {
    free((void *)(pool->fragments[i]));
  }
  free(pool->fragments_count);
  free(pool->fragments_last_index);
  free(pool->fragments_last_free_element);
  free(pool->fragments);
  free(pool);
}

void *automaton_fast_pool_get_instance(automaton_fast_pool *pool,
                                       uint32_t *fragment_ID) {
  // check if pool is full
  uint32_t i;
  uintptr_t value = (uintptr_t)NULL;
  if (pool->next_free_element == (uintptr_t)NULL) {
    uint32_t new_size = ++(pool->size);
    uintptr_t *ptr = realloc(pool->fragments, new_size * sizeof(uintptr_t));
    if (ptr != NULL) {
      pool->fragments = ptr;
      pool->size = new_size;
      pool->fragments[pool->size - 1] =
          (uintptr_t)calloc(pool->fragment_size, pool->sizeof_element);
    } else {
      printf("Could not get new instance from pool\n");
      exit(-1);
    }
    uint32_t *ptr_uint =
        realloc(pool->fragments_count, new_size * sizeof(uint32_t));
    if (ptr_uint != NULL) {
      pool->fragments_count = ptr_uint;
      pool->fragments_count[pool->size - 1] = 0;
    } else {
      printf("Could not get new instance from pool\n");
      exit(-1);
    }
    ptr_uint = realloc(pool->fragments_last_index, new_size * sizeof(uint32_t));
    if (ptr_uint != NULL) {
      pool->fragments_last_index = ptr_uint;
      pool->fragments_last_index[pool->size - 1] = 0;
    } else {
      printf("Could not get new instance from pool\n");
      exit(-1);
    }
    uintptr_t *ptr_void = realloc(pool->fragments_last_free_element,
                                  new_size * sizeof(uintptr_t));
    if (ptr_void != NULL) {
      pool->fragments_last_free_element = ptr_void;
      pool->fragments_last_free_element[pool->size - 1] =
          pool->fragments[pool->size - 1];
    } else {
      printf("Could not get new instance from pool\n");
      exit(-1);
    }
    pool->next_free_element = (uintptr_t)pool->fragments[pool->size - 1];
    pool->next_free_element_fragment_index = pool->size - 1;
  }
  value = (uintptr_t)pool->next_free_element;
  *fragment_ID = pool->next_free_element_fragment_index;
  // update structures
  pool->fragments_count[pool->next_free_element_fragment_index]++;
  pool->fragments_last_free_element[pool->next_free_element_fragment_index] =
      (pool->fragments_last_free_element
           [pool->next_free_element_fragment_index] +
       (uintptr_t)pool->sizeof_element);
  pool->fragments_last_index[pool->next_free_element_fragment_index]++;
  if (pool->fragments_last_index[pool->next_free_element_fragment_index] <
      (pool->fragment_size - 1)) {
    pool->next_free_element = ((uintptr_t)pool->fragments_last_free_element
                                   [pool->next_free_element_fragment_index]);
  } else {
    // check if there is an empty fragment somewhere
    bool found = false;
    for (i = 0; i < pool->size; i++) {
      if (pool->fragments_last_index[i] < (pool->fragment_size - 1)) {
        pool->next_free_element_fragment_index = i;
        pool->next_free_element =
            ((uintptr_t)pool->fragments_last_free_element
                 [pool->next_free_element_fragment_index]);
        found = true;
        break;
      }
    }
    if (!found) {
      pool->next_free_element = (uintptr_t)NULL;
    }
  }
#if DEBUG_POOL
  printf(ANSI_COLOR_GREEN "%p[++](pool)\n" ANSI_COLOR_RESET, value);
#endif
  pool->composite_count++;
  return (void *)value;
}
bool automaton_fast_pool_release_instance(automaton_fast_pool *pool,
                                          uint32_t fragment_ID) {
  if (pool->size <= fragment_ID) {
    printf("Fragment ID out of bounds\n");
    exit(-1);
  }
  if (pool->fragments_count[fragment_ID] == 0) {
    printf("Releasing on empty bucket\n");
    exit(-1);
  }
  pool->fragments_count[fragment_ID]--;
  if (pool->fragments_count[fragment_ID] ==
      0) { // if no element reset last index
    pool->fragments_last_index[fragment_ID] = 0;
    pool->fragments_last_free_element[fragment_ID] =
        pool->fragments[fragment_ID];
    if (pool->next_free_element_fragment_index == fragment_ID)
      pool->next_free_element =
          (uintptr_t)pool->fragments_last_free_element[fragment_ID];
  }
#if DEBUG_POOL
  printf(ANSI_COLOR_RED "%p[XX](pool)\n" ANSI_COLOR_RESET, instance);
#endif
  pool->composite_count--;
  return true;
}
