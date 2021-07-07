/*
 * automaton_compose.c
 *
 * Parallel composition implementations related to the automaton struct
 *
 *  Created on: Apr 16, 2021
 *      Author: mariano
 */

#include "automaton.h"
#include "assert.h"


uint32_t automaton_automata_get_composite_state(uint32_t states_count, uint32_t* states){
	return 0;
}

inline void automaton_automata_add_ordered_union_of_signals(automaton_automata_context* ctx, uint32_t* signals_union, uint32_t* signals_union_count, automaton_transition* left_transition
		, automaton_transition* right_transition, automaton_transition* target_transition){
	uint32_t i;
	for( i = 0; i < FIXED_SIGNALS_COUNT; i++)target_transition->signals[i] = left_transition->signals[i] | right_transition->signals[i];
}

/**
 * Gets the union of alphabets in the composition and a map of labels in the composition to global indexes
 * @param ctx automata composition context
 * @param automata an array of automata to be composed
 * @param automata_count the number of automata to be composed
 * @param alphabet_count placeholder for the size of the composition local alphabet
 * @param compose_to_global an array of uint that maps composition local entries to the global alphabet
 * @return an array of uint representing the composition alphabet
 */
uint32_t* automaton_automata_get_union_alphabet(automaton_automata_context **ctx, automaton_automaton** automata, uint32_t automata_count
	, uint32_t *alphabet_count){
	uint32_t alphabet_size	= 0;
	uint32_t i, j, k, l;
	for(i = 0; i < automata_count; i++){
		if(automata[i]->initial_states_count != 1)
			return NULL;
		if(*ctx == NULL){
			*ctx = automata[i]->context;
		}else if(*ctx != automata[i]->context){
			return NULL;
		}
		alphabet_size += automata[i]->local_alphabet_count;
	}
	uint32_t *alphabet	= malloc(sizeof(uint32_t) * alphabet_size);
	// alphabet <- Union of alphabets
	int32_t new_index	= -1;
	uint32_t current_entry;
	bool overlaps 		= false;
	for(i = 0; i < automata_count; i++){
		for(j = 0; j < automata[i]->local_alphabet_count; j++){
			//compute signal dependency
			overlaps = false;
			for(k = 0; k < automata_count; k++){
				if(k != i){
					for(l = 0; l < automata[k]->local_alphabet_count; l++){
						if(automata[i]->local_alphabet[j] == automata[k]->local_alphabet[l]){
							overlaps = true; break;
						}
					}
				}
				if(overlaps) break;
			}
			//get local signal global index
			new_index	= -1;
			bool found	= false;
			current_entry	= automata[i]->local_alphabet[j];
			for(k = 0; k < *alphabet_count; k++){
				if(alphabet[k] == current_entry){
					found	= true;
					break;
				}
				if(alphabet[k] > current_entry){
					new_index	= k;
					break;
				}
			}
			//keep global alphabet ordered
			if(new_index > -1 && !found){
				for(k = (*alphabet_count); k > (uint32_t)new_index; k--){
					alphabet[k]	= alphabet[k - 1];
				}
			}else{
				new_index	= *alphabet_count;
			}
			if(!found){
				alphabet[new_index]	= current_entry;
				(*alphabet_count)++;
			}
			//test for alphabet ordering
		}
	}
	return alphabet;
}

inline void automaton_automata_add_pending_transitions(automaton_automaton **automata, uint32_t automata_count, uint32_t from_state, uint32_t *partial_states, bool *partial_set_states
	, uint32_t *current_state, automaton_transition** pending, uint32_t *pending_count, uint32_t max_degree_sum){
	uint32_t i, j, k;
	uint32_t current_out_degree			= 0;
	for(i = 0; i < automata_count; i++){
		current_out_degree		= automata[i]->out_degree[current_state[i]];
		for(j = 0; j < current_out_degree; j++){
			//add new transition to p_sigs and partial_state
			automaton_transition* starting_transition					= automaton_transition_clone(&(automata[i]->transitions[current_state[i]][j]));
			starting_transition->state_from								= from_state;
			for(k = 0; k < automata_count; k++){
				if(k == i){
					partial_states[*pending_count * automata_count + k]	= starting_transition->state_to;
					partial_set_states[*pending_count * automata_count + k]	= true;
				}else{
					partial_states[*pending_count * automata_count + k]	= current_state[k];
					partial_set_states[*pending_count * automata_count + k]	= false;
				}
			}
			pending[(*pending_count)++]			= starting_transition;
			if(*pending_count >= max_degree_sum){
				printf("[FATAL ERROR] WRONG BOUNDARY AT PENDING COUNT\n");
				exit(-1);
			}
		}
	}
}

/**
 * Increments the indexes array as a counter, if the max value is reached the returned value is true
 * @param idxs an array of the current indexes of the components' transitions to be evaluated
 * @param idxs_sizes the outgoing degree for the current component's state plus one for the case where the transition
 * 	is not applied
 * @param idxs_skip the list of transitions to be skipped when incrementing (if one transition shares alphabet with an
 * 	automaton whose state has no transition bringing that element skip it altogether)
 * @param automata_count the number of automata being composed
 * @return true if the max value for the indexes is reached, false otherwise
 */
inline bool automaton_automata_compose_increment_idxs(uint32_t *idxs, uint32_t *idxs_sizes, bool** idxs_skip, uint32_t automata_count){
	int32_t i = automata_count;
	bool set_anew = false;
	do{
		i--;
#if DEBUG_COMPOSITION
		printf("\t[I]dx[%d] ", i);
#endif

		set_anew	= false;
		do{
			if(idxs[i] < (idxs_sizes[i] - 1)){//increment current digit if possible
					idxs[i]++;
#if DEBUG_COMPOSITION
					printf(">%d",idxs[i]);
#endif
					if(!idxs_skip[i][idxs[i]-1])//if transition will not be skipped exit
						return false;
			}else{
				idxs[i] = 0;
				set_anew	= true;
#if DEBUG_COMPOSITION
					printf("|0");
#endif
			}
		}while(!set_anew && idxs[i] > 0 && idxs_skip[i][idxs[i]-1]);//keep incrmenting if current transition should be skipped
#if DEBUG_COMPOSITION
		printf("\n");
#endif
	}while(i >0);
	return true;
}

/**
 * Checks if the given indexes array has reached maximum value
 * @param idxs the array of indexes values
 * @param automata_count the number of automata to be composed
 * @return true if indexes have reach maximum value, false otherwise
 */
inline bool automaton_automata_idxs_is_max(uint32_t *idxs, uint32_t automata_count){
	int32_t i;
	for(i = 0; i < automata_count; i++){
		if(idxs[i] != 0)return false;
	}
	return true;
}

/**
 * Evaluates overlapping of the accumulated label with a given transition
 * @param accum_label a boolean array describing the members of the accumulated label
 * @param current_label a boolean array describing the current transition label
 * @param alphabet_overlap a boolean array describing which elements should be considered in the check
 * @return 0 if overlapping is total, 1 if overlapping is partial, -1 if overlapping is empty
 */
inline int32_t automaton_automata_check_overlap(signal_bit_array_t *accum_label, signal_bit_array_t *current_label, signal_bit_array_t *alphabet_overlap
		, uint32_t alphabet_count){
	uint32_t i;
	//check if overlapping transitions are empty
	for(i = 0; i < FIXED_SIGNALS_COUNT; i++){
		if(accum_label[i] > 0 || current_label[i] > 0)break;
		if(i == (FIXED_SIGNALS_COUNT - 1))return 0;
	}
	bool at_least_one_overlaps	= false;
	bool at_least_one_does_not	= false;
	bool no_overlapping			= true;
	signal_bit_array_t mask_all = ~((signal_bit_array_t)0x0);
	signal_bit_array_t mask_not1 = ~((signal_bit_array_t)0x1);
	signal_bit_array_t current_mask;

	for(i = 0; i < FIXED_SIGNALS_COUNT; i++){
		current_mask = i==0? mask_not1 : mask_all;
		if((alphabet_overlap[i]&current_mask)>(signal_bit_array_t)0x0){
			if(
					(
							(alphabet_overlap[i]&current_mask) &
							(
									(accum_label[i]&current_mask)| (current_label[i]&current_mask)
							)
					)
					>(signal_bit_array_t)0x0)
				no_overlapping = false;
			if(((alphabet_overlap[i]&accum_label[i]&current_mask) == (alphabet_overlap[i]&current_label[i]&current_mask))
					&& ((alphabet_overlap[i]&accum_label[i]&current_mask)>(signal_bit_array_t)0x0)){
				at_least_one_overlaps = true;
			}
			if((alphabet_overlap[i]&accum_label[i]&current_mask) != (alphabet_overlap[i]&current_label[i]&current_mask)){
				at_least_one_does_not = true;
			}
			if(!no_overlapping && at_least_one_does_not)return 1;
		}
	}
	if(no_overlapping)return -1;
	if(at_least_one_does_not)return 1;
	if(at_least_one_overlaps)return 0;
	return -1;
}
/*
void automaton_automata_merge_transition_alphabet_into_bool
(uint32_t *tmp_alphabet, automaton_transition *t, uint32_t alphabet_count)
void automaton_automata_transition_alphabet_to_bool
(uint32_t *tmp_alphabet, automaton_transition *t, uint32_t alphabet_count)
void automaton_automata_bool_to_transition_alphabet
(uint32_t *tmp_alphabet, automaton_transition *t, uint32_t alphabet_count)
bool automaton_automata_compose_increment_idxs(uint32_t *idxs, uint32_t *idxs_sizes, uint32_t automata_count)

*/

/**
 * Composes the automata according to their synchronization type.
 * @param automata the array of pointers to automata to be composed
 * @param synch_type the array of synchronization types to be applied sequentially and pairwise for the provided automata
 * @param automata_count the number of automata to be composed
 * @param is_game true if the resulting automaton will hold valuations on its states, false otherwise
 * @param no_mixed_states true if the resulting automaton will have no mixed states (controllable transitions from non controllable states will be removed)
 * @param composition_name the name of the composition as specified in the syntax
 * @return the CLTS resulting from applying the composition type according to synch_type definitions over automata
 */
automaton_automaton* automaton_automata_compose(automaton_automaton** automata, automaton_synchronization_type* synch_type, uint32_t automata_count
		, bool is_game, bool no_mixed_states, char* composition_name){
	clock_t begin = clock();
	uint32_t transitions_added_count	= 0;
	uint32_t i, j, k, l, m, n, o, p;
	uint32_t alphabet_count, fluents_count;	uint32_t* alphabet; uint32_t **complementary_alphabet; uint32_t *complementary_alphabet_count;
	alphabet_count	= 0;
	// get union of alphabets check ctx and compute alphabet size
	automaton_automata_context* ctx	= NULL;

	/*
	for(i = 0; i < automata_count; i++)
		automaton_compact_states(automata[i]);
	 */
	alphabet = automaton_automata_get_union_alphabet(&ctx, automata, automata_count, &alphabet_count);
	// create automaton
	automaton_automaton* composition;
	char buff[255];
	snprintf(buff, sizeof(buff), "%s_Game", composition_name);
	if(is_game){
		composition = automaton_automaton_create(buff, ctx, alphabet_count, alphabet, true, false);
	}else{
		composition = automaton_automaton_create(composition_name, ctx, alphabet_count, alphabet, false, false);
	}

	alphabet_count = ctx->global_alphabet->count;

	// create boolean alphabets sigma_i,accumulated alphabet sigma_1..i-1 and the overlapping between the two
	signal_bit_array_t **boolean_alphabet = calloc(automata_count, sizeof(signal_bit_array_t*));
	signal_bit_array_t **accumulated_boolean_alphabet = calloc(automata_count, sizeof(signal_bit_array_t*));
	signal_bit_array_t **accumulated_alphabet_overlap = calloc(automata_count, sizeof(signal_bit_array_t*));
	signal_bit_array_t *current_label	= calloc(FIXED_SIGNALS_COUNT, sizeof(signal_bit_array_t));

	for(i = 0; i < automata_count; i++){
		boolean_alphabet[i]	= calloc(FIXED_SIGNALS_COUNT, sizeof(signal_bit_array_t));
		accumulated_boolean_alphabet[i] = calloc(FIXED_SIGNALS_COUNT, sizeof(signal_bit_array_t));
		accumulated_alphabet_overlap[i] = calloc(FIXED_SIGNALS_COUNT, sizeof(signal_bit_array_t));
		for(j = 0; j < automata[i]->local_alphabet_count; j++){
			k	= automata[i]->local_alphabet[j];
			SET_SIGNAL_ARRAY_BIT(boolean_alphabet[i], k);
			SET_SIGNAL_ARRAY_BIT(accumulated_boolean_alphabet[i], k);
		}
		if(i > 0){
			for(k = 0; k < alphabet_count; k++){
				if(TEST_SIGNAL_ARRAY_BIT(accumulated_boolean_alphabet[i-1],k))
					SET_SIGNAL_ARRAY_BIT(accumulated_boolean_alphabet[i], k);
				accumulated_alphabet_overlap[i][((k+1)/TRANSITION_ENTRY_SIZE)] &= ~(((signal_bit_array_t)1) << ((k+1)%TRANSITION_ENTRY_SIZE));
				accumulated_alphabet_overlap[i][((k+1)/TRANSITION_ENTRY_SIZE)] |=
						((boolean_alphabet[i][((k+1)/TRANSITION_ENTRY_SIZE)] & accumulated_boolean_alphabet[i - 1][((k+1)/TRANSITION_ENTRY_SIZE)] )
								& (((signal_bit_array_t)1) << ((k+1)%TRANSITION_ENTRY_SIZE)));
			}
		}
	}


#if DEBUG_COMPOSITION
	printf("[K] Composing %s, |ctx.alpha.|:%d alphabet_count: %d\n", composition->name, ctx->global_alphabet->count
			, alphabet_count);
	for(i = 0; i < automata_count; i++){
		printf("[A] Alphabets for i:%d, |local.alpha.|:%d\n", i, automata[i]->local_alphabet_count);
		printf("\t[B] Boolean alphabet:");
		for(j = 0; j < alphabet_count; j++){
			if(TEST_SIGNAL_ARRAY_BIT(boolean_alphabet[i],j))printf("%s ", ctx->global_alphabet->list[j].name);
		}
		printf("\n\t[C] ACcumulated boolean alphabet:");
		for(j = 0; j < alphabet_count; j++){
			if(TEST_SIGNAL_ARRAY_BIT(accumulated_boolean_alphabet[i],j))printf("%s ", ctx->global_alphabet->list[j].name);
		}
		printf("\n\t[O] Accumulated boolean alphabet Overlap 1..i:");
		for(j = 0; j < alphabet_count; j++){
			if(TEST_SIGNAL_ARRAY_BIT(accumulated_alphabet_overlap[i],j))printf("%s ", ctx->global_alphabet->list[j].name);
		}
		printf("\n");
	}
#endif

	// create key tree,frontier bucket list, get current state transitions list harness
#if USE_COMPOSITE_HASH_TABLE
	uint32_t *sizes	= calloc(is_game?
			automata_count + ctx->global_fluents_count :automata_count, sizeof(uint32_t));
	for(i = 0; i < automata_count; i++){
		sizes[i]	= automata[i]->transitions_count;
	}
	if(is_game){
		for(i = automata_count; i < automata_count + ctx->global_fluents_count; i++)
			sizes[i]	= 2;
	}
	automaton_composite_hash_table* tree	= automaton_composite_hash_table_create(is_game?
			automata_count + ctx->global_fluents_count :automata_count, sizes);
	free(sizes);
#else
	automaton_composite_tree* tree	= automaton_composite_tree_create(is_game?
			automata_count + ctx->global_fluents_count :automata_count);
#endif

	automaton_bucket_list* bucket_list	= automaton_bucket_list_create(BUCKET_SIZE);
	automaton_bucket_list* processed_list	= automaton_bucket_list_create(BUCKET_SIZE);
	uint32_t max_degree_sum		= 0;	uint32_t max_signals_count	= 0;
	uint32_t max_out_degree		= 0;
	// compute max_signals_count
	for(i = 0; i < automata_count; i++){
		max_degree_sum	+= (automata[i]->max_out_degree) * (automata[i]->max_concurrent_degree);
		if(max_out_degree < automata[i]->max_out_degree)
			max_out_degree	= automata[i]->max_out_degree;
		if(automata[i]->local_alphabet_count > max_signals_count)
			max_signals_count	= automata[i]->local_alphabet_count;
	}
	max_degree_sum							*= max_degree_sum; //check this boundary
	/** compose transition relation **/
	uint32_t frontier_size			= LIST_INITIAL_SIZE;//TODO: check this number is fixed
	uint32_t frontier_count			= 1;
	uint32_t key_size			= is_game ? automata_count + ctx->global_fluents_count : automata_count;
	uint32_t* frontier			= malloc(sizeof(uint32_t) * key_size * frontier_size);
	uint32_t* composite_frontier		= malloc(sizeof(uint32_t) * frontier_size);
	uint32_t* current_state			= malloc(sizeof(uint32_t) * key_size);
	uint32_t* current_to_state			= malloc(sizeof(uint32_t) * key_size);
	uint32_t from_state;	uint32_t to_state;	bool overlaps;	bool shared_equals;
	//uint32_t* signals_union			= malloc(sizeof(uint32_t) * max_degree_sum);
	//uint32_t signals_union_count	= 0;


	//indexes structures
	uint32_t* idxs	= calloc(automata_count, sizeof(uint32_t));
	uint32_t* idxs_size	= calloc(automata_count, sizeof(uint32_t));
	bool** idxs_skip	= calloc(automata_count, sizeof(bool*));
	signal_bit_array_t** automata_accum	= calloc(automata_count, sizeof(signal_bit_array_t*));
	for(i = 0; i < automata_count; i++){
		idxs_skip[i] 		= calloc(automata[i]->max_out_degree, sizeof(bool));
		automata_accum[i]	= calloc(FIXED_SIGNALS_COUNT, sizeof(signal_bit_array_t));
	}
	signal_bit_array_t* label_accum	= calloc(FIXED_SIGNALS_COUNT, sizeof(signal_bit_array_t));


	int32_t last_char	= -1;	signal_t current_signal, current_other_signal;
	long int found_hits	= 0;	long int found_misses	= 0;	//set initial state
	for(i = 0; i < automata_count; i++){
		frontier[i]	= automata[i]->initial_states[0];
	}
	if(is_game){
		for(i = 0; i < ctx->global_fluents_count; i++)
			frontier[automata_count + i]	= ctx->global_fluents[i].initial_valuation? 1 : 0;
	}
#if USE_COMPOSITE_HASH_TABLE
	uint32_t composite_initial_state	= automaton_composite_hash_table_get_state(tree, frontier);
#else
	uint32_t composite_initial_state	= automaton_composite_tree_get_key(tree, frontier);
#endif

	composite_frontier[0]				= composite_initial_state;
	automaton_bucket_add_entry(bucket_list, composite_initial_state);
	automaton_automaton_add_initial_state(composition, composite_initial_state);
	uint32_t fluent_count				= ctx->global_fluents_count;
	uint32_t liveness_valuations_count	= ctx->liveness_valuations_count;
	uint32_t vstates_count				= ctx->state_valuations_count;
	uint32_t* vstates_partial_mask		= NULL;
	double_fluent_entry_size_t* current_vstates			= NULL;
	uint32_t vstates_automata_count		= 0;
	uint32_t* vstates_automata_indexes	= NULL;
	// set initial state valuation
	if(is_game){
		if(vstates_count > 0){
			composition->state_valuations_declared_size	= GET_FLUENTS_ARR_SIZE(vstates_count, 1);
			composition->state_valuations_declared	= calloc(composition->state_valuations_declared_size, FLUENT_ENTRY_SIZE >> 3);
			composition->state_valuations_size	= GET_FLUENTS_ARR_SIZE(vstates_count, composition->transitions_size);
			composition->state_valuations		= calloc(composition->state_valuations_size, FLUENT_ENTRY_SIZE >> 3);
			vstates_partial_mask				= calloc(composition->state_valuations_declared_size, FLUENT_ENTRY_SIZE >> 3);
			current_vstates						= calloc(composition->state_valuations_declared_size, sizeof(double_fluent_entry_size_t));
			composition->inverted_state_valuations	= calloc(vstates_count, sizeof(automaton_bucket_list*));
			for(i = 0; i < vstates_count; i++){
				composition->inverted_state_valuations[i]	= automaton_bucket_list_create(FLUENT_BUCKET_SIZE);
			}
		}
		//get declared valuations in composition as disjunction of components declarations
		for(i = 0; i < automata_count; i++)
			if(automata[i]->state_valuations_declared_size > 0)
				vstates_automata_count++;
		vstates_automata_indexes	= calloc(vstates_automata_count, sizeof(uint32_t));
		vstates_automata_count	= 0;
		for(i = 0; i < automata_count; i++){
			if(automata[i]->state_valuations_declared_size > 0){
				vstates_automata_indexes[vstates_automata_count]	= i;
				vstates_automata_count++;
				for(j = 0; j < composition->state_valuations_declared_size; j++){
					composition->state_valuations_declared[j] =
							composition->state_valuations_declared[j] | automata[i]->state_valuations_declared[j];
				}
			}
		}
		uint32_t fluent_index;
		for(i = 0; i < fluent_count; i++){
			fluent_index	= GET_STATE_FLUENT_INDEX(fluent_count, 0, i);
			if(ctx->global_fluents[i].initial_valuation)
				SET_FLUENT_BIT(composition->valuations, fluent_index);
			else
				CLEAR_FLUENT_BIT(composition->valuations, fluent_index);
		}
		for(i = 0; i < liveness_valuations_count; i++){
			fluent_index	= GET_STATE_FLUENT_INDEX(liveness_valuations_count, 0, i);
			//TODO:we are assuming fluents start as false
			CLEAR_FLUENT_BIT(composition->liveness_valuations, fluent_index);
		}
		for(i = 0; i < vstates_count; i++){
			fluent_index	= GET_STATE_FLUENT_INDEX(vstates_count, 0, i);
			//TODO:we are assuming fluents start as false
			CLEAR_FLUENT_BIT(composition->state_valuations, fluent_index);
		}
	}
#if DEBUG_COMPOSITION
	for(i = 0; i < automata_count; i++){
		printf("[C] Composition type for %d: %s\n", i
				, synch_type[i] == SYNCHRONOUS ? "SYNCH" :
						(synch_type[i] == CONCURRENT ? "CONCURRENT" : "ASYNCH"));
	}
#endif
	// POP from frontier
	while(frontier_count > 0){
		from_state	= composite_frontier[frontier_count - 1];
#if DEBUG_COMPOSITION
		printf("[p] pop frontier state %d:[", from_state);
#endif
		automaton_bucket_remove_entry(bucket_list, from_state);
		automaton_bucket_add_entry(processed_list, from_state);
		//check if frontier state was already decomposed
		if(composition->out_degree[from_state] > 0){
			frontier_count--;
#if DEBUG_COMPOSITION
			printf("X] already processed \n");
#endif
			continue;
		}
		//pop a state
		automaton_transition *current_transition	= NULL;
		for(i = 0; i < automata_count; i++){
			//initialize current state and indexes structs.
			current_state[i]= frontier[(frontier_count - 1) * key_size + i];
			idxs_size[i]	= automata[i]->out_degree[current_state[i]] + 1;
			idxs[i]			= 0;
			//initialize label skips and automata label accum struct
			for(j = 0; j < FIXED_SIGNALS_COUNT; j++)	automata_accum[i][j]	= (signal_bit_array_t)0x0;
			for(j = 0; j < automata[i]->out_degree[current_state[i]]; j++)idxs_skip[i][j]	= false;
			for(j = 0; j < automata[i]->out_degree[current_state[i]]; j++){
				current_transition = &(automata[i]->transitions[current_state[i]][j]);
				for(k = 0; k < FIXED_SIGNALS_COUNT; k++){
					automata_accum[i][k] |= current_transition->signals[k];
				}
			}
#if DEBUG_COMPOSITION
			printf("%s%d", i > 0 ? "," : "", current_state[i]);
#endif
		}
		//if automata is game take fluent valuation into consideration
		if(is_game){
			for(i = automata_count; i < key_size; i++){
				current_state[i]= frontier[(frontier_count - 1) * key_size + i];
			}
		}

		frontier_count--;
#if DEBUG_COMPOSITION
		printf("]\n");
#endif

#if DEBUG_COMPOSITION
		for(i = 0; i < automata_count; i++){
			printf("\t[A] Accum alphabet[%d]:",i);
			for(j = 0; j < ctx->global_alphabet->count; j++){
				if(TEST_SIGNAL_ARRAY_BIT(automata_accum[i], j))printf("%s ", ctx->global_alphabet->list[j].name);
			}
			printf("\n");
		}
#endif
		signal_bit_array_t current_mask;
		//set idxs_skip marks
		bool transition_skipped = false;
		for(i = 0; i < automata_count; i++){
			for(j = 0; j < automata[i]->out_degree[current_state[i]]; j++){
				current_transition = &(automata[i]->transitions[current_state[i]][j]);
				transition_skipped = false;
				for(k = 0; k < automata_count; k++ && !transition_skipped){
					if(k == i)continue;
					//if l_i_j intersects sigma_k != l_i_j intersects automata_accum_k skip l_i_j
					for(l = 0; l < FIXED_SIGNALS_COUNT; l++ && !transition_skipped){
						current_mask	= l == 0 ? ~((signal_bit_array_t)1) : ~((signal_bit_array_t)0);
						if((boolean_alphabet[k][l]
												& current_transition->signals[l] & current_mask)
								!= (automata_accum[k][l]
													  & current_transition->signals[l] & current_mask)){
							idxs_skip[i][j]		= true;
							transition_skipped	= true;
						}
					}
				}

			}
		}

		//get first non null combination
		automaton_automata_compose_increment_idxs(idxs, idxs_size, idxs_skip, automata_count);
#if DEBUG_COMPOSITION
		printf("\t[i] Initial Idxs: [");
		for(j = 0; j < automata_count; j++){
			printf("%d%s", idxs[j], j < (automata_count -1)? "," : "");
		}
		printf("]\n");
#endif
		for(i = 0; i < FIXED_SIGNALS_COUNT; i++)label_accum[i]	= (signal_bit_array_t)0x0;


		bool viable, not_considered, local_overlap, vstates_consistent;
		//explore possible combinations of transitions

		while(!automaton_automata_idxs_is_max(idxs, automata_count)){
#if DEBUG_COMPOSITION
			printf("\t[I] Idxs: [");
			for(j = 0; j < automata_count; j++){
				printf("%d%s", idxs[j], j < (automata_count -1)? "," : "");
			}
			printf("]\n");
			char buff[20];
			printf("\t[T] Components transitions:\n");
			for(j = 0; j < automata_count; j++){
				snprintf(buff, sizeof(buff) - strlen(buff) - 1, "\t\t i:[%d]", j);
				if(idxs[j] > 0)
					automaton_transition_print(&(automata[j]->transitions[current_state[j]][idxs[j] - 1]), ctx, buff,"\n",-1);
				else
					printf("%s [not considered]\n", buff);
			}


#endif
			//initialize label accumulator
			bool accum_set = false; bool is_input = false;
			bool accum_input	= false;
			bool label_not_empty	= false;
			bool accum_not_empty	= false;
			if(idxs[0] > 0){
				is_input = is_input || TRANSITION_IS_INPUT(&(automata[0]->transitions[current_state[0]][idxs[0] - 1]));
				accum_input = accum_input || TRANSITION_IS_INPUT(&(automata[0]->transitions[current_state[0]][idxs[0] - 1]));
				for(j = 0; j < FIXED_SIGNALS_COUNT; j++)
					label_accum[j]	= automata[0]->transitions[current_state[0]][idxs[0] - 1].signals[j];
				accum_set = true;
			}else{
				for(j = 0; j < FIXED_SIGNALS_COUNT; j++)label_accum[j] = (signal_bit_array_t)0x0;
			}
			viable = true;
			//check if current combination is viable
			for(j = 1; j < automata_count; j++){
				label_not_empty	= false;
				accum_not_empty	= false;
				if(idxs[j] == 0){//transition not being considered
					//is_input = is_input || TRANSITION_IS_INPUT(automata[j]->transitions[current_state[j]]);
					not_considered = true;
					for(k = 0; k < FIXED_SIGNALS_COUNT; k++){
						current_label[k]	= (signal_bit_array_t)0x0;
					}
				}else{
					is_input = is_input || TRANSITION_IS_INPUT(&(automata[j]->transitions[current_state[j]][idxs[j] - 1]));
					not_considered = false;
					for(k = 0; k < FIXED_SIGNALS_COUNT; k++)
						current_label[k]	= automata[j]->transitions[current_state[j]][idxs[j] - 1].signals[k];
				}

				for(k = 0; k < FIXED_SIGNALS_COUNT; k++){
					if(!not_considered)label_not_empty = label_not_empty || (automata[j]->transitions[current_state[j]][idxs[j] - 1].signals[k] != 0x0);
					accum_not_empty = accum_not_empty || (label_accum[k] != 0x0);
				}

				//check blocking
				int32_t overlapping = automaton_automata_check_overlap(label_accum, current_label
						, accumulated_alphabet_overlap[j], alphabet_count);
#if DEBUG_COMPOSITION
				printf("\t\t[LCA] Accum. label:[");
				for(k = 0; k < alphabet_count; k++){
					if(TEST_SIGNAL_ARRAY_BIT(label_accum,k))printf("%s ", ctx->global_alphabet->list[k].name);
				}
				printf("]");
				printf("Current label:[");
				for(k = 0; k < alphabet_count; k++){
					if(TEST_SIGNAL_ARRAY_BIT(current_label,k))printf("%s ", ctx->global_alphabet->list[k].name);
				}
				printf("]");
				printf("Accum. alphabet overlap:[");
				for(k = 0; k < alphabet_count; k++){
					if(TEST_SIGNAL_ARRAY_BIT(accumulated_alphabet_overlap[j],k))printf("%s ", ctx->global_alphabet->list[k].name);
				}
				printf("]");

				fflush(stdout);
#endif

				if((!not_considered && (!label_not_empty && (!accum_set /* || (accum_not_empty )*/)))
						|| (not_considered && accum_set && !accum_not_empty)
						){
#if DEBUG_COMPOSITION
				printf("[BLOCKS](empty label not synching, label %s considered%s)\n", not_considered ? "not" : "", accum_set? " accum set":"");
#endif
					viable	= false;

					break;
				}

				if(overlapping == 1){//partial synch not allowed
					viable = false;
#if DEBUG_COMPOSITION
					printf("[BLOCKS]\n");
#endif
					break;
				}

				if(synch_type[j] == ASYNCHRONOUS && overlapping == -1 && idxs[j] > 0 && accum_set){//no overlapping not allowed in asynch
					viable = false;
#if DEBUG_COMPOSITION
					printf("[NOT ASYNCH]\n");
#endif
					break;
				}
				if(synch_type[j] == SYNCHRONOUS && (not_considered || !accum_set)){
					viable = false;
#if DEBUG_COMPOSITION
					printf("[NOT SYNCH]\n");
#endif
					break;
				}
				if(!viable){
					break;
				}
#if DEBUG_COMPOSITION
				printf(">");
#endif
				if(idxs[j] > 0){
					for(k = 0; k < FIXED_SIGNALS_COUNT; k++)
						label_accum[k] |= automata[j]->transitions[current_state[j]][idxs[j] - 1].signals[k];
					accum_set = true;
				}
			}

			//add viable transitions
			if(viable){
#if DEBUG_COMPOSITION
				printf("[OK]\n");
#endif
				for(k = 0; k < automata_count; k++){
					if(idxs[k] == 0)current_to_state[k]= current_state[k];
					else current_to_state[k]	= automata[k]->transitions[current_state[k]][idxs[k] - 1].state_to;
				}
				bool satisfies_one_condition = false;
				bool satisfies_fluent_condition = true;
				//TODO: update fluent valuation on current_to_state
				if(is_game){
					//check starting conditions
					for(k = 0; k < ctx->global_fluents_count; k++){
						current_to_state[automata_count + k]	= current_state[automata_count + k];
						if(current_state[automata_count + k]){//check ending for those that are up
							satisfies_one_condition	= false;
							for(l = 0; l < ctx->global_fluents[k].ending_signals_count; l++){
								satisfies_fluent_condition = true;
								for(m = 0; m < ctx->global_fluents[k].ending_signals_element_count[l]; m++){
									if(!(TEST_SIGNAL_ARRAY_BIT(label_accum,ctx->global_fluents[k].ending_signals[l][m]))){
										satisfies_fluent_condition = false;
										break;
									}
								}
								if(satisfies_fluent_condition){
									satisfies_one_condition	= true;
									break;
								}
							}
							if(satisfies_one_condition){
								current_to_state[automata_count + k]	= false;
#if DEBUG_COMPOSITION
								printf("\t[F] %s: %d ->", ctx->global_fluents[k].name, from_state);
								for(m = 0; m < ctx->global_fluents[k].ending_signals_element_count[l]; m++){
									printf("%s ", ctx->global_alphabet->list[ctx->global_fluents[k].ending_signals[l][m]].name);
								}
								printf(", accum: ");
								for(m = 0; m < (FIXED_SIGNALS_COUNT * TRANSITION_ENTRY_SIZE) - 1; m++){
									if(TEST_SIGNAL_ARRAY_BIT(label_accum, m))printf("%s", ctx->global_alphabet->list[m].name);
								}

								printf("(cleared) \n");
#endif
							}
						}else{//check starting for those that are down
							satisfies_one_condition	= false;
							for(l = 0; l < ctx->global_fluents[k].starting_signals_count; l++){
								satisfies_fluent_condition	= true;
								for(m = 0; m < ctx->global_fluents[k].starting_signals_element_count[l]; m++){
									if(!(TEST_SIGNAL_ARRAY_BIT(label_accum,ctx->global_fluents[k].starting_signals[l][m]))){
										satisfies_fluent_condition = false;
										break;
									}
								}
								if(satisfies_fluent_condition){
									satisfies_one_condition	= true;
									break;
								}
							}
							if(satisfies_one_condition){
								current_to_state[automata_count + k]	= true;
#if DEBUG_COMPOSITION
								printf("\t[F] %s: %d ->", ctx->global_fluents[k].name, from_state);
								for(m = 0; m < ctx->global_fluents[k].starting_signals_element_count[l]; m++){
									printf("%s ", ctx->global_alphabet->list[ctx->global_fluents[k].starting_signals[l][m]].name);
								}
								printf(", accum: ");
								for(m = 0; m < (FIXED_SIGNALS_COUNT * TRANSITION_ENTRY_SIZE) - 1; m++){
									if(TEST_SIGNAL_ARRAY_BIT(label_accum, m))printf("%s", ctx->global_alphabet->list[m].name);
								}
								printf("(set)\n");
#endif
							}
						}
					}
				}
#if USE_COMPOSITE_HASH_TABLE
				uint32_t composite_to			= automaton_composite_hash_table_get_state(tree, current_to_state);
#else
				uint32_t composite_to			= automaton_composite_tree_get_key(tree, current_to_state);
#endif
				automaton_transition *current_transition	= automaton_transition_create(from_state, composite_to);
				for(k = 0; k < FIXED_SIGNALS_COUNT; k++){
					current_transition->signals[k] = label_accum[k];
				}
				//if(accum_input){
				if(is_input){
					TRANSITION_SET_INPUT(current_transition);
				}else{
					TRANSITION_CLEAR_INPUT(current_transition);
				}
				//check if transition can be added (if vstates are consistent between components)
				//state is consistent if:
				//   (a_i & mask_i & mask & a_{i-1}) == (a_i & mask_i & mask)
				//then we update:
				//   mask <- mask | mask_i
				//   a_{i-1} = a_i | a_{i-1}

				uint32_t current_vstates_index	= 0;
				uint32_t fluent_index;
				vstates_consistent	= true;
				if(vstates_automata_count > 1){
					//initialize accumulator to the first value
					//check consistent "from" state
					for(i = 0; i < composition->state_valuations_declared_size; i++){
						vstates_partial_mask[i]	= automata[vstates_automata_indexes[0]]->state_valuations_declared[i];
						current_vstates_index	= GET_FLUENTS_ARR_SIZE(ctx->state_valuations_count, current_state[vstates_automata_indexes[0]]) + i - 1;
						current_vstates[i]	= automata[vstates_automata_indexes[0]]->state_valuations[current_vstates_index] & (~((uint64_t)(0x0)) >> FLUENT_ENTRY_SIZE);
						if(current_vstates_index < (composition->state_valuations_declared_size - 1))
							current_vstates[i]	|= ((double_fluent_entry_size_t) automata[vstates_automata_indexes[0]]->state_valuations[current_vstates_index + 1]) << FLUENT_ENTRY_SIZE;
						fluent_index	= GET_STATE_FLUENT_INDEX(ctx->state_valuations_count, current_state[vstates_automata_indexes[0]], 0);
						current_vstates[i]	= (current_vstates[i] >> fluent_index) & (((double_fluent_entry_size_t)(~0x0)) >> (sizeof(double_fluent_entry_size_t) * 8 - ctx->state_valuations_count));
					}
					//perform the comparison detailed above and update mask, vstates when possible
					for(i = 1; i < vstates_automata_count && vstates_consistent; i++){
						for(j = 0; j < composition->state_valuations_declared_size; j++){
							current_vstates_index	= GET_FLUENTS_ARR_SIZE(ctx->state_valuations_count, current_state[vstates_automata_indexes[i]]) + j - 1;

							double_fluent_entry_size_t new_vstates	= automata[vstates_automata_indexes[vstates_automata_indexes[i]]]->state_valuations[current_vstates_index] & (~((uint64_t)(0x0)) >> FLUENT_ENTRY_SIZE);
							if(current_vstates_index < (composition->state_valuations_declared_size - 1))
								new_vstates	|= ((double_fluent_entry_size_t)automata[vstates_automata_indexes[i]]->state_valuations[current_vstates_index + 1]) << FLUENT_ENTRY_SIZE;
							fluent_index	= GET_STATE_FLUENT_INDEX(ctx->state_valuations_count, current_state[vstates_automata_indexes[i]], 0) % FLUENT_ENTRY_SIZE;

							new_vstates	= (new_vstates >> fluent_index) & (((double_fluent_entry_size_t)(~0x0)) >> (sizeof(double_fluent_entry_size_t) * 8 - ctx->state_valuations_count));

							if((vstates_partial_mask[j] & automata[vstates_automata_indexes[i]]->state_valuations_declared[j] & new_vstates)
									!= (current_vstates[j])){
								vstates_consistent	= false;
								break;
							}
							vstates_partial_mask[j]	|= automata[vstates_automata_indexes[i]]->state_valuations_declared[j];
							current_vstates[j] 		|= new_vstates;
						}
					}
					if(vstates_consistent){

						//check consistent "to" state
						for(i = 0; i < composition->state_valuations_declared_size; i++){
							vstates_partial_mask[i]	= automata[vstates_automata_indexes[0]]->state_valuations_declared[i];
							current_vstates_index	= GET_FLUENTS_ARR_SIZE(ctx->state_valuations_count, current_to_state[vstates_automata_indexes[0]]) + i - 1;
							current_vstates[i]	= automata[vstates_automata_indexes[0]]->state_valuations[current_vstates_index] & (~((uint64_t)(0x0)) >> FLUENT_ENTRY_SIZE);
							if(current_vstates_index < (composition->state_valuations_declared_size - 1))
								current_vstates[i]	|= ((double_fluent_entry_size_t) automata[vstates_automata_indexes[0]]->state_valuations[current_vstates_index + 1]) << FLUENT_ENTRY_SIZE;
							fluent_index	= GET_STATE_FLUENT_INDEX(ctx->state_valuations_count, current_to_state[vstates_automata_indexes[0]], 0);
							current_vstates[i]	= (current_vstates[i] >> fluent_index) & (((double_fluent_entry_size_t)(~0x0)) >> (sizeof(double_fluent_entry_size_t) * 8 - ctx->state_valuations_count));
						}
						//perform the comparison detailed above and update mask, vstates when possible
						for(i = 1; i < vstates_automata_count && vstates_consistent; i++){
							for(j = 0; j < composition->state_valuations_declared_size; j++){
								current_vstates_index	= GET_FLUENTS_ARR_SIZE(ctx->state_valuations_count, current_to_state[vstates_automata_indexes[i]]) + j - 1;

								double_fluent_entry_size_t new_vstates	= automata[vstates_automata_indexes[vstates_automata_indexes[i]]]->state_valuations[current_vstates_index] & (~((uint64_t)(0x0)) >> FLUENT_ENTRY_SIZE);
								if(current_vstates_index < (composition->state_valuations_declared_size - 1))
									new_vstates	|= ((double_fluent_entry_size_t)automata[vstates_automata_indexes[i]]->state_valuations[current_vstates_index + 1]) << FLUENT_ENTRY_SIZE;
								fluent_index	= GET_STATE_FLUENT_INDEX(ctx->state_valuations_count, current_to_state[vstates_automata_indexes[i]], 0) % FLUENT_ENTRY_SIZE;

								new_vstates	= (new_vstates >> fluent_index) & (((double_fluent_entry_size_t)(~0x0)) >> (sizeof(double_fluent_entry_size_t) * 8 - ctx->state_valuations_count));

								if((vstates_partial_mask[j] & automata[vstates_automata_indexes[i]]->state_valuations_declared[j] & new_vstates)
										!= (current_vstates[j])){
									vstates_consistent	= false;
									break;
								}
								vstates_partial_mask[j]	|= automata[vstates_automata_indexes[i]]->state_valuations_declared[j];
								current_vstates[j] 		|= new_vstates;
							}
						}
					}
				}else{
					for(i = 0; i < composition->state_valuations_declared_size; i++)
						current_vstates[i]	= 0x0;
				}
 				if(vstates_consistent){

					automaton_automaton_add_transition(composition, current_transition);
	#if DEBUG_COMPOSITION
					printf("\t[+] Adding trans: %d {", current_transition->state_from);
					for(k = 0; k < (TRANSITION_ENTRY_SIZE * FIXED_SIGNALS_COUNT) - 1; k++){
						if(TEST_TRANSITION_BIT(current_transition, k)){
							printf("%s ", automata[0]->context->global_alphabet->list[k].name);
						}
					}
					printf("}->[");
					for(l = 0; l < automata_count; l++){
						printf("%s%d", l > 0 ? "," : "", current_to_state[l]);
					}
					printf("]:%d\n", current_transition->state_to);
	#endif
					// set state valuation
					if(is_game){

						uint32_t fluent_automata_index;
						int32_t state_position	= -1;
						bool state_found;
						for(i = 0; i < fluent_count; i++){
							fluent_index	= GET_STATE_FLUENT_INDEX(fluent_count, composite_to, i);
							//set new valuation
							fluent_automata_index	= automata_count + i;
							if(current_to_state[fluent_automata_index] == 1){
								//Check if it should be added to the inverted valuation list
								state_found		= automaton_bucket_has_entry(composition->inverted_valuations[i], composite_to);
								if(!state_found){
									automaton_bucket_add_entry(composition->inverted_valuations[i], composite_to);
								}
								SET_FLUENT_BIT(composition->valuations, fluent_index);
							}else{
								CLEAR_FLUENT_BIT(composition->valuations, fluent_index);
							}
						}
						//set liveness valuations
						for(i = 0; i < liveness_valuations_count; i++){
							bool current_valuation = true;
							for(j = 0; j < automata_count; j++){
								if(automata[j]->built_from_ltl){
									fluent_index	= GET_STATE_FLUENT_INDEX(liveness_valuations_count, current_to_state[j], i);
									current_valuation = current_valuation && TEST_FLUENT_BIT(automata[j]->liveness_valuations, fluent_index);
								}
								if(!current_valuation)break;
							}
							fluent_index	= GET_STATE_FLUENT_INDEX(liveness_valuations_count, composite_to, i);
							if(current_valuation){
								//Check if it should be added to the inverted valuation list
								state_found		= automaton_bucket_has_entry(composition->liveness_inverted_valuations[i], composite_to);
								//
								if(!state_found){
									automaton_bucket_add_entry(composition->liveness_inverted_valuations[i], composite_to);
								}
								SET_FLUENT_BIT(composition->liveness_valuations, fluent_index);
							}else{
								CLEAR_FLUENT_BIT(composition->liveness_valuations, fluent_index);
							}
						}
						//set vstates valuations
						bool has_state_valuation	= false;
						for(j = 0; j < composition->state_valuations_declared_size; j++){
							//uint32_t k;
							for(k = 0; k < vstates_count; k++){
								fluent_index	= GET_STATE_FLUENT_INDEX(vstates_count, composite_to, k);
								if(current_vstates[j] & (1 << k)){
									SET_FLUENT_BIT(composition->state_valuations, fluent_index);
									has_state_valuation	= true;
								}else
									CLEAR_FLUENT_BIT(composition->state_valuations,fluent_index);
							}
						}
						//update inverted valuations if needed
						if(has_state_valuation){
							for(j = 0; j < vstates_count; j++){
								fluent_index	= GET_STATE_FLUENT_INDEX(vstates_count, composite_to, j);
								if(TEST_FLUENT_BIT(composition->state_valuations, fluent_index)){
									automaton_bucket_add_entry(composition->inverted_state_valuations[j], composite_to);
								}
							}
						}
					}
					transitions_added_count++;
#if VERBOSE
					if(transitions_added_count % 10000 == 0){
						printf("\t[!] Added %d transitions\n", transitions_added_count);
						fflush(stdout);
					}
					if((found_hits + found_misses) % 5000 == 0){
						printf("\t[!] %d hits, %d misses\n", found_hits, found_misses);
						fflush(stdout);
					}
#endif
				}
				automaton_transition_destroy(current_transition, true);
				// expand frontier
				bool found = automaton_bucket_has_entry(bucket_list, composite_to) || automaton_bucket_has_entry(processed_list, composite_to);
				if(found){found_hits++;}else{found_misses++;}
				if(!found){
					composite_frontier[frontier_count]	= composite_to;
					automaton_bucket_add_entry(bucket_list, composite_to);
					for(n = 0; n < key_size; n++){
						frontier[frontier_count * key_size + n]	= current_to_state[n];
					}
					frontier_count++;
					if(frontier_count >= (frontier_size - 1)){
						uint32_t new_size	= frontier_size * LIST_INCREASE_FACTOR;
						uint32_t i, key_size;
						if(is_game){
							key_size	= automata_count + ctx->global_fluents_count;
						}else{
							key_size	= automata_count;
						}
						uint32_t* new_frontier				= malloc(sizeof(uint32_t) * new_size * key_size);
						uint32_t* new_composite_frontier	= malloc(sizeof(uint32_t) * new_size);
						for(i = 0; i < frontier_size * key_size; i++){
							new_frontier[i]					= frontier[i];
						}
						for(i = 0; i < frontier_size; i++){
							new_composite_frontier[i]			= composite_frontier[i];
						}
						free(frontier);
						free(composite_frontier);
						frontier_size						= new_size;
						frontier							= new_frontier;
						composite_frontier					= new_composite_frontier;
					}
				}
			}
			automaton_automata_compose_increment_idxs(idxs, idxs_size, idxs_skip, automata_count);
		}
	}
	if(vstates_partial_mask != NULL)free(vstates_partial_mask);
	if(vstates_automata_indexes != NULL)free(vstates_automata_indexes);
	if(current_vstates != NULL)free(current_vstates);
#if VERBOSE
	printf("TOTAL Composition has [%09d] states and [%09d] transitions run for [%08f] KEY ACCESS.: [Misses:%li,hits:%li]\n", tree->max_value, composition->transitions_composite_count, (double)(clock() - begin) / CLOCKS_PER_SEC, found_misses, found_hits);
#endif
	//CLEANUP
	for(i = 0; i < automata_count; i++){
		free(boolean_alphabet[i]);
		free(accumulated_alphabet_overlap[i]);
		free(accumulated_boolean_alphabet[i]);
		free(idxs_skip[i]);
		free(automata_accum[i]);
	}
	free(boolean_alphabet);free(accumulated_alphabet_overlap); free(accumulated_boolean_alphabet);
	free(current_label);free(frontier);free(composite_frontier);free(current_state);free(current_to_state);
	free(idxs);free(idxs_size);free(label_accum);free(idxs_skip);free(automata_accum);
	free(alphabet);
	//remove controllable transitions from mixed states
	if(no_mixed_states){
		for(i = 0; i < composition->transitions_count; i++){
			if(!(composition->is_controllable[i])){
				for(j = composition->out_degree[i] - 1; ((int32_t)j) >= 0; j--){
					if(!TRANSITION_IS_INPUT(&(composition->transitions[i][j]))){
						automaton_automaton_remove_transition(composition, &(composition->transitions[i][j]));
					}
				}
			}
		}
		automaton_automaton_remove_unreachable_states(composition);
		automaton_automaton_update_valuation(composition);
	}


#if USE_COMPOSITE_HASH_TABLE
	automaton_composite_hash_table_destroy(tree);
#else
	automaton_composite_tree_destroy(tree);
#endif
	automaton_bucket_destroy(bucket_list);
	automaton_bucket_destroy(processed_list);

	return composition;
}

/** COMPOSITE TREE **/
void automaton_composite_tree_entry_print(automaton_composite_tree_entry* entry, bool comes_from_next, uint32_t* tabs){
	uint32_t i;
	if(comes_from_next){
		for(i = 0; i < *tabs; i++)
			printf("\t");
	}
	if(entry->next == NULL && entry->succ == NULL){
		printf("->%d", entry->value);
	}else{
		printf("[%d]\t", entry->value);
	}
	if(entry->succ != NULL){
		(*tabs)++;
		automaton_composite_tree_entry_print(entry->succ, false, tabs);
		(*tabs)--;
	}
	if(entry->next != NULL){
		printf("\n");
		automaton_composite_tree_entry_print(entry->next, true, tabs);
	}
}

void automaton_composite_tree_print(automaton_composite_tree* tree){
	printf("Composite Tree.\n");
	if(tree->first_entry != NULL){
		uint32_t tabs	= 0;
		automaton_composite_tree_entry_print(tree->first_entry, false, &tabs);
	}
	printf("\n");
}

automaton_composite_tree* automaton_composite_tree_create(uint32_t key_length){
	automaton_composite_tree* tree	= malloc(sizeof(automaton_composite_tree));
	tree->key_length	= key_length;
	tree->max_value		= 0;
	tree->first_entry	= NULL;
	tree->entries_size_count		= 1;
	tree->entries_composite_count	= 0;
	tree->entries_size				= malloc(sizeof(uint32_t) * tree->entries_size_count);
	tree->entries_count				= malloc(sizeof(uint32_t) * tree->entries_size_count);
	tree->entries_size[0]			= LIST_INITIAL_SIZE * LIST_INITIAL_SIZE * 32;
	tree->entries_count[0]			= 0;
	tree->entries_composite_size	= tree->entries_size[0];
	tree->entries_pool				= malloc(sizeof(automaton_composite_tree_entry*) * tree->entries_size_count);
	tree->entries_pool[0]			= malloc(sizeof(automaton_composite_tree_entry) * tree->entries_size[0]);
	return tree;
}

automaton_composite_tree_entry* automaton_composite_tree_entry_get_from_pool(automaton_composite_tree* tree){
	uint32_t current_pool					= tree->entries_size_count - 1;
	uint32_t i;
	if(tree->entries_composite_count >= tree->entries_composite_size){
		uint32_t new_size					= tree->entries_size[current_pool] * LIST_INCREASE_FACTOR;
		tree->entries_composite_size		+= new_size;
		tree->entries_size_count++;
		current_pool						= tree->entries_size_count - 1;

		uint32_t* ptr	= realloc(tree->entries_size, sizeof(uint32_t) * tree->entries_size_count);
		if(ptr == NULL){printf("Could not allocate memory[automaton_composite_tree_entry_get_from_pool:1]\n");exit(-1);	}
		else tree->entries_size	= ptr;
		ptr	= realloc(tree->entries_count, sizeof(uint32_t) * tree->entries_size_count);
		if(ptr == NULL){printf("Could not allocate memory[automaton_composite_tree_entry_get_from_pool:2]\n");exit(-1);	}
		else tree->entries_count	= ptr;
		automaton_composite_tree_entry** ptr2	= realloc(tree->entries_pool, sizeof(automaton_composite_tree_entry*) * tree->entries_size_count);
		if(ptr2 == NULL){printf("Could not allocate memory[automaton_composite_tree_entry_get_from_pool:3]\n");exit(-1);	}
		else tree->entries_pool	= ptr2;
		tree->entries_size[current_pool]	= new_size;
		tree->entries_count[current_pool]	= 0;
		tree->entries_pool[current_pool]	= malloc(sizeof(automaton_composite_tree_entry) * new_size);
	}

	automaton_composite_tree_entry* entry	= &(tree->entries_pool[current_pool][tree->entries_count[current_pool]++]);
	tree->entries_composite_count++;
	return entry;
}

uint32_t automaton_composite_tree_get_key(automaton_composite_tree* tree, uint32_t* composite_key){
	uint32_t i,j;
	automaton_composite_tree_entry* current_entry 	= NULL;
	automaton_composite_tree_entry* last_entry		= NULL;
	automaton_composite_tree_entry* terminal_entry	= NULL;
	//if first entry then add whole strip and return first value
#if DEBUG_COMPOSITE_TREE
	printf("(");
	for(i = 0; i < tree->key_length; i++){
		printf("%d%s", composite_key[i], (i == (tree->key_length - 1)) ? "": ",");
	}
	printf("):");
	fflush(stdout);
#endif
	if(tree->first_entry == NULL){
		for(i = 0; i < tree->key_length; i++){
			current_entry			= automaton_composite_tree_entry_get_from_pool(tree);
			current_entry->value	= composite_key[i];
			current_entry->next		= NULL;
			current_entry->succ 	= NULL;
			if(i == 0){
				tree->first_entry	= current_entry;
			}
			if(last_entry != NULL){
				last_entry->succ = current_entry;
			}
			last_entry = current_entry;
			if(i == (tree->key_length -1)){
				terminal_entry			= automaton_composite_tree_entry_get_from_pool(tree);
				terminal_entry->value	= tree->max_value++;
				terminal_entry->succ	= NULL;
				terminal_entry->next	= NULL;
				last_entry->succ		= terminal_entry;
#if DEBUG_COMPOSITE_TREE
				printf("FIRST\n");
#endif
				return terminal_entry->value;
			}
		}
	}else{
		current_entry	= tree->first_entry;
		bool found;
		for(i = 0; i < tree->key_length; i++){
			found	= false;
			while(!found && current_entry != NULL){
				if(current_entry->value == composite_key[i]){
#if DEBUG_COMPOSITE_TREE
					printf("[%d]", composite_key[i]);
#endif
					if(i == (tree->key_length -1)){
#if DEBUG_COMPOSITE_TREE
						printf("<%d>\n",current_entry->succ->value);
#endif
						return current_entry->succ->value;
					}
					current_entry	= current_entry->succ;
					found	= true;
				}else{
					last_entry		= current_entry;
					current_entry	= current_entry->next;
				}
#if DEBUG_COMPOSITE_TREE
				fflush(stdout);
#endif
			}
			//if not found add whole strip from this point on
			if(!found){
#if DEBUG_COMPOSITE_TREE
				printf("X.");
#endif
				break;
			}
		}
		//last_entry	= NULL;
		for(j = i; j < tree->key_length; j++){
			current_entry			= automaton_composite_tree_entry_get_from_pool(tree);
			current_entry->value	= composite_key[j];
#if DEBUG_COMPOSITE_TREE
			printf("%d.", composite_key[j]);
#endif
			current_entry->next	= NULL;
			current_entry->succ = NULL;
			if(j == i && last_entry != NULL){
				last_entry->next = current_entry;
			}else if(last_entry != NULL){
				last_entry->succ = current_entry;
			}
			last_entry = current_entry;
			if(j == (tree->key_length -1)){
				terminal_entry			= automaton_composite_tree_entry_get_from_pool(tree);
				terminal_entry->value	= tree->max_value++;
				terminal_entry->succ	= NULL;
				terminal_entry->next	= NULL;
				last_entry->succ		= terminal_entry;
#if DEBUG_COMPOSITE_TREE
				printf("<%d>\n",terminal_entry->value);
#endif
				return terminal_entry->value;
			}
		}
	}
	return 0;
}
void automaton_composite_tree_destroy_entry(automaton_composite_tree* tree, automaton_composite_tree_entry* tree_entry){
	if(tree_entry->succ != NULL){
		automaton_composite_tree_destroy_entry(tree, tree_entry->succ);
		tree_entry->succ = NULL;
	}
	if(tree_entry->next != NULL){
		automaton_composite_tree_destroy_entry(tree, tree_entry->next);
		tree_entry->next = NULL;
	}
	tree_entry->value = 0;
	//free(tree_entry);
}

void automaton_composite_tree_destroy(automaton_composite_tree* tree){
	if(tree->first_entry != NULL){
		automaton_composite_tree_destroy_entry(tree, tree->first_entry);
		tree->first_entry = NULL;
	}
	tree->max_value 	= 0;
	tree->key_length	= 0;
	uint32_t i;
	for(i = 0; i < tree->entries_size_count; i++)free(tree->entries_pool[i]);free(tree->entries_pool);
	free(tree->entries_size);free(tree->entries_count);
	tree->entries_size_count		= 0;
	tree->entries_composite_count	= 0;
	tree->entries_composite_size	= 0;
	tree->entries_pool				= NULL;
	tree->entries_size				= NULL;
	tree->entries_count				= NULL;
	free(tree);
}

