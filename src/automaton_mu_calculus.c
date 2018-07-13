/*

 * automaton_mu_calculus.c
 *
 *  Created on: Jun 18, 2018
 *      Author: mariano
 */

#include "automaton_mu_calculus.h"

/*
 void GR1Context::computeWinningPositions() {
    // The greatest fixed point - called "Z" in the GR(1) synthesis paper
    BFFixedPoint nu2(mgr.constantTrue());
    // Iterate until we have found a fixed point
    for (;!nu2.isFixedPointReached();) {
        // To extract a strategy in case of realizability, we need to store a sequence of 'preferred' transitions in the
        // game structure. These preferred transitions only need to be computed during the last execution of the outermost
        // greatest fixed point. Since we don't know which one is the last one, we store them in every iteration,
        // so that after the last iteration, we obtained the necessary data. Before any new iteration, we need to
        // clear the old data, though.
        strategyDumpingData.clear();
        // Iterate over all of the liveness guarantees. Put the results into the variable 'nextContraintsForGoals' for every
        // goal. Then, after we have iterated over the goals, we can update nu2.
        BF nextContraintsForGoals = mgr.constantTrue();
        for (unsigned int j=0;j<livenessGuarantees.size();j++) {
            // Start computing the transitions that lead closer to the goal and lead to a position that is not yet known to be losing.
            // Start with the ones that actually represent reaching the goal (which is a transition in this implementation as we can have
            // nexts in the goal descriptions).
            BF livetransitions = livenessGuarantees[j] & (nu2.getValue().SwapVariables(varVectorPre,varVectorPost));
            // Compute the middle least-fixed point (called 'Y' in the GR(1) paper)
            BFFixedPoint mu1(mgr.constantFalse());
            for (;!mu1.isFixedPointReached();) {
                // Update the set of transitions that lead closer to the goal.
                livetransitions |= mu1.getValue().SwapVariables(varVectorPre,varVectorPost);
                // Iterate over the liveness assumptions. Store the positions that are found to be winning for *any*
                // of them into the variable 'goodForAnyLivenessAssumption'.
                BF goodForAnyLivenessAssumption = mu1.getValue();
                for (unsigned int i=0;i<livenessAssumptions.size();i++) {
                    // Prepare the variable 'foundPaths' that contains the transitions that stay within the inner-most
                    // greatest fixed point or get closer to the goal. Only used for strategy extraction
                    BF foundPaths = mgr.constantTrue();
                    // Inner-most greatest fixed point. The corresponding variable in the paper would be 'X'.
                    BFFixedPoint nu0(mgr.constantTrue());
                    for (;!nu0.isFixedPointReached();) {
                        // Compute a set of paths that are safe to take - used for the enforceable predecessor operator ('cox')
                        foundPaths = livetransitions | (nu0.getValue().SwapVariables(varVectorPre,varVectorPost) & !(livenessAssumptions[i]));
                        foundPaths &= safetySys;
                        // Update the inner-most fixed point with the result of applying the enforcable predecessor operator
                        nu0.update(safetyEnv.Implies(foundPaths).ExistAbstract(varCubePostOutput).UnivAbstract(varCubePostInput));
                    }
                    // Update the set of positions that are winning for some liveness assumption
                    goodForAnyLivenessAssumption |= nu0.getValue();
                    // Dump the paths that we just wound into 'strategyDumpingData' - store the current goal long
                    // with the BDD
                    strategyDumpingData.push_back(std::pair<unsigned int,BF>(j,foundPaths));
                }
                // Update the moddle fixed point
                mu1.update(goodForAnyLivenessAssumption);
            }
            // Update the set of positions that are winning for any goal for the outermost fixed point
            nextContraintsForGoals &= mu1.getValue();
        }
        // Update the outer-most fixed point
        nu2.update(nextContraintsForGoals);
    }
    // We found the set of winning positions
    winningPositions = nu2.getValue();
}
 * */
uint32_t automaton_transition_key_extractor(void* transition){
	return ((automaton_transition*)transition)->state_from;
}
/*
 * frontier es el conjunto de transiciones siendo expandido, old_set son las que ya están exploradas,
 * new_set es donde va a poner las nuevas transicioes, original_state_set es el conjunto inicial de estados
 * si devuelve 0 es porque llego a punto fijo, sino se puede hacer merge de new_set con old_set, cambiar
 * frontier por new_set y ya, porque cuando se llame de nuevo se resetea
 * */
uint32_t automaton_cox(automaton_automaton* current_automaton, automaton_ptr_bucket_list* frontier
		, automaton_ptr_bucket_list* old_set, automaton_ptr_bucket_list* new_set
		, automaton_bucket_list* original_state_set, bool negate_original_state_set){
	automaton_ptr_bucket_reset(new_set);
	automaton_transition *current_transition, *frontier_transition, *exit_transition;
	uint32_t new_transitions_count	= 0;
	uint32_t i,j,k,l,m,n;
	bool exits_set;

	for(i = 0; i < frontier->count; i++){
		for(j = 0; j < frontier->bucket_count[i]; j++){
			current_transition			= (automaton_transition*)frontier->buckets[i][j];
			for(k = 0; k < current_automaton->in_degree[current_transition->state_from]; k++){
				frontier_transition		= &(current_automaton->inverted_transitions[current_transition->state_from][k]);
				if(negate_original_state_set && automaton_bucket_has_entry(original_state_set, frontier_transition->state_to))
					continue;
				if(frontier_transition != current_transition){
					exits_set = false;
					for(l = 0; l < current_automaton->out_degree[frontier_transition->state_from]; l++){
						exit_transition	= &(current_automaton->transitions[frontier_transition->state_from][l]);
						if(exit_transition != frontier_transition && exit_transition != current_transition){
							if(!exit_transition->is_input || automaton_ptr_bucket_has_entry(old_set, exit_transition, exit_transition->state_from)
									|| automaton_ptr_bucket_has_entry(new_set, exit_transition, exit_transition->state_from))continue;
							if(!automaton_ptr_bucket_has_key(old_set, exit_transition->state_to, automaton_transition_key_extractor)
									&& !automaton_ptr_bucket_has_key(frontier, exit_transition->state_to, automaton_transition_key_extractor)
									&& !(!negate_original_state_set && automaton_bucket_has_entry(original_state_set, exit_transition->state_to))){
								exits_set	= true;
								break;
							}

						}
					}
					if(!exits_set){
						new_transitions_count++;
						automaton_ptr_bucket_add_entry(new_set, frontier_transition, frontier_transition->state_from);
					}
				}
			}
		}
	}
	return new_transitions_count;
}
automaton_automaton* automaton_get_gr1_winning_region(automaton_automaton* game_automaton, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count){
	uint32_t i, j; //i for assumptions, j for guarantees
	uint32_t k, l, m;
	automaton_automata_context*	ctx		= game_automaton->context;
	automaton_ptr_bucket_list	*swap_frontier;
	automaton_ptr_bucket_list	*transitions_frontier	= automaton_ptr_bucket_list_create(TRANSITIONS_BUCKET_SIZE);
	automaton_ptr_bucket_list	*z_old	= automaton_ptr_bucket_list_create(TRANSITIONS_BUCKET_SIZE);
	automaton_ptr_bucket_list	*z_new	= automaton_ptr_bucket_list_create(TRANSITIONS_BUCKET_SIZE);
	automaton_ptr_bucket_list	*y_old	= automaton_ptr_bucket_list_create(TRANSITIONS_BUCKET_SIZE);
	automaton_ptr_bucket_list	*y_new	= automaton_ptr_bucket_list_create(TRANSITIONS_BUCKET_SIZE);
	automaton_ptr_bucket_list	*x_old	= automaton_ptr_bucket_list_create(TRANSITIONS_BUCKET_SIZE);
	automaton_ptr_bucket_list	*x_new	= automaton_ptr_bucket_list_create(TRANSITIONS_BUCKET_SIZE);
	automaton_ptr_bucket_list	*true_list			= automaton_ptr_bucket_list_create(TRANSITIONS_BUCKET_SIZE);
	automaton_ptr_bucket_list	*assumptions_accum	= automaton_ptr_bucket_list_create(TRANSITIONS_BUCKET_SIZE);
	for(i = 0; i < game_automaton->transitions_count; i++){
		for(j = 0;  j < game_automaton->out_degree[i]; j++){
			automaton_ptr_bucket_add_entry(true_list, &(game_automaton->transitions[i][j]), game_automaton->transitions[i][j].state_from);
		}
	}
	automaton_bucket_list		*original_goals_states, *original_assumptions_states;
	uint32_t z_update_size				= -1;
	uint32_t y_update_size				= -1;
	uint32_t x_update_size				= -1;
	automaton_transition* current_transition;
	bool found;
	//get the set of states violating one assumption
	for(i = 0; i < assumptions_count; i++){
		//original assumptions states <- states not satisfying ass_i
		automaton_ptr_bucket_reset(x_old);
		automaton_ptr_bucket_reset(x_new);
		found	= false;
		for(k = 0; k < ctx->global_fluents_count; k++){
			if(strcmp(ctx->global_fluents[k].name, assumptions[i]) == 0){
				original_assumptions_states	= game_automaton->inverted_valuations[k];
				found	= true;
				break;
			}
		}
		if(!found){
			printf("[FATAL ERROR] assumption not found %s\n", assumptions[i]);
			exit(-1);
		}
		automaton_ptr_bucket_reset(transitions_frontier);
		automaton_ptr_bucket_list_copy(x_old,true_list);
		for(k = 0; k < x_old->count; k++){
			for(l = 0; l < x_old->bucket_count[k]; l++){
				current_transition	= (automaton_transition*)x_old->buckets[k][l];
				if(automaton_bucket_has_entry(original_assumptions_states, current_transition->state_to))
					continue;
				automaton_ptr_bucket_add_entry(transitions_frontier, current_transition, current_transition->state_from);
			}
		}
		automaton_cox(game_automaton, transitions_frontier, x_new, x_new, original_assumptions_states, true);
		while(x_update_size != 0){
			//get cox for assumptions, process x through intersect (is nu)
			swap_frontier	= transitions_frontier;
			transitions_frontier	= x_new;
			automaton_ptr_bucket_reset(swap_frontier);
			x_new			= swap_frontier;
			x_update_size	= automaton_cox(game_automaton, transitions_frontier, x_old, x_new, original_assumptions_states, true);
			if(x_update_size != 0){
				automaton_ptr_bucket_list_intersect(x_old, x_new, automaton_transition_key_extractor);
			}else{
				break;
			}
		}
		x_update_size	= -1;
		automaton_ptr_bucket_list_merge(assumptions_accum, x_old, automaton_transition_key_extractor);
	}
	automaton_ptr_bucket_list_copy(z_old,true_list);
	//compute winning for every guarantee
	while(z_update_size != 0){//nu2, Z
		for(j = 0; j < guarantees_count; j++){
			//original goals states <- states satisfying g_j
			automaton_ptr_bucket_reset(y_old);
			automaton_ptr_bucket_reset(y_new);
			found	= false;
			for(k = 0; k < ctx->global_fluents_count; k++){
				if(strcmp(ctx->global_fluents[k].name, guarantees[j]) == 0){
					original_goals_states	= game_automaton->inverted_valuations[k];
					found	= true;
					break;
				}
			}
			if(!found){
				printf("[FATAL ERROR] goal not found %s\n", guarantees[j]);
				exit(-1);
			}
			automaton_ptr_bucket_reset(transitions_frontier);
			//automaton_ptr_bucket_list_copy(y_old, true_list);
			for(k = 0; k < y_old->count; k++){
				for(l = 0; l < y_old->bucket_count[k]; l++){
					current_transition	= (automaton_transition*)y_old->buckets[k][l];
					if(!automaton_bucket_has_entry(original_goals_states, current_transition->state_to))
						continue;
					automaton_ptr_bucket_add_entry(transitions_frontier, current_transition, current_transition->state_from);
				}
			}
			automaton_cox(game_automaton, transitions_frontier, y_new, y_new, original_goals_states, false);

			while(y_update_size != 0){
				//check intersection with assumptions violation
				swap_frontier	= transitions_frontier;
				transitions_frontier	= y_new;
				automaton_ptr_bucket_reset(swap_frontier);
				y_new			= swap_frontier;
				y_update_size	= automaton_cox(game_automaton, transitions_frontier, y_old, y_new, original_goals_states, false);
				if(y_update_size != 0){
					automaton_ptr_bucket_list_merge(y_old, y_new, automaton_transition_key_extractor);
				}else{
					break;
				}
			}
			y_update_size 	= -1;
		}
	}
	if(original_assumptions_states != NULL)automaton_bucket_destroy(original_assumptions_states); original_assumptions_states	= NULL;
	if(original_goals_states != NULL)automaton_bucket_destroy(original_goals_states); original_goals_states	= NULL;
	automaton_ptr_bucket_destroy(true_list);	true_list	= NULL;
	automaton_ptr_bucket_destroy(assumptions_accum);	assumptions_accum		= NULL;
	automaton_ptr_bucket_destroy(transitions_frontier);	transitions_frontier	= NULL;
	automaton_ptr_bucket_destroy(z_old);automaton_ptr_bucket_destroy(z_new);
	automaton_ptr_bucket_destroy(y_old);automaton_ptr_bucket_destroy(y_new);
	automaton_ptr_bucket_destroy(x_old);automaton_ptr_bucket_destroy(x_new);
	z_old	= z_new	= NULL;
	x_old	= x_new	= NULL;
	y_old	= y_new	= NULL;
	return automaton_automaton_clone(game_automaton);
}


