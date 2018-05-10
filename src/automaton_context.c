/*
 * automaton_context.c
 *
 *  Created on: Apr 19, 2018
 *      Author: mariano
 */
#include "automaton_context.h"

automaton_parsing_tables* automaton_parsing_tables_create(){
	automaton_parsing_tables* tables	= malloc(sizeof(automaton_parsing_tables));
	tables->label_count	= 0;
	tables->set_count	= 0;
	tables->fluent_count= 0;
	tables->range_count	= 0;
	tables->const_count	= 0;
	tables->automaton_count		= 0;
	tables->composition_count	= 0;
	return tables;
}
void automaton_parsing_tables_destroy(automaton_parsing_tables* tables){
	aut_free_ptr_list((void***)&(tables->label_entries), &(tables->label_count));
	aut_free_ptr_list((void***)&(tables->set_entries), &(tables->set_count));
	aut_free_ptr_list((void***)&(tables->fluent_entries), &(tables->fluent_count));
	aut_free_ptr_list((void***)&(tables->range_entries), &(tables->range_count));
	aut_free_ptr_list((void***)&(tables->const_entries), &(tables->const_count));
	aut_free_ptr_list((void***)&(tables->automaton_entries), &(tables->automaton_count));
	aut_free_ptr_list((void***)&(tables->composition_entries), &(tables->composition_count));
	free(tables);
}
automaton_parsing_table_entry* automaton_parsing_table_entry_create(automaton_parsing_table_entry_type type, char* key, void* value, int32_t index){
	automaton_parsing_table_entry* entry	= malloc(sizeof(automaton_parsing_table_entry));
	entry->type		= type;
	aut_dupstr(&(entry->key), key);
	entry->value	= value;
	entry->index	= index;
	entry->solved	= false;
	entry->valuation.int_value = 0;
	return entry;
}
void automaton_parsing_table_entry_destroy(automaton_parsing_table_entry* entry){
	if(entry->value != NULL)
		free(entry->value);
	free(entry);
}
int32_t automaton_parsing_tables_get_entry_index(automaton_parsing_tables* tables, automaton_parsing_table_entry_type type, char* key){
	int32_t current_index = -1;
	int32_t i;
	automaton_parsing_table_entry* entry;
	switch(type){
	case LABEL_ENTRY_AUT:
		for(i = 0; i < (int32_t)tables->label_count; i++){
			if(strcmp(tables->label_entries[i]->key, key) == 0)
				return i;
		}
		break;
	case SET_ENTRY_AUT:
		for(i = 0; i < (int32_t)tables->set_count; i++){
			if(strcmp(tables->set_entries[i]->key, key) == 0)
				return i;
		}
		break;
	case FLUENT_ENTRY_AUT:
		for(i = 0; i < (int32_t)tables->fluent_count; i++){
			if(strcmp(tables->fluent_entries[i]->key, key) == 0)
				return i;
		}
		break;
	case RANGE_ENTRY_AUT:
		for(i = 0; i < (int32_t)tables->range_count; i++){
			if(strcmp(tables->range_entries[i]->key, key) == 0)
				return i;
		}
		break;
	case CONST_ENTRY_AUT:
		for(i = 0; i < (int32_t)tables->const_count; i++){
			if(strcmp(tables->const_entries[i]->key, key) == 0)
				return i;
		}
		break;
	case AUTOMATON_ENTRY_AUT:
		for(i = 0; i < (int32_t)tables->automaton_count; i++){
			if(strcmp(tables->automaton_entries[i]->key, key) == 0)
				return i;
		}
		break;
	case COMPOSITION_ENTRY_AUT:
		for(i = 0; i < (int32_t)tables->composition_count; i++){
			if(strcmp(tables->composition_entries[i]->key, key) == 0)
				return i;
		}
		break;
	}
	return current_index;
}

int32_t automaton_parsing_tables_add_entry(automaton_parsing_tables* tables, automaton_parsing_table_entry_type type, char* key, void* value){
	int32_t current_index;
	current_index	= automaton_parsing_tables_get_entry_index(tables, type, key);
	if(current_index >= 0)
		return current_index;
	automaton_parsing_table_entry* entry;
	switch(type){
	case LABEL_ENTRY_AUT:
		current_index	= tables->label_count;
		entry			= automaton_parsing_table_entry_create(type, key, value, current_index);
		aut_add_ptr_list((void***)&(tables->label_entries), (void*)entry, &(tables->label_count));
		break;
	case SET_ENTRY_AUT:
			current_index	= tables->set_count;
			entry			= automaton_parsing_table_entry_create(type, key, value, current_index);
			aut_add_ptr_list((void***)&(tables->set_entries), (void*)entry, &(tables->set_count));
			break;
	case FLUENT_ENTRY_AUT:
				current_index	= tables->fluent_count;
				entry			= automaton_parsing_table_entry_create(type, key, value, current_index);
				aut_add_ptr_list((void***)&(tables->fluent_entries), (void*)entry, &(tables->fluent_count));
				break;
	case RANGE_ENTRY_AUT:
					current_index	= tables->range_count;
					entry			= automaton_parsing_table_entry_create(type, key, value, current_index);
					aut_add_ptr_list((void***)&(tables->range_entries), (void*)entry, &(tables->range_count));
					break;
	case CONST_ENTRY_AUT:
					current_index	= tables->const_count;
					entry			= automaton_parsing_table_entry_create(type, key, value, current_index);
					aut_add_ptr_list((void***)&(tables->const_entries), (void*)entry, &(tables->const_count));
					break;
	case AUTOMATON_ENTRY_AUT:
					current_index	= tables->automaton_count;
					entry			= automaton_parsing_table_entry_create(type, key, value, current_index);
					aut_add_ptr_list((void***)&(tables->automaton_entries), (void*)entry, &(tables->automaton_count));
					break;
	case COMPOSITION_ENTRY_AUT:
					current_index	= tables->composition_count;
					entry			= automaton_parsing_table_entry_create(type, key, value, current_index);
					aut_add_ptr_list((void***)&(tables->composition_entries), (void*)entry, &(tables->composition_count));
					break;
	}
	return current_index;
}

void automaton_statement_syntax_to_table(automaton_statement_syntax* statement, automaton_parsing_tables* tables){
	automaton_expression_syntax* expr;
	automaton_fluent_syntax* fluent_def;
	automaton_set_def_syntax* set_def;
	automaton_composition_syntax* composition_def;
	switch(statement->type){
	case CONST_AUT:
		 expr	= statement->const_def;
		if(expr->string_terminal != NULL){
			automaton_parsing_tables_add_entry(tables, CONST_ENTRY_AUT, expr->string_terminal, (void*)expr);
		}
		break;
	case RANGE_AUT:
		expr	= statement->range_def;
		if(expr->string_terminal != NULL){
			automaton_parsing_tables_add_entry(tables, RANGE_ENTRY_AUT, expr->string_terminal, (void*)expr);
		}
		break;
	case FLUENT_AUT:
		fluent_def		= statement->fluent_def;
		automaton_parsing_tables_add_entry(tables, FLUENT_ENTRY_AUT, fluent_def->name, (void*)fluent_def);
		break;
	case SET_AUT:
		set_def			= statement->set_def;
		if(set_def->name != NULL){
			automaton_parsing_tables_add_entry(tables, SET_ENTRY_AUT, set_def->name, (void*)set_def);
		}
		break;
	case COMPOSITION_AUT:
		composition_def	= statement->composition_def;
		if(composition_def->name != NULL){
			automaton_parsing_tables_add_entry(tables, COMPOSITION_ENTRY_AUT, composition_def->name, (void*)composition_def);
		}
		break;
	default: break;
	//ASSERTION_AUT,
	//GOAL_AUT
	}
}

int32_t automaton_expression_syntax_evaluate(automaton_parsing_tables* tables, automaton_expression_syntax* expr){
	int32_t index	= -1;
	int32_t valuation	= -1;
	int32_t left_valuation	= -1;
	int32_t right_valuation	= -1;
	int32_t ref_index = -1;
	automaton_parsing_table_entry* ref_entry;
	if(expr->type == CONST_TYPE_AUT){
		if(expr->string_terminal != NULL){
			index = automaton_parsing_tables_get_entry_index(tables, CONST_ENTRY_AUT, expr->string_terminal);
		}
	}
	switch(expr->type){
	case RANGE_DEF_TYPE_AUT:
	case RANGE_TYPE_AUT:
		exit(-1);
		break;
	case PARENTHESIS_TYPE_AUT:
	case CONST_TYPE_AUT:
		valuation = automaton_expression_syntax_evaluate(tables, expr->first);
		break;
	case BINARY_TYPE_AUT:
		left_valuation = automaton_expression_syntax_evaluate(tables, expr->first);
		right_valuation = automaton_expression_syntax_evaluate(tables, expr->second);
		switch(expr->op){
		case EQ_OP_AUT: valuation = left_valuation == right_valuation; break;			// ==
		case NEQ_OP_AUT: valuation = left_valuation != right_valuation; break;			// !=
		case GE_OP_AUT: valuation = left_valuation >= right_valuation; break;			// >=
		case LE_OP_AUT: valuation = left_valuation <= right_valuation; break;			// <=
		case GT_OP_AUT: valuation = left_valuation > right_valuation; break;			// >
		case LT_OP_AUT: valuation = left_valuation < right_valuation; break;			// <
		case PLUS_OP_AUT: valuation = left_valuation + right_valuation; break;		// +
		case MINUS_OP_AUT: valuation = left_valuation - right_valuation; break;		// -
		case MUL_OP_AUT: valuation = left_valuation * right_valuation; break;			// *
		case DIV_OP_AUT: valuation = left_valuation / right_valuation; break;			// /
		case MOD_OP_AUT: valuation = left_valuation % right_valuation; break;			// %
		case AND_OP_AUT: valuation = left_valuation && right_valuation; break;			// &
		case OR_OP_AUT: valuation = left_valuation || right_valuation; break;			// |
		case XOR_OP_AUT: valuation = left_valuation ^ right_valuation; break;			// ^
		default: exit(-1);break;
		}
		break;

	case INTEGER_TERMINAL_TYPE_AUT:
		valuation = atoi(expr->string_terminal);
		break;
	case UPPER_IDENT_TERMINAL_TYPE_AUT:
		ref_index 	= automaton_parsing_tables_get_entry_index(tables, CONST_ENTRY_AUT, expr->string_terminal);
		ref_entry	= tables->const_entries[ref_index];
		if(!ref_entry->solved){
			automaton_expression_syntax_evaluate(tables, (automaton_expression_syntax*)ref_entry->value);
		}
		valuation = ref_entry->valuation.int_value;
		break;
	default:exit(-1);break;
	}
	if(expr->type == CONST_TYPE_AUT){
		if(index >= 0){
			tables->const_entries[index]->solved = true;
			tables->const_entries[index]->valuation.int_value	= valuation;
			tables->const_entries[index]->valuation_count	 	= 0;
		}
	}
	return valuation;
}
char** automaton_set_syntax_evaluate(automaton_parsing_tables* tables, automaton_set_syntax* set, int32_t *count, char* set_def_key){
	int32_t index = -1;
	uint32_t i, j;
	char* current_entry;
	char** ret_value			= NULL;
	int32_t inner_count			= 0;
	char** inner_value			= NULL;
	bool is_set;
	if(set->is_ident){
		index						= automaton_parsing_tables_get_entry_index(tables, SET_ENTRY_AUT, set->string_terminal);
		if(index >= 0)if(tables->set_entries[index]->solved)	return tables->set_entries[index]->valuation.labels_value;
	}
	//search until proper set def is found
	if(set->is_ident){
		while(set->is_ident){
			index	= automaton_parsing_tables_get_entry_index(tables, SET_ENTRY_AUT, set->string_terminal);
			set		= ((automaton_set_def_syntax*)tables->set_entries[index]->value)->set;
		}
		if(tables->set_entries[index]->solved)
			return tables->set_entries[index]->valuation.labels_value;
	}
	//if proper set was not solved try to solve it
	index						= automaton_parsing_tables_get_entry_index(tables, SET_ENTRY_AUT, set_def_key);
	for(i = 0; i < set->count; i++)for(j = 0; j < set->labels_count[i]; j++){
		automaton_label_syntax* label_syntax	= set->labels[i][j];
		is_set		= set->labels[i][j]->is_set;
		if(!is_set){
			inner_value		= &(set->labels[i][j]->string_terminal);
			inner_count		= 1;
			aut_merge_string_lists(&ret_value, count, inner_value, inner_count, true, false);
		}else{
			while(set->is_ident){
				index	= automaton_parsing_tables_get_entry_index(tables, SET_ENTRY_AUT, set->string_terminal);
				set		= ((automaton_set_def_syntax*)tables->set_entries[index]->value)->set;
			}
			if(!tables->set_entries[index]->solved){
				inner_value	= automaton_set_syntax_evaluate(tables, set, &inner_count, ((automaton_set_def_syntax*)tables->set_entries[index]->value)->name);
			}else{
				inner_count	= tables->set_entries[index]->valuation_count;
				inner_value	= tables->set_entries[index]->valuation.labels_value;
			}
			aut_merge_string_lists(&ret_value, count, inner_value, inner_count, true, false);
		}
	}
	//once solved, update set_def_entry
	if(index >= 0){
		tables->set_entries[index]->solved	= true;
		tables->set_entries[index]->valuation_count	 	= *count;
		tables->set_entries[index]->valuation.labels_value	= ret_value;
	}
	return ret_value;
}

automaton_alphabet* automaton_parsing_tables_get_global_alphabet(automaton_parsing_tables* tables){
	automaton_alphabet* global_alphabet	= automaton_alphabet_create();
	int32_t global_index		= automaton_parsing_tables_get_entry_index(tables, SET_ENTRY_AUT, GLOBAL_ALPHABET_NAME_AUT);
	int32_t controllable_index	= automaton_parsing_tables_get_entry_index(tables, SET_ENTRY_AUT, CONTROLLABLE_ALPHABET_NAME_AUT);
	if(global_index < 0)
		return NULL;
	if(controllable_index < 0)
		return NULL;
	int32_t i, j;
	int32_t global_count = 0;
	int32_t controllable_count = 0;
	char** global_values		= automaton_set_syntax_evaluate(tables, ((automaton_set_def_syntax*)tables->set_entries[global_index]->value)->set
			, &global_count, ((automaton_set_def_syntax*)tables->set_entries[global_index]->value)->name);
	char** controllable_values	= automaton_set_syntax_evaluate(tables, ((automaton_set_def_syntax*)tables->set_entries[controllable_index]->value)->set
			, &controllable_count, ((automaton_set_def_syntax*)tables->set_entries[controllable_index]->value)->name);

	bool is_controllable;

	for(i = 0; i < global_count; i++){
		is_controllable	= false;
		for(j = 0; j < controllable_count; j++){
			if(strcmp(global_values[i], controllable_values[j]) == 0){
				is_controllable	= true;
				break;
			}
		}
		automaton_alphabet_add_signal_event(global_alphabet, automaton_signal_event_create(global_values[i], is_controllable? OUTPUT_SIG : INPUT_SIG));
	}

	for(i = 0; i < global_count; i++)
		free(global_values[i]);
	free(global_values);
	for(i = 0; i < controllable_count; i++)
			free(controllable_values[i]);
		free(controllable_values);

	return global_alphabet;
}
bool automaton_statement_syntax_to_automaton(automaton_automata_context* ctx, automaton_composition_syntax* composition_syntax
		, automaton_parsing_tables* tables){
	int32_t main_index, index, i, j, k, l, m, n;
	main_index						= automaton_parsing_tables_get_entry_index(tables, COMPOSITION_ENTRY_AUT, composition_syntax->name);
	if(main_index > 0)if(tables->composition_entries[main_index]->solved)	return false;	
	//check whether composition syntax is a composition or a single automaton description
	if(composition_syntax->components != NULL){//MULTIPLE COMPONENTS (AUTOMATA)
		//if one component has not been solved then report pending automata
		for(i = 0; i < (int32_t)composition_syntax->count; i++){
			index						= automaton_parsing_tables_get_entry_index(tables, COMPOSITION_ENTRY_AUT, composition_syntax->components[i]->ident);
			if(index < 0) return true; if(!tables->composition_entries[index]->solved) return true;
		}
		//build composition and add to table
		automaton_automaton** automata	= malloc(sizeof(automaton_automaton*) * composition_syntax->count);
		for(i = 0; i < (int32_t)composition_syntax->count; i++){
			//TODO: update transitions with prefixes/indexes
			index						= automaton_parsing_tables_get_entry_index(tables, COMPOSITION_ENTRY_AUT, composition_syntax->components[i]->ident);
			automata[i]					= tables->composition_entries[index]->valuation.automaton_value;
		}
		automaton_automaton* automaton	= automaton_automata_compose(automata, composition_syntax->count, SYNCHRONOUS);
		tables->composition_entries[main_index]->solved	= true;
		tables->composition_entries[main_index]->valuation_count			= 1;
		tables->composition_entries[main_index]->valuation.automaton_value	= automaton;
		free(automata);
		return false;
	}else{//SINGLE COMPONENT (AUTOMATON)
		//build local alphabet
		uint32_t	local_alphabet_count	= 0;
		int32_t		element_global_index;
		int32_t		element_position;
		uint32_t*	local_alphabet		= NULL;
		automaton_state_syntax* state;
		automaton_transition_syntax* transition;
		automaton_trace_label_syntax* trace_label;
		automaton_trace_label_atom_syntax* trace_label_atom;
		automaton_label_syntax* atom_label;
		for(i = 0; i < (int32_t)composition_syntax->count; i++){
			state	= composition_syntax->states[i];
			for(j = 0; j < (int32_t)state->transitions_count; j++){
				transition	= state->transitions[j];
				//TODO: take indexes and guard into consideration
				for(k = 0; k < (int32_t)transition->count; k++){
					trace_label	= transition->labels[k];
					for(l = 0; l < (int32_t)trace_label->count; l++){
						trace_label_atom	= trace_label->atoms[l];
						//TODO: take indexes into computation
						atom_label			= trace_label_atom->label;
						element_global_index= -1;
						element_position	= 0;
						for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++){
							if(strcmp(ctx->global_alphabet->list[m].name, atom_label->string_terminal) == 0){
								element_global_index = m;
								break;
							}
						}
						if(element_global_index == -1){
							//TODO: report local element not found
							exit(-1);
						}
						for(m = 0; m < (int32_t)local_alphabet_count; m++){
							if(local_alphabet[m] == (uint32_t)element_global_index){
								element_position	= -1;
								break;
							}
							if(local_alphabet[m] > (uint32_t)element_global_index){
								element_position	= m;
								break;
							}
						}
						if(element_position >= 0){
							uint32_t* new_alphabet	= malloc(sizeof(uint32_t) * (local_alphabet_count + 1));
							for(m = 0; m < (int32_t)local_alphabet_count; m++){
								if(m < element_position)
									new_alphabet[m]	= local_alphabet[m];
								else
									new_alphabet[m+1]	= local_alphabet[m];
							}
							new_alphabet[element_position]	= (uint32_t)element_global_index; 
							local_alphabet_count++;
							if(local_alphabet != NULL) free(local_alphabet);
							local_alphabet	= new_alphabet;
						}
					}
				}
			}
		}
		automaton_automaton* automaton	= automaton_automaton_create(composition_syntax->name, ctx, local_alphabet_count, local_alphabet);
		//add transitions
		//map state label to int
		char** labels_list	= NULL;
		int32_t labels_list_count	= 0;
		int32_t label_position;
		uint32_t from_state, to_state, current_from_state;
		//bool aut_push_string_to_list(char*** list, int32_t* list_count, char* element, int32_t* position, bool repeat_values);
		//int32_t aut_string_list_index_of(char** list, int32_t list_count, char* element);
		automaton_transition* automaton_transition;
		uint32_t first_state;
		bool first_state_set	= false;
		uint32_t added_state	= composition_syntax->count + 1;
		for(i = 0; i < (int32_t)composition_syntax->count; i++){
			state	= composition_syntax->states[i];
			if(state->ref != NULL){
				//TODO: solve state refs in automaton				
				continue;
			}
			aut_push_string_to_list(&labels_list, &labels_list_count, state->label->name, &label_position, false);
			current_from_state	= from_state	= (uint32_t) label_position;
			if(!first_state_set){
				automaton_automaton_add_initial_state(automaton, from_state);
				first_state_set	= true;
			}
			for(j = 0; j < (int32_t)state->transitions_count; j++){
				current_from_state	= from_state;
				transition	= state->transitions[j];
				aut_push_string_to_list(&labels_list, &labels_list_count, transition->to_state->name, &label_position, false);
				to_state	= (uint32_t)label_position;
				//TODO: take indexes and guard into consideration
				for(k = 0; k < (int32_t)transition->count; k++){
					if(k < (((int32_t)transition->count) - 1)){
						to_state	= added_state++;
					}else{
						to_state	= (uint32_t)label_position;
					}
					automaton_transition	= automaton_transition_create(current_from_state, to_state);
					current_from_state	= to_state;
					trace_label	= transition->labels[k];
					for(l = 0; l < (int32_t)trace_label->count; l++){
						trace_label_atom	= trace_label->atoms[l];
						//TODO: take indexes into computation
						atom_label			= trace_label_atom->label;
						element_global_index= -1;
						element_position	= 0;
						for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++){
							if(strcmp(ctx->global_alphabet->list[m].name, atom_label->string_terminal) == 0){
								element_global_index = m;
								break;
							}
						}
						if(element_global_index >= 0)
							automaton_transition_add_signal_event(automaton_transition, ctx, &(ctx->global_alphabet->list[element_global_index]));
					}
					automaton_automaton_add_transition(automaton, automaton_transition);
				}

			}
		}
		free(labels_list);
		//set entry in table
		tables->composition_entries[main_index]->solved	= true;
		tables->composition_entries[main_index]->valuation_count			= 1;
		tables->composition_entries[main_index]->valuation.automaton_value	= automaton;
		return false;
	}
	return true;
}

automaton_automata_context* automaton_automata_context_create_from_syntax(automaton_program_syntax* program, char* ctx_name){
	automaton_parsing_tables* tables	= automaton_parsing_tables_create();
	automaton_automata_context* ctx		= malloc(sizeof(automaton_automata_context));
	aut_dupstr(&(ctx->name), ctx_name);
	//build look up tables
	uint32_t i;
	for(i = 0; i < program->count; i++){
		automaton_statement_syntax_to_table(program->statements[i], tables);
	}
	//get global alphabet
	ctx->global_alphabet		= automaton_parsing_tables_get_global_alphabet(tables);
	ctx->global_fluents_count	= 0;
	//build automata
	bool pending_automata	= true;
	while(pending_automata){
		pending_automata	= false;
		for(i = 0; i < program->count; i++){
			if(program->statements[i]->type == COMPOSITION_AUT)
				pending_automata = automaton_statement_syntax_to_automaton(ctx, program->statements[i]->composition_def, tables) || pending_automata;
		}
	}
	for(i = 0; i < tables->composition_count; i++){
		if(tables->composition_entries[i]->solved){
			automaton_automaton_print(tables->composition_entries[i]->valuation.automaton_value, true, true, true, "*\t", "*\t");
		}
	}
	automaton_alphabet* global_alphabet	= automaton_parsing_tables_get_global_alphabet(tables);
	automaton_parsing_tables_destroy(tables);
	return ctx;
}



