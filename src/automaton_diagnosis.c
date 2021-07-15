/*
 * automaton_diagnosis.c
 *
 *  Created on: Apr 16, 2021
 *      Author: mariano
 */

#include "automaton.h"
#include "assert.h"

extern int __automaton_global_print_id;


automaton_automaton* automaton_get_gr1_unrealizable_minimization_dd(automaton_automaton* game_automaton, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count, uint32_t *steps, uint32_t **steps_sizes
		, struct timeval **steps_times, uint32_t *steps_size){

	int32_t i,j,k, from_step = 0;
	uint32_t non_controllable_size	= 0;

	uint32_t t_size, t_count;
	uint32_t *t_states, *t_indexes, *t_indexes_count;
	//transitions to remove lists
	uint32_t r_size, r_count = 0;
	uint32_t *r_states, *r_indexes, *r_indexes_count;

	automaton_automaton *master = automaton_get_gr1_unrealizable_prepare_structs(game_automaton, &t_size, &t_count, &t_states, &t_indexes, &t_indexes_count,
			 &r_size, &r_count, &r_states, &r_indexes, &r_indexes_count);

	int32_t vector_size			= (int32_t)ceil(t_count / sizeof(uint8_t));
	uint8_t *partition_bit_vector	= calloc(vector_size, sizeof(uint8_t));
	for(i = 0; i < (int32_t)vector_size;i++)partition_bit_vector[i]	= (uint8_t)0xFFFF;
	uint32_t transitions_kept_size  = t_count;
	uint32_t partitions_count		= 2;
#if DEBUG_UNREAL
			printf("Minimizing %s through delta debugging\n", master->name);
#endif

#if DEBUG_DD
	if(t_count < 256){
		printf("Character meaning: \n▪\tkept\nx\tkept single trans.\n▫\tcurr.removed\nˆ\tcouldnt't remove single trans.\n◦\tprev.removed\n");

		//remove transitions that are in the current partition
		printf("[");
		for(i = 0; i < vector_size;i++)printf("%02x",partition_bit_vector[i]);
		printf("]raw partition vector\n");
		printf("[");
		for(i = 0; i < t_count; i++){
			if(TEST_BITVECTOR_BIT(partition_bit_vector, i))printf("▪");
			else printf("▫");
		}
		printf("]Kept transitions\n");


	}
#endif
	automaton_automaton *minimized	= automaton_automaton_clone(master);
	automaton_automaton *result 	= automaton_get_gr1_unrealizable_minimization_dd2(master, minimized, assumptions, assumptions_count, guarantees, guarantees_count
			, partition_bit_vector, transitions_kept_size, partitions_count, t_count, t_size, t_states, t_indexes, t_indexes_count, false, 0, steps
			, steps_sizes, steps_times, steps_size);
	free(t_states); free(t_indexes); free(t_indexes_count);
	free(r_states); free(r_indexes); free(r_indexes_count);
	free(partition_bit_vector);
	//print rankings
	/*
	automaton_automaton* strategy = automaton_get_gr1_strategy(result, assumptions, assumptions_count,
			guarantees, guarantees_count, true);
	automaton_automaton_destroy(strategy);
	*/
	automaton_automaton_destroy(minimized);
	automaton_automaton_destroy(master);

	return result;
}

void automaton_minimization_adjust_steps_report(uint32_t *steps, uint32_t **steps_sizes,
		struct timeval **steps_times, uint32_t *steps_size){
	if(*steps < (*steps_size) - 1)return;
	*steps_size	*= 2;
	uint32_t *int_ptr	= realloc(*steps_sizes, sizeof(uint32_t) * *steps_size);
	if(int_ptr == NULL){
		printf("Could not allocate memory[automaton_minimization_adjust_steps_report:1]\n"); exit(-1);
	}
	*steps_sizes	= int_ptr;
	struct timeval *time_ptr	= realloc(*steps_times, sizeof(struct timeval) * *steps_size);
	if(time_ptr == NULL){
		printf("Could not allocate memory[automaton_minimization_adjust_steps_report:2]\n"); exit(-1);
	}
	*steps_times	= time_ptr;
}

automaton_automaton* automaton_get_gr1_unrealizable_minimization_dd2_c_i_complement(automaton_automaton* master, automaton_automaton* minimized, automaton_automaton *inner_automaton
		, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count, uint8_t *partition_bit_vector, uint32_t transitions_kept_size
		, uint32_t partitions_count, uint32_t t_count, uint32_t t_size, uint32_t *t_states, uint32_t *t_indexes, uint32_t *t_indexes_count
		, bool start_with_complement, uint32_t last_partition
		, uint32_t *steps, uint32_t **steps_sizes, struct timeval **steps_times, uint32_t *steps_size){
	//bit indexes refer to locations in the whole transition space, linear indexes refer to locations in the kept transitions
	automaton_automaton *return_automaton	= NULL, *minimization = NULL;
	uint32_t last_linear_index = 0, first_linear_index = 0;
	uint32_t removed = 0, dd = 0, i, j;
	int32_t current_linear_index = -1;
	struct timeval tval_before, tval_after;
	float step	= transitions_kept_size / (partitions_count * 1.0f);
	automaton_transition *current_transition = NULL;
	//check if C_i complement achieves nonrealizability
	uint32_t dd2 = 0;
	for(dd2 = 0; dd2 <= (partitions_count-1); dd2++){
		dd = (last_partition + dd2) % (partitions_count);
		first_linear_index	= floor(step * dd);
		last_linear_index	= floor(step * (dd+1))-1;
		minimization = automaton_automaton_clone(inner_automaton);
		removed = 0;
		current_linear_index = -1;
		//remove transitions as the range is evaluated traversing the transitions' list
#if DEBUG_DD
		if(t_count < 256){
			for(i = 0; i < t_count; i++)printf("%s",automaton_automaton_has_transition(minimization, &(master->transitions[t_states[i]][t_indexes[i]]))? "▪" : "▫");
			printf("]Effective partition (previous)\n");
			for(i = 0; i < t_count; i++)printf("%s",TEST_BITVECTOR_BIT(partition_bit_vector, i)? "▪" : "▫");
			printf("]Bit vector (previous)\n");
		}
		bool index_changed = false;
#endif
		for(i = 0; i < t_count; i++){
#if DEBUG_DD
			index_changed = false;
#endif
			if(TEST_BITVECTOR_BIT(partition_bit_vector, i)){
#if DEBUG_DD
				index_changed = true;
#endif
				current_linear_index++;
			}
#if DEBUG_DD
			if(t_count < 256 && index_changed)
				if(current_linear_index == first_linear_index)printf(">");
#endif
			if(((current_linear_index >= first_linear_index && current_linear_index <= last_linear_index))
					&& (TEST_BITVECTOR_BIT(partition_bit_vector, i))
					 && (minimization->out_degree[t_states[i]] > t_indexes_count[i])){
				for(j = 0; j < t_indexes_count[i]; j++){
					automaton_automaton_remove_transition(minimization, &(master->transitions[t_states[i]][t_indexes[i]+j]));
					removed++;
				}
#if DEBUG_DD
				if(t_count < 256)
					printf("▫");//curr.removed
			}	else if(automaton_automaton_has_transition(minimization, &(master->transitions[t_states[i]][t_indexes[i]]))){
				if(t_count < 256){
					if(inner_automaton->out_degree[t_states[i]] > 1){
						printf("▪");//kept
					}else{
						printf("x");//kept single trans.
					}
				}
			}else{
				if(t_count < 256){
					if(inner_automaton->out_degree[t_states[i]] == 1){
						printf("ˆ");//single trans.
					}else{
						printf("◦");//prev.removed
					}
				}
#endif
			}
#if DEBUG_DD
			if(t_count < 256 && index_changed)
				if(current_linear_index == last_linear_index)printf("<");
#endif
		}
#if DEBUG_DD
		if(t_count < 256){
			printf("]Partition (%d) vector\n",dd);
			printf("[");
			for(i = 0; i < t_count; i++)printf("%s",automaton_automaton_has_transition(minimization, &(master->transitions[t_states[i]][t_indexes[i]]))? "▪" : "▫");
			printf("]Effective partition(removed:%d)(kept:%d)\n",removed, transitions_kept_size);
			for(i = 0; i < t_count; i++)printf("%s",TEST_BITVECTOR_BIT(partition_bit_vector, i)? "▪" : "▫");
			printf("]Bit vector (new)\n");
		}
		else
			printf("Partition (%d) Kept \t %d trans. from \t %d\n", dd, transitions_kept_size, t_count);
#endif
		//if non-realizable:update structs, perform recursive call and return
		//automaton_automaton_remove_deadlocks(minimization);
		automaton_automaton_remove_unreachable_states(minimization);
		automaton_automaton_update_valuations(minimization);
		bool cleared_bit	= false;
		if(automaton_is_gr1_realizable(minimization, assumptions, assumptions_count,
				guarantees, guarantees_count) || minimization->out_degree[minimization->initial_states[0]] == 0){
			automaton_automaton_destroy(minimization);
#if DEBUG_UNREAL || DEBUG_DD
			printf("(compl. to part. %d realizable)\n", dd);
#endif
		}else{//update bit vector to keep only current partition complement and set n to max(n-1, 2)
#if DEBUG_UNREAL || DEBUG_DD
			printf("(compl. to part. %d unrealizable)\n", dd);
#endif
			gettimeofday(&tval_before, NULL);
			//removed = 0;
			current_linear_index = -1; transitions_kept_size = 0;
#if DEBUG_DD
			if(t_count < 256)
				printf("[");
#endif
			//bit vector sets to false anything outside the range
			for(i = 0; i < t_count; i++){
				if(TEST_BITVECTOR_BIT(partition_bit_vector, i)){
					current_linear_index++;
				}
				if((current_linear_index >= first_linear_index && current_linear_index <= last_linear_index)){
					if(TEST_BITVECTOR_BIT(partition_bit_vector, i))
						cleared_bit	= true;
					CLEAR_BITVECTOR_BIT(partition_bit_vector, i);
					if(minimization->out_degree[t_states[i]] > t_indexes_count[i]){
						for(j = 0; j < t_indexes_count[i]; j++){
							automaton_automaton_remove_transition(minimization, &(master->transitions[t_states[i]][t_indexes[i]+j]));
							removed++;
						}
					}
				}else{
					transitions_kept_size++;
				}
			}

			//if(partitions_count > transitions_kept_size || removed == 0){
			if(!cleared_bit){
#if DEBUG_UNREAL
				printf("(minimal)partitions_count:%d\tkept:%d\tremoved:%d\n",partitions_count
						, transitions_kept_size, removed);
#endif
				automaton_automaton_destroy(inner_automaton);
				return minimization;
			}
			uint32_t next_partitions_count = max(partitions_count - 1, 2);
			//compute where to start the next step, project current ith position in the next partition size
			uint32_t next_partition = floor((uint32_t)(dd * ((next_partitions_count*1.0f) / partitions_count)));
#if DEBUG_UNREAL
			printf("Computing where to start next partition: floor(i/n*n'):: floor(%d*%d/%d)=%d\n", dd, next_partitions_count, partitions_count, next_partition);
#endif
			partitions_count	= next_partitions_count;
			gettimeofday(&tval_after, NULL);
			timersub(&tval_after, &tval_before, &((*steps_times)[*steps]));
			(*steps_sizes)[*steps]	= minimization->transitions_composite_count;
			return_automaton = automaton_get_gr1_unrealizable_minimization_dd2(master, minimization, assumptions, assumptions_count, guarantees, guarantees_count
					, partition_bit_vector, transitions_kept_size, partitions_count, t_count, t_size, t_states, t_indexes, t_indexes_count, true, next_partition
					, steps, steps_sizes, steps_times, steps_size);
			automaton_automaton_destroy(inner_automaton);
			automaton_automaton_destroy(minimization);
			return return_automaton;
		}
	}
	return NULL;
}
automaton_automaton* automaton_get_gr1_unrealizable_minimization_dd2_c_i(automaton_automaton* master, automaton_automaton* minimized, automaton_automaton *inner_automaton
		, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count, uint8_t *partition_bit_vector, uint32_t transitions_kept_size
		, uint32_t partitions_count, uint32_t t_count, uint32_t t_size, uint32_t *t_states, uint32_t *t_indexes, uint32_t *t_indexes_count
		, bool start_with_complement, uint32_t last_partition
		, uint32_t *steps, uint32_t **steps_sizes, struct timeval **steps_times, uint32_t *steps_size){
	//bit indexes refer to locations in the whole transition space, linear indexes refer to locations in the kept transitions
	automaton_automaton *return_automaton	= NULL, *minimization = NULL;
	uint32_t last_linear_index = 0, first_linear_index = 0;
	uint32_t removed = 0, dd = 0, i, j;
	int32_t current_linear_index = -1;
	struct timeval tval_before, tval_after;
	float step	= transitions_kept_size / (partitions_count * 1.0f);
	automaton_transition *current_transition = NULL;
	//check if C_i achieves nonrealizability

	gettimeofday(&tval_before, NULL);
	uint32_t dd2 = 0;
	for(dd2 = 0; dd2 <= (partitions_count-1); dd2++){
		dd = (last_partition + dd2) % (partitions_count);
		current_linear_index = -1;
		first_linear_index	= (uint32_t)floor(step * dd);
		last_linear_index	= (uint32_t)floor(step * (dd+1))-1;
		minimization = automaton_automaton_clone(inner_automaton);
		removed = 0;
#if DEBUG_DD
		if(t_count < 256)
			printf("[");
#endif
		//remove transitions as the range is evaluated traversing the transitions' list
#if DEBUG_DD
		if(t_count < 256){
			for(i = 0; i < t_count; i++)printf("%s",automaton_automaton_has_transition(minimization, &(master->transitions[t_states[i]][t_indexes[i]]))? "▪" : "▫");
			printf("]Effective partition (previous)\n");
			for(i = 0; i < t_count; i++)printf("%s",TEST_BITVECTOR_BIT(partition_bit_vector, i)? "▪" : "▫");
			printf("]Bit vector (previous)\n");
		}
		bool index_changed = false;
#endif
		for(i = 0; i < t_count; i++){
#if DEBUG_DD
			index_changed = false;
#endif
			if(TEST_BITVECTOR_BIT(partition_bit_vector, i)){
#if DEBUG_DD
				index_changed = true;
#endif
				current_linear_index++;
			}
#if DEBUG_DD
			if(t_count < 256 && index_changed)
				if(current_linear_index == first_linear_index)printf("<");
#endif
			if((current_linear_index < first_linear_index || current_linear_index > last_linear_index)
					&& TEST_BITVECTOR_BIT(partition_bit_vector, i)
					&& (minimization->out_degree[t_states[i]] > t_indexes_count[i])){
					for(j = 0; j < t_indexes_count[i]; j++){
						automaton_automaton_remove_transition(minimization, &(master->transitions[t_states[i]][t_indexes[i]+j]));
						removed++;
#if DEBUG_UNREAL && 0
						printf("Removing:");
						automaton_transition_print(&(master->transitions[t_states[i]][t_indexes[i]+j]), minimization->context, "", "\n",0);
#endif
					}

#if DEBUG_DD
				if(t_count < 256)
					printf("▫");//curr.removed
			}	else if(automaton_automaton_has_transition(minimization, &(master->transitions[t_states[i]][t_indexes[0]]))){
				if(t_count < 256){
					if(inner_automaton->out_degree[t_states[i]] > 1){
						printf("▪");//kept
					}else{
						printf("x");//kept single trans.
					}
				}
			}else{
				if(t_count < 256){
					if(inner_automaton->out_degree[t_states[i]] == 1){
						printf("ˆ");//single trans.
					}else{
						printf("◦");//prev.removed
					}
				}
#endif
			}
#if DEBUG_DD
			if(t_count < 256 && index_changed)
				if(current_linear_index == last_linear_index)printf(">");
#endif
		}
#if DEBUG_DD
		if(t_count < 256){
			printf("]Partition (%d) vector\n",dd);
			printf("[");
			for(i = 0; i < t_count; i++)printf("%s",automaton_automaton_has_transition(minimization, &(master->transitions[t_states[i]][t_indexes[i]]))? "▪" : "▫");
			printf("]Effective partition(removed:%d)(kept:%d)\n",removed,transitions_kept_size);
			for(i = 0; i < t_count; i++)printf("%s",TEST_BITVECTOR_BIT(partition_bit_vector, i)? "▪" : "▫");
			printf("]Bit vector (new)\n");
		}
		else
			printf("Partition (%d) Kept \t %d trans. from \t %d\n", dd, transitions_kept_size, t_count);
#endif
		//if non-realizable:update structs, perform recursive call and return
		//automaton_automaton_remove_deadlocks(minimization);
		automaton_automaton_remove_unreachable_states(minimization);
		automaton_automaton_update_valuations(minimization);
		bool cleared_bit	= false;
		if(automaton_is_gr1_realizable(minimization, assumptions, assumptions_count,
				guarantees, guarantees_count) || minimization->out_degree[minimization->initial_states[0]] == 0){
			automaton_automaton_destroy(minimization);
#if DEBUG_UNREAL
			printf("(part. %d realizable)\n", dd);
#endif
		}else{//update bit vector to keep only current partition and set n to 2
#if DEBUG_UNREAL
			printf("(part. %d unrealizable)\n", dd);
#endif
			//removed = 0;
			current_linear_index = -1; transitions_kept_size = 0;
			//bit vector sets to false anything outside the range
			for(i = 0; i < t_count; i++){
				if(TEST_BITVECTOR_BIT(partition_bit_vector, i)){
					current_linear_index++;
				}
				if((current_linear_index < first_linear_index || current_linear_index > last_linear_index)&& TEST_BITVECTOR_BIT(partition_bit_vector, i)){
					if(TEST_BITVECTOR_BIT(partition_bit_vector, i))
						cleared_bit	= true;
					CLEAR_BITVECTOR_BIT(partition_bit_vector, i);

					if(minimization->out_degree[t_states[i]] > t_indexes_count[i]){
						for(j = 0; j < t_indexes_count[i]; j++){
							automaton_automaton_remove_transition(minimization, &(master->transitions[t_states[i]][t_indexes[i]+j]));
							removed++;
						}
					}
				}else if(TEST_BITVECTOR_BIT(partition_bit_vector, i)){
					transitions_kept_size++;
				}
			}

			//if(partitions_count > transitions_kept_size || removed == 0){
			if(!cleared_bit){
#if DEBUG_UNREAL
				printf("(minimal)partitions:%d\tkept:%d\tremoved:%d\n", partitions_count
						, transitions_kept_size, removed);
#endif
				automaton_automaton_destroy(inner_automaton);
				return minimization;
			}
			partitions_count = 2;
			gettimeofday(&tval_after, NULL);
			timersub(&tval_after, &tval_before, &((*steps_times)[*steps]));
			(*steps_sizes)[*steps]	= minimization->transitions_composite_count;
			return_automaton = automaton_get_gr1_unrealizable_minimization_dd2(master, minimization, assumptions, assumptions_count, guarantees, guarantees_count
					, partition_bit_vector, transitions_kept_size, partitions_count, t_count, t_size, t_states, t_indexes, t_indexes_count, false, 0, steps
					, steps_sizes, steps_times, steps_size);
			automaton_automaton_destroy(inner_automaton);
			automaton_automaton_destroy(minimization);
			return return_automaton;
		}
	}
	return NULL;
}
/**
	 * Returns a minimal automaton preserving unrealizability by using a delta debugging approach over the set of non
	 * controllable transitions
	 *
	 * @param master the initial automaton to be minimized
	 * @param minimized the partially minimized automaton
	 * @param assumptions the set of assumptions' names in the gr1 formula
	 * @param assumptions_count the size of the assumptions' set
	 * @param guarantees the set of guarantees' names in the gr1 formula
	 * @param guarantees_count the size of the guarantees' set
	 * @param partition_bit_vector the boolean vector determining which transitions were kept in previous runs
	 * @param transitions_ketp_size the size of transitions kept in the boolean vector
	 * @param partitions_count the number of partitions to be tested on this particular call (n in dd)
	 * @param t_count the size of the non controllable transitions' set
	 * @param t_size the size of allocated elements fro the non controllable transitions' set
	 * @param t_states a linear projection of non controllable transitions to source states
	 * @param t_indexes a liner projection of non controllable transitions to index within source state out list
	 * @return the minimized automaton
	 */

automaton_automaton* automaton_get_gr1_unrealizable_minimization_dd2(automaton_automaton* master, automaton_automaton* minimized
		, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count, uint8_t *partition_bit_vector, uint32_t transitions_kept_size
		, uint32_t partitions_count, uint32_t t_count, uint32_t t_size, uint32_t *t_states, uint32_t *t_indexes, uint32_t *t_indexes_count
		, bool start_with_complement, uint32_t last_partition
		, uint32_t *steps, uint32_t **steps_sizes, struct timeval **steps_times, uint32_t *steps_size){
	(*steps)++;
	automaton_minimization_adjust_steps_report(steps, steps_sizes, steps_times, steps_size);
	struct timeval tval_before, tval_after;

	int32_t i,j,k,dd, from_step = 0;
	uint32_t new_size; uint32_t *ptr;
	automaton_automaton *minimization	= NULL;
	automaton_transition *current_transition	= NULL;
	transitions_kept_size = 0;
	uint32_t removed	= 0;
	automaton_automaton *inner_automaton	= automaton_automaton_clone(minimized);
	automaton_automaton *return_automaton	= NULL;

#if DEBUG_DD
	if(t_count < 256){
		printf("[");
		//remove transitions that are in the current partition
		for(i = 0; i < t_count; i++){
			if(TEST_BITVECTOR_BIT(partition_bit_vector, i))printf("▪");
			else printf("▫");
		}
		printf("]Partition vector\n");
		printf("[");
	}
#endif
	//remove transitions that are in the current partition
	for(i = 0; i < t_count; i++){
		current_transition	= &(master->transitions[t_states[i]][t_indexes[i]]);
		if(!(TEST_BITVECTOR_BIT(partition_bit_vector, i)) ){
			if(automaton_automaton_has_transition(inner_automaton, current_transition)
					&& inner_automaton->out_degree[t_states[i]] > t_indexes_count[i]){
				for(j = 0; j < t_indexes_count[i]; j++){
					removed++;
					automaton_automaton_remove_transition(inner_automaton, &(master->transitions[t_states[i]][t_indexes[i]+j]));
#if DEBUG_UNREAL && 0
					printf("Removing:");
					automaton_transition_print(&(master->transitions[t_states[i]][t_indexes[i]+j]), inner_automaton->context, "", "\n",0);
#endif
				}
#if DEBUG_DD
				if(t_count < 256)
					printf("▫");//curr.removed
#endif
			}
#if DEBUG_DD
			else if(t_count < 256){
				if(inner_automaton->out_degree[current_transition->state_from] == 1)
					printf("ˆ");//single trans.
				else
					printf("◦");//prev.removed
			}
#endif
		}else{
			transitions_kept_size++;
#if DEBUG_DD
			if(t_count < 256){
				if(inner_automaton->out_degree[current_transition->state_from] > 1){
					printf("▪");//kept
				}else{
					printf("x");//kept single trans.
				}
			}
#endif
		}
	}
#if DEBUG_DD
	if(t_count < 256)
		printf("]Kept transitions\n");
	else
		printf("Kept \t %d trans. from \t %d\n", transitions_kept_size, t_count);
				//automaton_automaton_print(inner_automaton, false, false, false, NULL, NULL);
#endif

#if DEBUG_UNREAL
	printf("\tDD, kept:[%d]\tn:[%d]\td(s_0):[%d]\n", transitions_kept_size,partitions_count,inner_automaton->out_degree[inner_automaton->initial_states[0]]);
#endif
	//in order to get the set of transitions for C_i or its complement the range is computed over kept_transitions
	//then a lineal run on the set of transitions is performed to reach the start of the range (could be kept updated
	//between iterations, the end of a range is the beginning of the next) and then from that point onwards
	//the transitions are preserved
	//start the search from where the last call succeded
#if DEBUG_UNREAL
			printf("Calling %s first with index %d\n", start_with_complement? "complement": "partition", start_with_complement? 0 : last_partition);
#endif
	if(!start_with_complement){
		return_automaton = automaton_get_gr1_unrealizable_minimization_dd2_c_i(master, minimization, inner_automaton, assumptions, assumptions_count, guarantees, guarantees_count
				, partition_bit_vector, transitions_kept_size, partitions_count, t_count, t_size, t_states, t_indexes, t_indexes_count, false, 0, steps
				, steps_sizes, steps_times, steps_size);
		if(return_automaton != NULL)return return_automaton;
		return_automaton = automaton_get_gr1_unrealizable_minimization_dd2_c_i_complement(master, minimization, inner_automaton, assumptions, assumptions_count, guarantees, guarantees_count
						, partition_bit_vector, transitions_kept_size, partitions_count, t_count, t_size, t_states, t_indexes, t_indexes_count, true, last_partition, steps
						, steps_sizes, steps_times, steps_size);
		if(return_automaton != NULL)return return_automaton;
	}else{
		return_automaton = automaton_get_gr1_unrealizable_minimization_dd2_c_i_complement(master, minimization, inner_automaton, assumptions, assumptions_count, guarantees, guarantees_count
				, partition_bit_vector, transitions_kept_size, partitions_count, t_count, t_size, t_states, t_indexes, t_indexes_count, true, last_partition, steps
				, steps_sizes, steps_times, steps_size);
		if(return_automaton != NULL)return return_automaton;
		return_automaton = automaton_get_gr1_unrealizable_minimization_dd2_c_i(master, minimization, inner_automaton, assumptions, assumptions_count, guarantees, guarantees_count
						, partition_bit_vector, transitions_kept_size, partitions_count, t_count, t_size, t_states, t_indexes, t_indexes_count, false, 0, steps
						, steps_sizes, steps_times, steps_size);
		if(return_automaton != NULL)return return_automaton;
	}


	//if n is less than the size of kept transitions perform recursive call with n = min(|kept|, 2 * n)
	if(partitions_count < transitions_kept_size){
		partitions_count = min(transitions_kept_size, 2 * partitions_count);
#if DEBUG_UNREAL
			printf("(refining n)\n");
#endif
		(*steps_sizes)[*steps]	= inner_automaton->transitions_composite_count;
		return_automaton	= automaton_get_gr1_unrealizable_minimization_dd2(master, inner_automaton, assumptions, assumptions_count, guarantees, guarantees_count
				, partition_bit_vector, transitions_kept_size, partitions_count, t_count, t_size, t_states, t_indexes, t_indexes_count, false, 0
				, steps, steps_sizes, steps_times, steps_size);
		automaton_automaton_destroy(inner_automaton);
		return return_automaton;
	}else{
#if DEBUG_UNREAL
		printf("(minimal)kept:%d\tremoved:%d\n", transitions_kept_size, removed);
#endif
		(*steps_sizes)[*steps]	= minimized->transitions_composite_count;
		if(automaton_is_gr1_realizable(inner_automaton, assumptions, assumptions_count,
				guarantees, guarantees_count) || inner_automaton->out_degree[inner_automaton->initial_states[0]] == 0){
			printf("Inner automaton was realizable, minimization %s\n", automaton_is_gr1_realizable(minimized, assumptions, assumptions_count,
					guarantees, guarantees_count)? "too" :"was not" );
			automaton_automaton_destroy(inner_automaton);
			inner_automaton = automaton_get_gr1_unrealizable_minimization(master, assumptions, assumptions_count,
					guarantees, guarantees_count, steps, steps_sizes, steps_times, steps_size);
			//exit(-1);
		}
		return inner_automaton;
	}
}

automaton_automaton *automaton_get_gr1_unrealizable_prepare_structs(automaton_automaton *game_automaton,
		uint32_t *t_size, uint32_t *t_count, uint32_t **t_states, uint32_t **t_indexes, uint32_t **t_indexes_count,
		uint32_t *r_size, uint32_t *r_count, uint32_t **r_states, uint32_t **r_indexes, uint32_t **r_indexes_count){
	//candidate transitions lists
	int32_t i,j,k;
	*t_size = LIST_INITIAL_SIZE, *t_count = 0;
	*t_states	= calloc(*t_size, sizeof(uint32_t));
	*t_indexes = calloc(*t_size, sizeof(uint32_t));
	*t_indexes_count = calloc(*t_size, sizeof(uint32_t));
	//transitions to remove lists
	*r_size = LIST_INITIAL_SIZE, r_count = 0;
	*r_states	= calloc(*r_size, sizeof(uint32_t));
	*r_indexes = calloc(*r_size, sizeof(uint32_t));
	*r_indexes_count = calloc(*r_size, sizeof(uint32_t));

	automaton_automaton_monitored_order_transitions(game_automaton);

	uint32_t new_size; uint32_t *ptr;
	automaton_automaton *master	= automaton_automaton_clone(game_automaton);
	master->source_type = (master->source_type & ~(SOURCE_STRAT)) | SOURCE_DIAG;
	automaton_automaton_add_initial_state(master, game_automaton->initial_states[0]);
	automaton_transition *current_transition	= NULL;

	bool new_monitored = false, was_set	= false;
	uint32_t to_state;
	int32_t last_t_index	= 0, current_range;

	//add all options, if only one monitored option is available per state do not add
	for(i = 0; i < master->transitions_count; i++){
		new_monitored	= false;
		last_t_index = 0;
		was_set	= false;
		for(j = 0; j < game_automaton->out_degree[i]; j++){
			if(j > 0)
				new_monitored = !(automaton_automaton_transition_monitored_eq(game_automaton,
						&(game_automaton->transitions[i][j - 1]),
						&(game_automaton->transitions[i][j])));
			if(new_monitored){

				//add set of options to t_states, t_indexes
				if(*t_count >= (*t_size - 2)){//always leave room for one more item for check after loop
					new_size	= (*t_count + 2) * LIST_INCREASE_FACTOR;
					ptr	= realloc(*t_states, new_size * sizeof(uint32_t));
					if(ptr == NULL){printf("Could not allocate memory[automaton_get_gr1_unrealizable_minimization:1]\n"); exit(-1);}
					*t_states	= ptr;
					ptr	= realloc(*t_indexes, new_size * sizeof(uint32_t));
					if(ptr == NULL){printf("Could not allocate memory[automaton_get_gr1_unrealizable_minimization:2]\n"); exit(-1);}
					*t_indexes	= ptr;
					ptr	= realloc(*t_indexes_count, new_size * sizeof(uint32_t));
					if(ptr == NULL){printf("Could not allocate memory[automaton_get_gr1_unrealizable_minimization:3]\n"); exit(-1);}
					*t_indexes_count	= ptr;
					*t_size		= new_size;
				}
				current_range		= j - last_t_index;
				(*t_states)[*t_count]	= i;
				(*t_indexes)[*t_count]	= last_t_index;
				(*t_indexes_count)[*t_count]	= current_range;
				*t_count	= *t_count + 1;
				last_t_index	= j;
				new_monitored	= false;
				was_set			= true;
			}
		}
		//if never set by new_monitored don't add to t_states, t_indexes (was only monitored option)
		if(was_set){
			//add set of options to t_states, t_indexes
			current_range		= game_automaton->out_degree[i] - last_t_index;
			(*t_states)[*t_count]	= i;
			(*t_indexes)[*t_count]	= last_t_index;
			(*t_indexes_count)[*t_count]	= current_range;
			*t_count	= *t_count + 1;
			last_t_index	= j;
		}

	}
#if DEBUG_UNREAL
	printf("[Recreated indexes][from_state:index:inner_index](trans)\n");
	for(i = 0; i < *t_count; i++){
		for(j = 0; j < (*t_indexes_count)[i]; j++){
			printf("[%d:%d:%d]", (*t_states)[i], i, (*t_indexes)[i] + j);
			automaton_transition_print(&(game_automaton->transitions[(*t_states)[i]][(*t_indexes)[i] + j]), game_automaton->context, "","\n",0);
		}
	}
	printf("\n");
#endif
	//automaton_automaton_remove_deadlocks(master);
	automaton_automaton_remove_unreachable_states(master);
	automaton_automaton_update_valuations(master);
	return master;
}

automaton_automaton* automaton_get_gr1_unrealizable_minimization(automaton_automaton* game_automaton, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count, uint32_t *steps, uint32_t **steps_sizes
		, struct timeval **steps_times, uint32_t *steps_size){
	//candidate transitions lists
	int32_t i,j,k, from_step = 0;
	uint32_t t_size, t_count;
	uint32_t *t_states, *t_indexes, *t_indexes_count;
	//transitions to remove lists
	uint32_t r_size, r_count = 0;
	uint32_t *r_states, *r_indexes, *r_indexes_count;

	automaton_automaton *master = automaton_get_gr1_unrealizable_prepare_structs(game_automaton, &t_size, &t_count, &t_states, &t_indexes, &t_indexes_count,
			 &r_size, &r_count, &r_states, &r_indexes, &r_indexes_count);

	uint32_t new_size; uint32_t *ptr;

	automaton_transition *current_transition	= NULL;
/*
	bool new_monitored	= false;
	uint32_t to_state;
	int32_t last_t_index	= 0, current_range;

	//add all options, if only one monitored option is available per state do not add
	for(i = 0; i < master->transitions_count; i++){
		new_monitored	= false;
		for(j = 0; j < game_automaton->out_degree[i]; j++){
			if(j > 0)
				new_monitored = !(automaton_automaton_transition_monitored_eq(game_automaton,
						&(game_automaton->transitions[i][j - 1]),
						&(game_automaton->transitions[i][j])));
			if(new_monitored){

				//add set of options to t_states, t_indexes
				if(t_count == (t_size - 1)){//always leave room for one more item for check after loop
					new_size	= t_size * LIST_INCREASE_FACTOR;
					ptr	= realloc(t_states, new_size * sizeof(uint32_t));
					if(ptr == NULL){printf("Could not allocate memory[automaton_get_gr1_unrealizable_minimization:1]\n"); exit(-1);}
					t_states	= ptr;
					ptr	= realloc(t_indexes, new_size * sizeof(uint32_t));
					if(ptr == NULL){printf("Could not allocate memory[automaton_get_gr1_unrealizable_minimization:2]\n"); exit(-1);}
					t_indexes	= ptr;
					ptr	= realloc(t_indexes_count, new_size * sizeof(uint32_t));
					if(ptr == NULL){printf("Could not allocate memory[automaton_get_gr1_unrealizable_minimization:3]\n"); exit(-1);}
					t_indexes_count	= ptr;
					t_size		= new_size;
				}
				current_range		= j - last_t_index;
				t_states[t_count]	= i;
				t_indexes[t_count]	= last_t_index;
				t_indexes_count[t_count++]	= current_range;
				last_t_index	= j;
			}
		}
		//if never set by new_monitored don't add to t_states, t_indexes (was only monitored option)
		if(new_monitored){
			//add set of options to t_states, t_indexes
			current_range		= game_automaton->out_degree[i] - last_t_index;
			t_states[t_count]	= i;
			t_indexes[t_count]	= last_t_index;
			t_indexes_count[t_count++]	= current_range;
			last_t_index	= j;
		}

	}
	//automaton_automaton_remove_deadlocks(master);
	automaton_automaton_remove_unreachable_states(master);
	automaton_automaton_update_valuations(master);
	*/
	automaton_automaton *minimization	= automaton_automaton_clone(master);

	bool minimized	= false;
	struct timeval tval_before, tval_after;
	while(t_count > 0){
		(*steps)++;
		automaton_minimization_adjust_steps_report(steps, steps_sizes, steps_times, steps_size);
		gettimeofday(&tval_before, NULL);
		//get next candidate
		//do not consider missing transitions or transitions that would induce deadlocks
		while(minimization->out_degree[t_states[t_count - 1]] == 1){
			t_count--;
			if(t_count == 0){
				minimized = true;
				break;
			}
		}
		if(minimized){
			/*
			automaton_automaton_destroy(minimization);
			minimization = automaton_automaton_clone(master);
			for(i =0 ; i < r_count; i++){
				for(j = 0; j < r_indexes_count[i]; j++){
					current_transition	= &(master->transitions[r_states[i]][r_indexes[i]+j]);
					automaton_automaton_remove_transition(minimization, current_transition);
				}
			}*/
			break;
		}
		//evaluate next reduction
		if(r_count == r_size){
			new_size	= r_size * LIST_INCREASE_FACTOR;
			ptr	= realloc(r_states, new_size * sizeof(uint32_t));
			if(ptr == NULL){printf("Could not allocate memory[automaton_get_gr1_unrealizable_minimization:4]\n"); exit(-1);}
			r_states	= ptr;
			ptr	= realloc(r_indexes, new_size * sizeof(uint32_t));
			if(ptr == NULL){printf("Could not allocate memory[automaton_get_gr1_unrealizable_minimization:5]\n"); exit(-1);}
			r_indexes	= ptr;
			ptr	= realloc(r_indexes_count, new_size * sizeof(uint32_t));
			if(ptr == NULL){printf("Could not allocate memory[automaton_get_gr1_unrealizable_minimization:6]\n"); exit(-1);}
			r_indexes_count	= ptr;
			r_size		= new_size;
		}
		if(minimization->out_degree[t_states[t_count -1]] > t_indexes_count[t_count - 1]){
			r_states[r_count]	= t_states[t_count-1];
			r_indexes[r_count]	= t_indexes[t_count-1];
			r_indexes_count[r_count++]	= t_indexes_count[t_count-1];

			for(j = 0; j < r_indexes_count[r_count - 1] ; j++){
				current_transition	= &(master->transitions[r_states[r_count - 1]][r_indexes[r_count - 1]+j]);
				automaton_automaton_remove_transition(minimization, current_transition);
#if DEBUG_UNREAL && 0
			printf("Removing:");
			automaton_transition_print(current_transition, minimization->context, "", "\n",0);
#endif
			}
		}
		t_count--;
		//automaton_automaton_remove_deadlocks(minimization);
		automaton_automaton_remove_unreachable_states(minimization);
		automaton_automaton_update_valuations(minimization);

		if(automaton_is_gr1_realizable(minimization, assumptions, assumptions_count,
				guarantees, guarantees_count) || minimization->out_degree[minimization->initial_states[0]] == 0){
			r_count--;
			automaton_automaton_destroy(minimization);
			minimization = automaton_automaton_clone(master);
			for(i =0 ; i < r_count; i++){
				for(j = 0; j < r_indexes_count[i]; j++){
					current_transition	= &(master->transitions[r_states[i]][r_indexes[i]+j]);
					automaton_automaton_remove_transition(minimization, current_transition);
				}
			}
			//automaton_automaton_remove_deadlocks(minimization);
			automaton_automaton_remove_unreachable_states(minimization);
			automaton_automaton_update_valuations(minimization);
#if DEBUG_UNREAL
			printf("Minimizing [%d,%d,R,%d](Real.)\n", t_count, r_count,steps);
#endif
		}else{
			/*
			automaton_automaton_destroy(last_unrealizable);
			last_unrealizable = automaton_automaton_clone(minimization);
			*/
			//automaton_automaton_remove_deadlocks(minimization);
			automaton_automaton_remove_unreachable_states(minimization);
			automaton_automaton_update_valuations(minimization);
			from_step = steps;
#if DEBUG_UNREAL
			printf("Minimizing [%d,%d,N,%d](Unreal.)\n", t_count, r_count, from_step);
#endif
		}
		gettimeofday(&tval_after, NULL);
		timersub(&tval_after, &tval_before, &((*steps_times)[*steps]));
		(*steps_sizes)[*steps]	= minimization->transitions_composite_count;
		fflush(stdout);
	}



	if(automaton_is_gr1_realizable(minimization, assumptions, assumptions_count,
				guarantees, guarantees_count) || minimization->out_degree[minimization->initial_states[0]] == 0){
		automaton_automaton_destroy(minimization);
		minimization = automaton_automaton_clone(master);
		for(i =0 ; i < r_count; i++){
			for(j = 0; j < r_indexes_count[i]; j++){
				current_transition	= &(master->transitions[r_states[i]][r_indexes[i]+j]);
				automaton_automaton_remove_transition(minimization, current_transition);
			}
		}
		//automaton_automaton_remove_deadlocks(minimization);
		automaton_automaton_remove_unreachable_states(minimization);
		automaton_automaton_update_valuations(minimization);
#if DEBUG_UNREAL
		printf("[%d,%d,R,%d]\n", t_count, r_count,steps);
		printf("[%d,%d]Returning restored from %d\n", t_count, r_count, from_step);
	}else{
		printf("[%d,%d]Returning last from %d\n", t_count, r_count, from_step);
#endif
	}

	free(t_states); free(t_indexes); free(t_indexes_count);
	free(r_states); free(r_indexes); free(r_indexes_count);
	//print rankings
	automaton_automaton* strategy = automaton_get_gr1_strategy(minimization, assumptions, assumptions_count,
			guarantees, guarantees_count, true);
	automaton_automaton_destroy(strategy);

	automaton_automaton_destroy(master);
#if DEBUG_UNREAL
	automaton_automaton_print(minimization, false, false, false, "", "");
#endif
	return minimization;
}

void automaton_automaton_print_traces_to_deadlock(automaton_automaton* automaton, uint32_t max_traces){
	uint32_t count;
	uint32_t printed_traces = 0;
	uint32_t *sizes;
	uint32_t **traces	= automaton_automaton_traces_to_deadlock(automaton, automaton->initial_states[0], &count, &sizes);
	if(traces == NULL)return;
	uint32_t i, j, k,l;
	uint32_t current_state, next_state;
	automaton_transition *current_transition;
	if(count > 0){
		printf("Traces to deadlock in %s\n==============\n", automaton->name);
		for(i = 0; i < count; i++){
			for(j = 0; j < sizes[i]; j++){
				current_state	= traces[i][j];
				if(j < (sizes[i] - 1)){
					printf("[%d]<", current_state);
					next_state	= traces[i][j + 1];
					for(k = 0; k < automaton->out_degree[current_state]; k++){
						current_transition	= &(automaton->transitions[current_state][k]);
						if(current_transition->state_to == next_state){
							bool first_print = true;
							for(l = 0; l < (TRANSITION_ENTRY_SIZE * FIXED_SIGNALS_COUNT) - 1; l++){
								if(TEST_TRANSITION_BIT(current_transition, l)){
									if(first_print)first_print = false;
									else printf(",");
									printf("%s", automaton->context->global_alphabet->list[l].name);
								}
							}
						}
					}
					printf(">");
				}else{
					printf("[%d](x)\n", current_state);
				}
			}
			printed_traces++;
			if(printed_traces > max_traces && max_traces != 0)
				break;
		}
	}
	free(sizes);
	for(i = 0; i < count; i++)free(traces[i]);
	free(traces);
}

uint32_t** automaton_automaton_traces_to_deadlock(automaton_automaton* automaton, uint32_t initial_state, uint32_t *count, uint32_t **sizes){
	if(initial_state >= automaton->transitions_count){
		printf("Initial state %d higher than transitions count %d\n", initial_state, automaton->transitions_count);
		*count 				= 0;
		return NULL;
	}

	*count 				= 0;
	uint32_t* distances	= calloc(automaton->transitions_count, sizeof(uint32_t));
	bool* visited		= calloc(automaton->transitions_count, sizeof(bool));
	uint32_t* visited_through = calloc(automaton->transitions_count, sizeof(uint32_t));
	uint32_t* deadlocks = calloc(automaton->transitions_count, sizeof(uint32_t));
	automaton_concrete_bucket_list* pending_list		= automaton_concrete_bucket_list_create(DISTANCE_BUCKET_SIZE, automaton_int_extractor, sizeof(uint32_t));
	int32_t i, j;
	uint32_t min_distance;
	automaton_transition* current_transition;
	uint32_t current_state;
	//add initial states (excluding incoming state)
	visited[initial_state]		= true;
	for(i = 0; i < automaton->out_degree[initial_state]; i++){
		current_transition	= &(automaton->transitions[initial_state][i]);
		if(current_transition->state_to != initial_state && !visited[current_transition->state_to]){
			automaton_concrete_bucket_add_entry(pending_list, &(current_transition->state_to));
			visited_through[current_transition->state_to]	= initial_state;
		}
	}
	//update distances
	while(pending_list->composite_count > 0){
		automaton_concrete_bucket_pop_entry(pending_list, &current_state);
		if(visited[current_state])continue;
		visited[current_state]	= true;
		min_distance			= 0;
		for(i = 0; i < automaton->in_degree[current_state]; i++){
			current_transition	= &(automaton->inverted_transitions[current_state][i]);
			if(!visited[current_transition->state_from])continue;
			if(min_distance == 0 || (current_transition->state_from + 1 < min_distance))min_distance	= distances[current_transition->state_from] + 1;
		}
		distances[current_state]	= min_distance;
		if(automaton->out_degree[current_state] == 0){
			deadlocks[*count]	= current_state;
			*count += 1;
		}
		for(i = 0; i < automaton->out_degree[current_state]; i++){
			current_transition	= &(automaton->transitions[current_state][i]);
			if(!visited[current_transition->state_to]){
				automaton_concrete_bucket_add_entry(pending_list, &(current_transition->state_to));
				visited_through[current_transition->state_to]	= current_state;
			}
		}

	}

	automaton_concrete_bucket_destroy(pending_list, true);

	uint32_t **deadlock_traces = calloc(*count, sizeof(uint32_t*));
	uint32_t *local_sizes	= calloc(*count,sizeof(uint32_t));
	uint32_t current_length;
	uint32_t last_state;
	for(i = 0; i < *count; i++){
		current_length	= distances[deadlocks[i]] + 1;
		local_sizes[i] 	= current_length;
		last_state		= deadlocks[i];
		deadlock_traces[i]	= calloc((current_length), sizeof(uint32_t));
		deadlock_traces[i][current_length - 1]	= last_state;
		for(j = current_length - 2; j >= 0 ; j--){
			last_state = deadlock_traces[i][j]	=  visited_through[last_state];
		}
	}
	free(visited);
	free(visited_through);
	free(deadlocks);
	free(distances);
	*sizes	= local_sizes;
	return deadlock_traces;
}



