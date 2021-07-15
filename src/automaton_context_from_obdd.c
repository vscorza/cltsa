/*
 * automaton_context_from_obdd.c
 *
 *  Created on: Apr 16, 2021
 *      Author: mariano
 */
#include "automaton_context.h"



/*
 * TAKE into consideration that valuation will bet ordered following XYX'Y' layout while partial_valuation
 * follows the dictionary order from the obdd manager
 * if it is initial and input it will receive a valuation over X and it will store it on X
 * if it is initial and output it will receive a valuation over XY and it will store it on XY
 * if it is rho and input it will receive a valuation over XYX' and it will store X' on the X' of a XYX' state
 * if it is rho and output it will receive a valuation over XYX'Y' and it will store X'Y' on the XY of an XY
 *
 * @param valuation the composed valuation to be set according to the particular case
 * @param partial valuation the partial valuation from where the values will be taken
 * @param valuation_offset the index where the current valuation lies within the consecutive array of valuations
 * @param is_initial true if the valuation correspond to the interpretation of an initial formula
 * @param x_count number of input variables in the specification
 * @param y_count number of output variables in the specification
 */
void automaton_set_composed_valuation(bool* valuation, bool* partial_valuation
		, uint32_t x_y_count){
	uint32_t i;
	for(i = 0; i < x_y_count; i++)valuation[i]	= partial_valuation[x_y_count + i];
}

uint64_t __global_signals_count = 0;
uint32_t __global_max_signals	= 0;
bool automaton_add_transition_from_valuations(obdd_mgr* mgr, automaton_automaton* automaton, uint32_t from_state, uint32_t to_state, bool* from_valuation,
		bool* to_valuation, bool is_initial, bool is_input, uint32_t x_y_count, uint32_t x_count, uint32_t* obdd_on_indexes, uint32_t* obdd_off_indexes,
		uint32_t* x_y_alphabet, uint32_t* x_y_order){
	uint32_t i, j, fluent_index, fluent_count	= automaton->context->liveness_valuations_count;
	automaton_transition* transition		= automaton_transition_create(from_state, to_state);
	//TODO: optimize alphabet indexes computation
#if DEBUG_LTL_AUTOMATON
	printf("(");
	for(i = 0; i < x_y_count; i++)
		printf("%s", from_valuation[i]?"1":"0");
	printf("):%d-[", from_state);
#endif
	uint32_t signal_dict_index, valuation_index;
	uint32_t var_count = x_y_count;
	bool take_on;
#if DEBUG_LTL_AUTOMATON
	bool has_action = false;
#endif
	for(i = 0; i < var_count; i++){
		if(!is_initial){
			if(from_valuation[i] != to_valuation[i]){
#if DEBUG_LTL_AUTOMATON
				has_action 	= true;
#endif
				take_on				= from_valuation[i]; //is_input? from_valuation[left_offset + i] : !from_valuation[left_offset + i];
				signal_dict_index	= (take_on? obdd_off_indexes[i]: obdd_on_indexes[i]);
				automaton_transition_add_signal_event_ID(transition, automaton->context, signal_dict_index, i > (2 * (x_count + 1))? OUTPUT_SIG : INPUT_SIG);
#if DEBUG_LTL_AUTOMATON
				printf("(%d:%s:%s)%s", signal_dict_index, automaton->context->global_alphabet->list[signal_dict_index].name,
						take_on? "OFF" : "ON", i < var_count - 1 ? "," : "");
#endif
			}
		}else{
#if DEBUG_LTL_AUTOMATON
			has_action 	= true;
#endif
			take_on		= to_valuation[i];
			if(take_on){
				signal_dict_index	= (take_on? obdd_on_indexes[i]: obdd_off_indexes[i]);
				automaton_transition_add_signal_event_ID(transition, automaton->context, signal_dict_index, i > (2 * (x_count + 1))? OUTPUT_SIG : INPUT_SIG);
#if DEBUG_LTL_AUTOMATON
				printf("(%d:%s:%s)%s", signal_dict_index, automaton->context->global_alphabet->list[signal_dict_index].name,
						take_on? "ON" : "OFF", i < var_count - 1 ? "," : "");
#endif
			}

		}
	}
#if DEBUG_LTL_AUTOMATON
	printf("%s]->(", !has_action? "__tau__" : "");
	for(i = 0; i < var_count; i++)
		printf("%s", to_valuation[i]?"1":"0");
	printf(")%d",to_state);
#endif
	/*
	if(is_input){
		TRANSITION_SET_INPUT(transition);
	}else{
		TRANSITION_CLEAR_INPUT(transition);
	}*/
	bool has_transition = automaton_automaton_has_transition(automaton, transition);
	if(!has_transition){
		automaton_automaton_add_transition(automaton, transition);
#if DEBUG_LTL_AUTOMATON
		automaton_transition_print(transition, automaton->context, "[T+]","\n", 0);
#endif
	}
#if DEBUG_LTL_AUTOMATON
	printf("%s", has_transition? "" : "*");
#endif
#if VERBOSE || DEBUG_LTL_AUTOMATON
	GET_TRANSITION_SIGNAL_COUNT(transition);
	__global_signals_count += signal_count;
	if(signal_count > __global_max_signals){
		__global_max_signals = signal_count;
	}
	if(__global_signals_count % 100000 == 0){
		printf("transition load: %" PRIu64 "\t(%d / %d) max: %d\n", __global_signals_count / automaton->transitions_composite_count
				, __global_signals_count, automaton->transitions_composite_count, __global_max_signals);
	}
#endif
	automaton_transition_destroy(transition, true);
	//TODO:check this
	//should add after adding transition since structure resizing may not have been triggered
	bool obdd_sat_vector = false;
	if(!has_transition){
#if DEBUG_LTL_AUTOMATON
	printf("(");
	for(i = 0; i < x_y_count; i++)
		printf("%s", to_valuation[i]?"1":"0");
	printf(")(");
	for(i = 0; i < x_y_count; i++)
		printf("%d", x_y_alphabet[i]);
	printf(")V:");
#endif
		for(i = 0; i < fluent_count; i++){
			fluent_index	= GET_STATE_FLUENT_INDEX(fluent_count, to_state, i);
			//evaluate each liveness formula on landing state to check if formula is satisfied and then set bit
			if(!is_initial){
				obdd_sat_vector	= obdd_satisfies_vector(automaton->context->liveness_valuations[i],
						x_y_alphabet, to_valuation, x_y_count, x_y_order);
				if(obdd_sat_vector){
#if DEBUG_LTL_AUTOMATON
					printf("1");
#endif
						SET_FLUENT_BIT(automaton->liveness_valuations, fluent_index);
				}else{
#if DEBUG_LTL_AUTOMATON
					printf("0");
#endif
					CLEAR_FLUENT_BIT(automaton->liveness_valuations, fluent_index);
				}
			}
		}
	}
#if DEBUG_LTL_AUTOMATON
	printf("\n");
#endif
	return has_transition;
}


void automaton_add_transition_from_obdd_valuation(obdd_mgr* mgr, automaton_automaton* ltl_automaton,
		obdd_composite_state* from_state, obdd_composite_state* to_state, obdd_state_tree* obdd_state_map,
		uint32_t x_y_count, uint32_t x_count,
		uint32_t* x_y_alphabet, uint32_t* x_y_order,
		uint32_t signals_count, bool* valuation, bool* hashed_valuation,
		automaton_bool_array_hash_table* x_y_hash_table,
		uint32_t* obdd_on_signals_indexes, uint32_t* obdd_off_signals_indexes){
	uint32_t j;
	bool has_transition;
	//xy ->xy'
	//automaton_set_composed_valuation(env_state->valuation, valuations, i, false, true, x_y_x_p_alphabet, x_count, y_count);
	for(j = 0; j < x_y_count; j++)to_state->valuation[j]	= valuation[j*2+1];
	for(j = 0; j < x_y_count; j++)from_state->valuation[j]	= valuation[j*2];
#if DEBUG_LTL_AUTOMATON
	printf("Original val.:");
	for(j = 0; j < x_y_count * 2; j++)printf("%d",valuation[j]);
	printf(" From val.:");
	for(j = 0; j < x_y_count; j++)printf("%d",from_state->valuation[j]);
	printf(" To val.:");
	for(j = 0; j < x_y_count; j++)printf("%d",to_state->valuation[j]);
	printf("\n");
#endif
	hashed_valuation		= automaton_bool_array_hash_table_add_or_get_entry(x_y_hash_table, to_state->valuation, true);
	from_state->state		= obdd_state_tree_get_key(obdd_state_map, from_state->valuation, x_y_count);
	to_state->state			= obdd_state_tree_get_key(obdd_state_map, to_state->valuation, x_y_count);
	has_transition	= automaton_add_transition_from_valuations(mgr, ltl_automaton, from_state->state, to_state->state
			, from_state->valuation, hashed_valuation,false, true
			, x_y_count, x_count
			, obdd_on_signals_indexes, obdd_off_signals_indexes, x_y_alphabet
			, x_y_order);


#if DEBUG_LTL_AUTOMATON
	printf("(%d-[", from_state->state);
	for(j = 0; j < x_y_count; j++)
		printf("%s", to_state->valuation[j] ? "1" : "0");
	printf("]->%d)%s", to_state->state, has_transition? "" : "*");
#endif
	if(!has_transition){
#if DEBUG_LTL_AUTOMATON
	printf("[ADDED]");
#endif
	}
}

void automaton_add_transitions_from_valuations(obdd_mgr* mgr, obdd* root, automaton_automaton* automaton, uint32_t* valuations_count,
		bool* dont_care_list, bool* partial_valuation, bool* initialized_values, bool* valuation_set, obdd_node** last_nodes,
		obdd_composite_state* from_state, obdd_composite_state* to_state, obdd_state_tree* obdd_state_map, uint32_t x_y_count, uint32_t x_count,
		uint32_t* x_y_alphabet, uint32_t* x_y_order, uint32_t signals_count, bool* hashed_valuation, automaton_bool_array_hash_table* x_y_hash_table,
		uint32_t* obdd_on_signals_indexes, uint32_t* obdd_off_signals_indexes){
	int32_t i, j, dont_cares_count = 0, variable_index;
	uint32_t nodes_count;
	*valuations_count			= 0;
	uint32_t variables_count	= mgr->vars_dict->size - 2;
	bool* valuation	= calloc(variables_count, sizeof(bool));
	int32_t last_bit_index		= -1;
	for( i = 0; i < (int32_t)variables_count; i++){
		dont_care_list[i]		= true;
		partial_valuation[i]	= false;
		initialized_values[i]	= false;
		last_nodes[i]			= NULL;
	}
	int32_t last_node_index	= 0;
	//starts from the root and explore until true leaf is found
	obdd_node* current_node		= root->root_obdd;
	obdd_node* last_node;


	if(current_node->var_ID == mgr->false_obdd->root_obdd->var_ID){
		free(valuation);
		return;
	}
	//solve case where var_ID is 0 (obdd == true, retrieve all values for img)
	if(current_node->var_ID == mgr->true_obdd->root_obdd->var_ID){
		dont_cares_count	= 1;
		for(i = 0; i < x_y_count; i++)
			dont_cares_count *= 2;
		//get new valuations according to dont care list
		uint32_t modulo	= dont_cares_count;

#if DEBUG_OBDD_VALUATIONS
						printf("Partial Valuation\t<");
				for(i = 0; i < (int32_t)variables_count; i++)
					printf("%s", partial_valuation[i]? "1" : "0");
				printf(">\n");
				printf("Dont care list\t<");
				for(i = 0; i < (int32_t)signals_count; i++){
					printf("%s", dont_care_list[i]?  "?" : "0");
				}
				printf("> count:%d\n", dont_cares_count);
#endif

		uint32_t k;
		uint32_t img_index;
		for(k = 0; k < (uint32_t)dont_cares_count; k++){
			for(i = 0; i < (int32_t)variables_count; i++)
				valuation_set[i] = false;
			last_bit_index = -1;
			for(i = 0; i < (int32_t)variables_count; i++){
				last_bit_index++;
				//set according to modulo division if current position was set to dont care
				valuation[i]	= (k & (0x1 << last_bit_index)) != 0;
			}
			automaton_add_transition_from_obdd_valuation(mgr, automaton, from_state, to_state, obdd_state_map,
					x_y_count, x_count, x_y_alphabet, x_y_order, signals_count, valuation,
					hashed_valuation, x_y_hash_table, obdd_on_signals_indexes, obdd_off_signals_indexes);
#if DEBUG_OBDD_VALUATIONS
			printf("[");
			for(i = 0; i < (int32_t)variables_count; i++){
				bool value = GET_VAR_IN_VALUATION((valuation), variables_count, *valuations_count + k, i);
				bool care_for_value = dont_care_list[i];
				printf("%s", value ? (care_for_value ? "i" : "1") : (care_for_value ? "o" : "0"));
			}
			printf("]\n");
#endif
		}
		*valuations_count	+= dont_cares_count;
		free(valuation);
		return;
	}
	int32_t current_index		= current_node->var_ID - 2;
	int32_t next_index			= 0;
	bool taking_high			= false;
	bool found_node_to_expand	= false;
#if DEBUG_OBDD_VALUATIONS
	printf(ANSI_COLOR_GREEN);
#endif

	//update the current branch list
	last_nodes[current_index]	= current_node;
	dont_care_list[current_index]	= false;

	while(partial_valuation[current_index] != true && current_node != NULL){
		if(!initialized_values[current_index]){
			initialized_values[current_index]	= true;
			taking_high	= false;
		}else{
			partial_valuation[current_index]	= true;
			taking_high	= true;
		}
		dont_care_list[current_index]	= false;

		if(current_node->var_ID > 1){
			if(taking_high)current_node		= current_node->high_obdd;
			else	current_node			= current_node->low_obdd;
		}
		if(current_node->var_ID > 1){
			next_index					= current_node->var_ID - 2;
			//update range of variables not fixed by current branch
			for(i = (current_index + 1); i < (next_index - 1); i++)
				dont_care_list[i]		= true;
			current_index				= next_index;
			last_nodes[current_index]	= current_node;
		}else{
			//update range of variables not fixed by current branch
			for(i = (current_index + 1); i < (int32_t)variables_count; i++)
				dont_care_list[i]		= true;
			if(current_node->var_ID == mgr->false_obdd->root_obdd->var_ID){//wrong branch
				//printf("\n[!!!!!]Should not be exploring this branch\n\n");
			}else if(current_node->var_ID == mgr->true_obdd->root_obdd->var_ID){//found terminal
				//add valuations
				dont_cares_count	= 1;
				//count dont cares to get number of new valuations (only for variables belonging to the image)
				for(i = 0; i < (int32_t)variables_count; i++){
					if(dont_care_list[i]){
							dont_cares_count *= 2;
					}
				}
				uint32_t modulo	= dont_cares_count;

#if DEBUG_OBDD_VALUATIONS
				printf("[T]erminals on node: %d (%d:%s) :\n", last_node_index, last_nodes[current_index]->var_ID,dictionary_key_for_value(mgr->vars_dict,last_nodes[current_index]->var_ID));

				for(i = 0; i <= (int32_t)current_index; i++)
					printf("%s", dont_care_list[i]? "?" : (partial_valuation[i]? "1" : "0"));
				for(i = variables_count - 1; i > current_index; i--)
					printf("x");
				printf(">\t");
				printf("Partial Valuation\t<");
				for(i = 0; i < (int32_t)variables_count; i++)
					printf("%s", partial_valuation[i]? "1" : "0");
				printf(">\t");
				printf("Dont care list\t<");
				for(i = 0; i < (int32_t)variables_count; i++){
					variable_index	= -1;
					for(j = 0; j < (int32_t)x_y_count; j++){
						if(x_y_alphabet[j] == (uint32_t)i + 2){
							variable_index	= x_y_alphabet[j] - 2;
							break;
						}
					}
					printf("%s", dont_care_list[i]? (variable_index > -1 ? "?" : "_") : "0");
				}
				printf("> count:%d\n", dont_cares_count);
#endif

				uint32_t k;
				uint32_t img_index;
				for(k = 0; k < (uint32_t)dont_cares_count; k++){
					for(i = 0; i < (int32_t)variables_count; i++)
						valuation_set[i] = true;
					last_bit_index = -1;
					for(i = 0; i < (int32_t)variables_count; i++){
						if(dont_care_list[i]){
							last_bit_index++;
							//set according to modulo division if current position was set to dont care
							valuation[i]	= (k & (0x1 << last_bit_index)) != 0;
						}else{
							//set to predefined value for this search
							valuation[i]	= partial_valuation[i];
						}
					}
					automaton_add_transition_from_obdd_valuation(mgr, automaton, from_state, to_state, obdd_state_map,
							x_y_count, x_count, x_y_alphabet, x_y_order,signals_count, valuation,
							hashed_valuation, x_y_hash_table,
							obdd_on_signals_indexes, obdd_off_signals_indexes);
#if DEBUG_OBDD_VALUATIONS
					printf("[");
					for(i = 0; i < (int32_t)variables_count; i++){
						bool care_for_value = dont_care_list[i];
						printf("%s", valuation[i] ? (care_for_value ? "i" : "1") : (care_for_value ? "o" : "0"));
					}
					printf("][");
					for(i = 0; i < (int32_t)variables_count; i++){
						if(valuation[i])
							printf("%s ", mgr->vars_dict->entries[i+2].key);
					}

					printf("]\n");
#endif
				}


				*valuations_count	+= dont_cares_count;
			}//ADD VALUATIONS

			//fire backtrack, check if a node needs to be expanded
			uint32_t previous_index	= current_index;
			while(current_index > 0 && (partial_valuation[current_index] == true || dont_care_list[current_index])){
				current_index--;
			}
			current_node				= last_nodes[current_index];
			//reset backtracked values
			for(i = current_index + 1; i <= previous_index; i++){
				dont_care_list[i]		= true;
				partial_valuation[i]	= false;
				initialized_values[i]	= false;
			}
		}
	}
#if DEBUG_OBDD_VALUATIONS
	printf(ANSI_COLOR_RESET);
#endif
	free(valuation);
}


automaton_automaton* automaton_build_automaton_from_obdd(automaton_automata_context* ctx, char* name, obdd** env_theta_obdd, uint32_t env_theta_count, obdd** sys_theta_obdd, uint32_t sys_theta_count
		, obdd** env_rho_obdd, uint32_t env_rho_count, obdd** sys_rho_obdd, uint32_t sys_rho_count, automaton_parsing_tables* tables){
	//remember that if automaton was built from ltl its valuations should be added when building it
	//and should be kept when composing it, if several automata are to be composed from ltl their composed valuation
	//equals to the conjunction of the components' valuations
	obdd_mgr* mgr						= parser_get_obdd_mgr();
	obdd_state_tree* state_map			= obdd_state_tree_create(mgr->vars_dict->size);
	uint32_t local_alphabet_count		= (mgr->vars_dict->size - 2);
	uint32_t i, j, current_element 		= 0;
	uint32_t* local_alphabet			= malloc(sizeof(uint32_t) * local_alphabet_count);
	char current_dict_entry[255];
	parser_get_primed_variables();

	uint32_t x_count = 0, y_count = 0, x_y_count = 0, signals_count = 0;
	uint32_t alphabet_element_index;
	int32_t global_index;
	bool is_primed;
	char current_key[255];
	bool is_input;
	uint32_t* obdd_on_signals_indexes	= malloc(sizeof(uint32_t) * ((mgr->vars_dict->size - 2)/2));
	uint32_t* obdd_off_signals_indexes	= malloc(sizeof(uint32_t) * ((mgr->vars_dict->size - 2)/2));
	uint32_t obdd_on_count  = 0, obdd_off_count = 0;
	/**
	 * BUILD LOCAL ALPHABET
	 */
#if VERBOSE
	printf("OBDD->Automaton\nBuilding Alphabets\n");
#endif
	for(i = 0; i < mgr->vars_dict->size; i++){
		if((strcmp(mgr->vars_dict->entries[i].key, TRUE_VAR) == 0) || (strcmp(mgr->vars_dict->entries[i].key, FALSE_VAR) == 0))
			continue;
		alphabet_element_index	= mgr->vars_dict->entries[i].value;
		is_primed				= false;
		for(j = 0; j < parser_primed_variables_count; j++)
			if(parser_primed_variables[j] == alphabet_element_index){
				is_primed	= true;
				break;
			}

		signals_count++;
		if(!is_primed){
			strcpy(current_key, mgr->vars_dict->entries[i].key);
			strcat(current_key, SIGNAL_ON_SUFFIX);
			global_index	= automaton_alphabet_get_value_index(ctx->global_alphabet, current_key);
			is_input		= ctx->global_alphabet->list[global_index].type == INPUT_SIG;
			if(is_input){
				x_count++;
			}else{
				y_count++;
			}
			strcpy(current_dict_entry, mgr->vars_dict->entries[i].key);
			strcat(current_dict_entry, SIGNAL_ON_SUFFIX);
			local_alphabet[current_element++]	= automaton_alphabet_get_value_index(ctx->global_alphabet, current_dict_entry);
			strcpy(current_dict_entry, mgr->vars_dict->entries[i].key);
			strcat(current_dict_entry, SIGNAL_OFF_SUFFIX);
			local_alphabet[current_element++]	= automaton_alphabet_get_value_index(ctx->global_alphabet, current_dict_entry);
		}
	}

	/**
	 * get x, y, x', y' alphabets
	 */
	uint32_t* x_alphabet	= malloc(sizeof(uint32_t) * x_count);
	uint32_t* y_alphabet	= malloc(sizeof(uint32_t) * y_count);
	uint32_t* x_y_alphabet	= malloc(sizeof(uint32_t) * (x_count + y_count));
	uint32_t* signals_alphabet	= malloc(sizeof(uint32_t) * signals_count);
	//set of duplicated alphabets respecting obdd original ordering for usage within the restrict function
	uint32_t* x_alphabet_o	= malloc(sizeof(uint32_t) * x_count);
	uint32_t* y_alphabet_o	= malloc(sizeof(uint32_t) * y_count);
	uint32_t* x_y_alphabet_o	= malloc(sizeof(uint32_t) * (x_count + y_count));
	uint32_t* signals_alphabet_o	= malloc(sizeof(uint32_t) * signals_count);
	x_count = 0, y_count = 0, signals_count = 0, x_y_count = 0;
	for(i = 0; i < mgr->vars_dict->size; i++){//X
		//avoid searching for the true/false variables
		if((strcmp(mgr->vars_dict->entries[i].key, TRUE_VAR) == 0) || (strcmp(mgr->vars_dict->entries[i].key, FALSE_VAR) == 0))	continue;
		alphabet_element_index	= mgr->vars_dict->entries[i].value;
		is_primed				= false;
		for(j = 0; j < parser_primed_variables_count; j++)
			if(parser_primed_variables[j] == alphabet_element_index){is_primed	= true;	break;}
		if(!is_primed){
			strcpy(current_key, mgr->vars_dict->entries[i].key);strcat(current_key, SIGNAL_ON_SUFFIX);
			global_index	= automaton_alphabet_get_value_index(ctx->global_alphabet, current_key);
			is_input		= ctx->global_alphabet->list[global_index].type == INPUT_SIG;
			x_y_alphabet_o[x_y_count++] = i;
			if(is_input){x_alphabet_o[x_count++] = i;}else{y_alphabet_o[y_count++] = i;}
		}
		signals_alphabet_o[signals_count++]	= i;
	}
	x_count = 0, y_count = 0, signals_count = 0, x_y_count = 0;
	//this code seems repeated but should be kept this way since enforces
	//sequentialization of X Y X' Y' variables
	for(i = 0; i < mgr->vars_dict->size; i++){//X
		//avoid searching for the true/false variables
		if((strcmp(mgr->vars_dict->entries[i].key, TRUE_VAR) == 0) || (strcmp(mgr->vars_dict->entries[i].key, FALSE_VAR) == 0))	continue;
		alphabet_element_index	= mgr->vars_dict->entries[i].value;
		is_primed				= false;
		for(j = 0; j < parser_primed_variables_count; j++)
			if(parser_primed_variables[j] == alphabet_element_index){is_primed	= true;	break;}
		if(!is_primed){
			strcpy(current_key, mgr->vars_dict->entries[i].key);strcat(current_key, SIGNAL_ON_SUFFIX);
			global_index	= automaton_alphabet_get_value_index(ctx->global_alphabet, current_key);
			is_input		= ctx->global_alphabet->list[global_index].type == INPUT_SIG;
			if(is_input){x_alphabet[x_count++]	= i;
			}else{y_alphabet[y_count++]	= i;}
			if(is_input){
				x_y_alphabet[x_y_count++] = i;
				signals_alphabet[signals_count++]	= i;
			}
		}
	}
	for(i = 2; i < mgr->vars_dict->size; i+=2){//X
		obdd_on_count++;obdd_off_count++;
		strcpy(current_dict_entry, mgr->vars_dict->entries[i].key);	strcat(current_dict_entry, SIGNAL_ON_SUFFIX);
		obdd_on_signals_indexes[(i-2)/2]			= automaton_alphabet_get_value_index(ctx->global_alphabet, current_dict_entry);//dictionary_value_for_key(mgr->vars_dict, current_dict_entry);
#if DEBUG_LTL_AUTOMATON
		printf("[%s:%d->%d]", current_dict_entry, i, (i-2)/2);
#endif
		strcpy(current_dict_entry, mgr->vars_dict->entries[i].key);	strcat(current_dict_entry, SIGNAL_OFF_SUFFIX);
		obdd_off_signals_indexes[(i-2)/2]		= automaton_alphabet_get_value_index(ctx->global_alphabet, current_dict_entry);//dictionary_value_for_key(mgr->vars_dict, current_dict_entry);
#if DEBUG_LTL_AUTOMATON
		printf("[%s:%d->%d]", current_dict_entry, i, (i-2)/2);
#endif
	}
	for(i = 0; i < mgr->vars_dict->size; i++){//Y
			//avoid searching for the true/false variables
			if((strcmp(mgr->vars_dict->entries[i].key, TRUE_VAR) == 0) || (strcmp(mgr->vars_dict->entries[i].key, FALSE_VAR) == 0))	continue;
			alphabet_element_index	= mgr->vars_dict->entries[i].value;
			is_primed				= false;
			for(j = 0; j < parser_primed_variables_count; j++)
				if(parser_primed_variables[j] == alphabet_element_index){is_primed	= true;	break;}
			if(is_primed)continue;
			strcpy(current_key, mgr->vars_dict->entries[i].key);strcat(current_key, SIGNAL_ON_SUFFIX);
			global_index	= automaton_alphabet_get_value_index(ctx->global_alphabet, current_key);
			if(global_index < 0){
				printf("%s not found on signals's dictionary\n", current_key);
				exit(-1);
			}
			is_input		= ctx->global_alphabet->list[global_index].type == INPUT_SIG;
			if(!is_primed && !is_input){
				x_y_alphabet[x_y_count++] = i;
				signals_alphabet[signals_count++]	= i;
			}
		}
	uint32_t effective_var_index;
	for(i = 0; i < mgr->vars_dict->size; i++){//X'
		//avoid searching for the true/false variables
		if((strcmp(mgr->vars_dict->entries[i].key, TRUE_VAR) == 0) || (strcmp(mgr->vars_dict->entries[i].key, FALSE_VAR) == 0))	continue;
		alphabet_element_index	= mgr->vars_dict->entries[i].value;
		is_primed				= false;
		effective_var_index	= i;
		for(j = 0; j < parser_primed_variables_count; j++)
			if(parser_primed_variables[j] == alphabet_element_index){
				is_primed	= true;
				effective_var_index	= parser_primed_original_variables[j];
				break;
			}
		strcpy(current_key, mgr->vars_dict->entries[effective_var_index].key);strcat(current_key, SIGNAL_ON_SUFFIX);
		global_index	= automaton_alphabet_get_value_index(ctx->global_alphabet, current_key);
		if(global_index < 0){
						printf("%s not found on signals's dictionary\n", current_key);
						exit(-1);
					}
		is_input		= ctx->global_alphabet->list[global_index].type == INPUT_SIG;
		if(is_primed && is_input){
			signals_alphabet[signals_count++]	= i;
		}
	}
	for(i = 0; i < mgr->vars_dict->size; i++){//Y'
		//avoid searching for the true/false variables
		if((strcmp(mgr->vars_dict->entries[i].key, TRUE_VAR) == 0) || (strcmp(mgr->vars_dict->entries[i].key, FALSE_VAR) == 0))	continue;
		alphabet_element_index	= mgr->vars_dict->entries[i].value;
		is_primed				= false;
		effective_var_index	= i;
		for(j = 0; j < parser_primed_variables_count; j++)
			if(parser_primed_variables[j] == alphabet_element_index){
				is_primed	= true;
				effective_var_index	= parser_primed_original_variables[j];
				break;
			}
		strcpy(current_key, mgr->vars_dict->entries[effective_var_index].key);strcat(current_key, SIGNAL_ON_SUFFIX);
		global_index	= automaton_alphabet_get_value_index(ctx->global_alphabet, current_key);
		if(global_index < 0){
						printf("%s not found on signals's dictionary\n", current_key);
						exit(-1);
					}
		is_input		= ctx->global_alphabet->list[global_index].type == INPUT_SIG;
		if(is_primed && !is_input){
			signals_alphabet[signals_count++]	= i;
		}
	}

	//initialize cache variables
	for(i = 0; i < mgr->vars_dict->size; i++){
		obdd_destroy(obdd_mgr_var(mgr, mgr->vars_dict->entries[i].key));
		obdd_destroy(obdd_mgr_not_var(mgr, mgr->vars_dict->entries[i].key));
	}

	//initialize boolean array hash tables
	automaton_bool_array_hash_table *x_y_hash_table	= automaton_bool_array_hash_table_create(x_y_count);
	automaton_bool_array_hash_table *x_hash_table	= automaton_bool_array_hash_table_create(x_count);
	bool *hashed_valuation = NULL;


#if DEBUG_LTL_AUTOMATON
	printf("OBDD mgr dict order\n[");
	for(i = 0; i < mgr->vars_dict->size; i++)
			printf("%s%s", mgr->vars_dict->entries[i].key, i == mgr->vars_dict->size - 1 ? "" : ",");
	printf("]\nMGR alphabet\n[");
	for(i = 0; i < ctx->global_alphabet->count; i++)
			printf("%s%s", ctx->global_alphabet->list[i].name, i == ctx->global_alphabet->count - 1 ? "" : ",");
	printf("]\nX alphabet\n[");
	for(i = 0; i < x_count; i++)
		printf("%s%s", mgr->vars_dict->entries[x_alphabet[i]].key, i == x_count - 1 ? "" : ",");
	printf("]\nY alphabet\n[");
	for(i = 0; i < y_count; i++)
		printf("%s%s", mgr->vars_dict->entries[y_alphabet[i]].key, i == y_count - 1 ? "" : ",");
	printf("]\nXY alphabet\n[");
	for(i = 0; i < x_y_count; i++)
		printf("%s%s", mgr->vars_dict->entries[x_y_alphabet[i]].key, i == x_y_count - 1 ? "" : ",");
	printf("]\nXY ordered alphabet\n[");
	for(i = 0; i < x_y_count; i++)
		printf("%s%s", mgr->vars_dict->entries[x_y_alphabet_o[i]].key, i == x_y_count - 1 ? "" : ",");
	printf("]\nXYX'Y' alphabet\n[");
	for(i = 0; i < signals_count; i++)
		printf("%s%s", mgr->vars_dict->entries[signals_alphabet[i]].key, i == signals_count - 1 ? "" : ",");
	printf("]\nObdd on alphabet\n[");
	for(i = 0; i < obdd_on_count; i++)
		printf("%d%s", obdd_on_signals_indexes[i], i == obdd_on_count - 1 ? "" : ",");
	printf("]\nObdd off alphabet\n[");
		for(i = 0; i < obdd_off_count; i++)
			printf("%d%s", obdd_off_signals_indexes[i], i == obdd_off_count - 1 ? "" : ",");
	printf("]\n");
#endif
	//get automaton
	automaton_automaton* ltl_automaton	= automaton_automaton_create(name, ctx, local_alphabet_count, local_alphabet, true, true, false, false);

	/**
	 * get the conjunction of rho and theta formulae
	 */
	obdd* old_obdd;
	obdd* env_theta_composed			= NULL;
	obdd* sys_theta_composed			= NULL;
	obdd* env_sys_theta_composed		= NULL;
	obdd* env_rho_composed				= NULL;
	obdd* sys_rho_composed				= NULL;
	obdd* env_sys_rho_composed			= NULL;


	//build ordered ref for x_y_alphabet to be used in fluent satisfaction
	uint32_t* x_y_order	= calloc(x_y_count, sizeof(uint32_t));
	uint32_t order_index 	= 0;
	uint32_t last_var 		= 0;
	uint32_t current_var 	= 0;
	uint32_t biggest_index	= 0;
	bool var_initialized	= false;
	for(i = 0; i < x_y_count; i++){
		current_var		= 0;
		var_initialized	= false;
		for(j = 0; j < x_y_count; j++){
			if(x_y_alphabet_o[j] > last_var && (x_y_alphabet_o[j] < current_var || !var_initialized)){
				var_initialized	= true;
				current_var		= x_y_alphabet_o[j];
				biggest_index	= (int32_t)j;
			}
		}
		x_y_order[i]	= biggest_index;
		last_var		= current_var;
	}

#if VERBOSE || DEBUG_LTL_AUTOMATON
	printf(ANSI_COLOR_RED);
	printf("Composing env/sys theta functions\n");
#endif
	for(i = 0; i < env_theta_count; i++){
		if(i == 0){ env_theta_composed	= env_theta_obdd[i];
		}else{
			old_obdd	= env_theta_composed;
			env_theta_composed	= obdd_apply_and(env_theta_composed, env_theta_obdd[i]);
			obdd_destroy(env_theta_obdd[i]);
			if(i > 1)obdd_destroy(old_obdd);
		}
	}
	for(i = 0; i < sys_theta_count; i++){
		if(i == 0){ sys_theta_composed	= sys_theta_obdd[i];
		}else{
			old_obdd	= sys_theta_composed;
			sys_theta_composed	= obdd_apply_and(sys_theta_composed, sys_theta_obdd[i]);
			obdd_destroy(sys_theta_obdd[i]);
			if(i > 1)obdd_destroy(old_obdd);
		}
	}
#if VERBOSE || DEBUG_LTL_AUTOMATON
	printf("Composing env rho functions\n");
	uint32_t buff_size = 20000;
	char buff[20000];
	buff[0] = '\0';
#endif
	for(i = 0; i < env_rho_count; i++){
#if DEBUG_LTL_AUTOMATON
			printf("Env rho %d\n",i);
			obdd_print(env_rho_obdd[i], buff, buff_size);
			printf("%s", buff); buff[0] = '\0';
#endif
		if(i == 0){ env_rho_composed	= env_rho_obdd[i];
		}else{
			old_obdd	= env_rho_composed;
			env_rho_composed	= obdd_apply_and(env_rho_composed, env_rho_obdd[i]);
			obdd_destroy(env_rho_obdd[i]);
			if(i > 1)obdd_destroy(old_obdd);
		}
#if (VERBOSE || DEBUG_LTL_AUTOMATON) && OBDD_USE_POOL
		printf("[%d]", env_rho_composed->mgr->nodes_pool->composite_count);
		fflush(stdout);
#endif
	}

#if VERBOSE || DEBUG_LTL_AUTOMATON
	printf("\nComposing sys rho functions\n");
#endif
	for(i = 0; i < sys_rho_count; i++){
#if DEBUG_LTL_AUTOMATON
			printf("Sys rho %d\n",i);
			obdd_print(sys_rho_obdd[i], buff, buff_size);
			printf("%s", buff); buff[0] = '\0';
#endif
		if(i == 0){ sys_rho_composed	= sys_rho_obdd[i];
		}else{
			old_obdd	= sys_rho_composed;
			sys_rho_composed	= obdd_apply_and(sys_rho_composed, sys_rho_obdd[i]);
			obdd_destroy(sys_rho_obdd[i]);
			if(i > 1)obdd_destroy(old_obdd);
		}
#if (VERBOSE || DEBUG_LTL_AUTOMATON) && OBDD_USE_POOL
		printf("[%d]", sys_rho_composed->mgr->nodes_pool->composite_count);
		fflush(stdout);
#endif
	}
#if VERBOSE || DEBUG_LTL_AUTOMATON
	printf("\n");
	fflush(stdout);
	printf(ANSI_COLOR_RESET);
#endif
#if (VERBOSE || DEBUG_LTL_AUTOMATON) && OBDD_USE_POOL
	uint32_t previous_node_count = sys_rho_composed == NULL ? 0 : sys_rho_composed->mgr->nodes_pool->composite_count;
#endif
	if(sys_theta_composed == NULL && env_theta_composed == NULL){
			env_sys_theta_composed			= obdd_clone(mgr->true_obdd);
			env_theta_composed				= obdd_clone(mgr->true_obdd);
			sys_theta_composed				= obdd_clone(mgr->true_obdd);
	}else if(env_theta_composed == NULL){
		env_sys_theta_composed			= obdd_clone(sys_theta_composed);
		env_theta_composed				= obdd_clone(mgr->true_obdd);
	}else if(sys_theta_composed == NULL){
		env_sys_theta_composed			= obdd_clone(env_theta_composed);
		sys_theta_composed				= obdd_clone(mgr->true_obdd);
	}else{
		env_sys_theta_composed				= obdd_apply_and(env_theta_composed, sys_theta_composed);
	}
	if(sys_rho_composed == NULL && env_rho_composed == NULL){
			env_sys_rho_composed			= obdd_clone(mgr->true_obdd);
			env_rho_composed			= obdd_clone(mgr->true_obdd);
			sys_rho_composed			= obdd_clone(mgr->true_obdd);
	}else if(env_rho_composed == NULL){
		env_sys_rho_composed		= obdd_clone(sys_rho_composed);
		env_rho_composed			= obdd_clone(mgr->true_obdd);
	}else if(sys_rho_composed == NULL){
		env_sys_rho_composed		= obdd_clone(env_rho_composed);
		sys_rho_composed			= obdd_clone(mgr->true_obdd);
	}else{
		env_sys_rho_composed				= obdd_apply_and(env_rho_composed, sys_rho_composed);
	}

	// apply restriction over V when needed, this should restrict current vars according to V restrictions on rho
	obdd *exists_env_sys_rho_composed	= obdd_exists_vector(env_sys_rho_composed, parser_primed_variables, parser_primed_variables_count);
	obdd *exists_swapped_env_sys_rho_composed	= obdd_swap_vars(exists_env_sys_rho_composed, parser_primed_original_variables, parser_primed_variables, parser_primed_variables_count);
	obdd_destroy(exists_env_sys_rho_composed);
	obdd *exists_swapped_env_sys_rho_composed_and	= obdd_apply_and(env_sys_rho_composed, exists_swapped_env_sys_rho_composed);
	obdd_destroy(env_sys_rho_composed);
	obdd_destroy(exists_swapped_env_sys_rho_composed);
	env_sys_rho_composed	= exists_swapped_env_sys_rho_composed_and;

#if DEBUG_LTL_AUTOMATON
	printf("\n");
	buff[0]	= '\0';
	printf("Env sys theta composed\n");
	obdd_print(env_sys_theta_composed, buff, buff_size);
	printf("%s", buff); buff[0] = '\0';
	printf("Env sys rho composed\n");
	obdd_print(env_sys_rho_composed, buff, buff_size);
	printf("%s", buff); buff[0] = '\0';
	fflush(stdout);
	printf(ANSI_COLOR_RESET);
#endif
#if (VERBOSE || DEBUG_LTL_AUTOMATON) && OBDD_USE_POOL
	printf("[%d] [%d]\n", previous_node_count, sys_rho_composed->mgr->nodes_pool->composite_count);
#endif
	if(env_rho_count > 1)obdd_destroy(env_rho_composed);
	if(sys_rho_count > 1)obdd_destroy(sys_rho_composed);
	/**
	 * BEHAVIOUR CONSTRUCTION (RHO AND THETA)
	 */
	uint32_t current_valuations_count;
	uint32_t valuations_size	= mgr->vars_dict->size * 2 * LIST_INITIAL_SIZE;
	bool* valuations			= calloc(sizeof(bool), valuations_size);
	obdd_composite_state* sys_state = obdd_composite_state_create(0, x_count + y_count);
	obdd_composite_state* env_state = obdd_composite_state_create(0, x_count + y_count);
	uint32_t fluent_count	= x_count + y_count;
	obdd_state_tree* obdd_state_map	= obdd_state_tree_create(x_count * 2 + y_count);
	automaton_concrete_bucket_list* rho_sys_bucket_list	= automaton_concrete_bucket_list_create(
			LTL_BUCKET_SIZE, obdd_composite_state_extractor, sizeof(uint32_t) + sizeof(bool) * (x_count + y_count));
	automaton_concrete_bucket_list* rho_sys_processed_bucket_list	= automaton_concrete_bucket_list_create(
			LTL_PROCESSED_BUCKET_SIZE, obdd_composite_state_extractor, sizeof(uint32_t) + sizeof(bool) * (x_count + y_count));
	sys_state->state		= 0;//obdd_state_tree_get_key(obdd_state_map, sys_state->valuation, x_y_count);
	automaton_automaton_add_initial_state(ltl_automaton, sys_state->state);
	/**
	 * THETA INITIAL CONDITION
	 */
	//add initial env valuations
	//structs needed for get_valuations
	uint32_t variables_count	= mgr->vars_dict->size - 2;
	bool* dont_care_list		= malloc(sizeof(bool) * variables_count);
	bool* partial_valuation		= calloc(variables_count, sizeof(bool));
	bool* initialized_values	= calloc(variables_count, sizeof(bool));
	bool* valuation_set			= malloc(sizeof(bool) * variables_count);
	//keeps a stack of visited nodes
	obdd_node** last_nodes		= malloc(sizeof(obdd_node*) * variables_count);
#if VERBOSE
	printf(ANSI_COLOR_RED "Building theta valuations\n" ANSI_COLOR_RESET);
#endif
#if VERBOSE || DEBUG_LTL_AUTOMATON
	int32_t state_counter = 0;
#endif

////////////////////////////////////////////

	//NEW OBDD CONSTRUCTION APPROACH
	bool* tmp_state_valuation = calloc((x_count * 2 + y_count), sizeof(bool));
	bool has_transition;

	obdd_get_valuations(mgr, env_sys_theta_composed, &valuations, &valuations_size, &current_valuations_count, x_y_alphabet, x_y_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
#if DEBUG_LTL_AUTOMATON
	printf("Init env valuations\n");
	obdd_print_valuations_names(mgr, valuations, current_valuations_count, x_y_alphabet, x_y_count);
#endif

	for(i = 0; i < current_valuations_count; i++){
		//automaton_set_composed_valuation(env_state->valuation, valuations, i, true, true, x_count, y_count);
		for(j = 0; j < x_y_count; j++)sys_state->valuation[j]	= valuations[x_y_count * i + j];
		hashed_valuation		= automaton_bool_array_hash_table_add_or_get_entry(x_y_hash_table, sys_state->valuation, true);
		sys_state->state		= obdd_state_tree_get_key(obdd_state_map, sys_state->valuation, x_y_count);
		has_transition	= automaton_add_transition_from_valuations(mgr, ltl_automaton, 0, sys_state->state
				, env_state->valuation, hashed_valuation, true, true
				, x_y_count, x_count
				, obdd_on_signals_indexes, obdd_off_signals_indexes, x_y_alphabet_o, x_y_order);
		if(!has_transition){
#if DEBUG_LTL_AUTOMATON
			printf("(%d-[", sys_state->state);
			for(j = 0; j < x_count; j++)
				printf("%s", env_state->valuation[j] ? "1" : "0");
			state_counter++;
			printf("]->%d)\n", env_state->state);
			if(state_counter % 1000 == 0){
				printf("States processed for ltl: %d\n", state_counter) < 0 ? abort() : (void)0;
				fflush(stdout);
			}
#endif
		}

		if(!has_transition && !automaton_concrete_bucket_has_entry(rho_sys_bucket_list, sys_state)){
			automaton_concrete_bucket_add_entry(rho_sys_bucket_list, sys_state);
		}

	}
#if DEBUG_LTL_AUTOMATON
	state_counter = 0;
	obdd_get_valuations(mgr, env_sys_theta_composed, &valuations, &valuations_size, &current_valuations_count, x_y_alphabet, x_y_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
	printf("Init sys valuations\n");
	obdd_print_valuations_names(mgr, valuations, current_valuations_count, x_y_alphabet, x_y_count);
#endif

	uint32_t state_ptr = 0;
	obdd* obdd_current_state	= NULL;
	obdd* obdd_restricted_state	= NULL;
#define CNTR_LIMIT 200
#if VERBOSE || DEBUG_LTL_AUTOMATON
	printf("[#obdd nodes:val.size:val.count:bucket_count]\n");
#endif
	uint32_t rho_counter = 0, skipped = 0, evaluated = 0;
#if VERBOSE || DEBUG_LTL_AUTOMATON
	printf(ANSI_COLOR_RED "Building rho valuations\n" ANSI_COLOR_RESET);
#endif
	//NEW RHO BUILD APPROACH

#if APPLY_OBDD_REACHABLE
	obdd *tmp_obdd = obdd_reachable_states(env_sys_theta_composed, env_sys_rho_composed);
	obdd *and_obdd = obdd_apply_and(env_sys_rho_composed, tmp_obdd);
	obdd_destroy(tmp_obdd);
	obdd_destroy(env_sys_rho_composed);
	env_sys_rho_composed	= and_obdd;
#endif
#if DEBUG_LTL_AUTOMATON
	state_counter = 0;
	obdd_get_valuations(mgr, env_sys_rho_composed, &valuations, &valuations_size, &current_valuations_count, signals_alphabet,signals_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
	printf("Init sys valuations\n");
	obdd_print_valuations_names(mgr, valuations, current_valuations_count, signals_alphabet, signals_count);
#endif
	automaton_add_transitions_from_valuations(mgr, env_sys_rho_composed, ltl_automaton, &current_valuations_count,
			dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes, env_state, sys_state, obdd_state_map, x_y_count, x_count,
			x_y_alphabet_o, x_y_order, signals_count, hashed_valuation, x_y_hash_table,
			obdd_on_signals_indexes, obdd_off_signals_indexes);
#if VERBOSE || DEBUG_LTL_AUTOMATON
	//TODO:print cache table info
#endif
free(valuations);

/////////////////////////////////////

	/**
	 * RHO TRANSITION RELATION
	 */
	//add transitions valuations
	/**
	 * We restrict robdd(rho_env) with the valuation for each pending state s_i rho_bucket_list
	 * and then ask for S_env = obdd_get_valuations, we build and add the transition between s_i and each s_e in S_env
	 * adding s_e to rho_env pending list, once rho_bucket_list is empty
	 * we restrict robdd(rho_env && rho_sys) with the valuation for each pending state s_e rho_env_bucket_list
	 * and then ask for S_j = obdd_get_valuations, we build and add the transition between s_e and each s_j in S_j
	 * once rho_env_bucket_list is empty we start again with rho_bucket_list until both lists are empty
	 */
	free(x_y_order);
	free(initialized_values);
	free(valuation_set);
	free(last_nodes);
	free(dont_care_list);
	free(partial_valuation);
#if VERBOSE  || DEBUG_LTL_AUTOMATON
	printf("\nOBDD->Automaton done\n");
#endif
	int32_t main_index					= automaton_parsing_tables_add_entry(tables, COMPOSITION_ENTRY_AUT, name, ltl_automaton);
	tables->composition_entries[main_index]->solved	= true;
	tables->composition_entries[main_index]->valuation_count			= 1;
	tables->composition_entries[main_index]->valuation.automaton_value	= ltl_automaton;

	if(env_theta_count > 1)obdd_destroy(env_theta_composed);
	if(sys_theta_count > 1)obdd_destroy(sys_theta_composed);

	obdd_destroy(env_sys_theta_composed); obdd_destroy(env_sys_rho_composed);

	automaton_bool_array_hash_table_destroy(x_y_hash_table);
	automaton_bool_array_hash_table_destroy(x_hash_table);
	free(env_state);
	free(sys_state); free(tmp_state_valuation);
	free(obdd_on_signals_indexes); free(obdd_off_signals_indexes);
	automaton_concrete_bucket_destroy(rho_sys_bucket_list, true);
	automaton_concrete_bucket_destroy(rho_sys_processed_bucket_list, true);
	free(x_alphabet); free(y_alphabet); free(x_y_alphabet); free(signals_alphabet);
	free(x_alphabet_o); free(y_alphabet_o); free(x_y_alphabet_o); free(signals_alphabet_o);
	free(local_alphabet);
	obdd_state_tree_destroy(state_map);
	obdd_state_tree_destroy(obdd_state_map);
	return ltl_automaton;
}
