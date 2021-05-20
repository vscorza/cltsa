/*
 * automaton_gr1.c
 *
 *	GR(1) implementations related to the automaton struct
 *
 *  Created on: Apr 16, 2021
 *      Author: mariano
 */

#include "automaton.h"
#include "assert.h"

extern int __automaton_global_print_id;

uint32_t automaton_ranking_key_extractor(void* ranking){return ((automaton_ranking*)ranking)->state;}
automaton_ranking* automaton_ranking_create_infinity(uint32_t state, int32_t assumption_to_satisfy){
	automaton_ranking* ranking	= malloc(sizeof(automaton_ranking));
	ranking->state			= state;
	ranking->value			= RANKING_INFINITY;
	ranking->assumption_to_satisfy	= assumption_to_satisfy;
	return ranking;
}
automaton_ranking* automaton_ranking_create(uint32_t state, int32_t assumption_to_satisfy){
	automaton_ranking* ranking	= malloc(sizeof(automaton_ranking));
	ranking->state			= state;
	ranking->value			= 0;
	ranking->assumption_to_satisfy		= assumption_to_satisfy;
	return ranking;
}
void automaton_ranking_destroy(automaton_ranking*  ranking){
	ranking->state			= 0;
	ranking->assumption_to_satisfy	= 0;
	free(ranking);
}
uint32_t automaton_pending_state_extractor(void* pending_state){return ((automaton_pending_state*)pending_state)->state;}
uint32_t automaton_pending_state_ranking_extractor(void* pending_state){return ((automaton_pending_state*)pending_state)->value;}
automaton_pending_state* automaton_pending_state_create(uint32_t state, int32_t goal_to_satisfy, int32_t assumption_to_satisfy,
		int32_t value, uint32_t timestamp){
	automaton_pending_state* pending	= malloc(sizeof(automaton_pending_state));
	pending->state			= state;
	pending->goal_to_satisfy= goal_to_satisfy;
	pending->assumption_to_satisfy	= assumption_to_satisfy;
	pending->value			= value;
	pending->timestamp		= timestamp;
	return pending;
}

void automaton_pending_state_copy(void* target_input, void* source_input){
	automaton_pending_state* target	= (automaton_pending_state*)target_input;
	automaton_pending_state* source	= (automaton_pending_state*)source_input;
	target->state				= source->state;
	target->goal_to_satisfy		= source->goal_to_satisfy;
	target->assumption_to_satisfy	= source->assumption_to_satisfy;
	target->value				= source->value;
	target->timestamp			= source->timestamp;
}
uint32_t automaton_int_extractor(void* value){return *((uint32_t*)value);}
int32_t automaton_int_compare(void* left_int, void* right_int){
	uint32_t left	= *((uint32_t*)left_int);
	uint32_t right	= *((uint32_t*)right_int);

	if(left > right){
		return 1;
	}else if(left < right){
		return -1;
	}else{
		return 0;
	}
}
void automaton_pending_state_destroy(automaton_pending_state*  pending_state){
	free(pending_state);
}


#if DEBUG_SYNTHESIS
	uint32_t __count_updates_gr1	= 0;
	uint32_t __count_upgrades_gr1_tabs = 6;
#endif
bool _print_ranking;
/**
 * best_j(v)
 */
automaton_ranking* automaton_state_best_successor_ranking(automaton_automaton* game_automaton, uint32_t state, automaton_concrete_bucket_list** ranking
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t* guarantees_indexes){
	if(game_automaton->out_degree[state] == 0)return NULL;
	uint32_t fluent_count		= game_automaton->context->global_fluents_count;
	uint32_t fluent_index		= GET_STATE_FLUENT_INDEX(fluent_count, state, guarantees_indexes[current_guarantee]);
	bool satisfies_guarantee	= TEST_FLUENT_BIT(game_automaton->valuations, fluent_index);
	bool is_controllable		= game_automaton->is_controllable[state];
	int32_t min_value			= RANKING_INFINITY;
	automaton_ranking* min_ranking	= NULL;
	int32_t max_value			= RANKING_INFINITY;
	automaton_ranking* max_ranking	= NULL;

	automaton_ranking* current_value;
	uint32_t i, j, to_state;
	j = satisfies_guarantee? (current_guarantee + 1) % guarantee_count : current_guarantee;
	/**
	 * Transitions should be ordered by monitored signals
	 */
	//TODO: apply SIMD
	bool new_monitored	= false;
	for(i = 0; i < game_automaton->out_degree[state]; i++){
		if(i > 0)
			new_monitored = !(automaton_automaton_transition_monitored_eq(game_automaton,
					&(game_automaton->transitions[state][i - 1]),
					&(game_automaton->transitions[state][i])));
		if(new_monitored){
			if((min_value == RANKING_INFINITY) ||(max_ranking == NULL) || (automaton_ranking_gt(min_ranking, max_ranking))){
				max_ranking	= min_ranking;
				max_value	= min_value;
			}
			min_value	= RANKING_INFINITY;
			min_ranking	= NULL;
			new_monitored	= false;
#if DEBUG_SYNTHESIS
			//printf("[?!]\t%d\ts:%d[Min](_, _)\t[Max](%d, %d)", current_guarantee,state,
				//max_ranking->assumption_to_satisfy, max_ranking->value);
#endif
		}
		to_state	= game_automaton->transitions[state][i].state_to;
		//if(to_state == state)continue;
		current_value	= ((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking[j], to_state));
		//TODO:check this next line, is it right to retrieve NULL when initializing [Deadlock]?
		if(current_value == NULL)continue;
		if((min_ranking == NULL) || automaton_ranking_lt(current_value, min_ranking)){
			min_value = current_value->value;
			min_ranking = current_value;
		}
	}
	if((min_value == RANKING_INFINITY) || (max_ranking == NULL) || (automaton_ranking_gt(min_ranking, max_ranking))){
		max_ranking	= min_ranking;
		max_value	= min_value;
	}
#if DEBUG_SYNTHESIS
	if(max_ranking == NULL){
		printf("NO PROPER PREDECESSOR FOR RANKING\n");
	}
#endif

	return max_ranking;
}
//return query for: left > right
bool automaton_ranking_gt(automaton_ranking* left, automaton_ranking* right){
	if(left->value == RANKING_INFINITY)return right->value != RANKING_INFINITY;
	if(right->value == RANKING_INFINITY)return false;
	return (left->value > right->value)
			|| (left->value == right->value && left->assumption_to_satisfy > right->assumption_to_satisfy);
}
bool automaton_ranking_eq(automaton_ranking* left, automaton_ranking* right){
	if(left->value == RANKING_INFINITY) return right->value == RANKING_INFINITY;
	return (left->value == right->value) && (left->assumption_to_satisfy == right->assumption_to_satisfy);
}
bool automaton_ranking_lt(automaton_ranking* left, automaton_ranking* right){
	return automaton_ranking_gt(right, left);
}
bool automaton_state_is_stable(automaton_automaton* game_automaton, uint32_t state, automaton_concrete_bucket_list** ranking
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t assumptions_count
		, uint32_t* guarantees_indexes, uint32_t* assumptions_indexes, int32_t first_assumption_index){
	uint32_t i, j, to_state;
	automaton_ranking concrete_ranking;
	/**
	 * r_j(v)
	 */
	automaton_ranking*	current_ranking	= ((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking[current_guarantee], state));
	//TODO: check if this fix is hiding an error
	if(current_ranking == NULL){
		printf("[%d]!! had no ranking defined for %d, set as INF.\n", state, current_guarantee);
		concrete_ranking.state	= state; concrete_ranking.assumption_to_satisfy	= 0;
		concrete_ranking.value	= RANKING_INFINITY;
		automaton_concrete_bucket_add_entry(ranking[current_guarantee], &concrete_ranking);
		current_ranking			= ((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking[current_guarantee], state));
	}
	/**
	 * v in Q_j
	 */
	uint32_t fluent_count		= game_automaton->context->global_fluents_count;
	uint32_t fluent_index		= GET_STATE_FLUENT_INDEX(fluent_count, state, guarantees_indexes[current_guarantee]);//guarantees_indexes[current_guarantee]);
	bool satisfies_guarantee	= TEST_FLUENT_BIT(game_automaton->valuations, fluent_index);
	/**
	 * v in P_i
	 */
	int32_t current_value		=  current_ranking->value;
	//TODO: check this against definition
	if(current_value == RANKING_INFINITY)return true;
	/**
	 * r_j(w)
	 */
	j = current_guarantee;
	automaton_ranking* sr		= automaton_state_best_successor_ranking(game_automaton, state, ranking, j
			, guarantee_count, guarantees_indexes);

	fluent_index				= GET_STATE_FLUENT_INDEX(fluent_count, state, assumptions_indexes[current_ranking->assumption_to_satisfy]);
	bool satisfies_assumption	= assumptions_count == 0 || TEST_FLUENT_BIT(game_automaton->valuations, fluent_index);
	j = satisfies_guarantee? (current_guarantee + 1) % guarantee_count : current_guarantee;
	if(sr == NULL){
#if DEBUG_SYNTHESIS
	if(_print_ranking)
		printf("[S ]\t%d <%d:_, _> No best successor\n", state, j);
#endif
		return true;
	}
	/**
	 * is ranking good?
	 */
	bool is_stable = false;
 	if(satisfies_guarantee){
		if(sr->value == RANKING_INFINITY)
			is_stable = current_value == RANKING_INFINITY;
		else
			is_stable = (current_ranking->value == INITIAL_RANKING_VALUE && current_ranking->assumption_to_satisfy == first_assumption_index);
	}else{
		if(satisfies_assumption)
			is_stable = automaton_ranking_gt(current_ranking, sr);
		else
			is_stable = automaton_ranking_gt(current_ranking, sr) || automaton_ranking_eq(current_ranking, sr);
	}
#if DEBUG_SYNTHESIS
 	if(_print_ranking)
 		printf("%s%s", is_stable? "\t[stable]\t" : "", (is_stable && (__count_updates_gr1++ % __count_upgrades_gr1_tabs) == 0) ? "\n": "");
#endif
	return is_stable;
}

void automaton_add_unstable_predecessors(automaton_automaton* game_automaton, automaton_pending_state_max_heap* pending_list, automaton_concrete_bucket_list* key_list
		, uint32_t state, automaton_concrete_bucket_list** ranking
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t assumptions_count
		, uint32_t* guarantees_indexes, uint32_t* assumptions_indexes, int32_t first_assumption_index
		, uint32_t timestamp){
	uint32_t i;
	automaton_transition* current_transition;
	automaton_pending_state current_pending_state;
	automaton_pending_state *existing_pending_state;
	automaton_ranking* current_ranking;

	for(i = 0; i < game_automaton->in_degree[state]; i++){
		current_transition	= &(game_automaton->inverted_transitions[state][i]);
		//TODO:check if we should add another structure to answer inclusion queries (heap of state_from synched with pending_state)
		//if(!automaton_state_is_stable(game_automaton, current_transition->state_from, ranking, current_guarantee, guarantee_count
		//		, assumptions_count, guarantees_indexes, assumptions_indexes, first_assumption_index) || current_transition->is_input){
#if DEBUG_SYNTHESIS
	//if(_print_ranking)
		//printf("[->] %d <%d:_, _> Pushing into pending pred. for state %d\n", current_transition->state_from, current_guarantee, state);
#endif
			//check if entry exists in the pending structure, if so update previous value
			current_ranking							= ((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking[current_guarantee], current_transition->state_from));
			if(current_ranking == NULL)
				continue;
			if(automaton_concrete_bucket_has_key(key_list, current_transition->state_from)){
				existing_pending_state					= automaton_concrete_bucket_get_entry(key_list, current_transition->state_from);
				existing_pending_state->value			= current_ranking->value;
				existing_pending_state->assumption_to_satisfy			= current_ranking->assumption_to_satisfy;
				existing_pending_state->timestamp		= timestamp;
				existing_pending_state->goal_to_satisfy	= current_guarantee;
			}else{
				current_pending_state.goal_to_satisfy	= current_guarantee;
				current_pending_state.state = current_transition->state_from;
				current_pending_state.assumption_to_satisfy	= current_ranking->assumption_to_satisfy;
				current_pending_state.value				= current_ranking->value;
				current_pending_state.timestamp			= timestamp;
				void* new_entry							= automaton_pending_state_max_heap_add_entry(pending_list, &current_pending_state);
				automaton_concrete_bucket_add_entry(key_list, new_entry);
			}
		//}
	}
}
void automaton_ranking_increment(automaton_automaton* game_automaton, automaton_concrete_bucket_list** ranking, automaton_ranking* current_ranking, uint32_t ref_state, uint32_t* max_delta
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t assumptions_count
		, uint32_t* guarantees_indexes, uint32_t* assumptions_indexes, uint32_t first_assumption_index, automaton_ranking* target_ranking){
	automaton_automata_context* ctx		= game_automaton->context;
	uint32_t i;
	bool satisfies_goal, satisfies_assumption;
	//set default values
	target_ranking->state					= ref_state;
	target_ranking->assumption_to_satisfy	= current_ranking->assumption_to_satisfy;
	target_ranking->value					= current_ranking->value;
	//infinity is preserved
	if(current_ranking->value == RANKING_INFINITY){
		return;
	}

	uint32_t fluent_count				= ctx->global_fluents_count;
	uint32_t fluent_index				= GET_STATE_FLUENT_INDEX(fluent_count, ref_state, guarantees_indexes[current_guarantee]);
	satisfies_goal						= TEST_FLUENT_BIT(game_automaton->valuations, fluent_index);
	//goal satisfaction resets ranking
	if(satisfies_goal){
		target_ranking->value					= INITIAL_RANKING_VALUE;
		target_ranking->assumption_to_satisfy	= first_assumption_index;
		return;
	}
	//assumption being satisfied increments ranking
	fluent_index				= GET_STATE_FLUENT_INDEX(fluent_count, ref_state, assumptions_indexes[current_ranking->assumption_to_satisfy]);
	satisfies_assumption		= assumptions_count == 0 || TEST_FLUENT_BIT(game_automaton->valuations, fluent_index);
	if(satisfies_assumption){
		if(current_ranking->assumption_to_satisfy < (int32_t)(assumptions_count - 1)){
			target_ranking->assumption_to_satisfy++;
		}else if(current_ranking->value < (int32_t)(max_delta[current_guarantee])){
			target_ranking->value++;
			target_ranking->assumption_to_satisfy = first_assumption_index;
		}else{
			target_ranking->value 		= RANKING_INFINITY;
		}
	}
}


void automaton_ranking_update(automaton_automaton* game_automaton, automaton_concrete_bucket_list** ranking, automaton_ranking* current_ranking, uint32_t* max_delta
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t assumptions_count
		, uint32_t* guarantees_indexes, uint32_t* assumptions_indexes, uint32_t first_assumption_index){
	automaton_ranking incr_ranking;
	/**
	 * get best_j(v)
	 */
	automaton_ranking* best_ranking		= automaton_state_best_successor_ranking(game_automaton, current_ranking->state, ranking, current_guarantee, guarantee_count, guarantees_indexes);
#if DEBUG_SYNTHESIS
	bool satisfies_guarantee	= false;
	if(_print_ranking){
		uint32_t fluent_index		= GET_STATE_FLUENT_INDEX(game_automaton->context->global_fluents_count,
				current_ranking->state, guarantees_indexes[current_guarantee]);
		satisfies_guarantee	= TEST_FLUENT_BIT(game_automaton->valuations, fluent_index);
	}
#endif
	/**
	 * get incr_j_v(best_j(v))
	 */
	automaton_ranking_increment(game_automaton, ranking, best_ranking, current_ranking->state, max_delta, current_guarantee, guarantee_count, assumptions_count
			, guarantees_indexes, assumptions_indexes, first_assumption_index, &incr_ranking);

	/**
	 * update to max between current ranking and the next
	 */
#if DEBUG_SYNTHESIS
	uint32_t old_value	= current_ranking->value;
	uint32_t old_ass	= current_ranking->assumption_to_satisfy;
#endif
	if(automaton_ranking_gt(&incr_ranking, current_ranking)){
		current_ranking->value			= incr_ranking.value;
		current_ranking->assumption_to_satisfy	= incr_ranking.assumption_to_satisfy;
	}
#if DEBUG_SYNTHESIS
	printf("\t[U]<%d,%d,(%d,%d)->(%d,%d)> <%d,(%d,%d)>%s", incr_ranking.state, current_guarantee, old_value, old_ass,
			incr_ranking.value, incr_ranking.assumption_to_satisfy, best_ranking->state, best_ranking->value, best_ranking->assumption_to_satisfy,
			(__count_updates_gr1++ % __count_upgrades_gr1_tabs) == 0? "\n":"");
#endif
}

int __ranking_link_id = -1;

uint32_t* automaton_compute_infinity(automaton_automaton* game_automaton, uint32_t assumptions_count,
		uint32_t guarantees_count, uint32_t* assumptions_indexes, uint32_t* guarantees_indexes){
	//get max_l(|fi_l - gamma_g|)
	uint32_t i, j, l, k;
	automaton_bucket_list *bigger_bucket_list, *smaller_bucket_list;
	uint32_t* max_delta	= malloc(sizeof(uint32_t) * guarantees_count);
	uint32_t current_delta;
#if DEBUG_SYNTHESIS
	printf("<a id='synthesis'>Starting synthesis</a>[<a href='#synthesis'>Synthesis start</a>|<a href='#synthesis_completed'>Synthesis end</a>|<a href='#game'>Game</a>|<a href='#strategy'>Strategy</a>]\n");
	if(_print_ranking)
		printf("!!COMPUTING INFINITY!!\n");
#endif
	for(l = 0; l < guarantees_count; l++){
		max_delta[l]	= 0;
		for(k = 0; k < assumptions_count; k++){
			if(game_automaton->inverted_valuations[assumptions_indexes[k]]->composite_count >
				game_automaton->inverted_valuations[guarantees_indexes[l]]->composite_count){
				bigger_bucket_list	= game_automaton->inverted_valuations[assumptions_indexes[k]];
				smaller_bucket_list	= game_automaton->inverted_valuations[guarantees_indexes[l]];
			}else{
				smaller_bucket_list	= game_automaton->inverted_valuations[assumptions_indexes[k]];
				bigger_bucket_list	= game_automaton->inverted_valuations[guarantees_indexes[l]];
			}
			current_delta	= 0;
			for(i = 0; i < bigger_bucket_list->count; i++)
				for(j = 0; j < bigger_bucket_list->bucket_count[i]; j++)
					if(!automaton_bucket_has_entry(smaller_bucket_list, bigger_bucket_list->buckets[i][j]))
						current_delta++;
			max_delta[l]	= current_delta > max_delta[l] ? current_delta : max_delta[l];
#if DEBUG_SYNTHESIS
	if(_print_ranking)
		printf("Infinity_%d:%d", l, max_delta[l]);
#endif
		}
	}
#if DEBUG_SYNTHESIS
	if(_print_ranking){
		printf("\n");
		automaton_automaton_print(game_automaton, false, false, true, "", "\n");
	}
#endif
	return max_delta;
}

void automaton_get_gr1_liveness_indexes(automaton_automaton* game_automaton, char** assumptions, uint32_t assumptions_count,
		 char** guarantees, uint32_t guarantees_count, uint32_t** assumptions_indexes, uint32_t** guarantees_indexes){
	//these are the indexes from the global fluents list, should not be used in ranking_list
	*assumptions_indexes				= malloc(sizeof(uint32_t) * assumptions_count);
	*guarantees_indexes				= malloc(sizeof(uint32_t) * guarantees_count);
	int32_t first_assumption_index				= 0;
	uint32_t i,j;
	automaton_automata_context* ctx	= game_automaton->context;
	//get assumptions and guarantees indexes
	for(i = 0; i < assumptions_count; i++){
		for(j = 0; j < ctx->global_fluents_count; j++){
			if(strcmp(assumptions[i], ctx->global_fluents[j].name) == 0){
				(*assumptions_indexes)[i]	= j;
				/*if (i == 0)
					first_assumption_index	= (int32_t)j;*/
				break;
			}
		}
	}
	for(i = 0; i < guarantees_count; i++){
		for(j = 0; j < ctx->global_fluents_count; j++){
			if(strcmp(guarantees[i], ctx->global_fluents[j].name) == 0){
				(*guarantees_indexes)[i]	= j;
				break;
			}
		}
	}
}

automaton_automaton* automaton_get_gr1_strategy(automaton_automaton* game_automaton, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count, bool print_ranking){
	automaton_automaton_monitored_order_transitions(game_automaton);
	clock_t begin = clock();
	__ranking_link_id++;
	_print_ranking	= true;//print_ranking;
	uint32_t pending_processed	= 0;
	//TODO: preallocate pending states and rankings
	automaton_automata_context* ctx				= game_automaton->context;
	uint32_t i, j ,k, l, m;
	automaton_concrete_bucket_list** ranking_list	= malloc(sizeof(automaton_concrete_bucket_list*) * guarantees_count);
	automaton_pending_state_max_heap* pending_list	= automaton_pending_state_max_heap_create();
	automaton_concrete_bucket_list** key_lists		= calloc(guarantees_count, sizeof(automaton_concrete_bucket_list*));
	for(i = 0; i < guarantees_count; i++)
				key_lists[i]					= automaton_concrete_bucket_list_create(RANKING_BUCKET_SIZE, automaton_pending_state_extractor, sizeof(automaton_pending_state));
	uint32_t current_state;
	uint32_t fluent_count						= ctx->global_fluents_count;
	uint32_t fluent_index;
	//these are the indexes from the global fluents list, should not be used to access the ranking_list
	uint32_t* assumptions_indexes				= NULL;
	uint32_t* guarantees_indexes				= NULL;
	int32_t first_assumption_index				= 0;

	automaton_get_gr1_liveness_indexes(game_automaton, assumptions, assumptions_count,
			 guarantees, guarantees_count, &assumptions_indexes, &guarantees_indexes);

	//get assumptions and guarantees indexes
	for(i = 0; i < assumptions_count; i++){
		for(j = 0; j < ctx->global_fluents_count; j++){
			if(strcmp(assumptions[i], ctx->global_fluents[j].name) == 0){
				assumptions_indexes[i]	= j;
				/*if (i == 0)
					first_assumption_index	= (int32_t)j;*/
				break;
			}
		}
	}
	for(i = 0; i < guarantees_count; i++){
		for(j = 0; j < ctx->global_fluents_count; j++){
			if(strcmp(guarantees[i], ctx->global_fluents[j].name) == 0){
				guarantees_indexes[i]	= j;
				break;
			}
		}
	}
	uint32_t * max_delta = automaton_compute_infinity(game_automaton, assumptions_count,
			guarantees_count, assumptions_indexes, guarantees_indexes);
	//initialize transitions ranking
	automaton_ranking concrete_ranking;
	automaton_pending_state concrete_pending_state;
	for(i = 0; i < guarantees_count; i++)
		ranking_list[i]	= automaton_concrete_bucket_list_create(RANKING_BUCKET_SIZE, automaton_ranking_key_extractor, sizeof(automaton_ranking));
	for(i = 0; i < game_automaton->transitions_count; i++){
		for(j = 0; j < guarantees_count; j++){
			if(game_automaton->out_degree[i] == 0){
				//state is deadlock
				concrete_ranking.state	= i; concrete_ranking.assumption_to_satisfy	= 0;
				concrete_ranking.value	= RANKING_INFINITY;
				automaton_concrete_bucket_add_entry(ranking_list[j], &concrete_ranking);
			}else{
				//rank_g(state) = (0, 1)
				concrete_ranking.state	= i; concrete_ranking.assumption_to_satisfy	= 0;
				concrete_ranking.value	= 0;
				automaton_concrete_bucket_add_entry(ranking_list[j], &concrete_ranking);
				//g falsifies one guarantee and satisfies ass_1 then add to pending
				fluent_index	= GET_STATE_FLUENT_INDEX(fluent_count, i, assumptions_indexes[first_assumption_index]);
				if(assumptions_count == 0 ||  (TEST_FLUENT_BIT(game_automaton->valuations, fluent_index))){
					for(l = 0; l < guarantees_count; l++){
						fluent_index	= GET_STATE_FLUENT_INDEX(fluent_count, i, guarantees_indexes[l]);
						if(!TEST_FLUENT_BIT(game_automaton->valuations, fluent_index)){
#if DEBUG_SYNTHESIS
	if(_print_ranking)
		printf("[Init] Pushing into pending (%d, %d)\n", i, guarantees_indexes[j]);
#endif
							concrete_pending_state.state 	= i; concrete_pending_state.goal_to_satisfy	= j;//TODO: check this, was ...goal_to_satisfy = guarantees_indexes[j];
							concrete_pending_state.value	= 0; concrete_pending_state.timestamp		= 0;
							concrete_pending_state.assumption_to_satisfy = 0;
							void* new_entry					= automaton_pending_state_max_heap_add_entry(pending_list, &concrete_pending_state);
							automaton_concrete_bucket_add_entry(key_lists[j], new_entry);
							break;
						}
					}
				}
			}
		}

	}
	//add unstable pred.
	for(i = 0; i < game_automaton->transitions_count; i++){
		for(j = 0; j < guarantees_count; j++){
			if(game_automaton->out_degree[i] == 0){
#if DEBUG_SYNTHESIS
	if(_print_ranking)
		printf("[Deadlock] Adding unstable pred for %d\n", i);
#endif
				automaton_add_unstable_predecessors(game_automaton, pending_list, key_lists[j], i, ranking_list, /*guarantees_indexes[j] <- WAS*/ j, guarantees_count
						, assumptions_count, guarantees_indexes, assumptions_indexes, assumptions_indexes[first_assumption_index], pending_processed);
			}
		}
	}



	char strategy_name[255];
	sprintf(strategy_name, "%s_strategy", game_automaton->name);
	//stabilization
	automaton_pending_state current_pending_state;
	automaton_ranking* current_ranking;
	automaton_automaton* strategy = NULL;
#if DEBUG_SYNTHESIS
	if(_print_ranking){
		printf("!!STABILIZING GR1 GAME FOR %s!!\n", strategy_name);
		printf("[Action]<State, goal,(ranking,assumption)>\n");
	}
#endif
	automaton_pending_state* state_location;
	uint32_t goal_to_satisfy;
	bool ranking_was_infinity;

	bool max_delta_set	= false;
	for(l = 0; l < guarantees_count; l++){
		if(max_delta[l] != 0){
			max_delta_set	= true;
			break;
		}
	}
#if DEBUG_SYNTHESIS
	if(!max_delta_set){
		printf("[WARNING] CLTSA synthesis current implementation can not work when all states satisfy both all liveness assumptions and all guarantees (eq. to safety check)\n");
	}
#endif
	while(pending_list->count > 0){
		//current_pending_state	= (automaton_pending_state*)automaton_ptr_bucket_pop_entry(pending_list);
		automaton_pending_state_max_heap_pop_entry(pending_list, &current_pending_state);//**30

		goal_to_satisfy	= current_pending_state.goal_to_satisfy;

		state_location	= (automaton_pending_state*)automaton_concrete_bucket_get_entry(key_lists[goal_to_satisfy]
																								 , current_pending_state.state);
		//TODO:check, why are we getting a NULL at state location
		if(state_location != NULL)
			automaton_concrete_bucket_remove_entry(key_lists[goal_to_satisfy], state_location);
		current_ranking			= (automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[current_pending_state.goal_to_satisfy], current_pending_state.state);
#if DEBUG_SYNTHESIS
		if(_print_ranking)
			printf("\t[<-]<%d,%d,(%d,%d)>%s", current_pending_state.state, current_pending_state.goal_to_satisfy, current_ranking->value, current_ranking->assumption_to_satisfy
					, (__count_updates_gr1++ % __count_upgrades_gr1_tabs) == 0 ? "\n": "");
#endif
		if(current_ranking->value == RANKING_INFINITY){
			pending_processed++;
			continue;
		}

		if(automaton_state_is_stable(game_automaton, current_pending_state.state, ranking_list
				, current_pending_state.goal_to_satisfy, guarantees_count, assumptions_count
				, guarantees_indexes, assumptions_indexes, first_assumption_index)){//**20
			pending_processed++;
			continue;
		}
		ranking_was_infinity	= current_ranking->value == RANKING_INFINITY;

		automaton_ranking_update(game_automaton, ranking_list,
				current_ranking
				//(automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[current_pending_state.goal_to_satisfy], current_pending_state.state)
				, max_delta, current_pending_state.goal_to_satisfy, guarantees_count, assumptions_count
				, guarantees_indexes, assumptions_indexes, first_assumption_index);
#if DEBUG_SYNTHESIS
	if(_print_ranking)
		printf("\t[->]<%d,%d,(%d, %d)>%s" , current_pending_state.state, current_pending_state.goal_to_satisfy, current_ranking->value, current_ranking->assumption_to_satisfy
				, (__count_updates_gr1++ % __count_upgrades_gr1_tabs) == 0 ? "\n": "");
#endif
		automaton_add_unstable_predecessors(game_automaton, pending_list, key_lists[goal_to_satisfy], current_pending_state.state
				, ranking_list, current_pending_state.goal_to_satisfy, guarantees_count
				, assumptions_count, guarantees_indexes, assumptions_indexes, first_assumption_index, pending_processed);//**21
		//if ranking reached infinity, update other rankings for the same state
		if(!ranking_was_infinity && current_ranking->value == RANKING_INFINITY){
			for(i = 0; i < guarantees_count; i++){
				if(i != current_pending_state.goal_to_satisfy){
					current_ranking			= (automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[i], current_pending_state.state);
					current_ranking->value	= RANKING_INFINITY;
					automaton_add_unstable_predecessors(game_automaton, pending_list, key_lists[i], current_pending_state.state
									, ranking_list, i, guarantees_count
									, assumptions_count, guarantees_indexes, assumptions_indexes, first_assumption_index, pending_processed);
				}
			}
		}
		pending_processed++;
#if PRINT_PARTIAL_SYNTHESIS
		if((pending_processed % 900000) == 0){
			printf("Partial Synthesis has [%09d] pending states and [%012d] processed states, running for [%08f]s\n",
					pending_list->count, pending_processed, (double)(clock() - begin) / CLOCKS_PER_SEC);

		}
#endif
	}
#if DEBUG_SYNTHESIS || DEBUG_STRATEGY_BUILD
	if(_print_ranking){
		printf("<a id='synthesis_completed'>RANKING STABILIZATION ACHIEVED FOR %s</a>[<a href='#synthesis'>Synthesis start</a>|<a href='#synthesis_completed'>Synthesis end</a>|<a href='#game'>Game</a>|<a href='#strategy'>Strategy</a>]\n", strategy_name);
		for(i = 0; i < game_automaton->transitions_count; i++){
			//if(game_automaton->out_degree[i] <= 0) continue;
			printf("[%s][<a id='rank_%d_%d'>S %d</a>:", game_automaton->is_controllable[i]? "C" : "U",
					__ranking_link_id, i, i);
			for(j = 0; j < guarantees_count; j++){
				current_ranking	= ((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[j], i));
				printf("(g:%d,<%d,%d>)", j, current_ranking->value, current_ranking->assumption_to_satisfy);
			}
			printf("]");
			for(j = 0; j < game_automaton->out_degree[i]; j++){
				if(j > 0)printf("|");
				printf("<a href='#rank_%d_%d'>{", __ranking_link_id, game_automaton->transitions[i][j].state_to);
				bool first_print = true;
				for(k = 0; k < (TRANSITION_ENTRY_SIZE * FIXED_SIGNALS_COUNT) - 1; k++){
					if(TEST_TRANSITION_BIT((&(game_automaton->transitions[i][j])), k)){
						if(first_print)first_print = false;
						else printf(",");
						printf("%s", ctx->global_alphabet->list[k].name);
					}
				}
				printf("}->%d</a>", game_automaton->transitions[i][j].state_to);
			}
			printf("\n");
		}
		printf("\tpending:%d\n", pending_list->count);
	}
#endif
#if VERBOSE
	printf("Synthesis processed [%09d] states, run for [%08f]s\n", pending_processed, (double)(clock() - begin) / CLOCKS_PER_SEC);
#endif

	//build strategy
	strategy	= automaton_automaton_create(strategy_name, game_automaton->context, game_automaton->local_alphabet_count, game_automaton->local_alphabet, false, false);



/*
	bool is_winning = true;
	bool one_option = false;
	bool all_options	= true;
	//check if all rankings have the initial state

	for(i = 0; i < guarantees_count; i++){
		one_option	= false;
		all_options = true;
		for(j = 0; j < game_automaton->out_degree[game_automaton->initial_states[0]]; j++){
#if DEBUG_STRATEGY_BUILD
			printf("[CHK] RANKING FOR %d: %d\n", game_automaton->transitions[game_automaton->initial_states[0]][j].state_to,
					((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[i], game_automaton->transitions[game_automaton->initial_states[0]][j].state_to))->value);
#endif
			if(((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[i]
																					 , game_automaton->transitions[game_automaton->initial_states[0]][j].state_to))->value != RANKING_INFINITY){

				one_option = true;
			}else{
				all_options = false;
			}
		}
		if(!one_option || (!(game_automaton->is_controllable[game_automaton->initial_states[0]]) && !all_options)){
			is_winning = false;
			//break;
		}
	}
*/
	bool is_winning	= true;
	for(i = 0; i < guarantees_count; i++){
		is_winning	&= ((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[i],
			game_automaton->initial_states[0]))->value != RANKING_INFINITY;
		if(!is_winning)break;
	}
	if(is_winning){
		__automaton_global_print_id++;
		//keep winning states
		uint32_t new_count;
		void** new_list;
		automaton_ranking *current_ranking, *succ_ranking;
		automaton_transition* current_transition;
		int32_t b;
		//build strategy automaton out of ranking_list
		//define map between strategy compose state (s x goal) to strategy automaton state (s)
		uint32_t** strategy_maps			= malloc(sizeof(uint32_t*) * guarantees_count);
		bool** strategy_maps_is_set	= malloc(sizeof(bool*) * guarantees_count);
		uint32_t last_strategy_state	= 0;
		uint32_t strategy_from_state, strategy_to_state;
		uint32_t succ_guarantee;
		bool may_increase, is_controllable;
		automaton_transition* strategy_transition	= automaton_transition_create(0, 0);
		for(i = 0; i < guarantees_count; i++){
			strategy_maps[i]		= malloc(sizeof(uint32_t) * game_automaton->transitions_count);
			strategy_maps_is_set[i]	= malloc(sizeof(bool) * game_automaton->transitions_count);
			for(j = 0; j < game_automaton->transitions_count; j++)
				strategy_maps_is_set[i][j]	= false;
		}
		bool new_monitored	= false;
		int32_t min_value	= RANKING_INFINITY;
		automaton_ranking* min_ranking	= NULL;
		uint32_t min_to_state	= 0;
		automaton_transition* min_transition 	= NULL;
		for(i = 0; i < guarantees_count; i++){
			for(j = 0; j < ranking_list[i]->count; j++){
				for(b = (int32_t)ranking_list[i]->bucket_count[j] - 1; b >= 0; b--){

					current_ranking = (automaton_ranking*)GET_CONCRETE_BUCKET_LIST_ENTRY(ranking_list[i], j, b);
					if(!strategy_maps_is_set[i][current_ranking->state]){
						strategy_maps[i][current_ranking->state]	= last_strategy_state++;
						strategy_maps_is_set[i][current_ranking->state]	= true;
					}
					//v satisfies current guarantee v in Q_j
					may_increase	= automaton_bucket_has_entry(game_automaton->inverted_valuations[guarantees_indexes[i]], current_ranking->state);

					//is_controllable	= game_automaton->is_controllable[current_ranking->state];
					is_controllable = true;
					//TODO: check, was this, but changed in order to ensure strategy swap
					//succ_guarantee	= may_increase? ((i + 1) % guarantees_count) : i;
					succ_guarantee	= may_increase ? ((i + 1) % guarantees_count) : i;
					for(l = 0; l < game_automaton->out_degree[current_ranking->state] && is_controllable; l++){
						current_transition	= &(game_automaton->transitions[current_ranking->state][l]);
						for(m = 0; m < (ctx)->global_alphabet->count; m++){
							if(TEST_TRANSITION_BIT(current_transition, m)){
								automaton_signal_event * evt =
										&(ctx->global_alphabet->list[m]);
								if(evt->type == INPUT_SIG){
									is_controllable = false;
									break;
								}
							}
						}
					}
					new_monitored	= false;
					//keep min succ for each monitored option
					if(game_automaton->out_degree[current_ranking->state] > 0){
						for(l = 0; l <= game_automaton->out_degree[current_ranking->state]; l++){
							if(l > 0 && l < game_automaton->out_degree[current_ranking->state])
								new_monitored = !(automaton_automaton_transition_monitored_eq(game_automaton,
										&(game_automaton->transitions[current_ranking->state][l - 1]),
										&(game_automaton->transitions[current_ranking->state][l])));
							if(new_monitored || (l == game_automaton->out_degree[current_ranking->state])){
								if((min_ranking == NULL)){
									current_transition	= &(game_automaton->transitions[current_ranking->state][(l == game_automaton->out_degree[current_ranking->state])? (l - 1): l]);
									succ_ranking		=  (automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[succ_guarantee], current_transition->state_to);
									min_value	= succ_ranking->value;
									min_ranking = succ_ranking;
									min_transition	= current_transition;
								}else{
									succ_ranking		=  (automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[succ_guarantee], min_transition->state_to);
								}
								if(!strategy_maps_is_set[i][min_ranking->state]){
									strategy_maps[i][min_ranking->state]	= last_strategy_state++;
									strategy_maps_is_set[i][min_ranking->state]	= true;
								}
								if(!strategy_maps_is_set[succ_guarantee][succ_ranking->state]){
									strategy_maps[succ_guarantee][succ_ranking->state]	= last_strategy_state++;
									strategy_maps_is_set[succ_guarantee][succ_ranking->state] = true;
								}
								automaton_transition_copy(min_transition, strategy_transition);
								strategy_transition->state_from	= strategy_maps[i][current_ranking->state];
								strategy_transition->state_to	= strategy_maps[succ_guarantee][succ_ranking->state];
#if DEBUG_STRATEGY_BUILD
								printf("(g:%d)[ADD%s] From (",i, (l == game_automaton->out_degree[current_ranking->state])? "*" : "");
								automaton_transition_print(min_transition, strategy->context, "", "", -1);
								printf(") strategy transition: ");
								automaton_transition_print(strategy_transition, strategy->context, "", "\n", __automaton_global_print_id);
#endif
								automaton_automaton_add_transition(strategy, strategy_transition);
								min_value	= RANKING_INFINITY;
								min_ranking = NULL;
								min_transition	= NULL;
								new_monitored	= false;
							}
							if(l < game_automaton->out_degree[current_ranking->state]){
								current_transition	= &(game_automaton->transitions[current_ranking->state][l]);
								succ_ranking		=  (automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[succ_guarantee], current_transition->state_to);
								if((min_ranking == NULL) || automaton_ranking_lt(succ_ranking, min_ranking)){
									min_value	= succ_ranking->value;
									min_ranking = succ_ranking;
									min_transition	= current_transition;
								}
							}
						}
					}
					//add initial state if not already added
					if(strategy->initial_states_count == 0 && current_ranking->state == game_automaton->initial_states[0]){
#if DEBUG_STRATEGY_BUILD
						printf("(g:%d)[INI] Initial state added as %d\n",i, strategy_maps[i][current_ranking->state]);
#endif
						automaton_automaton_add_initial_state(strategy, strategy_maps[i][current_ranking->state]);
					}
				}
			}
		}
		for(i = 0; i < guarantees_count; i++){
			free(strategy_maps[i]);
			free(strategy_maps_is_set[i]);
		}
		free(strategy_maps);
		free(strategy_maps_is_set);
		//we dont want to call destroy since signals are owned by original transition
		free(strategy_transition);

	}
	if(print_ranking){
		automaton_automaton *clone = automaton_automaton_clone(game_automaton);
		automaton_ranking_print_report(clone, ranking_list, max_delta, guarantees_count, guarantees);
		automaton_automaton_destroy(clone);
	}
	//destroy structures
	free(max_delta);
	for(i = 0; i < guarantees_count; i++){
		automaton_concrete_bucket_destroy(ranking_list[i], true);
	}
	free(ranking_list);
	automaton_pending_state_max_heap_destroy(pending_list);
	for(i = 0; i < guarantees_count; i++)
		automaton_concrete_bucket_destroy(key_lists[i], true);
	free(key_lists);
	free(assumptions_indexes);
	free(guarantees_indexes);
	automaton_automaton_remove_unreachable_states(strategy);
#if DEBUG_STRATEGY_BUILD
	printf("<a id='game'>Game</a>[<a href='#synthesis'>Synthesis start</a>|<a href='#synthesis_completed'>Synthesis end</a>|<a href='#game'>Game</a>|<a href='#strategy'>Strategy</a>]\n=====\n");
	automaton_automaton_print(game_automaton, false, false, false, "", "\n");

	printf("<a id='strategy'>Built Strategy</a>[<a href='#synthesis'>Synthesis start</a>|<a href='#synthesis_completed'>Synthesis end</a>|<a href='#game'>Game</a>|<a href='#strategy'>Strategy</a>]\n=====\n");
	automaton_automaton_print(strategy, false, false, false, "", "\n");
	printf("<a href='#synthesis'>Synthesis start</a>|<a href='#synthesis_completed'>Synthesis end</a>|<a href='#game'>Game</a>|<a href='#strategy'>Strategy</a>\n=====\n");
#endif
	return strategy;

}

bool automaton_is_gr1_realizable(automaton_automaton* game_automaton, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count){
	automaton_automaton* strategy = automaton_get_gr1_strategy(game_automaton, assumptions, assumptions_count,
			guarantees, guarantees_count, true);
	bool is_realizable = (strategy->transitions_count != 0
			&& strategy->out_degree[strategy->initial_states[0]] > 0);
	automaton_automaton_destroy(strategy);
	return is_realizable;
}
