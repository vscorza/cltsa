/*
 * automaton_utils.c
 *
 *  Created on: Apr 20, 2018
 *      Author: mariano
 */
#include "automaton_utils.h"
/** UTILS FUNCTIONS **/
void aut_add_ptr_list(void ***list, void *element, uint32_t *count) {
  void **new_list = malloc(sizeof(void *) * (*count + 1));
  uint32_t i;
  for (i = 0; i < *count; i++)
    new_list[i] = (*list)[i];
  new_list[*count] = element;
  if (*count > 0)
    free(*list);
  *count = *count + 1;
  *list = new_list;
}
void aut_add_incr_ptr_list(void ***list, void *element, uint32_t *size, uint32_t *count) {
  if (*count < *size) {
    *list[*count] = element;
    *count = *count + 1;
  } else {
    uint32_t new_size = *size * LIST_INCREASE_FACTOR;
    void **new_list = malloc(sizeof(void *) * (new_size));
    uint32_t i;
    for (i = 0; i < *count; i++)
      new_list[i] = *list[i];
    new_list[*count] = element;
    *count = *count + 1;
    *size = new_size;
    free(*list);
    *list = new_list;
  }
}
void aut_free_ptr_list(void ***list, uint32_t *count) {
  uint32_t i;
  for (i = 0; i < *count; i++) {
    if ((*list)[i] != NULL) {
      free((*list)[i]);
      (*list)[i] = NULL;
    }
  }
  *count = 0;
  if (*list != NULL)
    free(*list);
  list = NULL;
}
void aut_dupstr(char **dst, char *src) {
  if (src != NULL) {
    *dst = malloc(strlen(src) + 1);
    strcpy(*dst, src);
  } else {
    *dst = NULL;
  }
}

automaton_string_list *automaton_string_list_create(bool sorted, bool repeat_values) {
  automaton_string_list *ret_value = calloc(1, sizeof(automaton_string_list));
  ret_value->sorted = sorted;
  ret_value->repeat_values = repeat_values;
  ret_value->count = 0;
  ret_value->size = LIST_INITIAL_SIZE;
  ret_value->raw_count = 0;
  ret_value->raw_size = LIST_INITIAL_SIZE * 32;
  ret_value->list = calloc(ret_value->size, sizeof(char *));
  ret_value->ordered_list = calloc(ret_value->size, sizeof(uint32_t));
  ret_value->counts = calloc(ret_value->size, sizeof(uint32_t));
  ret_value->raw_data = calloc(ret_value->raw_size, sizeof(char));

  return ret_value;
}

void automaton_string_list_destroy(automaton_string_list *list) {
  free(list->list);
  free(list->ordered_list);
  free(list->counts);
  free(list->raw_data);
  free(list);
}

void aut_merge_string_lists(automaton_string_list *a, char **b, uint32_t count) {
  if ((a->count == 0 && count == 0) || (count == 0))
    return;
  // a = a+b
  uint32_t i, position;
  for (i = 0; i < count; i++) {
    aut_push_string_to_list(a, b[i], &position);
  }
}
/**
 * Adds an element to a list of strings and then returns its index
 * @param list the list where the new entry should be added
 * @param element the string to be added
 * @param position the position where the string was added at, if the string was
 * already present and repeat_values is set to false, previous position is set
 */
bool aut_push_string_to_list(automaton_string_list *list, char *element, int32_t *position) {
  int32_t i;
  int32_t a_b_cmp;
  // get the position of the element to be added
  *position = -1;
  int32_t ordered_position = -1;
  // if sorted do bin search
  if (list->count != 0) {
    uint32_t left = 0, right = list->count - 1, medium = 0;
    while (left <= right) {
      medium = floor((left + right) / 2);
      a_b_cmp = strcmp(list->list[list->ordered_list[medium]], element);
#if DEBUG_STRING_LIST
      printf("l:%d\tr:%d\tm:%d\to[%d]:%d\tv[o[m]=%d]:%s\te:%s\n", left, right, medium, medium, list->ordered_list[medium],
             list->ordered_list[medium], list->list[list->ordered_list[medium]], element);
#endif
      if (a_b_cmp < 0) {
        if (medium >= right) {
          ordered_position = medium + 1;
          break;
        }
        left = medium + 1;
      } else if (a_b_cmp > 0) {
        if (medium <= left) {
          ordered_position = medium;
          break;
        }
        right = medium - 1;
      } else {
        ordered_position = medium;
        *position = list->ordered_list[medium];
        if (!(list->repeat_values)) {
#if DEBUG_STRING_LIST
          printf("%s found at %d\n", element, *position);
#endif
          return false;
        }
        break;
      }
    }
  } else {
    ordered_position = 0;
  }

#if DEBUG_STRING_LIST
  for (i = 0; i < list->count; i++) {
    printf("%s%s ", i == *position ? "*" : "", list->list[i]);
  }
  printf("\n");
#endif

  if (!(list->sorted)) {
    *position = list->count;
  } else {
    *position = ordered_position;
  }
#if DEBUG_STRING_LIST
  printf("Position:%d\tOrdered pos.:%d\n", *position, ordered_position);
#endif

  list->count++;
  // update count and list structs if needed
  if (list->count >= list->size) {
    list->size *= LIST_INCREASE_FACTOR;
    uint32_t *new_counts = realloc(list->counts, sizeof(uint32_t) * list->size);
    if (new_counts == NULL) {
      printf("Could not reallocate memory [aut_push_string_to_list:1]\n");
      exit(-1);
    }
    list->counts = new_counts;
    uint32_t *new_ordered_list = realloc(list->ordered_list, sizeof(uint32_t) * list->size);
    if (new_ordered_list == NULL) {
      printf("Could not reallocate memory [aut_push_string_to_list:4]\n");
      exit(-1);
    }
    list->ordered_list = new_ordered_list;
    char **new_list = realloc(list->list, sizeof(char *) * list->size);
    if (new_list == NULL) {
      printf("Could not reallocate memory [aut_push_string_to_list:3]\n");
      exit(-1);
    }
    list->list = new_list;
  }
  // compute raw size and update raw struct if needed
  uint32_t element_length = strlen(element);
  uint32_t new_length = list->raw_count + element_length + 1;
  if (new_length >= list->raw_size) {
    list->raw_size *= LIST_INCREASE_FACTOR;
    char *new_raw_data = realloc(list->raw_data, sizeof(char) * list->raw_size);
    if (new_raw_data == NULL) {
      printf("Could not reallocate memory [aut_push_string_to_list:5]\n");
      exit(-1);
    }
    list->raw_data = new_raw_data;
    for (i = 0; i < list->count; i++) {
      list->list[i] = (char *)((uintptr_t)(list->raw_data) + (uintptr_t)(list->counts[i]));
    }
  }
  // rearrange pointers if needed
  int32_t position_value = *position;
  if (list->count > 0) {
    for (i = (list->count) - 1; i >= 0; i--) {
      if (i >= position_value) {
        list->list[i + 1] = list->list[i];
        list->counts[i + 1] = list->counts[i];
      }
    }
    for (i = (list->count) - 1; i >= 0; i--) {
      if (list->ordered_list[i] >= position_value)
        list->ordered_list[i]++;
    }
    for (i = (list->count) - 1; i >= 0; i--) {
      if (i >= ordered_position) {
        list->ordered_list[i + 1] = list->ordered_list[i];
      }
    }
  }
  // copy raw data
  list->list[*position] = &(list->raw_data[list->raw_count]);
  list->counts[*position] = list->raw_count;
  list->ordered_list[ordered_position] = *position;

  for (i = 0; i < element_length; i++) {
    list->raw_data[list->raw_count + i] = element[i];
  }
  list->raw_data[list->raw_count + element_length] = '\0';
  list->raw_count = new_length;

  return true;
}

int32_t aut_string_list_index_of(automaton_string_list *list, char *element) {
  int32_t i;
  for (i = 0; i < list->count; i++)
    if (strcmp(list->list[i], element) == 0)
      return i;
  return -1;
}
