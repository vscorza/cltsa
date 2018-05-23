/*
 * automaton_context.c
 *
 *  Created on: Apr 19, 2018
 *      Author: mariano
 */
#include "automaton_context.h"

automaton_parsing_tables* automaton_parsing_tables_create(){
	automaton_parsing_tables* tables	= malloc(sizeof(automaton_parsing_tables));
	tables->label_count			= 0;
	tables->label_entries		= NULL;
	tables->set_count			= 0;
	tables->set_entries			= NULL;
	tables->fluent_count		= 0;
	tables->fluent_entries		= NULL;
	tables->range_count			= 0;
	tables->range_entries		= NULL;
	tables->const_count			= 0;
	tables->const_entries		= NULL;
	tables->automaton_count		= 0;
	tables->automaton_entries	= NULL;
	tables->composition_count	= 0;
	tables->composition_entries	= NULL;
	return tables;
}
void automaton_parsing_tables_destroy_entry(automaton_parsing_table_entry** entries, uint32_t count){
	uint32_t i;
	for(i = 0; i < count; i++)
		automaton_parsing_table_entry_destroy(entries[i]);
	free(entries);
}

void automaton_parsing_tables_destroy(automaton_parsing_tables* tables){
	automaton_parsing_tables_destroy_entry(tables->label_entries,tables->label_count);
	automaton_parsing_tables_destroy_entry(tables->set_entries, tables->set_count);
	automaton_parsing_tables_destroy_entry(tables->fluent_entries, tables->fluent_count);
	automaton_parsing_tables_destroy_entry(tables->range_entries, tables->range_count);
	automaton_parsing_tables_destroy_entry(tables->const_entries, tables->const_count);
	automaton_parsing_tables_destroy_entry(tables->automaton_entries, tables->automaton_count);
	automaton_parsing_tables_destroy_entry(tables->composition_entries, tables->composition_count);
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
	if(entry->key != NULL)
		free(entry->key);
	int32_t i;
	switch(entry->type){
	case SET_ENTRY_AUT:
		if(entry->valuation.labels_value != NULL){
			char** labels	= entry->valuation.labels_value;
			for(i = 0; i < entry->valuation_count; i++){
				free(labels[i]);
			}
			free(entry->valuation.labels_value);
		}
		break;
	case COMPOSITION_ENTRY_AUT:
		if(entry->valuation.automaton_value != NULL)
			automaton_automaton_destroy(entry->valuation.automaton_value);
		break;
	case AUTOMATON_ENTRY_AUT:
		if(entry->valuation.automaton_value != NULL){
			automaton_automaton_destroy(entry->valuation.automaton_value);
			entry->valuation.automaton_value	= NULL;
		}
		break;
	case RANGE_ENTRY_AUT:
		if(entry->valuation.range_value != NULL){
			automaton_range_destroy(entry->valuation.range_value);
			entry->valuation.range_value	= NULL;
		}
		break;
	default:
		break;
	}
	/*
	if(entry->value != NULL){
		switch(entry->type){
		case LABEL_ENTRY_AUT:
			automaton_label_syntax_destroy((automaton_label_syntax*)entry->value);
			break;
		case SET_ENTRY_AUT:
			automaton_set_def_syntax_destroy((automaton_set_def_syntax*)entry->value);
			break;
		case FLUENT_ENTRY_AUT:
			automaton_fluent_syntax_destroy((automaton_fluent_syntax*)entry->value);
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
	}
	*/
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
			if(tables->const_entries[index]->solved)
				return tables->const_entries[index]->valuation.int_value;
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
		valuation = expr->integer_terminal;
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
	automaton_indexes_syntax* indexes	= NULL;
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
		indexes		= set->labels[i][j]->indexes;
		if(!is_set){//todo:solve indexes
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
		if(indexes != NULL){
			automaton_indexes_syntax_eval_strings(tables, &ret_value, count, indexes);
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
		automaton_signal_event* sig_event = automaton_signal_event_create(global_values[i], is_controllable? OUTPUT_SIG : INPUT_SIG);
		automaton_alphabet_add_signal_event(global_alphabet, sig_event);
		automaton_signal_event_destroy(sig_event, true);
	}
/*
	for(i = 0; i < global_count; i++)
		free(global_values[i]);
	free(global_values);
	for(i = 0; i < controllable_count; i++)
			free(controllable_values[i]);
		free(controllable_values);
*/
	return global_alphabet;
}
bool automaton_statement_syntax_to_automaton(automaton_automata_context* ctx, automaton_composition_syntax* composition_syntax
		, automaton_parsing_tables* tables, bool is_synchronous){
	int32_t main_index, index, i, j, k, l, m, n, o, p;
	main_index						= automaton_parsing_tables_get_entry_index(tables, COMPOSITION_ENTRY_AUT, composition_syntax->name);
	if(main_index >= 0)if(tables->composition_entries[main_index]->solved)	return false;
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
		automaton_automaton* automaton	= automaton_automata_compose(automata, composition_syntax->count, is_synchronous? SYNCHRONOUS : ASYNCHRONOUS);//SYNCHRONOUS);
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
		char** ret_value				= NULL;
		int32_t count					= 0;
		automaton_state_syntax* state;
		automaton_transition_syntax* transition;
		automaton_trace_label_syntax* trace_label;
		automaton_trace_label_atom_syntax* trace_label_atom;
		automaton_label_syntax* atom_label;
		char* element_to_find;
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
						if(!atom_label->is_set){

							if(atom_label->indexes != NULL){
								if(ret_value != NULL){
									for(n = 0; n < count; n++){
										free(ret_value[n]);
									}
									free(ret_value);
									ret_value = NULL;
									count = 0;
								}
								ret_value	= malloc(sizeof(char*));
								aut_dupstr(&(ret_value[0]),  atom_label->string_terminal);
								count 		= 1;
								automaton_indexes_syntax_eval_strings(tables, &ret_value, &count, atom_label->indexes);
								for(n = 0; n < count; n++){
									element_to_find		= ret_value[n];
									element_global_index= -1;
									element_position	= 0;
									for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++){
										if(strcmp(ctx->global_alphabet->list[m].name, element_to_find) == 0){
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
							}else{
								element_to_find		= atom_label->string_terminal;
								element_global_index= -1;
								element_position	= 0;
								for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++){
									if(strcmp(ctx->global_alphabet->list[m].name, element_to_find) == 0){
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
						}else{
							for(n = 0; n < (int32_t)(atom_label->set->count); n++){
								for(o = 0; o < (int32_t)(atom_label->set->labels_count[n]); o++){
									if (atom_label->set->labels[n][o]->indexes != NULL){
										if(ret_value != NULL){
											for(n = 0; n < count; n++){
												free(ret_value[n]);
											}
											free(ret_value);
											ret_value = NULL;
											count = 0;
										}
										ret_value	= malloc(sizeof(char*));
										aut_dupstr(&(ret_value[0]),  atom_label->set->labels[n][o]->string_terminal);
										count 		= 1;
										automaton_indexes_syntax_eval_strings(tables, &ret_value, &count, atom_label->set->labels[n][o]->indexes);
										for(n = 0; n < count; n++){
											element_to_find		= ret_value[n];
											element_global_index= -1;
											element_position	= 0;
											for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++){
												if(strcmp(ctx->global_alphabet->list[m].name, element_to_find) == 0){
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
									}else{
										element_to_find		= atom_label->set->labels[n][o]->string_terminal;
										element_global_index= -1;
										element_position	= 0;
										for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++){
											if(strcmp(ctx->global_alphabet->list[m].name, element_to_find) == 0){
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
					}
				}
			}
		}
		if(ret_value != NULL){
			for(n = 0; n < count; n++){
				free(ret_value[n]);
			}
			free(ret_value);
			ret_value = NULL;
			count = 0;
		}
		automaton_automaton* automaton	= automaton_automaton_create(composition_syntax->name, ctx, local_alphabet_count, local_alphabet);
		free(local_alphabet);
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
/*
 typedef struct automaton_state_label_syntax_str{
	char* name;
	struct automaton_indexes_syntax_str* indexes;
}automaton_state_label_syntax;
typedef struct automaton_state_syntax_str{
	bool is_ref;
	struct automaton_state_label_syntax_str* label;
	struct automaton_state_label_syntax_str* ref;
	uint32_t transitions_count;
	struct automaton_transition_syntax_str** transitions;
}automaton_state_syntax;
 * */

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
						if(!atom_label->is_set){
							if(atom_label->indexes != NULL){
								if(ret_value != NULL){
									for(n = 0; n < count; n++){
										free(ret_value[n]);
									}
									free(ret_value);
									ret_value = NULL;
									count = 0;
								}
								ret_value	= malloc(sizeof(char*));
								aut_dupstr(&(ret_value[0]),  atom_label->string_terminal);
								count 		= 1;
								automaton_indexes_syntax_eval_strings(tables, &ret_value, &count, atom_label->indexes);
								for(n = 0; n < count; n++){
									element_to_find		= ret_value[n];
									element_global_index= -1;
									element_position	= 0;
									for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++){
										if(strcmp(ctx->global_alphabet->list[m].name, element_to_find) == 0){
											element_global_index = m;
											break;
										}
									}
									if(element_global_index >= 0)
										automaton_transition_add_signal_event(automaton_transition, ctx, &(ctx->global_alphabet->list[element_global_index]));
								}
							}else{
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
						}else{
							for(n = 0; n < (int32_t)(atom_label->set->count); n++){
								for(o = 0; o < (int32_t)(atom_label->set->labels_count[n]); o++){
									if (atom_label->set->labels[n][o]->indexes != NULL){
										if(ret_value != NULL){
											for(n = 0; n < count; n++){
												free(ret_value[n]);
											}
											free(ret_value);
											ret_value = NULL;
											count = 0;
										}
										ret_value	= malloc(sizeof(char*));
										aut_dupstr(&(ret_value[0]),  atom_label->set->labels[n][o]->string_terminal);
										count 		= 1;
										automaton_indexes_syntax_eval_strings(tables, &ret_value, &count, atom_label->set->labels[n][o]->indexes);
										for(p = 0; p < count; p++){
											element_to_find		= ret_value[p];
											element_global_index= -1;
											element_position	= 0;
											for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++){
												if(strcmp(ctx->global_alphabet->list[m].name, element_to_find) == 0){
													element_global_index = m;
													break;
												}
											}
											if(element_global_index >= 0)
												automaton_transition_add_signal_event(automaton_transition, ctx, &(ctx->global_alphabet->list[element_global_index]));
										}
									}else{
										element_to_find		= atom_label->set->labels[n][o]->string_terminal;
										element_global_index= -1;
										element_position	= 0;
										for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++){
											if(strcmp(ctx->global_alphabet->list[m].name, element_to_find) == 0){
												element_global_index = m;
												break;
											}
										}
										if(element_global_index >= 0)
											automaton_transition_add_signal_event(automaton_transition, ctx, &(ctx->global_alphabet->list[element_global_index]));
									}
								}
							}
						}
					}
					automaton_automaton_add_transition(automaton, automaton_transition);
					automaton_transition_destroy(automaton_transition, true);
				}

			}
		}
		if(ret_value != NULL){
			for(n = 0; n < count; n++){
				free(ret_value[n]);
			}
			free(ret_value);
			ret_value = NULL;
			count = 0;
		}
		for(i = 0; i < labels_list_count; i++)
			free(labels_list[i]);
		free(labels_list);
		//set entry in table
		tables->composition_entries[main_index]->solved	= true;
		tables->composition_entries[main_index]->valuation_count			= 1;
		tables->composition_entries[main_index]->valuation.automaton_value	= automaton;
		return false;
	}
	return true;
}

bool automaton_statement_syntax_to_constant(automaton_automata_context* ctx, automaton_expression_syntax* const_def_syntax
		, automaton_parsing_tables* tables){
	uint32_t main_index	= automaton_parsing_tables_get_entry_index(tables, CONST_ENTRY_AUT, const_def_syntax->string_terminal);
	tables->composition_entries[main_index]->solved					= true;
	tables->composition_entries[main_index]->valuation_count		= 1;
	tables->composition_entries[main_index]->valuation.int_value	= automaton_expression_syntax_evaluate(tables, const_def_syntax);
	return false;
}

void automaton_indexes_syntax_eval_strings(automaton_parsing_tables* tables, char*** a, int32_t* a_count, automaton_indexes_syntax* indexes){
	uint32_t i, j, k;
	int32_t *lower_index, *upper_index, *current_index, position;
	uint32_t total_combinations = 1;

	char buffer[400], buffer2[400];

	char** ret_value			= NULL;
	int32_t inner_count			= 0;

	int32_t effective_count	= 0;

	for(i = 0; i < indexes->count; i++){
		if(!(indexes->indexes[i]->is_expr)){
			effective_count++;
		}
	}

	if(effective_count == 0)return;

	lower_index	= malloc(sizeof(int32_t) * effective_count);
	upper_index	= malloc(sizeof(int32_t) * effective_count);
	current_index	= malloc(sizeof(int32_t) * effective_count);

	j = 0;
	for(i = 0; i < indexes->count; i++){
		if(!(indexes->indexes[i]->is_expr)){
			automaton_index_syntax_get_range(tables, indexes->indexes[i], &(lower_index[j]), &(upper_index[j]));
			if(lower_index[j] >= upper_index[j]){//TODO: report bad index
				lower_index[j]	= 	upper_index[j]	= 	current_index[j]	= -1;
			}else{
				total_combinations *= (uint32_t)(upper_index[j] - lower_index[j]);
				current_index[j]	= lower_index[j];
			}
			j++;
		}
	}

	for(i = 0; i < total_combinations; i++){
		buffer[0] = '\0';
		for(j = 0; j < (uint32_t)effective_count; j++){
			sprintf(buffer, "%s_%d", buffer, current_index[j]);
		}

		for(k = 0; k < (uint32_t)*a_count; k++){
			sprintf(buffer2, "%s%s", (*a)[k], buffer);

			aut_push_string_to_list(&ret_value, &inner_count, buffer2, &position, false);
		}


		j	= effective_count -1;
		current_index[j]++;
		while(current_index[j] > upper_index[j]){
			current_index[j]	= lower_index[j];
			if(j == 0)break;
			j--;
			current_index[j]++;
		}
	}

	for(i = 0; i < (uint32_t)*a_count; i++){
		free((*a)[i]);
	}
	if(*a != NULL)
		free(*a);

	*a_count	= inner_count;
	(*a)		= ret_value;

	free(lower_index);
	free(upper_index);
	free(current_index);
}

void automaton_index_syntax_get_range(automaton_parsing_tables* tables, automaton_index_syntax* index, int32_t *lower_index, int32_t *upper_index){
	int32_t i;
	if(!index->is_expr){
		if(index->is_range){
			i			 	= automaton_parsing_tables_get_entry_index(tables, CONST_ENTRY_AUT, index->lower_ident);
			*lower_index	=tables->const_entries[i]->valuation.int_value;
			i	 			= automaton_parsing_tables_get_entry_index(tables, CONST_ENTRY_AUT, index->upper_ident);
			*upper_index	=tables->const_entries[i]->valuation.int_value;
		}else{
			//TODO: no está guardando el valuation del range en el range_def
			i	 			= automaton_parsing_tables_get_entry_index(tables, RANGE_ENTRY_AUT, index->upper_ident);
			*lower_index	=tables->range_entries[i]->valuation.range_value->lower_value;
			*upper_index	=tables->range_entries[i]->valuation.range_value->upper_value;
		}
	}
}

automaton_range* automaton_range_syntax_evaluate(automaton_parsing_tables *tables, char* name, automaton_expression_syntax *range_def_syntax){
	int32_t lower_value	= automaton_expression_syntax_evaluate(tables, range_def_syntax->first);
	int32_t upper_value	= automaton_expression_syntax_evaluate(tables, range_def_syntax->second);
	return automaton_range_create(name, lower_value, upper_value);
}

bool automaton_statement_syntax_to_range(automaton_automata_context* ctx, automaton_expression_syntax* range_def_syntax
		, automaton_parsing_tables* tables){
	uint32_t main_index	= automaton_parsing_tables_get_entry_index(tables, RANGE_ENTRY_AUT, range_def_syntax->string_terminal);
	tables->range_entries[main_index]->solved					= true;
	tables->range_entries[main_index]->valuation_count		= 1;
	tables->range_entries[main_index]->valuation.range_value	= automaton_range_syntax_evaluate(tables,range_def_syntax->string_terminal,  range_def_syntax->first);
	return false;
}

automaton_automata_context* automaton_automata_context_create_from_syntax(automaton_program_syntax* program, char* ctx_name, bool is_synchronous){
	automaton_parsing_tables* tables	= automaton_parsing_tables_create();
	automaton_automata_context* ctx		= malloc(sizeof(automaton_automata_context));

	//build look up tables
	uint32_t i;
	for(i = 0; i < program->count; i++){
		automaton_statement_syntax_to_table(program->statements[i], tables);
	}
	//build constants and ranges
	for(i = 0; i < program->count; i++){
		if(program->statements[i]->type == CONST_AUT)
			automaton_statement_syntax_to_constant(ctx, program->statements[i]->const_def, tables);
		if(program->statements[i]->type == RANGE_AUT)
			automaton_statement_syntax_to_range(ctx, program->statements[i]->range_def, tables);
	}
	//get global alphabet
	automaton_alphabet* global_alphabet		= automaton_parsing_tables_get_global_alphabet(tables);
	automaton_automata_context_initialize(ctx, ctx_name, global_alphabet, 0, NULL);
	automaton_alphabet_destroy(global_alphabet);
	//build automata
	bool pending_statements	= true;
	while(pending_statements){
		pending_statements	= false;
		for(i = 0; i < program->count; i++){
			if(program->statements[i]->type == COMPOSITION_AUT){
				pending_statements = automaton_statement_syntax_to_automaton(ctx, program->statements[i]->composition_def, tables, is_synchronous) || pending_statements;
			}
		}
	}

	char buf[255];

	for(i = 0; i < tables->composition_count; i++){
		if(tables->composition_entries[i]->solved){
			//automaton_automaton_print(tables->composition_entries[i]->valuation.automaton_value, true, true, true, "*\t", "*\t");
			sprintf(buf, "%s_%d_result_%s.fsp", ctx_name, i, is_synchronous? "synch": "asynch");
			automaton_automaton_print_fsp(tables->composition_entries[i]->valuation.automaton_value, buf);
		}
	}

	automaton_parsing_tables_destroy(tables);
	return ctx;
}



