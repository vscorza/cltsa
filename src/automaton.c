#include "automaton.h"
#include "assert.h"
#include "qsort.h"

int __automaton_global_print_id = -1;

/** CLONING AND COPYING FUNCTIONS **/
automaton_signal_event *automaton_signal_event_clone(automaton_signal_event *source) {
  automaton_signal_event *copy = malloc(sizeof(automaton_signal_event));
  automaton_signal_event_copy(source, copy);
  return copy;
}
void automaton_signal_event_copy(automaton_signal_event *source, automaton_signal_event *target) {
  target->name = malloc(sizeof(char) * (strlen(source->name) + 1));
  strcpy(target->name, source->name);
  target->type = source->type;
}
automaton_alphabet *automaton_alphabet_clone(automaton_alphabet *source) {
  automaton_alphabet *copy = malloc(sizeof(automaton_alphabet));
  automaton_alphabet_copy(source, copy);
  return copy;
}
void automaton_alphabet_copy(automaton_alphabet *source, automaton_alphabet *target) {
  target->count = source->count;
  target->size = source->size;
  target->list = calloc(target->size, sizeof(automaton_signal_event));
  uint32_t i;
  for (i = 0; i < target->count; i++) {
    automaton_signal_event_copy(&(source->list[i]), &(target->list[i]));
  }
}
automaton_transition *automaton_transition_clone(automaton_transition *source) {
  automaton_transition *copy = calloc(1, sizeof(automaton_transition));
  automaton_transition_copy(source, copy);
  return copy;
}
void automaton_transition_copy(automaton_transition *source, automaton_transition *target) {
  target->state_from = source->state_from;
  target->state_to = source->state_to;
  uint32_t i;
  for (i = 0; i < FIXED_SIGNALS_COUNT; i++)
    target->signals[i] = source->signals[i];
}
automaton_fluent *automaton_fluent_clone(automaton_fluent *source) {
  automaton_fluent *copy = malloc(sizeof(automaton_fluent));
  automaton_fluent_copy(source, copy);
  return copy;
}
void automaton_fluent_copy(automaton_fluent *source, automaton_fluent *target) {
  target->name = malloc(sizeof(char) * (strlen(source->name) + 1));
  strcpy(target->name, source->name);
  uint32_t i, j;
  if (target->starting_signals_count > 0) {
    for (i = 0; i < target->starting_signals_count; i++) {
      free(target->starting_signals[i]);
    }
    free(target->starting_signals);
    free(target->starting_signals_element_count);
  }
  target->starting_signals_count = source->starting_signals_count;
  target->starting_signals = calloc(target->starting_signals_count, sizeof(signal_t *));
  target->starting_signals_element_count = calloc(target->starting_signals_count, sizeof(uint32_t));
  for (i = 0; i < target->starting_signals_count; i++) {
    target->starting_signals_element_count[i] = source->starting_signals_element_count[i];
    target->starting_signals[i] = calloc(target->starting_signals_element_count[i], sizeof(signal_t));
    for (j = 0; j < target->starting_signals_element_count[i]; j++) {
      target->starting_signals[i][j] = source->starting_signals[i][j];
    }
  }
  if (target->ending_signals_count > 0) {
    for (i = 0; i < target->ending_signals_count; i++) {
      free(target->ending_signals[i]);
    }
    free(target->ending_signals);
    free(target->ending_signals_element_count);
  }
  target->ending_signals_count = source->ending_signals_count;
  target->ending_signals = calloc(target->ending_signals_count, sizeof(signal_t *));
  target->ending_signals_element_count = calloc(target->ending_signals_count, sizeof(uint32_t));
  for (i = 0; i < target->ending_signals_count; i++) {
    target->ending_signals_element_count[i] = source->ending_signals_element_count[i];
    target->ending_signals[i] = calloc(target->ending_signals_element_count[i], sizeof(signal_t));
    for (j = 0; j < target->ending_signals_element_count[i]; j++) {
      target->ending_signals[i][j] = source->ending_signals[i][j];
    }
  }

  target->initial_valuation = source->initial_valuation;
}
automaton_valuation *automaton_valuation_clone(automaton_valuation *source) {
  automaton_valuation *copy = malloc(sizeof(automaton_valuation));
  automaton_valuation_copy(source, copy);
  return copy;
}
void automaton_valuation_copy(automaton_valuation *source, automaton_valuation *target) {
  target->state = source->state;
  target->active_fluents_count = source->active_fluents_count;
  target->active_fluents = malloc(sizeof(uint32_t) * target->active_fluents_count);
  uint32_t i;
  for (i = 0; i < target->active_fluents_count; i++) {
    target->active_fluents[i] = source->active_fluents[i];
  }
}
automaton_automata_context *automaton_automata_context_clone(automaton_automata_context *source) {
  automaton_automata_context *copy = malloc(sizeof(automaton_automata_context));
  automaton_automata_context_copy(source, copy);
  return copy;
}
void automaton_automata_context_copy(automaton_automata_context *source, automaton_automata_context *target) {
  target->name = malloc(sizeof(char) * (strlen(source->name) + 1));
  strcpy(target->name, source->name);
  target->global_alphabet = automaton_alphabet_clone(source->global_alphabet);
  target->global_fluents_count = source->global_fluents_count;
  target->global_fluents = malloc(sizeof(automaton_fluent) * target->global_fluents_count);
  uint32_t i;
  for (i = 0; i < target->global_fluents_count; i++) {
    automaton_fluent_copy(&(source->global_fluents[i]), &(target->global_fluents[i]));
  }
  target->liveness_valuations_count = source->liveness_valuations_count;
  target->liveness_valuations = malloc(sizeof(obdd *) * target->liveness_valuations_count);
  target->liveness_valuations_names = malloc(sizeof(char *) * target->liveness_valuations_count);
  for (i = 0; i < target->liveness_valuations_count; i++) {
    target->liveness_valuations[i] = obdd_clone(source->liveness_valuations[i]);
    aut_dupstr(&(target->liveness_valuations_names[i]), source->liveness_valuations_names[i]);
  }
  target->state_valuations_count = source->state_valuations_count;
  target->state_valuations_names = malloc(sizeof(char *) * target->state_valuations_count);
  for (i = 0; i < target->state_valuations_count; i++) {
    aut_dupstr(&(target->state_valuations_names[i]), source->state_valuations_names[i]);
  }
}
automaton_automaton *automaton_automaton_clone(automaton_automaton *source) {
  automaton_automaton *copy = malloc(sizeof(automaton_automaton));
  automaton_automaton_copy(source, copy);
  return copy;
}
void automaton_automaton_copy(automaton_automaton *source, automaton_automaton *target) {
  uint32_t i, j, in_degree, out_degree, in_size, out_size;
  target->name = malloc(sizeof(char) * (strlen(source->name) + 1));
  target->source_type = source->source_type;
  strcpy(target->name, source->name);
  target->context = source->context;
  target->local_alphabet_count = source->local_alphabet_count;
  target->local_alphabet = malloc(sizeof(uint32_t) * target->local_alphabet_count);
  for (i = 0; i < target->local_alphabet_count; i++) {
    target->local_alphabet[i] = source->local_alphabet[i];
  }
  target->transitions_composite_count = source->transitions_composite_count;
  target->transitions_count = source->transitions_count;
  target->transitions_size = source->transitions_size;
  target->in_degree = malloc(sizeof(uint32_t) * target->transitions_size);
  target->in_size = malloc(sizeof(uint32_t) * target->transitions_size);
  target->max_out_degree = source->max_out_degree;
  target->max_concurrent_degree = source->max_concurrent_degree;
  target->is_controllable = malloc(sizeof(bool) * target->transitions_size);
  target->out_degree = malloc(sizeof(uint32_t) * target->transitions_size);
  target->out_size = malloc(sizeof(uint32_t) * target->transitions_size);
  target->transitions = malloc(sizeof(automaton_transition *) * target->transitions_size);
  target->inverted_transitions = malloc(sizeof(automaton_transition *) * target->transitions_size);
  for (i = 0; i < target->transitions_size; i++) {
    target->is_controllable[i] = source->is_controllable[i];
    target->in_degree[i] = source->in_degree[i];
    target->in_size[i] = source->in_size[i];
    in_degree = target->in_degree[i];
    in_size = target->in_size[i];
    if (in_size > 0) {
      target->inverted_transitions[i] = calloc(in_size, sizeof(automaton_transition));
      for (j = 0; j < in_degree; j++) {
        automaton_transition_copy(&(source->inverted_transitions[i][j]), &(target->inverted_transitions[i][j]));
      }
    }
    target->out_degree[i] = source->out_degree[i];
    target->out_size[i] = source->out_size[i];
    out_degree = target->out_degree[i];
    out_size = target->out_size[i];
    if (out_size > 0) {
      target->transitions[i] = calloc(out_size, sizeof(automaton_transition));
      for (j = 0; j < out_degree; j++) {
        automaton_transition_copy(&(source->transitions[i][j]), &(target->transitions[i][j]));
      }
    }
  }
  target->initial_states_count = source->initial_states_count;
  target->initial_states = malloc(sizeof(uint32_t) * target->initial_states_count);
  for (i = 0; i < target->initial_states_count; i++) {
    target->initial_states[i] = source->initial_states[i];
  }
  if (source->source_type & SOURCE_GAME) {
    if (source->valuations_size > 0) {
      target->valuations_size = source->valuations_size;
      target->valuations = malloc(FLUENT_ENTRY_SIZE * target->valuations_size);
    } else {
      target->valuations_size = 0;
      target->valuations = NULL;
    }
    for (i = 0; i < target->valuations_size; i++) {
      target->valuations[i] = source->valuations[i];
    }
    if (source->liveness_valuations_size > 0) {
      target->liveness_valuations_size = source->liveness_valuations_size;
      target->liveness_valuations = calloc(target->liveness_valuations_size, FLUENT_ENTRY_SIZE);
      for (i = 0; i < target->liveness_valuations_size; i++) {
        target->liveness_valuations[i] = source->liveness_valuations[i];
      }
      target->liveness_inverted_valuations = calloc(target->context->liveness_valuations_count, sizeof(automaton_bucket_list *));
      for (i = 0; i < target->context->liveness_valuations_count; i++) {
        target->liveness_inverted_valuations[i] = automaton_bucket_list_clone(source->liveness_inverted_valuations[i]);
      }
    } else {
      target->liveness_valuations_size = 0;
      target->liveness_valuations = NULL;
      target->liveness_inverted_valuations = NULL;
    }
    if (source->state_valuations_size > 0) {
      target->state_valuations_size = source->state_valuations_size;
      target->state_valuations = malloc(FLUENT_ENTRY_SIZE * target->state_valuations_size);
      for (i = 0; i < source->state_valuations_size; i++)
        target->state_valuations[i] = source->state_valuations[i];
      for (i = 0; i < target->context->state_valuations_count; i++) {
        target->inverted_state_valuations[i] = automaton_bucket_list_clone(source->inverted_state_valuations[i]);
      }
      target->state_valuations_declared_size = source->state_valuations_declared_size;
      target->state_valuations_declared = malloc(FLUENT_ENTRY_SIZE * target->state_valuations_declared_size);
      for (i = 0; i < target->state_valuations_declared_size; i++) {
        target->state_valuations_declared[i] = source->state_valuations_declared[i];
      }
    } else {
      target->state_valuations_size = 0;
      target->state_valuations = NULL;
      target->inverted_state_valuations = NULL;
      target->state_valuations_declared_size = 0;
      target->state_valuations_declared = NULL;
    }
    if (source->context->global_fluents_count > 0) {
      target->inverted_valuations = calloc(target->context->global_fluents_count, sizeof(automaton_bucket_list *));
      for (i = 0; i < target->context->global_fluents_count; i++) {
        target->inverted_valuations[i] = automaton_bucket_list_clone(source->inverted_valuations[i]);
      }
    } else {
      target->inverted_valuations = NULL;
    }
  }
  for (i = 0; i < FIXED_SIGNALS_COUNT; i++)
    target->monitored_mask[i] = source->monitored_mask[i];
  target->ordered = source->ordered;
}
automaton_automata *automaton_automata_clone(automaton_automata *source) {
  automaton_automata *copy = malloc(sizeof(automaton_automata));
  automaton_automata_copy(source, copy);
  return copy;
}
void automaton_automata_copy(automaton_automata *source, automaton_automata *target) {
  target->automata_count = source->automata_count;
  target->automata_list = malloc(sizeof(automaton_automaton) * target->automata_count);
  uint32_t i, j;
  for (i = 0; i < target->automata_count; i++) {
    target->automata_list[i] = source->automata_list[i];
  }
  target->states_explored_count = source->states_explored_count;
  target->states_explored_size = source->states_explored_size;
  target->states_explored = malloc(sizeof(uint32_t) * target->automata_count * target->states_explored_size);
  target->states_decomposed = malloc(sizeof(uint32_t) * target->states_explored_size);
  target->states_valuations = malloc(sizeof(automaton_valuation) * target->states_explored_size);
  for (i = 0; i < target->states_explored_count; i++) {
    for (j = 0; j < target->automata_count; j++) {
      target->states_explored[i][j] = source->states_explored[i][j];
    }
    target->states_decomposed[i] = source->states_decomposed[i];
    automaton_valuation_copy(&(source->states_valuations[i]), &(target->states_valuations[i]));
  }
}
automaton_range *automaton_range_clone(automaton_range *source) {
  automaton_range *copy = malloc(sizeof(automaton_range));
  automaton_range_copy(source, copy);
  return copy;
}
void automaton_range_copy(automaton_range *source, automaton_range *target) {
  target->name = malloc(sizeof(char) * (strlen(source->name) + 1));
  strcpy(target->name, source->name);
  target->lower_value = source->lower_value;
  target->upper_value = source->upper_value;
}
automaton_indexes_valuation *automaton_indexes_valuation_clone(automaton_indexes_valuation *source) {
  automaton_indexes_valuation *copy = malloc(sizeof(automaton_indexes_valuation));
  automaton_indexes_valuation_copy(source, copy);
  return copy;
}
void automaton_indexes_valuation_copy(automaton_indexes_valuation *source, automaton_indexes_valuation *target) {
  target->count = source->count;
  uint32_t i;
  target->current_values = malloc(sizeof(int32_t) * target->count);
  for (i = 0; i < target->count; i++) {
    target->current_values[i] = source->current_values[i];
  }
  target->ranges = malloc(sizeof(automaton_range *) * target->count);
  for (i = 0; i < target->count; i++) {
    target->ranges[i] = automaton_range_clone(source->ranges[i]);
  }
  target->current_combination = source->current_combination;
  target->total_combinations = source->total_combinations;
}

/** CREATE FUNCTIONS **/
automaton_signal_event *automaton_signal_event_create(char *name, automaton_signal_type type) {
  automaton_signal_event *signal_event = malloc(sizeof(automaton_signal_event));
  automaton_signal_event_initialize(signal_event, name, type);
  return signal_event;
}
automaton_alphabet *automaton_alphabet_create() {
  automaton_alphabet *alphabet = malloc(sizeof(automaton_alphabet));
  automaton_alphabet_initialize(alphabet);
  return alphabet;
}
automaton_transition *automaton_transition_create(uint32_t from_state, uint32_t to_state) {
  automaton_transition *transition = calloc(1, sizeof(automaton_transition));
  automaton_transition_initialize(transition, from_state, to_state);
  return transition;
}
automaton_fluent *automaton_fluent_create(char *name, bool initial_valuation) {
  automaton_fluent *fluent = malloc(sizeof(automaton_fluent));
  automaton_fluent_initialize(fluent, name, initial_valuation);
  return fluent;
}
automaton_valuation *automaton_valuation_create(uint32_t state) {
  automaton_valuation *valuation = malloc(sizeof(automaton_valuation));
  automaton_valuation_initialize(valuation, state);
  return valuation;
}
automaton_automata_context *automaton_automata_context_create(char *name, automaton_alphabet *alphabet, uint32_t fluents_count,
                                                              automaton_fluent **fluents, uint32_t liveness_valuations_count,
                                                              obdd **liveness_valuations, char **liveness_valuations_names,
                                                              uint32_t state_valuations_count, char **state_valuations_names) {
  automaton_automata_context *ctx = malloc(sizeof(automaton_automata_context));
  automaton_automata_context_initialize(ctx, name, alphabet, fluents_count, fluents, liveness_valuations_count, liveness_valuations,
                                        liveness_valuations_names, state_valuations_count, state_valuations_names);

  return ctx;
}
automaton_automaton *automaton_automaton_create(char *name, automaton_automata_context *ctx, uint32_t local_alphabet_count,
                                                uint32_t *local_alphabet, bool is_game, bool built_from_ltl, bool is_strat, bool is_diag) {
  automaton_automaton *automaton = malloc(sizeof(automaton_automaton));
  automaton_automaton_initialize(automaton, name, ctx, local_alphabet_count, local_alphabet, is_game, built_from_ltl, is_strat, is_diag);
  return automaton;
}
automaton_range *automaton_range_create(char *name, uint32_t lower_value, uint32_t upper_value) {
  automaton_range *range = malloc(sizeof(automaton_range));
  automaton_range_initialize(range, name, lower_value, upper_value);
  return range;
}
automaton_indexes_valuation *automaton_indexes_valuation_create() {
  automaton_indexes_valuation *valuation = malloc(sizeof(automaton_indexes_valuation));
  valuation->count = 0;
  valuation->current_values = NULL;
  valuation->ranges = NULL;
  valuation->current_combination = 0;
  valuation->total_combinations = 0;
  return valuation;
}
/** INIT FUNCTIONS **/
void automaton_signal_event_initialize(automaton_signal_event *signal_event, char *name, automaton_signal_type type) {
  signal_event->name = malloc(sizeof(char) * (strlen(name) + 1));
  strcpy(signal_event->name, name);
  signal_event->type = type;
}
void automaton_alphabet_initialize(automaton_alphabet *alphabet) {
  alphabet->size = LIST_INITIAL_SIZE;
  alphabet->count = 0;
  alphabet->list = malloc(sizeof(automaton_signal_event) * alphabet->size);
}
void automaton_transition_initialize(automaton_transition *transition, uint32_t from_state, uint32_t to_state) {
  transition->state_from = from_state;
  transition->state_to = to_state;
  uint32_t i;
  for (i = 0; i < FIXED_SIGNALS_COUNT; i++)
    transition->signals[i] = 0x0;
}
void automaton_fluent_initialize(automaton_fluent *fluent, char *name, bool initial_valuation) {
  fluent->name = malloc(sizeof(char) * (strlen(name) + 1));
  strcpy(fluent->name, name);
  fluent->starting_signals_count = 0;
  fluent->starting_signals_element_count = NULL;
  fluent->starting_signals = NULL;
  fluent->ending_signals_count = 0;
  fluent->ending_signals_element_count = NULL;
  fluent->ending_signals = NULL;
  fluent->initial_valuation = initial_valuation;
}
void automaton_valuation_initialize(automaton_valuation *valuation, uint32_t state) {
  valuation->state = state;
  valuation->active_fluents_count = 0;
  valuation->active_fluents = NULL;
}
void automaton_automata_context_initialize(automaton_automata_context *ctx, char *name, automaton_alphabet *alphabet,
                                           uint32_t fluents_count, automaton_fluent **fluents, uint32_t liveness_valuations_count,
                                           obdd **liveness_valuations, char **liveness_valuations_names, uint32_t state_valuations_count,
                                           char **state_valuations_names) {
  ctx->name = malloc(sizeof(char) * (strlen(name) + 1));
  strcpy(ctx->name, name);
  if (alphabet->count > (TRANSITION_ENTRY_SIZE * FIXED_SIGNALS_COUNT)) {
    printf("[BUILD ERROR] Current build supports an alphabet of up to %d "
           "elements.\nAdjust TRANSITION_ENTRY_SIZE or FIXED_SIGNALS_COUNT and "
           "rebuild.\n",
           (TRANSITION_ENTRY_SIZE * FIXED_SIGNALS_COUNT));
    printf("Provided alphabet has %d elements.\n", alphabet->count);
    exit(-1);
  }
  ctx->global_alphabet = automaton_alphabet_clone(alphabet);
  ctx->global_fluents_count = fluents_count;
  ctx->global_fluents = calloc(ctx->global_fluents_count, sizeof(automaton_fluent));
  uint32_t i;
  for (i = 0; i < ctx->global_fluents_count; i++) {
    automaton_fluent_copy(fluents[i], &(ctx->global_fluents[i]));
  }
  ctx->liveness_valuations_count = liveness_valuations_count;
  ctx->liveness_valuations = malloc(sizeof(obdd *) * liveness_valuations_count);
  ctx->liveness_valuations_names = malloc(sizeof(char *) * liveness_valuations_count);
  for (i = 0; i < ctx->liveness_valuations_count; i++) {
    ctx->liveness_valuations[i] = obdd_clone(liveness_valuations[i]);
    aut_dupstr(&(ctx->liveness_valuations_names[i]), liveness_valuations_names[i]);
  }
  ctx->state_valuations_count = state_valuations_count;
  ctx->state_valuations_names = malloc(sizeof(char *) * state_valuations_count);
  for (i = 0; i < ctx->state_valuations_count; i++) {
    aut_dupstr(&(ctx->state_valuations_names[i]), state_valuations_names[i]);
  }
}
void automaton_automaton_initialize(automaton_automaton *automaton, char *name, automaton_automata_context *ctx,
                                    uint32_t local_alphabet_count, uint32_t *local_alphabet, bool is_game, bool built_from_ltl,
                                    bool is_strat, bool is_diag) {
  automaton->source_type =
      (is_game ? SOURCE_GAME : 0x0) | (built_from_ltl ? SOURCE_LTL : 0x0) | (is_strat ? SOURCE_STRAT : 0x0) | (is_diag ? SOURCE_DIAG : 0x0);
  automaton->name = malloc(sizeof(char) * (strlen(name) + 1));
  strcpy(automaton->name, name);
  automaton->context = ctx;
  automaton->local_alphabet = malloc(sizeof(uint32_t) * local_alphabet_count);
  automaton->local_alphabet_count = 0;
  uint32_t i, j, current_entry;
  int32_t new_index;
  bool found;
  // insert ordered
  for (i = 0; i < local_alphabet_count; i++) {
    new_index = -1;
    found = false;
    for (j = 0; j < i; j++) {
      if (automaton->local_alphabet[j] == local_alphabet[i]) {
        found = true;
        break;
      }
      if (local_alphabet[i] < automaton->local_alphabet[j]) {
        new_index = j;
        break;
      }
    }
    // keep global alphabet ordered
    if (new_index > -1 && !found) {
      for (j = (automaton->local_alphabet_count); j > (uint32_t)new_index; j--) {
        automaton->local_alphabet[j] = automaton->local_alphabet[j - 1];
      }
    } else {
      new_index = automaton->local_alphabet_count;
    }
    if (!found) {
      automaton->local_alphabet[new_index] = local_alphabet[i];
      automaton->local_alphabet_count++;
    }
    // for(j = 0; j < automaton->local_alphabet_count; j++){printf("%d.",
    // automaton->local_alphabet[j]);	}printf("\n");
  }

  automaton->transitions_size = LIST_INITIAL_SIZE;
  automaton->transitions_count = 0;
  automaton->transitions_composite_count = 0;
  automaton->max_out_degree = 0;
  automaton->max_concurrent_degree = 0;
  automaton->is_controllable = malloc(sizeof(bool) * automaton->transitions_size);
  automaton->out_degree = malloc(sizeof(uint32_t) * automaton->transitions_size);
  automaton->out_size = malloc(sizeof(uint32_t) * automaton->transitions_size);
  automaton->in_degree = malloc(sizeof(uint32_t) * automaton->transitions_size);
  automaton->in_size = malloc(sizeof(uint32_t) * automaton->transitions_size);
  automaton->transitions = malloc(sizeof(automaton_transition *) * automaton->transitions_size);
  automaton->inverted_transitions = malloc(sizeof(automaton_transition *) * automaton->transitions_size);
  for (i = 0; i < automaton->transitions_size; i++) {
    automaton->is_controllable[i] = true;
    automaton->out_degree[i] = 0;
    automaton->in_degree[i] = 0;
    automaton->in_size[i] = TRANSITIONS_INITIAL_SIZE;
    automaton->out_size[i] = TRANSITIONS_INITIAL_SIZE;
    automaton->transitions[i] = calloc(automaton->out_size[i], sizeof(automaton_transition));
    for (j = 0; j < automaton->out_size[i]; j++)
      automaton_transition_initialize(&(automaton->transitions[i][j]), 0, 0);
    automaton->inverted_transitions[i] = calloc(automaton->in_size[i], sizeof(automaton_transition));
    for (j = 0; j < automaton->in_size[i]; j++)
      automaton_transition_initialize(&(automaton->inverted_transitions[i][j]), 0, 0);
  }
  automaton->initial_states_count = 0;
  automaton->initial_states = NULL;
  automaton->liveness_valuations_size = 0;
  if (is_game) {
    if (automaton->context->global_fluents_count > 0) {
      automaton->valuations_size = GET_FLUENTS_ARR_SIZE(automaton->context->global_fluents_count, automaton->transitions_size);
      automaton->valuations = calloc(automaton->valuations_size, FLUENT_ENTRY_SIZE);
      automaton->inverted_valuations = malloc(sizeof(automaton_bucket_list *) * automaton->context->global_fluents_count);
      for (i = 0; i < automaton->context->global_fluents_count; i++) {
        automaton->inverted_valuations[i] = automaton_bucket_list_create(FLUENT_BUCKET_SIZE);
      }
    } else {
      automaton->valuations_size = 0;
      automaton->valuations = NULL;
      automaton->inverted_valuations = NULL;
    }
    if (automaton->context->liveness_valuations_count > 0) {
      automaton->liveness_valuations_size =
          GET_FLUENTS_ARR_SIZE(automaton->context->liveness_valuations_count, automaton->transitions_size);
      // automaton->liveness_valuations_size			= new_size;
      automaton->liveness_valuations = calloc(automaton->liveness_valuations_size, FLUENT_ENTRY_SIZE);
      automaton->liveness_inverted_valuations = malloc(sizeof(automaton_bucket_list *) * automaton->context->liveness_valuations_count);
      for (i = 0; i < automaton->context->liveness_valuations_count; i++) {
        automaton->liveness_inverted_valuations[i] = automaton_bucket_list_create(FLUENT_BUCKET_SIZE);
      }
    } else {
      automaton->liveness_valuations_size = 0;
      automaton->liveness_valuations = NULL;
      automaton->liveness_inverted_valuations = NULL;
    }
  } else {
    automaton->valuations_size = 0;
    automaton->valuations = NULL;
    automaton->inverted_valuations = NULL;
    automaton->liveness_valuations_size = 0;
    automaton->liveness_valuations = NULL;
    automaton->liveness_inverted_valuations = NULL;
  }
  automaton->state_valuations_size = 0;
  automaton->state_valuations = NULL;
  automaton->inverted_state_valuations = NULL;
  automaton->state_valuations_declared_size = 0;
  automaton->state_valuations_declared = NULL;
  for (i = 0; i < FIXED_SIGNALS_COUNT; i++)
    automaton->monitored_mask[i] = 0x0;
  for (i = 0; i < ctx->global_alphabet->count; i++)
    if (ctx->global_alphabet->list[i].type == INPUT_SIG)
      SET_TRANSITION_MASK_BIT((automaton->monitored_mask), i);
  automaton->ordered = false;
}
void automaton_range_initialize(automaton_range *range, char *name, uint32_t lower_value, uint32_t upper_value) {
  if (name != NULL) {
    range->name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(range->name, name);
  }
  range->lower_value = lower_value;
  range->upper_value = upper_value;
}
/** DESTROY FUNCTIONS **/
void automaton_signal_event_destroy(automaton_signal_event *signal_event, bool freeBase) {
  free(signal_event->name);
  signal_event->name = NULL;
  if (freeBase)
    free(signal_event);
}
void automaton_alphabet_destroy(automaton_alphabet *alphabet) {
  uint32_t i;
  for (i = 0; i < alphabet->count; i++) {
    automaton_signal_event_destroy(&(alphabet->list[i]), false);
  }
  free(alphabet->list);
  alphabet->list = NULL;
  alphabet->count = 0;
  alphabet->size = 0;
  free(alphabet);
}
void automaton_transition_destroy(automaton_transition *transition, bool freeBase) {
  uint32_t i;
  for (i = 0; i < FIXED_SIGNALS_COUNT; i++)
    transition->signals[i] = 0x0;
  transition->state_from = 0;
  transition->state_to = 0;

  if (freeBase)
    free(transition);
}
void automaton_fluent_destroy(automaton_fluent *fluent, bool freeBase) {
  free(fluent->name);
  uint32_t i;
  for (i = 0; i < fluent->starting_signals_count; i++) {
    free(fluent->starting_signals[i]);
  }
  free(fluent->starting_signals);
  for (i = 0; i < fluent->ending_signals_count; i++) {
    free(fluent->ending_signals[i]);
  }
  free(fluent->ending_signals);
  fluent->name = NULL;
  fluent->starting_signals_count = 0;
  free(fluent->starting_signals_element_count);
  fluent->starting_signals_element_count = NULL;
  fluent->starting_signals = NULL;
  free(fluent->ending_signals_element_count);
  fluent->ending_signals_element_count = NULL;
  fluent->ending_signals_count = 0;
  fluent->ending_signals = NULL;
  fluent->initial_valuation = false;
  if (freeBase)
    free(fluent);
}
void automaton_valuation_destroy(automaton_valuation *valuation) {
  free(valuation->active_fluents);
  valuation->active_fluents_count = 0;
  valuation->state = 0;
  valuation->active_fluents = NULL;
  free(valuation);
}
void automaton_automata_context_destroy(automaton_automata_context *ctx) {
  free(ctx->name);
  automaton_alphabet_destroy(ctx->global_alphabet);
  uint32_t i;
  for (i = 0; i < ctx->global_fluents_count; i++) {
    automaton_fluent_destroy(&(ctx->global_fluents[i]), false);
  }
  if (ctx->global_fluents != NULL)
    free(ctx->global_fluents);
  for (i = 0; i < ctx->liveness_valuations_count; i++) {
    obdd_destroy(ctx->liveness_valuations[i]);
    free(ctx->liveness_valuations_names[i]);
  }
  if (ctx->liveness_valuations != NULL)
    free(ctx->liveness_valuations);
  if (ctx->liveness_valuations_names != NULL)
    free(ctx->liveness_valuations_names);
  for (i = 0; i < ctx->state_valuations_count; i++) {
    free(ctx->state_valuations_names[i]);
  }
  if (ctx->state_valuations_names != NULL)
    free(ctx->state_valuations_names);
  ctx->liveness_valuations = NULL;
  ctx->liveness_valuations_names = NULL;
  ctx->liveness_valuations_count = 0;
  ctx->state_valuations_names = NULL;
  ctx->state_valuations_count = 0;
  ctx->name = NULL;
  ctx->global_alphabet = NULL;
  ctx->global_fluents_count = 0;
  ctx->global_fluents = NULL;
  free(ctx);
}
void automaton_automaton_destroy(automaton_automaton *automaton) {
  free(automaton->name);
  free(automaton->local_alphabet);
  uint32_t i, j;
  for (i = 0; i < automaton->transitions_size; i++) {
    for (j = 0; j < automaton->out_degree[i]; j++) {
      automaton_transition_destroy(&(automaton->transitions[i][j]), false);
    }
    for (j = 0; j < automaton->in_degree[i]; j++) {
      automaton_transition_destroy(&(automaton->inverted_transitions[i][j]), false);
    }
    free(automaton->transitions[i]);
    free(automaton->inverted_transitions[i]);
  }
  free(automaton->is_controllable);
  free(automaton->out_degree);
  free(automaton->transitions);
  free(automaton->in_degree);
  free(automaton->inverted_transitions);
  free(automaton->in_size);
  free(automaton->out_size);
  if (automaton->initial_states_count > 0)
    free(automaton->initial_states);
  if (automaton->source_type & SOURCE_GAME) {
    for (i = 0; i < automaton->context->global_fluents_count; i++)
      automaton_bucket_destroy(automaton->inverted_valuations[i]);
    if (automaton->valuations_size > 0) {
      free(automaton->inverted_valuations);
      free(automaton->valuations);
    }
    if (automaton->liveness_valuations_size > 0) {
      for (i = 0; i < automaton->context->liveness_valuations_count; i++)
        automaton_bucket_destroy(automaton->liveness_inverted_valuations[i]);
    }
    if (automaton->liveness_inverted_valuations != NULL)
      free(automaton->liveness_inverted_valuations);
    if (automaton->liveness_valuations != NULL)
      free(automaton->liveness_valuations);
    if (automaton->state_valuations_size > 0) {
      for (i = 0; i < automaton->context->state_valuations_count; i++)
        automaton_bucket_destroy(automaton->inverted_state_valuations[i]);
    }
    if (automaton->inverted_state_valuations != NULL)
      free(automaton->inverted_state_valuations);
    if (automaton->state_valuations != NULL)
      free(automaton->state_valuations);
    if (automaton->state_valuations_declared != NULL)
      free(automaton->state_valuations_declared);
  }
  automaton->name = NULL;
  automaton->context = NULL;
  automaton->local_alphabet_count = 0;
  automaton->local_alphabet = NULL;
  automaton->state_valuations_size = 0;
  automaton->state_valuations = NULL;
  automaton->state_valuations_declared_size = 0;
  automaton->state_valuations_declared = NULL;
  automaton->transitions_size = 0;
  automaton->transitions_count = 0;
  automaton->max_out_degree = 0;
  automaton->max_concurrent_degree = 0;
  automaton->out_degree = NULL;
  automaton->transitions = NULL;
  automaton->in_degree = NULL;
  automaton->inverted_transitions = NULL;
  automaton->initial_states_count = 0;
  automaton->initial_states = NULL;
  automaton->valuations_size = 0;
  automaton->valuations = NULL;
  automaton->inverted_transitions = NULL;
  automaton->liveness_valuations = NULL;
  automaton->liveness_inverted_valuations = NULL;
  free(automaton);
}
void automaton_range_destroy(automaton_range *range) {
  if (range->name != NULL)
    free(range->name);
  range->lower_value = 0;
  range->upper_value = 0;
  free(range);
}
void automaton_indexes_valuation_destroy(automaton_indexes_valuation *valuation) {
  free(valuation->current_values);
  uint32_t i;
  for (i = 0; i < valuation->count; i++) {
    if (valuation->ranges[i] != NULL)
      automaton_range_destroy(valuation->ranges[i]);
  }

  valuation->current_values = NULL;
  if (valuation->ranges != NULL)
    free(valuation->ranges);
  valuation->ranges = NULL;
  free(valuation);
}
/** INDEXES VALUATION **/
bool automaton_indexes_valuation_has_range(automaton_indexes_valuation *valuation, automaton_range *range) {
  uint32_t i;
  for (i = 0; i < valuation->count; i++) {
    if (strcmp(valuation->ranges[i]->name, range->name))
      return true;
  }
  return false;
}
bool automaton_indexes_valuation_add_range(automaton_indexes_valuation *valuation, automaton_range *range) {
  if (automaton_indexes_valuation_has_range(valuation, range)) {
    return false;
  }
  uint32_t new_count = valuation->count + 1;
  int32_t *new_values = malloc(sizeof(int32_t) * new_count);
  automaton_range **new_ranges = malloc(sizeof(automaton_range *) * new_count);
  uint32_t i;
  for (i = 0; i < valuation->count; i++) {
    new_values[i] = valuation->current_values[i];
    new_ranges[i] = valuation->ranges[i];
  }
  new_values[valuation->count] = range->lower_value;
  new_ranges[valuation->count] = range;
  free(valuation->current_values);
  free(valuation->ranges);
  valuation->current_values = new_values;
  valuation->ranges = new_ranges;
  return true;
}
/**
 * Returns the index's value with name according to parameter provided
 *
 * @param valuation current valuation to be fixed over a single variable
 * @param index_name the name of the variable to be fixed
 * @return the value of the given index
 */

int32_t automaton_indexes_valuation_get_value(automaton_indexes_valuation *valuation, char *range_name) {
  uint32_t i;
  for (i = 0; i < valuation->count; i++) {
    if (strcmp(valuation->ranges[i]->name, range_name) == 0) {
      return valuation->current_values[i];
    }
  }
  printf("No index found with name %s \n", range_name);
  exit(-1);
}
void automaton_indexes_valuation_set_value(automaton_indexes_valuation *valuation, char *range_name, int32_t value) {
  uint32_t i;
  for (i = 0; i < valuation->count; i++) {
    if (strcmp(valuation->ranges[i]->name, range_name))
      valuation->current_values[i] = value;
  }
}
/** ALPHABET **/
bool automaton_alphabet_has_signal_event(automaton_alphabet *alphabet, automaton_signal_event *signal_event) {
  uint32_t i;
  for (i = 0; i < alphabet->count; i++) {
    if (strcmp(alphabet->list[i].name, signal_event->name) == 0)
      return true;
  }
  return false;
}
bool automaton_alphabet_add_signal_event(automaton_alphabet *alphabet, automaton_signal_event *signal_event) {
  if (automaton_alphabet_has_signal_event(alphabet, signal_event))
    return true;
  // search for position to insert event in order
  int32_t i, signal_ordered_index = -1;
  int32_t old_count = (int32_t)alphabet->count;
  for (i = 0; i < old_count; i++) {
    if (strcmp(signal_event->name, alphabet->list[i].name) <= 0) {
      signal_ordered_index = i;
      break;
    }
  }
  // resize array if needed keeping proper order
  if (alphabet->count == alphabet->size) {
    uint32_t new_size = alphabet->size * LIST_INCREASE_FACTOR;
    automaton_signal_event *new_list = malloc(sizeof(automaton_signal_event) * new_size);
    for (i = 0; i < old_count; i++) {
      if (signal_ordered_index <= i && signal_ordered_index >= 0) {
        automaton_signal_event_copy(&(alphabet->list[i]), &(new_list[i + 1]));
      } else {
        automaton_signal_event_copy(&(alphabet->list[i]), &(new_list[i]));
      }
      automaton_signal_event_destroy(&(alphabet->list[i]), false);
    }
    free(alphabet->list);
    alphabet->list = new_list;
    alphabet->size = new_size;
  } else if (signal_ordered_index > -1) {
    for (i = (alphabet->count) - 1; i >= signal_ordered_index; i--) {
      automaton_signal_event_copy(&(alphabet->list[i]), &(alphabet->list[i + 1]));
      automaton_signal_event_destroy(&(alphabet->list[i]), false);
    }
  }
  if (signal_ordered_index == -1) {
    signal_ordered_index = alphabet->count;
  }
  automaton_signal_event_copy(signal_event, &(alphabet->list[signal_ordered_index]));
  alphabet->count++;
  return false;
}
int32_t automaton_alphabet_get_value_index(automaton_alphabet *alphabet, char *signal_name) {
  int32_t i;
  for (i = 0; i < (int32_t)alphabet->count; i++) {
    if (strcmp(alphabet->list[i].name, signal_name) == 0)
      return i;
  }
  return -1;
}
signal_t automaton_alphabet_get_signal_index(automaton_alphabet *alphabet, automaton_signal_event *signal_event) {
  signal_t i;
  for (i = 0; i < alphabet->count; i++) {
    if (strcmp(alphabet->list[i].name, signal_event->name) == 0)
      return i;
  }
  return 0;
}
/** TRANSITION **/
void automaton_transition_set_from_to(automaton_transition *transition, uint32_t from_state, uint32_t to_state) {
  transition->state_from = from_state;
  transition->state_to = to_state;
}
bool automaton_transition_has_signal_event(automaton_transition *transition, automaton_automata_context *ctx,
                                           automaton_signal_event *signal_event) {
  uint32_t signal_index = automaton_alphabet_get_signal_index(ctx->global_alphabet, signal_event);
  return automaton_transition_has_signal_event_ID(transition, ctx, signal_index);
}

bool automaton_transition_has_signal_event_ID(automaton_transition *transition, automaton_automata_context *ctx, uint32_t signal_index) {
  return (TEST_TRANSITION_BIT(transition, signal_index));
}

bool automaton_transition_add_signal_event(automaton_transition *transition, automaton_automata_context *ctx,
                                           automaton_signal_event *signal_event) {
  uint32_t signal_index = automaton_alphabet_get_signal_index(ctx->global_alphabet, signal_event);
  return automaton_transition_add_signal_event_ID(transition, ctx, signal_index, signal_event->type);
}

bool automaton_transition_add_signal_event_ID(automaton_transition *transition, automaton_automata_context *ctx, uint32_t signal_index,
                                              automaton_signal_type signal_type) {
  if (automaton_transition_has_signal_event_ID(transition, ctx, signal_index))
    return true;
  if (signal_type == INPUT_SIG)
    TRANSITION_SET_INPUT(transition)
  SET_TRANSITION_BIT(transition, signal_index);
  return false;
}
/** FLUENT **/
bool automaton_fluent_has_starting_signals(automaton_fluent *fluent, automaton_alphabet *alphabet, uint32_t signals_count,
                                           automaton_signal_event **signal_events) {
  uint32_t i, j, k;
  signal_t signal_index;
  bool found = false;
  for (i = 0; i < fluent->starting_signals_count; i++) {
    if (fluent->starting_signals_element_count[i] != signals_count)
      continue;
    for (k = 0; k < signals_count; k++) {
      signal_index = automaton_alphabet_get_signal_index(alphabet, signal_events[k]);
      found = false;
      for (j = 0; j < fluent->starting_signals_element_count[i]; j++) {
        if (fluent->starting_signals[i][j] == signal_index) {
          found = true;
          break;
        }
      }
      if (!found)
        return false;
    }
    return true;
  }
  return false;
}
bool automaton_fluent_add_starting_signals(automaton_fluent *fluent, automaton_alphabet *alphabet, uint32_t signals_count,
                                           automaton_signal_event **signal_events) {
  if (automaton_fluent_has_starting_signals(fluent, alphabet, signals_count, signal_events))
    return true;
  uint32_t i;
  uint32_t new_count = fluent->starting_signals_count + 1;
  signal_t **ptr = realloc(fluent->starting_signals, sizeof(signal_t *) * new_count);
  if (ptr == NULL) {
    printf("Could not allocate memory[automaton_fluent_add_starting_signals:1]\n");
    exit(-1);
  } else
    fluent->starting_signals = ptr;
  uint32_t *i_ptr = realloc(fluent->starting_signals_element_count, sizeof(uint32_t) * new_count);
  if (ptr == NULL) {
    printf("Could not allocate memory[automaton_fluent_add_starting_signals:2]\n");
    exit(-1);
  } else
    fluent->starting_signals_element_count = i_ptr;
  signal_t *new_signals = calloc(signals_count, sizeof(signal_t));
  for (i = 0; i < signals_count; i++) {
    new_signals[i] = automaton_alphabet_get_signal_index(alphabet, signal_events[i]);
  }
  fluent->starting_signals[fluent->starting_signals_count] = new_signals;
  fluent->starting_signals_element_count[fluent->starting_signals_count] = signals_count;
  fluent->starting_signals_count++;
  return false;
}
bool automaton_fluent_has_ending_signals(automaton_fluent *fluent, automaton_alphabet *alphabet, uint32_t signals_count,
                                         automaton_signal_event **signal_events) {
  uint32_t i, j, k;
  signal_t signal_index;
  bool found = false;
  for (i = 0; i < fluent->ending_signals_count; i++) {
    if (fluent->ending_signals_element_count[i] != signals_count)
      continue;
    for (k = 0; k < signals_count; k++) {
      signal_index = automaton_alphabet_get_signal_index(alphabet, signal_events[k]);
      found = false;
      for (j = 0; j < fluent->ending_signals_element_count[i]; j++) {
        if (fluent->ending_signals[i][j] == signal_index) {
          found = true;
          break;
        }
      }
      if (!found)
        return false;
    }
    return true;
  }
  return false;
}
bool automaton_fluent_add_ending_signals(automaton_fluent *fluent, automaton_alphabet *alphabet, uint32_t signals_count,
                                         automaton_signal_event **signal_events) {
  if (automaton_fluent_has_ending_signals(fluent, alphabet, signals_count, signal_events))
    return true;
  uint32_t i;
  uint32_t new_count = fluent->ending_signals_count + 1;
  signal_t **ptr = realloc(fluent->ending_signals, sizeof(signal_t *) * new_count);
  if (ptr == NULL) {
    printf("Could not allocate memory[automaton_fluent_add_ending_signals:1]\n");
    exit(-1);
  } else
    fluent->ending_signals = ptr;
  uint32_t *i_ptr = realloc(fluent->ending_signals_element_count, sizeof(uint32_t) * new_count);
  if (ptr == NULL) {
    printf("Could not allocate memory[automaton_fluent_add_ending_signals:2]\n");
    exit(-1);
  } else
    fluent->ending_signals_element_count = i_ptr;
  signal_t *new_signals = calloc(signals_count, sizeof(signal_t));
  for (i = 0; i < signals_count; i++) {
    new_signals[i] = automaton_alphabet_get_signal_index(alphabet, signal_events[i]);
  }
  fluent->ending_signals[fluent->ending_signals_count] = new_signals;
  fluent->ending_signals_element_count[fluent->ending_signals_count] = signals_count;
  fluent->ending_signals_count++;
  return false;
}
/** VALUATION **/
bool automaton_valuation_has_fluent(automaton_valuation *valuation, automaton_automata_context *ctx, automaton_fluent *fluent) {
  uint32_t i;
  uint32_t fluent_index = automaton_automata_context_get_fluent_index(ctx, fluent);
  for (i = 0; i < valuation->active_fluents_count; i++) {
    if (valuation->active_fluents[i] == fluent_index)
      return true;
  }
  return false;
}
bool automaton_valuation_add_fluent(automaton_valuation *valuation, automaton_automata_context *ctx, automaton_fluent *fluent) {
  if (automaton_valuation_has_fluent(valuation, ctx, fluent))
    return true;
  uint32_t i;
  uint32_t fluent_index = automaton_automata_context_get_fluent_index(ctx, fluent);
  uint32_t *new_fluents = malloc(sizeof(uint32_t) * (valuation->active_fluents_count + 1));
  for (i = 0; i < valuation->active_fluents_count; i++)
    new_fluents[i] = valuation->active_fluents[i];
  free(valuation->active_fluents);
  new_fluents[valuation->active_fluents_count] = fluent_index;
  valuation->active_fluents = new_fluents;
  valuation->active_fluents_count++;
  return false;
}
/** AUTOMATA_CONTEXT **/
uint32_t automaton_automata_context_get_fluent_index(automaton_automata_context *ctx, automaton_fluent *fluent) {
  uint32_t i;
  for (i = 0; i < ctx->global_fluents_count; i++) {
    if (strcmp(ctx->global_fluents[i].name, fluent->name) == 0)
      return i;
  }
  return 0;
}
/** AUTOMATON **/
bool automaton_automaton_has_state(automaton_automaton *current_automaton, uint32_t state) {
  return current_automaton->transitions_count > state &&
         (current_automaton->out_degree[state] > 0 || current_automaton->in_degree[state] > 0);
}
uint32_t automaton_automaton_get_in_degree(automaton_automaton *current_automaton, uint32_t state) {
  return current_automaton->in_degree[state];
}
uint32_t automaton_automaton_get_out_degree(automaton_automaton *current_automaton, uint32_t state) {
  return current_automaton->out_degree[state];
}
automaton_transition *automaton_automaton_get_transitions(automaton_automaton *current_automaton, uint32_t state) {
  return current_automaton->transitions[state];
}
bool automaton_automaton_has_transition(automaton_automaton *current_automaton, automaton_transition *transition) {
  uint32_t from_state = transition->state_from;
  uint32_t to_state = transition->state_to;
  if (!automaton_automaton_has_state(current_automaton, from_state) || !automaton_automaton_has_state(current_automaton, to_state))
    return false;
  automaton_transition *current_transitions = current_automaton->transitions[from_state];
  uint32_t out_degree = current_automaton->out_degree[from_state];
  uint32_t i, j, k;
  if (out_degree == 0)
    return false;
  bool result = false;
  uint32_t fixed_max_count = (uint32_t)ceil(current_automaton->context->global_alphabet->count * 1.0f) / (sizeof(signal_bit_array_t) * 8);
  for (i = 0; i < out_degree; i++) {
    result = true;
    result &= transition->state_from == (&current_transitions[i])->state_from;
    if (!result)
      continue;
    result &= transition->state_to == (&current_transitions[i])->state_to;
    if (!result)
      continue;
    for (j = 0; (j < fixed_max_count) && result; j++) {
      result &= (transition->signals[j] == (&current_transitions[i])->signals[j]);
      if (!result)
        break;
    }
    TRANSITION_EQUALS(transition, (&current_transitions[i]), result);
    if (result)
      return true;
  }
  return false;
}
void automaton_automaton_resize_to_state(automaton_automaton *current_automaton, uint32_t state) {
  if (state < current_automaton->transitions_size)
    return;
  uint32_t i, j;
  uint32_t old_size = current_automaton->transitions_size;
  uint32_t next_size = old_size;
  while (state >= next_size)
    next_size *= LIST_INCREASE_FACTOR;
  // printf("%d\t%d\t%d\n", state, next_size,
  // current_automaton->transitions_composite_count);
  bool *next_is_controllable = malloc(sizeof(bool) * next_size);
  uint32_t *next_out_degree = malloc(sizeof(uint32_t) * next_size);
  uint32_t *next_out_size = malloc(sizeof(uint32_t) * next_size);
  uint32_t *next_in_degree = malloc(sizeof(uint32_t) * next_size);
  uint32_t *next_in_size = malloc(sizeof(uint32_t) * next_size);
  automaton_transition **next_trans = malloc(sizeof(automaton_transition *) * next_size);
  automaton_transition **next_inv_trans = malloc(sizeof(automaton_transition *) * next_size);
  for (i = 0; i < current_automaton->transitions_size; i++) {
    next_is_controllable[i] = current_automaton->is_controllable[i];
    next_out_degree[i] = current_automaton->out_degree[i];
    next_in_degree[i] = current_automaton->in_degree[i];
    next_in_size[i] = current_automaton->in_size[i];
    next_out_size[i] = current_automaton->out_size[i];
    next_trans[i] = current_automaton->transitions[i];
    next_inv_trans[i] = current_automaton->inverted_transitions[i];
  }
  for (i = current_automaton->transitions_size; i < next_size; i++) {
    next_is_controllable[i] = true;
    next_out_degree[i] = 0;
    next_in_degree[i] = 0;
    next_in_size[i] = TRANSITIONS_INITIAL_SIZE;
    next_out_size[i] = TRANSITIONS_INITIAL_SIZE;
    next_trans[i] = calloc(next_out_size[i], sizeof(automaton_transition));
    for (j = 0; j < next_out_size[i]; j++) {
      automaton_transition_initialize(&(next_trans[i][j]), 0, 0);
    }
    next_inv_trans[i] = calloc(next_in_size[i], sizeof(automaton_transition));
    for (j = 0; j < next_in_size[i]; j++) {
      automaton_transition_initialize(&(next_inv_trans[i][j]), 0, 0);
    }
  }

  free(current_automaton->is_controllable);
  free(current_automaton->out_degree);
  free(current_automaton->in_degree);
  free(current_automaton->out_size);
  free(current_automaton->in_size);
  free(current_automaton->transitions);
  free(current_automaton->inverted_transitions);

  current_automaton->is_controllable = next_is_controllable;
  current_automaton->out_degree = next_out_degree;
  current_automaton->in_degree = next_in_degree;
  current_automaton->out_size = next_out_size;
  current_automaton->in_size = next_in_size;
  current_automaton->transitions = next_trans;
  current_automaton->inverted_transitions = next_inv_trans;
  current_automaton->transitions_size = next_size;

  uint32_t old_valuations_size, new_valuations_size;
  if (current_automaton->source_type & SOURCE_GAME) {
    uint32_t *valuations_ptr = NULL;
    if (current_automaton->context->global_fluents_count > 0) {
      old_valuations_size = current_automaton->valuations_size;
      current_automaton->valuations_size =
          GET_FLUENTS_ARR_SIZE(current_automaton->context->global_fluents_count, current_automaton->transitions_size);
      valuations_ptr = realloc(current_automaton->valuations, current_automaton->valuations_size * FLUENT_ENTRY_SIZE);
      if (valuations_ptr == NULL) {
        printf("Could not allocate memory for vstates_ptr\n");
        exit(-1);
      }
      current_automaton->valuations = valuations_ptr;
      for (i = old_valuations_size; i < current_automaton->valuations_size; i++)
        current_automaton->valuations[i] = 0x0;
    }
    if (current_automaton->context->liveness_valuations_count > 0) {
      old_valuations_size = current_automaton->liveness_valuations_size;
      current_automaton->liveness_valuations_size =
          GET_FLUENTS_ARR_SIZE(current_automaton->context->liveness_valuations_count, current_automaton->transitions_size);
      valuations_ptr = realloc(current_automaton->liveness_valuations, current_automaton->liveness_valuations_size * FLUENT_ENTRY_SIZE);
      if (valuations_ptr == NULL) {
        printf("Could not allocate memory for vstates_ptr\n");
        exit(-1);
      }
      current_automaton->liveness_valuations = valuations_ptr;
      for (i = old_valuations_size; i < current_automaton->liveness_valuations_size; i++)
        current_automaton->liveness_valuations[i] = 0x0;
    }
    if (current_automaton->context->state_valuations_count > 0) {
      old_valuations_size = current_automaton->state_valuations_size;
      current_automaton->state_valuations_size =
          GET_FLUENTS_ARR_SIZE(current_automaton->context->state_valuations_count, current_automaton->transitions_size);
      valuations_ptr = realloc(current_automaton->state_valuations, current_automaton->state_valuations_size * FLUENT_ENTRY_SIZE);
      if (valuations_ptr == NULL) {
        printf("Could not allocate memory for vstates_ptr\n");
        exit(-1);
      }
      current_automaton->state_valuations = valuations_ptr;
      for (i = old_valuations_size; i < current_automaton->state_valuations_size; i++)
        current_automaton->state_valuations[i] = 0x0;
    }
  }
}
bool automaton_automaton_transition_monitored_leq(automaton_automaton *current_automaton, automaton_transition *left,
                                                  automaton_transition *right) {
  int32_t i;
  for (i = 0; i < FIXED_SIGNALS_COUNT; i++) {
    if ((left->signals[i] & current_automaton->monitored_mask[i]) > (right->signals[i] & current_automaton->monitored_mask[i]))
      return false;
  }
  return true;
}
bool automaton_automaton_transition_monitored_eq(automaton_automaton *current_automaton, automaton_transition *left,
                                                 automaton_transition *right) {
  uint32_t i;
  for (i = 0; i < FIXED_SIGNALS_COUNT; i++) {
    if ((left->signals[i] & current_automaton->monitored_mask[i]) != (right->signals[i] & current_automaton->monitored_mask[i]))
      return false;
  }
  return true;
}
bool automaton_automaton_transition_lt(automaton_automaton *current_automaton, automaton_transition *left, automaton_transition *right) {
  uint32_t i;
  for (i = 0; i < FIXED_SIGNALS_COUNT; i++)
    if (left->signals[i] < right->signals[i])
      return true;
  return false;
}
bool automaton_automaton_transition_eq(automaton_automaton *current_automaton, automaton_transition *left, automaton_transition *right) {
  uint32_t i;
  for (i = 0; i < FIXED_SIGNALS_COUNT; i++)
    if (left->signals[i] != right->signals[i])
      return false;
  return true;
}
void automaton_automaton_monitored_order_transitions(automaton_automaton *current_automaton) {
  if (current_automaton->ordered)
    return;
  uint32_t i, j, k;
  uint32_t max_degree = 0;
  for (i = 0; i < current_automaton->transitions_count; i++) {
    if (current_automaton->out_degree[i] > max_degree)
      max_degree = current_automaton->out_degree[i];
    if (current_automaton->in_degree[i] > max_degree)
      max_degree = current_automaton->in_degree[i];
  }
  automaton_transition *tmp_transitions = calloc(max_degree, sizeof(automaton_transition));
  bool *checked_transitions = calloc(max_degree, sizeof(bool));
  automaton_transition *min_transition = NULL;
  uint32_t min_index = 0;
  for (i = 0; i < current_automaton->transitions_count; i++) {
    // order outgoing transitions
    if (current_automaton->out_degree[i] > 1) {
      // clear checked bookkeeping struct
      for (j = 0; j < current_automaton->out_degree[i]; j++)
        checked_transitions[j] = false;
      min_transition = NULL;
      // selection sort

      bool new_monitored = false;

      for (j = 0; j < current_automaton->out_degree[i]; j++) {
        for (k = 0; k < current_automaton->out_degree[i]; k++) {
          // if already picked skip
          if (checked_transitions[k])
            continue;
          // if min is NULL or current transition < min update
          if (min_transition == NULL ||
              (automaton_automaton_transition_monitored_leq(current_automaton, &(current_automaton->transitions[i][k]), min_transition))) {
            min_transition = &(current_automaton->transitions[i][k]);
            min_index = k;
          }
        }
        // copy in order to tmp_struct
        checked_transitions[min_index] = true;
        automaton_transition_copy(&(current_automaton->transitions[i][min_index]), &(tmp_transitions[j]));
        min_transition = NULL;
      }
      // ordered insertion into source
      for (j = 0; j < current_automaton->out_degree[i]; j++)
        automaton_transition_copy(&(tmp_transitions[j]), &(current_automaton->transitions[i][j]));
    }
    // order incoming transitions
    if (current_automaton->in_degree[i] > 1) {
      for (j = 0; j < current_automaton->in_degree[i]; j++)
        checked_transitions[j] = false;
      for (j = 0; j < current_automaton->in_degree[i]; j++) {
        for (k = 0; k < current_automaton->in_degree[i]; k++) {
          if (checked_transitions[k])
            continue;
          if (min_transition == NULL || (automaton_automaton_transition_monitored_leq(
                                            current_automaton, &(current_automaton->inverted_transitions[i][k]), min_transition))) {
            min_transition = &(current_automaton->inverted_transitions[i][k]);
            min_index = k;
          }
        }
        checked_transitions[min_index] = true;
        automaton_transition_copy(&(current_automaton->inverted_transitions[i][min_index]), &(tmp_transitions[j]));
        min_transition = NULL;
      }
      for (j = 0; j < current_automaton->in_degree[i]; j++)
        automaton_transition_copy(&(tmp_transitions[j]), &(current_automaton->inverted_transitions[i][j]));
    }
  }
  free(tmp_transitions);
  free(checked_transitions);
  current_automaton->ordered = true;
}
bool automaton_automaton_add_transition(automaton_automaton *current_automaton, automaton_transition *transition) {
  if (automaton_automaton_has_transition(current_automaton, transition))
    return false;
  current_automaton->ordered = false;
  uint32_t i;
  uint32_t from_state = transition->state_from;
  uint32_t to_state = transition->state_to;
  automaton_automaton_resize_to_state(current_automaton, from_state);
  automaton_automaton_resize_to_state(current_automaton, to_state);
  if ((from_state + 1) > current_automaton->transitions_count) {
    current_automaton->transitions_count = from_state + 1;
  }
  if ((to_state + 1) > current_automaton->transitions_count) {
    current_automaton->transitions_count = to_state + 1;
  }
  uint32_t old_out_degree = current_automaton->out_degree[from_state];
  uint32_t old_in_degree = current_automaton->in_degree[to_state];
  uint32_t old_out_size = current_automaton->out_size[from_state];
  uint32_t old_in_size = current_automaton->in_size[to_state];

  if (old_out_degree >= old_out_size) {
    current_automaton->out_size[from_state] = (old_out_size * LIST_INCREASE_FACTOR);
    automaton_transition *new_out =
        realloc(current_automaton->transitions[from_state], current_automaton->out_size[from_state] * sizeof(automaton_transition));
    if (new_out == NULL) {
      printf("Could not allocate memory for out transitions at state %d\n", from_state);
      exit(-1);
    }
    memset(&(new_out[old_out_degree]), 0, (current_automaton->out_size[from_state] - old_out_degree) * sizeof(automaton_transition));
    current_automaton->transitions[from_state] = new_out;
  }
  if (TRANSITION_IS_INPUT(transition))
    current_automaton->is_controllable[from_state] = false;
  automaton_transition_copy(transition, &(current_automaton->transitions[from_state][old_out_degree]));
  if (old_in_degree >= old_in_size) {
    current_automaton->in_size[to_state] = (old_in_size * LIST_INCREASE_FACTOR);
    automaton_transition *new_in =
        realloc(current_automaton->inverted_transitions[to_state], current_automaton->in_size[to_state] * sizeof(automaton_transition));
    if (new_in == NULL) {
      printf("Could not allocate memory for in transitions at state %d\n", to_state);
      exit(-1);
    }
    memset(&(new_in[old_in_degree]), 0, (current_automaton->in_size[to_state] - old_in_degree) * sizeof(automaton_transition));
    current_automaton->inverted_transitions[to_state] = new_in;
  }

  automaton_transition_copy(transition, &(current_automaton->inverted_transitions[to_state][old_in_degree]));

  current_automaton->out_degree[from_state]++;
  current_automaton->in_degree[to_state]++;
  if (current_automaton->max_out_degree < current_automaton->out_degree[from_state])
    current_automaton->max_out_degree = current_automaton->out_degree[from_state];
  GET_TRANSITION_SIGNAL_COUNT(transition);
  if (current_automaton->max_concurrent_degree < signal_count)
    current_automaton->max_concurrent_degree = signal_count;
  current_automaton->transitions_composite_count++;
  return true;
}

bool automaton_automaton_remove_transition(automaton_automaton *current_automaton, automaton_transition *transition) {
  uint32_t from_state = transition->state_from;
  uint32_t to_state = transition->state_to;
  automaton_transition *other_transition;
  int32_t index = -1, inverse_index = -1;
  uint32_t i, j, k;
  bool transition_found = true;
  for (i = 0; i < current_automaton->out_degree[from_state]; i++) { // find transition index in automaton
    other_transition = &(current_automaton->transitions[from_state][i]);
    TRANSITION_EQUALS(transition, other_transition, transition_found);
    if (transition_found) {
      index = i;
      break;
    }
  }
  if (!transition_found)
    return false;
  for (i = 0; i < current_automaton->in_degree[to_state]; i++) { // find inverse index in automaton
    other_transition = &(current_automaton->inverted_transitions[to_state][i]);
    TRANSITION_EQUALS(transition, other_transition, transition_found);
    if (transition_found) {
      inverse_index = i;
      break;
    }
  }
  if (!transition_found)
    return false;
  current_automaton->ordered = false;
  // remove from transitions
  if (index > -1) {
    automaton_transition_destroy(&(current_automaton->transitions[from_state][index]), false);
    for (i = index; i < current_automaton->out_degree[from_state] - 1; i++) {
      memcpy(&(current_automaton->transitions[from_state][i]), &(current_automaton->transitions[from_state][i + 1]),
             sizeof(automaton_transition));
    }
    current_automaton->out_degree[from_state]--;
  }
  // remove from inverted transitions
  if (inverse_index > -1) {
    automaton_transition_destroy(&(current_automaton->inverted_transitions[to_state][inverse_index]), false);
    for (i = inverse_index; i < current_automaton->in_degree[to_state] - 1; i++) {
      memcpy(&(current_automaton->inverted_transitions[to_state][i]), &(current_automaton->inverted_transitions[to_state][i + 1]),
             sizeof(automaton_transition));
    }
    current_automaton->in_degree[to_state]--;
  }
  // update structures
  // current_automaton->transitions_count--;
  current_automaton->transitions_composite_count--;
  return true;
}

bool automaton_automaton_add_initial_state(automaton_automaton *current_automaton, uint32_t state) {
  // if(!automaton_automaton_has_state(current_automaton, state))return true;
  uint32_t i;
  for (i = 0; i < current_automaton->initial_states_count; i++) {
    if (current_automaton->initial_states[i] == state)
      return true;
  }
  uint32_t *new_initial_states = malloc(sizeof(uint32_t) * (current_automaton->initial_states_count + 1));
  for (i = 0; i < current_automaton->initial_states_count; i++) {
    new_initial_states[i] = current_automaton->initial_states[i];
  }
  new_initial_states[current_automaton->initial_states_count] = state;
  if (current_automaton->initial_states_count > 0) {
    free(current_automaton->initial_states);
  }
  current_automaton->initial_states_count++;
  current_automaton->initial_states = new_initial_states;
  return false;
}

uint32_t *automaton_automaton_distance_to_state(automaton_automaton *automaton, uint32_t state) {
  uint32_t *distances = calloc(automaton->transitions_count, sizeof(uint32_t));
  bool *visited = calloc(automaton->transitions_count, sizeof(bool));
  automaton_concrete_bucket_list *pending_list =
      automaton_concrete_bucket_list_create(DISTANCE_BUCKET_SIZE, automaton_int_extractor, sizeof(uint32_t));
  uint32_t i;
  uint32_t min_distance;
  automaton_transition *current_transition;
  uint32_t current_state;
  // add initial states (excluding incoming state)
  visited[state] = true;
  for (i = 0; i < automaton->out_degree[state]; i++) {
    current_transition = &(automaton->transitions[state][i]);
    if (current_transition->state_to != state && !visited[current_transition->state_to]) {

      automaton_concrete_bucket_add_entry(pending_list, &(current_transition->state_to));
    }
  }
  // update distances
  while (pending_list->composite_count > 0) {
    automaton_concrete_bucket_pop_entry(pending_list, &current_state);
    if (current_state >= automaton->transitions_count)
      continue;
    if (visited[current_state])
      continue;
    visited[current_state] = true;
    min_distance = 0;
    for (i = 0; i < automaton->in_degree[current_state]; i++) {
      current_transition = &(automaton->inverted_transitions[current_state][i]);
      if (!visited[current_transition->state_from])
        continue;
      if (min_distance == 0 || (current_transition->state_from + 1 < min_distance))
        min_distance = distances[current_transition->state_from] + 1;
    }
    distances[current_state] = min_distance;

    for (i = 0; i < automaton->out_degree[current_state]; i++) {
      current_transition = &(automaton->transitions[current_state][i]);
      if (!visited[current_transition->state_to]) {
        automaton_concrete_bucket_add_entry(pending_list, &(current_transition->state_to));
      }
    }
  }

  automaton_concrete_bucket_destroy(pending_list, true);
  free(visited);
  return distances;
}

void automaton_automaton_remove_unreachable_states(automaton_automaton *automaton) {
  if (automaton->initial_states_count < 1)
    return;
  if (automaton->transitions_count < 1)
    return;
  uint32_t *distances = automaton_automaton_distance_to_state(automaton, automaton->initial_states[0]);
  uint32_t i, j;
  bool unreachable_found = false;
  do {
    unreachable_found = false;
    for (i = 0; i < automaton->transitions_count; i++) {
      if (i == automaton->initial_states[0])
        continue;
      if ((automaton->in_degree[i] != 0) && (distances[i] != 0))
        continue;
      if (automaton->in_degree[i] == 0 && automaton->out_degree[i] == 0)
        continue;
      unreachable_found = true;
      for (j = 0; j < automaton->out_degree[i]; j++) {
        automaton_automaton_remove_transition(automaton, &(automaton->transitions[i][j]));
      }
    }
  } while (unreachable_found);
  free(distances);
}

void automaton_automaton_remove_deadlocks(automaton_automaton *automaton) {
  if (automaton->initial_states_count < 1)
    return;
  uint32_t i, j;
  bool deadlock_found = false;
  do {
    deadlock_found = false;
    for (i = 0; i < automaton->transitions_count; i++) {
      if (automaton->out_degree[i] == 0 && automaton->in_degree[i] > 0) {
        deadlock_found = true;
        for (j = 0; j < automaton->in_degree[i]; j++) {
          automaton_automaton_remove_transition(automaton, &(automaton->inverted_transitions[i][j]));
        }
      }
    }
  } while (deadlock_found);
  automaton_automaton_remove_unreachable_states(automaton);
}

void automaton_automaton_update_valuations(automaton_automaton *automaton) {
  uint32_t i, j;
  for (i = 0; i < automaton->transitions_count; i++) {
    if (automaton->in_degree[i] == 0 && automaton->out_degree[i] == 0) {
      for (j = 0; j < automaton->context->global_fluents_count; j++) {
        if (automaton_bucket_has_entry(automaton->inverted_valuations[j], i)) {
          automaton_bucket_remove_entry(automaton->inverted_valuations[j], i);
        }
      }
    }
  }
}

bool automaton_automaton_check_invariant(automaton_automaton *current_automaton) { return false; }
bool automaton_automaton_update_valuation(automaton_automaton *current_automaton) { return false; }

automaton_automaton *automaton_automaton_determinize(automaton_automaton *left_automaton) { return NULL; }
automaton_automaton *automaton_automaton_obs_minimize(automaton_automaton *left_automaton) { return NULL; }

#define DFAMIN_PART(i)(partition[i * 2])
#define DFAMIN_PART_STATE(i)(partition[i * 2 + 1])
#define DFAMIN_PART_SIZE(i)(partition_size[i * 3])
#define DFAMIN_CROSS_SIZE(i)(partition_size[i * 3 + 1])
#define DFAMIN_PART_FIRST_ENTRY(i)(partition_size[i * 3 + 2])
#define DFAMIN_PART_SIZE_CURR (DFAMIN_PART_SIZE(waiting[waiting_count -1]))
#define DFAMIN_CROSS_SIZE_CURR (DFAMIN_CROSS_SIZE(waiting[waiting_count -1]))
#define DFAMIN_PART_FIRST_ENTRY_CURR (DFAMIN_PART_FIRST_ENTRY(waiting[waiting_count -1]))

automaton_automaton *automaton_automaton_minimize(automaton_automaton *current_automaton) {
  if (current_automaton->is_game) {
    printf("Could no minimize automaton %s since it has valuations associated to it", current_automaton->name);
    exit(-1);
  }
  // (P) partition will hold an array of double int entries, the first int indicates
  // partition the second is the state
  uint32_t *partition = malloc(sizeof(uint32_t) * current_automaton->transitions_count * 2);
  // (W) waiting will hold an array of int entries indicating if a partition is waiting to be processed
  uint32_t *waiting = malloc(sizeof(uint32_t) * current_automaton->transitions_count);
  // indicate whether i-th partition is in waiting queue
  bool *inverse_waiting = calloc(current_automaton->transitions_count, sizeof(bool));

  // these are the double buffers for partition and waiting
  uint32_t *partition_buffer = malloc(sizeof(uint32_t) * current_automaton->transitions_count * 2);
  uint32_t *tmp_buffer = NULL;
  // partition size, entry[i * 3] holds partitition_i size, entry[i * 3 + 1] holds P_i * Y size
  // entry[i * 3 + 2] holds partition start index at array
  uint32_t *partition_size = malloc(sizeof(uint32_t) * current_automaton->transitions_count * 3);
  // inverse partition indicates at which index a state resides in the partition
  // array
  uint32_t *inverse_partition = calloc(current_automaton->transitions_count, sizeof(uint32_t));
  uint32_t waiting_count = 2;
  uint32_t partition_count = 2;
  // current element being inspected, follows states order as in automaton
  bool *current_partition = calloc(current_automaton->transitions_count, sizeof(bool));
  // arithmetic results with current element being inspected,
  // if |X * Y| > 0 it is set to true, following partition order
  bool *partition_cross = calloc(current_automaton->transitions_count, sizeof(bool));
  int32_t i, j, k;
  // initialize partition
#if DEBUG_MINIMIZATION
  printf("\tInitializing partition for %d states.\n", current_automaton->transitions_count);
#endif
  for (i = 0; i < current_automaton->transitions_count; i++) {
    if (i == current_automaton->initial_states[0]) {
      DFAMIN_PART(0) = 0;
      DFAMIN_PART_STATE(0) = current_automaton->initial_states[0];
      inverse_partition[i] = 0;
    } else if (i < current_automaton->initial_states[0]) {
      DFAMIN_PART(i + 1) = 1;
      DFAMIN_PART_STATE(i + 1) = i;
      inverse_partition[i] = i + 1;
    } else {
      DFAMIN_PART(i) = 1;
      DFAMIN_PART_STATE(i) = i;
      inverse_partition[i] = i;
    }
  }
#if DEBUG_MINIMIZATION
  printf("\tFirst partition contains the initial state, second partition contains the rest.\n");
#endif
  partition_size[0] = 1;
  partition_size[1] = 0;
  partition_size[2] = 0;
  partition_size[3] = current_automaton->transitions_count - 1;
  partition_size[4] = 0;
  partition_size[5] = 1;
  // initialize waiting
  waiting[0] = 0;
  waiting[1] = 1;
  inverse_waiting[0] = true;
  inverse_waiting[1] = true;

  uint32_t last_source_index = 0;
  uint32_t last_cap_index = 0;
  uint32_t last_minus_index = 0;
  uint32_t added_partitions = 0;
  // start refining
  uint32_t current_state, incoming_transition_from_state;
  while (waiting_count > 0) {
    // clear current partition and parttion cross
#if DEBUG_MINIMIZATION
    printf("\t\t[waiting: %d]\n\t\tClearing current partition and cross structures.\n", waiting_count);
#endif
    for (i = 0; i < current_automaton->transitions_count; i++) {
      current_partition[i] = false;
      partition_cross[i] = false;
    }
#if DEBUG_MINIMIZATION
    printf("\t\t[current_partition (X): ");
#endif
    // get X as all the states leading to the current partition
    for (i = DFAMIN_PART_FIRST_ENTRY_CURR; i <= DFAMIN_PART_FIRST_ENTRY_CURR + DFAMIN_PART_SIZE_CURR; i++) {
      current_state = DFAMIN_PART_STATE(i);
      for (j = 0; j < current_automaton->in_degree[current_state]; j++) {
        incoming_transition_from_state = current_automaton->inverted_transitions[current_state][j].state_from;
#if DEBUG_MINIMIZATION
        printf("%d ", incoming_transition_from_state);
#endif
        // update current partition and partition cross
        current_partition[incoming_transition_from_state] = true;
        partition_cross[inverse_partition[incoming_transition_from_state]] = true;
        // update intersection size
        DFAMIN_CROSS_SIZE(DFAMIN_PART(inverse_partition[incoming_transition_from_state]))++;
      }
    }
#if DEBUG_MINIMIZATION
    printf("]\n");
#endif
    inverse_waiting[(waiting_count - 1)] = false;
    waiting_count--;
    // update partition (move from - to buffer)
    // P <- P - {Y} + {X * Y, Y - X}
    last_source_index = 0;
    last_cap_index = 0;
    last_minus_index = 0;
    added_partitions = 0;

    for (i = 0; i < partition_count; i++) {
      // check * and - not empty
      if (DFAMIN_CROSS_SIZE(i) == 0 || DFAMIN_CROSS_SIZE(i) == DFAMIN_PART_SIZE(i))
        continue;
#if DEBUG_MINIMIZATION
    printf("\t\t[Update W]\n");
    printf("\t\t\t[Update start indexes]\n");
#endif
      // cap set starts at the same place as the original partition
      last_cap_index = last_source_index;
      // cap set starts at the same place as the original partition + cup size - minus size
      last_minus_index = last_source_index + DFAMIN_PART_SIZE(i) - DFAMIN_CROSS_SIZE(i);
      // update W
      // Y in W: W <- W - {Y} + {X * Y, Y - X}
      // |X * Y| <= |Y - X|: W <- W + {X * Y}
      // otherwise: W <- W + {Y - X}
      if (inverse_waiting[i]) {
#if DEBUG_MINIMIZATION
    printf("\t\t\t[Y?W]\n");
#endif
        waiting[waiting_count++] = DFAMIN_PART(j);
        waiting[waiting_count++] = partition_count + added_partitions;
        inverse_waiting[partition[j * 2]] = true;
        inverse_waiting[partition_count + added_partitions] = true;
      } else if (DFAMIN_CROSS_SIZE(i) <= (DFAMIN_PART_SIZE(i) - DFAMIN_CROSS_SIZE(i))) {
#if DEBUG_MINIMIZATION
    printf("\t\t\t[|X*Y|<=|Y-X|]\n");
#endif
        waiting[waiting_count++] = DFAMIN_PART(j);
        inverse_waiting[partition[j * 2]] = true;
      } else {
#if DEBUG_MINIMIZATION
    printf("\t\t\t[|X*Y|>|Y-X|]\n");
#endif
        waiting[waiting_count++] = partition_count + added_partitions;
        inverse_waiting[partition_count + added_partitions] = true;
      }
#if DEBUG_MINIMIZATION
    printf("\t\t[Update partition buffer]\n");
#endif
      for (j = last_source_index; j <= (last_source_index + DFAMIN_PART_SIZE(i)); j++) {
#if DEBUG_MINIMIZATION
    printf("\t\t\t[");
#endif
        if (partition_cross[j]) {
          // previous partition index is used for *
          partition_buffer[last_cap_index * 2] = DFAMIN_PART(j);
          // update state in partition and inverse partition
          partition_buffer[last_cap_index * 2 + 1] = DFAMIN_PART_STATE(j);
          inverse_partition[DFAMIN_PART_STATE(j)] = last_cap_index * 2 + 1;
          last_cap_index++;
#if DEBUG_MINIMIZATION
    printf("(*%d:(%d,%d))", last_cap_index, DFAMIN_PART(j), DFAMIN_PART_STATE(j));
#endif
        } else {
          // - set is set as fresh partition
          partition_buffer[last_minus_index * 2] = partition_count + added_partitions;
          // update state in partition and inverse partition
          partition_buffer[last_minus_index * 2 + 1] = DFAMIN_PART_STATE(j);
          inverse_partition[DFAMIN_PART_STATE(j)] = last_minus_index * 2 + 1;
          last_minus_index++;
#if DEBUG_MINIMIZATION
    printf("(-%d:(%d,%d))", last_minus_index, partition_count + added_partitions, DFAMIN_PART_STATE(j));
#endif
        }
      }
#if DEBUG_MINIMIZATION
    printf("]\n");
    printf("\t\t[Update partition idxs and sizes]\n");
    printf("\t\t[|P|:%d]\n", partition_count + added_partitions);
#endif
      // update partition size
      // order of updates is important to keep dependencies working
      uint32_t next_source_index = last_source_index + DFAMIN_PART_SIZE(i);
      DFAMIN_PART_SIZE(partition_count + added_partitions) = last_minus_index;
      DFAMIN_CROSS_SIZE(partition_count + added_partitions) = 0;
      DFAMIN_PART_FIRST_ENTRY(partition_count + added_partitions) = last_source_index + DFAMIN_PART_SIZE(i) - DFAMIN_CROSS_SIZE(i);
      DFAMIN_PART_SIZE(i) = last_cap_index;
      DFAMIN_CROSS_SIZE(i) = 0;
      DFAMIN_PART_FIRST_ENTRY(i) = last_source_index;
      last_source_index = next_source_index;
      added_partitions++;
    }
    partition_count += added_partitions;

    tmp_buffer = partition;
    partition = partition_buffer;
    partition_buffer = tmp_buffer;
  }

  // NOTE: we are not checking for valuations inconsistencies when merging, this will be too expensive
  // due to the way in which we encode valuations for each state (unaligned)
  // merge states  //sort according to valuations
  last_source_index = 0;
  uint32_t candidate_state = 0;
#if DEBUG_MINIMIZATION
  printf("\t\t[Normalize states]\n");
  printf("\t\t\t[");
#endif
  for (i = 0; i < partition_count; i++) {
    candidate_state = DFAMIN_PART_STATE(i);
    for (j = last_source_index; j <= (last_source_index + DFAMIN_PART_SIZE(i)); j++){
#if DEBUG_MINIMIZATION
      printf("(%d<-%d)", DFAMIN_PART_STATE(j), candidate_state);
#endif

      DFAMIN_PART_STATE(j) = candidate_state;
    }
    last_source_index += DFAMIN_PART_SIZE(i);
  }
#if DEBUG_MINIMIZATION
  printf("]\n");
#endif
  automaton_transition tmp_transition = {.state_from = 0, .state_to = 0};
  automaton_transition *current_transition = NULL;
  // if the current transition is redundant after minimization
  // it is removed from the automaton
  for (i = 0; i < current_automaton->transitions_count; i++) {
    for (j = current_automaton->out_degree[i] - 1; j >= 0; j--) {
      current_transition = &(current_automaton->transitions[i][j]);
      automaton_transition_copy(current_transition, &tmp_transition);
      // check if current transition is not the candidate to be kept
      if(tmp_transition.state_from != DFAMIN_PART_STATE(inverse_partition[tmp_transition.state_from]) ||
          tmp_transition.state_to != DFAMIN_PART_STATE(inverse_partition[tmp_transition.state_to])){
        // normalize transition
        tmp_transition.state_from = DFAMIN_PART_STATE(inverse_partition[tmp_transition.state_from]);
        tmp_transition.state_to = DFAMIN_PART_STATE(inverse_partition[tmp_transition.state_to]);
        if(automaton_automaton_has_transition(current_automaton, &tmp_transition)){
#if DEBUG_MINIMIZATION
          printf("\t\t[Removing");
          automaton_transition_print(current_transition, current_automaton->context, false, NULL, NULL, 0);
          printf("]\n");
#endif
          automaton_automaton_remove_transition(current_automaton, current_transition);
        }
      }
    }
  }
  // update inverse partition to use a map from state to representative state

  free(partition);
  free(waiting);
  free(partition_buffer);
  free(inverse_waiting);
  free(partition_size);
  free(inverse_partition);
  free(current_partition);
  free(partition_cross);
  return current_automaton;
}

/**
 * Checks if two deterministic and minimized automata are equivalent.
 * @param left_automaton the first deterministic minimal automaton to be check
 * against equivalence
 * @param right_automaton the second deterministic minimal automaton to be check
 * against equivalence
 * @return true if both automata are equivalent down to state renaming, false
 * otherwise
 */
bool automaton_automata_are_equivalent(automaton_automaton *left_automaton, automaton_automaton *right_automaton) {
  if (left_automaton->out_degree[left_automaton->initial_states[0]] != right_automaton->out_degree[right_automaton->initial_states[0]])
    return false;
  // if(left_automaton->transitions_composite_count !=
  // right_automaton->transitions_composite_count)return false;
  if (left_automaton->transitions_composite_count == 0)
    return (right_automaton->transitions_composite_count == 0);
  if (right_automaton->transitions_composite_count == 0)
    return (left_automaton->transitions_composite_count == 0);
  bool *processed = calloc(left_automaton->transitions_count * 2, sizeof(bool));
  uint32_t *left_frontier = calloc(left_automaton->transitions_composite_count * 2, sizeof(uint32_t));
  uint32_t *right_frontier = calloc(right_automaton->transitions_composite_count * 2, sizeof(uint32_t));
  uint32_t frontier_count = 0;
  left_frontier[0] = left_automaton->initial_states[0];
  right_frontier[0] = right_automaton->initial_states[0];
  frontier_count++;

  uint32_t left_state, right_state, i, j, k;
  bool match_found = false, label_mismatch = false, transition_mismatch = false;

  while (frontier_count > 0 && !transition_mismatch) {
    frontier_count--;
    left_state = left_frontier[frontier_count];
    right_state = right_frontier[frontier_count];

    if (processed[left_state])
      continue;
    processed[left_state] = true;

    for (i = 0; i < left_automaton->out_degree[left_state]; i++) {
      match_found = false;
      for (j = 0; j < right_automaton->out_degree[right_state]; j++) {
        label_mismatch = false;
        for (k = 0; k < FIXED_SIGNALS_COUNT; k++) {
          if ((left_automaton->transitions[left_state][i].signals[k] & ~((signal_bit_array_t)1)) !=
              (right_automaton->transitions[right_state][j].signals[k] & ~((signal_bit_array_t)1))) {
            label_mismatch = true;
            break;
          }
        }
        if (!label_mismatch) {
          match_found = true;
          if (!processed[left_automaton->transitions[left_state][i].state_to]) {
            if (left_automaton->out_degree[left_automaton->transitions[left_state][i].state_to] !=
                right_automaton->out_degree[right_automaton->transitions[right_state][j].state_to]) {
              match_found = false;
            } else {
              left_frontier[frontier_count] = left_automaton->transitions[left_state][i].state_to;
              right_frontier[frontier_count] = right_automaton->transitions[right_state][j].state_to;
              frontier_count++;
            }
          }
          break;
        }
      }
      if (!match_found) {
        transition_mismatch = true;
        break;
      }
    }
  }

  free(processed);
  free(left_frontier);
  free(right_frontier);
  return !transition_mismatch;
}

bool automaton_automaton_check_reachability(automaton_automaton *current_automaton, automaton_valuation target) { return false; }
bool automaton_automaton_check_liveness(automaton_automaton *current_automaton, automaton_valuation target) { return false; }
