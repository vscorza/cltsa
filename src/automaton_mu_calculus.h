/*
 * automaton_mu_calculus.h
 *
 *  Created on: Jun 18, 2018
 *      Author: mariano
 */

#ifndef AUTOMATON_MU_CALCULUS_H_
#define AUTOMATON_MU_CALCULUS_H_

#include "automaton.h"

#define TRANSITIONS_BUCKET_SIZE 10000

uint32_t automaton_transition_key_extractor(void* transition);
uint32_t automaton_cox(automaton_automaton* current_automaton, automaton_ptr_bucket_list* frontier
		, automaton_ptr_bucket_list* old_set, automaton_ptr_bucket_list* new_set
		, automaton_bucket_list* original_state_set, bool negate_original_state_set);
automaton_automaton* automaton_get_gr1_winning_region(automaton_automaton* game_automaton, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count);

#endif /* AUTOMATON_MU_CALCULUS_H_ */
