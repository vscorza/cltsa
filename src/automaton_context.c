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
	case FLUENT_ENTRY_AUT:
		if(entry->valuation.fluent_value != NULL){
			automaton_fluent_destroy(entry->valuation.fluent_value, true);
			entry->valuation.fluent_value	= NULL;
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

int32_t automaton_expression_syntax_evaluate(automaton_parsing_tables* tables, automaton_expression_syntax* expr, automaton_indexes_valuation* indexes_valuation){
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
		valuation = automaton_expression_syntax_evaluate(tables, expr->first, indexes_valuation);
		break;
	case BINARY_TYPE_AUT:
		left_valuation = automaton_expression_syntax_evaluate(tables, expr->first, indexes_valuation);
		right_valuation = automaton_expression_syntax_evaluate(tables, expr->second, indexes_valuation);
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
		;bool found = false;
		if(indexes_valuation != NULL){
			uint32_t i;
			for(i = 0; i < indexes_valuation->count; i++){
				if(strcmp(expr->string_terminal, indexes_valuation->ranges[i]->name) == 0){
					valuation	= indexes_valuation->current_values[i];
					found	= true;
				}
				if(found){
					break;
				}
			}
		}
		if(!found){
			ref_index 	= automaton_parsing_tables_get_entry_index(tables, CONST_ENTRY_AUT, expr->string_terminal);
			ref_entry	= tables->const_entries[ref_index];
			if(!ref_entry->solved){
				automaton_expression_syntax_evaluate(tables, (automaton_expression_syntax*)ref_entry->value, indexes_valuation);
			}
			valuation = ref_entry->valuation.int_value;
		}
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
	uint32_t i, j,k;
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
		}
		if(indexes != NULL){
			char** tmp_value	= malloc(sizeof(char*) * inner_count);
			for(k = 0; k < (uint32_t)inner_count;k++)aut_dupstr(&(tmp_value[k]), inner_value[k]);
			automaton_indexes_syntax_eval_strings(tables, NULL, &tmp_value, &inner_count, indexes);
			aut_merge_string_lists(&ret_value, count, tmp_value, inner_count, true, false);
			for(k = 0; k < (uint32_t)inner_count;k++)free(tmp_value[k]);
			free(tmp_value);
		}else{
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
	int32_t global_signals_index	= automaton_parsing_tables_get_entry_index(tables, SET_ENTRY_AUT, GLOBAL_SIGNALS_NAME_AUT);
	int32_t output_signals_index	= automaton_parsing_tables_get_entry_index(tables, SET_ENTRY_AUT, OUTPUT_SIGNALS_NAME_AUT);
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
	automaton_signal_event* sig_event;
	for(i = 0; i < global_count; i++){
		is_controllable	= false;
		for(j = 0; j < controllable_count; j++){
			if(strcmp(global_values[i], controllable_values[j]) == 0){
				is_controllable	= true;
				break;
			}
		}
		sig_event = automaton_signal_event_create(global_values[i], is_controllable? OUTPUT_SIG : INPUT_SIG);
		automaton_alphabet_add_signal_event(global_alphabet, sig_event);
		automaton_signal_event_destroy(sig_event, true);
	}
	char signal_name[255];
	if(global_signals_index >= 0){
		global_count = 0;
		controllable_count = 0;
		global_values		= automaton_set_syntax_evaluate(tables, ((automaton_set_def_syntax*)tables->set_entries[global_signals_index]->value)->set
					, &global_count, ((automaton_set_def_syntax*)tables->set_entries[global_signals_index]->value)->name);
		if(output_signals_index >= 0){
			controllable_values	= automaton_set_syntax_evaluate(tables, ((automaton_set_def_syntax*)tables->set_entries[output_signals_index]->value)->set
					, &controllable_count, ((automaton_set_def_syntax*)tables->set_entries[output_signals_index]->value)->name);
		}
		for(i = 0; i < global_count; i++){
				is_controllable	= false;
				for(j = 0; j < controllable_count; j++){
					if(strcmp(global_values[i], controllable_values[j]) == 0){
						is_controllable	= true;
						break;
					}
				}
				//add on, off and primed elements to alphabet
				strcpy(signal_name, global_values[i]);
				strcat(signal_name, SIGNAL_ON_SUFFIX);
				sig_event = automaton_signal_event_create(signal_name, is_controllable? OUTPUT_SIG : INPUT_SIG);
				automaton_alphabet_add_signal_event(global_alphabet, sig_event);
				automaton_signal_event_destroy(sig_event, true);
				strcpy(signal_name, global_values[i]);
				strcat(signal_name, SIGNAL_OFF_SUFFIX);
				sig_event = automaton_signal_event_create(signal_name, is_controllable? OUTPUT_SIG : INPUT_SIG);
				automaton_alphabet_add_signal_event(global_alphabet, sig_event);
				automaton_signal_event_destroy(sig_event, true);
				strcpy(signal_name, global_values[i]);
				strcat(signal_name, SIGNAL_PRIME_SUFFIX);
				strcat(signal_name, SIGNAL_ON_SUFFIX);
				sig_event = automaton_signal_event_create(signal_name, is_controllable? OUTPUT_SIG : INPUT_SIG);
				automaton_alphabet_add_signal_event(global_alphabet, sig_event);
				automaton_signal_event_destroy(sig_event, true);
				strcpy(signal_name, global_values[i]);
				strcat(signal_name, SIGNAL_PRIME_SUFFIX);
				strcat(signal_name, SIGNAL_OFF_SUFFIX);
				sig_event = automaton_signal_event_create(signal_name, is_controllable? OUTPUT_SIG : INPUT_SIG);
				automaton_alphabet_add_signal_event(global_alphabet, sig_event);
				automaton_signal_event_destroy(sig_event, true);
			}
	}
	return global_alphabet;
}
automaton_indexes_valuation* automaton_indexes_valuation_create_from_indexes(automaton_parsing_tables* tables, automaton_indexes_syntax* indexes){
	automaton_indexes_valuation* valuation	= automaton_indexes_valuation_create();
	valuation->count	= indexes->count;
	valuation->current_values	= malloc(sizeof(int32_t) * valuation->count);
	valuation->ranges			= malloc(sizeof(automaton_range*) * valuation->count);
	uint32_t i;
	int32_t lower_index, upper_index;
	char name[40];
	automaton_range* range;
	valuation->total_combinations	= 1;
	for(i = 0; i < valuation->count; i++){
		automaton_index_syntax_get_range(tables, indexes->indexes[i], &(lower_index), &(upper_index));
		if(indexes->indexes[i]->is_expr){
			if(indexes->indexes[i]->expr->type == INTEGER_TERMINAL_TYPE_AUT){
				sprintf(name, "%d", indexes->indexes[i]->expr->integer_terminal);
			}else{
				sprintf(name, "%s", indexes->indexes[i]->expr->string_terminal);
			}
		}else{
			sprintf(name, "%s", indexes->indexes[i]->lower_ident);
		}
		range	= automaton_range_create(name, lower_index, upper_index);
		valuation->current_values[i]	= lower_index;
		valuation->ranges[i]			= range;
		valuation->total_combinations	*= (upper_index - lower_index);
	}
	return valuation;
}

bool automaton_statement_syntax_to_automaton(automaton_automata_context* ctx, automaton_composition_syntax* composition_syntax
		, automaton_parsing_tables* tables, bool is_synchronous){
	int32_t main_index, index, i, j, k, l, m, n, o, p, r, s;
	main_index						= automaton_parsing_tables_get_entry_index(tables, COMPOSITION_ENTRY_AUT, composition_syntax->name);
	if(main_index >= 0)if(tables->composition_entries[main_index]->solved)	return false;
	//check whether composition syntax is a composition or a single automaton description
	if(composition_syntax->components != NULL){//MULTIPLE COMPONENTS (AUTOMATA)
		aut_context_log("mult. components.%s\n", composition_syntax->name);
		//if one component has not been solved then report pending automata
		for(i = 0; i < (int32_t)composition_syntax->count; i++){
			index						= automaton_parsing_tables_get_entry_index(tables, COMPOSITION_ENTRY_AUT, composition_syntax->components[i]->ident);
			if(index < 0)
				return true;
			if(!tables->composition_entries[index]->solved)
				return true;
		}
		//build composition and add to table
		automaton_automaton** automata	= malloc(sizeof(automaton_automaton*) * composition_syntax->count);
		for(i = 0; i < (int32_t)composition_syntax->count; i++){
			//TODO: update transitions with prefixes/indexes
			index						= automaton_parsing_tables_get_entry_index(tables, COMPOSITION_ENTRY_AUT, composition_syntax->components[i]->ident);
			automata[i]					= tables->composition_entries[index]->valuation.automaton_value;
		}
		//if is game build fluents and add to automata
		uint32_t composition_count	= composition_syntax->count;
		if(composition_syntax->is_game){
			uint32_t new_composition_count		= composition_count + ctx->global_fluents_count;
			automaton_automaton** new_automata	= malloc(sizeof(automaton_automaton*) * new_composition_count);
			for(i = 0; i < (int32_t)composition_count; i++){
				new_automata[i]	= automata[i];
			}
			//build fluent automata
			for(i = 0; i < (int32_t)ctx->global_fluents_count; i++){
				new_automata[i + composition_count]	= automaton_fluent_build_automaton(ctx, i);
				char buf[255];
				//automaton_automaton_print(tables->composition_entries[i]->valuation.automaton_value, true, true, true, "*\t", "*\t");
				sprintf(buf, "%s_%d_fluent_%s.fsp", ctx->name, i + composition_count, is_synchronous? "synch": "asynch");
				automaton_automaton_print_fsp(new_automata[i + composition_count], buf);
			}
			free(automata);
			automata							= new_automata;
			composition_count					= new_composition_count;
		}
		aut_context_log("composing.\n");
		automaton_automaton* automaton	= automaton_automata_compose(automata, composition_count, is_synchronous? CONCURRENT : ASYNCHRONOUS, composition_syntax->is_game);//SYNCHRONOUS);
		if(composition_syntax->is_game)
			for(i = 0; i < (int32_t)ctx->global_fluents_count; i++)
				automaton_automaton_destroy(automata[composition_count - i - 1]);
		tables->composition_entries[main_index]->solved	= true;
		tables->composition_entries[main_index]->valuation_count			= 1;
		tables->composition_entries[main_index]->valuation.automaton_value	= automaton;
		aut_context_log("done, %d states\n", automaton->transitions_count);
		free(automata);
		return false;
	}else{//SINGLE COMPONENT (AUTOMATON)
		aut_context_log("single component.%s\n", composition_syntax->name);
		/** BUILD LOCAL ALPHABET **/
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
		automaton_indexes_valuation* current_valuation;
		for(i = 0; i < (int32_t)composition_syntax->count; i++){
			state	= composition_syntax->states[i];
			if(state->label->indexes != NULL)
				current_valuation 	= automaton_indexes_valuation_create_from_indexes(tables, state->label->indexes);
			else
				current_valuation	= NULL;
			for(j = 0; j < (int32_t)state->transitions_count; j++){
				transition	= state->transitions[j];
				//TODO: take guards into consideration
				for(k = 0; k < (int32_t)transition->count; k++){
					trace_label	= transition->labels[k];
					for(l = 0; l < (int32_t)trace_label->count; l++){
						trace_label_atom	= trace_label->atoms[l];
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

								automaton_indexes_syntax_eval_strings(tables, current_valuation, &ret_value, &count, atom_label->indexes);
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
										printf("Looking for: %s [NOT FOUND]\nGlobal Alphabet:\n(", element_to_find);
										for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++){
											printf("%s%s", ctx->global_alphabet->list[m].name, m < ((int32_t)ctx->global_alphabet->count - 1)? ",":"");
										}
										printf(")\n");
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
									printf("Looking for: %s [NOT FOUND]\nGlobal Alphabet:\n(", element_to_find);
									for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++){
										printf("%s%s", ctx->global_alphabet->list[m].name, m < ((int32_t)ctx->global_alphabet->count - 1)? ",":"");
									}
									printf(")\n");
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
										automaton_indexes_syntax_eval_strings(tables, NULL, &ret_value, &count, atom_label->set->labels[n][o]->indexes);
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
		aut_context_log("local alphabet built with size %d\n", local_alphabet_count);
		/** CREATE AUTOMATON **/
		automaton_automaton* automaton	= automaton_automaton_create(composition_syntax->name, ctx, local_alphabet_count, local_alphabet, false, false);
		free(local_alphabet);
		//add transitions
		//map state label to int
		char** labels_list	= NULL;
		int32_t labels_list_count	= 0;
		int32_t label_position;
		uint32_t from_state, to_state, *current_from_state, *next_from_state;
		uint32_t current_from_state_size	= LIST_INITIAL_SIZE;
		uint32_t current_from_state_count	= 0;
		current_from_state					= malloc(sizeof(uint32_t) * current_from_state_size);
		uint32_t next_from_state_size		= LIST_INITIAL_SIZE;
		uint32_t next_from_state_count		= 0;
		next_from_state						= malloc(sizeof(uint32_t) * next_from_state_size);
		uint32_t automaton_transition_size	= LIST_INITIAL_SIZE;
		uint32_t automaton_transition_count	= 0;
		automaton_transition** current_automaton_transition	= malloc(sizeof(automaton_transition*) * automaton_transition_size);
		uint32_t first_state;
		bool first_state_set	= false;
		uint32_t added_state	= composition_syntax->count + 1;
		char label_indexes[255];
		label_indexes[0] = '\0';
		bool first_index_set	= false;
		//HANDLE REF STATES
		//initialize states list

		for(i = 0; i < (int32_t)composition_syntax->count; i++){
			state	= composition_syntax->states[i];
			if(state->label->indexes != NULL)
				current_valuation 	= automaton_indexes_valuation_create_from_indexes(tables, state->label->indexes);
			else
				current_valuation	= NULL;
			bool first_run_from	= true;

			automaton_indexes_valuation_set_label(current_valuation, state->label->name, label_indexes);
			aut_push_string_to_list(&labels_list, &labels_list_count, label_indexes, &label_position, true, false);

			if(current_valuation != NULL){
				automaton_indexes_valuation_destroy(current_valuation);
				current_valuation	= NULL;
			}
		}

		//update ref states
		for(i = 0; i < (int32_t)composition_syntax->count; i++){
			state	= composition_syntax->states[i];
			if(state->ref != NULL){
				if(state->label->indexes != NULL)
					current_valuation 	= automaton_indexes_valuation_create_from_indexes(tables, state->label->indexes);
				else
					current_valuation	= NULL;
				label_indexes[0] = '\0';
				automaton_indexes_valuation_set_label(current_valuation, state->label->name, label_indexes);
				aut_push_string_to_list(&labels_list, &labels_list_count, label_indexes, &label_position, true, false);
				current_from_state[0]	= from_state	= (uint32_t) label_position;
				current_from_state_count= 1;
				if(current_valuation != NULL){
					automaton_indexes_valuation_destroy(current_valuation);
					current_valuation	= NULL;
				}
				if(state->ref->indexes != NULL)
					current_valuation 	= automaton_indexes_valuation_create_from_indexes(tables, state->ref->indexes);
				else
					current_valuation	= NULL;
				automaton_indexes_valuation_set_label(current_valuation, state->ref->name, label_indexes);
				aut_push_string_to_list(&labels_list, &labels_list_count, label_indexes, &label_position, true, false);
				to_state	= (uint32_t) label_position;
				if(current_valuation != NULL){
					automaton_indexes_valuation_destroy(current_valuation);
					current_valuation	= NULL;
				}

				if(!first_state_set){
					automaton_automaton_add_initial_state(automaton, to_state);
					first_state_set	= true;
				}else{
					//create empty transitions
					current_automaton_transition[0]	= automaton_transition_create(current_from_state[0], to_state);
					automaton_automaton_add_transition(automaton, current_automaton_transition[0]);
					automaton_transition_destroy(current_automaton_transition[0], true);
				}
				continue;
			}
		}

		//STATE ITERATION
		aut_context_log("State count iteration for %d states %d\n", composition_syntax->count);
		for(i = 0; i < (int32_t)composition_syntax->count; i++){
			state	= composition_syntax->states[i];
			if(state->ref != NULL){//do not process ref states at this loop
				continue;
			}
			if(state->label->indexes != NULL)
				current_valuation 	= automaton_indexes_valuation_create_from_indexes(tables, state->label->indexes);
			else
				current_valuation	= NULL;
			bool first_run_from	= true;
			//FROM STATE INDEXES ITERATION
			aut_context_log("\ts[%d]\n", i);
			while(first_run_from || (current_valuation != NULL && automaton_indexes_valuation_has_next(current_valuation))){
#if DEBUG_PARSE_STATES
				if(current_valuation != NULL)
					automaton_indexes_valuation_print(current_valuation, "", "\n");
#endif
				first_run_from = false;
				label_indexes[0] = '\0';
				//get incremental valuation on from state indexes
				automaton_indexes_valuation_set_label(current_valuation, state->label->name, label_indexes);
				aut_push_string_to_list(&labels_list, &labels_list_count, label_indexes, &label_position, false, false);
				from_state	= (uint32_t)label_position;
#if DEBUG_PARSE_STATES
				aut_context_log("LAB.LIST(state):");
				for(k = 0; k < labels_list_count; k++){
					aut_context_log("%s%s", labels_list[k], k == labels_list_count - 1 ? "" : ",");
				}
				aut_context_log("\n");
#endif

				//set first state if needed
				if(!first_state_set){
					aut_context_log("Init.state set at: %d\n", from_state);
					automaton_automaton_add_initial_state(automaton, from_state);
					first_state_set	= true;
				}
				//aut_context_log("\t(%s):", label_indexes);
				//FROM STATE TRANSITIONS ITERATION
				for(j = 0; j < (int32_t)state->transitions_count; j++){
					current_from_state[0]	= from_state;
					current_from_state_count= 1;
					aut_context_log("curr_from:%d\n", current_from_state[0]);
					transition	= state->transitions[j];
					if(transition->condition != NULL){
						aut_context_log("\ncond.:%d\n",automaton_expression_syntax_evaluate(tables, transition->condition, current_valuation));
						if(!automaton_expression_syntax_evaluate(tables, transition->condition, current_valuation)){
							continue;
						}
					}
					automaton_indexes_valuation_set_to_label(tables, current_valuation, state->label->indexes, transition->to_state->indexes,transition->to_state->name, label_indexes);
					aut_push_string_to_list(&labels_list, &labels_list_count, label_indexes, &label_position, false, false);
#if DEBUG_PARSE_STATES
					aut_context_log("LAB.LIST(trans, %s):", label_indexes);
					for(k = 0; k < labels_list_count; k++){
						aut_context_log("%s%s", labels_list[k], k == labels_list_count - 1 ? "" : ",");
					}
					aut_context_log("\n");
#endif
					to_state	= (uint32_t)label_position;


					//TODO: take indexes and guard into consideration
					for(k = 0; k < (int32_t)transition->count; k++){
						if(k < (((int32_t)transition->count) - 1)){
							to_state	= added_state++;
						}else{
							to_state	= (uint32_t)label_position;
						}
						for(r = 0; r < (int32_t)current_from_state_count; r++){
							if(automaton_transition_count >= (automaton_transition_size - 1)){
								uint32_t new_size	= automaton_transition_size * LIST_INCREASE_FACTOR;
								automaton_transition** new_automaton_transitions;
								new_automaton_transitions =  malloc(sizeof(automaton_transition*) * new_size);
								for(l = 0; l < (int32_t)automaton_transition_count; l++)
									new_automaton_transitions[l]	= current_automaton_transition[l];
								free(current_automaton_transition);
								automaton_transition_size	= new_size;
								current_automaton_transition		= new_automaton_transitions;
							}
							current_automaton_transition[automaton_transition_count++]	= automaton_transition_create(current_from_state[r], to_state);
							aut_context_log("(%d->%d)[General]<%d>", current_from_state[r], to_state,automaton_transition_count);
							if(next_from_state_count >= (next_from_state_size - 1)){
								uint32_t new_size	= next_from_state_size * LIST_INCREASE_FACTOR;
								uint32_t* new_next_from	= malloc(sizeof(uint32_t) * new_size);
								for(l = 0; l < (int32_t)next_from_state_count; l++){
									new_next_from[l]	= next_from_state[l];
								}
								free(next_from_state);
								next_from_state_size	= new_size;
								next_from_state			= new_next_from;
							}
							next_from_state[next_from_state_count++]	= to_state;
							trace_label	= transition->labels[k];
							//IMPLICIT STATE TRANSITIONS ITERATION ( s_i = (a -> b -> c -> S_j).
							first_index_set = false;
							for(l = 0; l < (int32_t)trace_label->count; l++){

								trace_label_atom	= trace_label->atoms[l];
								atom_label			= trace_label_atom->label;
								if(!atom_label->is_set){
									//process set ( Alphabet -> ...)
									if(atom_label->indexes != NULL){
										if(!first_index_set){
											automaton_transition_destroy(current_automaton_transition[--automaton_transition_count], true);
											next_from_state_count--;
											first_index_set	= true;
										}
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
										automaton_indexes_syntax_eval_strings(tables, current_valuation, &ret_value, &count, atom_label->indexes);
										for(n = 0; n < count; n++){
											element_to_find		= ret_value[n];
											element_global_index= -1;
											element_position	= 0;
											aut_context_log("%s->)", ret_value[n]);
											for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++){
												if(strcmp(ctx->global_alphabet->list[m].name, element_to_find) == 0){
													element_global_index = m;
													break;
												}
											}
											if(element_global_index >= 0){
												automaton_indexes_valuation_set_to_label(tables, current_valuation, state->label->indexes, transition->to_state->indexes,transition->to_state->name, label_indexes);
												aut_push_string_to_list(&labels_list, &labels_list_count, label_indexes, &label_position, false, false);
#if DEBUG_PARSE_STATES
												aut_context_log("LAB.LIST(set):");
												for(l = 0; l < labels_list_count; l++){
													aut_context_log("%s%s", labels_list[l], l == labels_list_count - 1 ? "" : ",");
												}
												aut_context_log("\n");
#endif
												to_state	= (uint32_t)label_position;
												if(automaton_transition_count >= (automaton_transition_size - 1)){
													uint32_t new_size	= automaton_transition_size * LIST_INCREASE_FACTOR;
													automaton_transition** new_transitions	= malloc(sizeof(automaton_transition*) * new_size);
													for(l = 0; l < (int32_t)automaton_transition_count; l++){
														new_transitions[l]	= current_automaton_transition[l];
													}
													free(current_automaton_transition);
													automaton_transition_size	= new_size;
													current_automaton_transition		= new_transitions;
												}
												current_automaton_transition[automaton_transition_count++]	= automaton_transition_create(current_from_state[r], to_state);
												aut_context_log("(%d->%d)[!SET|index]<%d>", current_from_state[r], to_state,automaton_transition_count);
												if(next_from_state_count >= (next_from_state_size - 1)){
													uint32_t new_size	= next_from_state_size * LIST_INCREASE_FACTOR;
													uint32_t* new_next_from	= malloc(sizeof(uint32_t) * new_size);
													for(l = 0; l < (int32_t)next_from_state_count; l++){
														new_next_from[l]	= next_from_state[l];
													}
													free(next_from_state);
													next_from_state_size	= new_size;
													next_from_state			= new_next_from;
												}
												next_from_state[next_from_state_count++]	= to_state;
												automaton_transition_add_signal_event(current_automaton_transition[automaton_transition_count - 1], ctx, &(ctx->global_alphabet->list[element_global_index]));
												aut_context_log(".%s", ctx->global_alphabet->list[element_global_index].name);
												//automaton_automaton_add_transition(automaton, automaton_transition[automaton_transition_count - 1]);
											}else{
												printf("Element not found:%s\nAlphabet:", element_to_find);
												for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++)
													printf("%s%s", ctx->global_alphabet->list[m].name, m == ((int32_t)ctx->global_alphabet->count - 1) ? "": ",");
												printf("\n");
												fflush(stdout);
												exit(-1);
											}
										}
									}else{
										if(!first_index_set){
											automaton_transition_destroy(current_automaton_transition[--automaton_transition_count], true);
											next_from_state_count--;
											first_index_set	= true;
										}
										element_global_index= -1;
										element_position	= 0;
										for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++){
											if(strcmp(ctx->global_alphabet->list[m].name, atom_label->string_terminal) == 0){
												element_global_index = m;
												break;
											}
										}
										if(element_global_index >= 0){
											if(automaton_transition_count >= (automaton_transition_size - 1)){
												uint32_t new_size	= automaton_transition_size * LIST_INCREASE_FACTOR;
												automaton_transition** new_transitions	= malloc(sizeof(automaton_transition*) * new_size);
												for(l = 0; l < (int32_t)automaton_transition_count; l++){
													new_transitions[l]	= current_automaton_transition[l];
												}
												free(current_automaton_transition);
												automaton_transition_size	= new_size;
												current_automaton_transition		= new_transitions;
											}
											current_automaton_transition[automaton_transition_count++]	= automaton_transition_create(current_from_state[r], to_state);
											aut_context_log("(%d->%d)[!SET]<%d>", current_from_state[r], to_state,automaton_transition_count);
											automaton_indexes_valuation_set_to_label(tables, current_valuation, state->label->indexes, transition->to_state->indexes,transition->to_state->name, label_indexes);
											aut_push_string_to_list(&labels_list, &labels_list_count, label_indexes, &label_position, false, false);
#if DEBUG_PARSE_STATES
											aut_context_log("LAB.LIST(!set):");
											for(l = 0; l < labels_list_count; l++){
												aut_context_log("%s%s", labels_list[l], l == labels_list_count - 1 ? "" : ",");
											}
											aut_context_log("\n");
#endif
											if(next_from_state_count >= (next_from_state_size - 1)){
												uint32_t new_size	= next_from_state_size * LIST_INCREASE_FACTOR;
												uint32_t* new_next_from	= malloc(sizeof(uint32_t) * new_size);
												for(l = 0; l < (int32_t)next_from_state_count; l++){
													new_next_from[l]	= next_from_state[l];
												}
												free(next_from_state);
												next_from_state_size	= new_size;
												next_from_state			= new_next_from;
											}
											next_from_state[next_from_state_count++]	= to_state;
											to_state	= (uint32_t)label_position;
											automaton_transition_add_signal_event(current_automaton_transition[automaton_transition_count - 1], ctx, &(ctx->global_alphabet->list[element_global_index]));
											aut_context_log(".%s", ctx->global_alphabet->list[element_global_index].name);
											//automaton_automaton_add_transition(automaton, automaton_transition[automaton_transition_count - 1]);
										}else{
											printf("Element not found:%s\nAlphabet:", element_to_find);
											for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++)
												printf("%s%s", ctx->global_alphabet->list[m].name, m == ((int32_t)ctx->global_alphabet->count - 1) ? "": ",");
											printf("\n");
											fflush(stdout);
											exit(-1);
										}
									}

								}else{
									//CONCURRENT ELEMENTS ITERATION (<a, b, c> -> ...)
									aut_context_log("<");
									for(n = 0; n < (int32_t)(atom_label->set->count); n++){
										for(o = 0; o < (int32_t)(atom_label->set->labels_count[n]); o++){
											if (atom_label->set->labels[n][o]->indexes != NULL){
												//automaton_transition_destroy(current_automaton_transition[automaton_transition_count--], true);
												if(ret_value != NULL){
													for(s = 0; s < count; s++){
														free(ret_value[s]);
													}
													free(ret_value);
													ret_value = NULL;
													count = 0;
												}
												ret_value	= malloc(sizeof(char*));
												aut_dupstr(&(ret_value[0]),  atom_label->set->labels[n][o]->string_terminal);
												count 		= 1;
												automaton_indexes_syntax_eval_strings(tables, current_valuation, &ret_value, &count, atom_label->set->labels[n][o]->indexes);
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
													if(element_global_index >= 0){
														aut_context_log("%s,", ctx->global_alphabet->list[element_global_index].name);
														automaton_transition_add_signal_event(current_automaton_transition[automaton_transition_count - 1], ctx, &(ctx->global_alphabet->list[element_global_index]));
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
												if(element_global_index >= 0){
													aut_context_log("%s,", ctx->global_alphabet->list[element_global_index].name);
													automaton_transition_add_signal_event(current_automaton_transition[automaton_transition_count - 1], ctx, &(ctx->global_alphabet->list[element_global_index]));
												}
											}
										}
									}
									aut_context_log(">");
								}

							}
							for(s= 0; s < (int32_t)automaton_transition_count; s++){
								automaton_automaton_add_transition(automaton, current_automaton_transition[s]);
								automaton_transition_destroy(current_automaton_transition[s], true);
							}
							automaton_transition_count	= 0;
						}
						current_from_state_count	= 0;
						for(s = 0; s < (int32_t)next_from_state_count; s++){
							if(current_from_state_count >= (current_from_state_size - 1)){
								uint32_t new_size	= current_from_state_size * LIST_INCREASE_FACTOR;
								uint32_t* new_current_from	= malloc(sizeof(uint32_t) * new_size);
								for(l = 0; l < (int32_t)current_from_state_count; l++){
									new_current_from[l]	= current_from_state[l];
								}
								free(current_from_state);
								current_from_state_size	= new_size;
								current_from_state			= new_current_from;
							}
							current_from_state[current_from_state_count++]	= next_from_state[s];
						}
						next_from_state_count	= 0;
					}
				}
				aut_context_log("\n");
				automaton_indexes_valuation_increase(current_valuation);
			}
			if(current_valuation != NULL){
				automaton_indexes_valuation_destroy(current_valuation);current_valuation = NULL;
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
		free(current_from_state);
		free(next_from_state);
		free(current_automaton_transition);
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
	tables->const_entries[main_index]->solved				= true;
	tables->const_entries[main_index]->valuation_count		= 1;
	tables->const_entries[main_index]->valuation.int_value	= automaton_expression_syntax_evaluate(tables, const_def_syntax, NULL);
	return false;
}

void automaton_indexes_valuation_increase(automaton_indexes_valuation* valuation){
	if(valuation == NULL)return;
	//if(!automaton_indexes_valuation_has_next(valuation))return;
	uint32_t i, j, k;
	int32_t *lower_index, *upper_index, *current_index, position;

	j	= valuation->count -1;
	valuation->current_values[j]++;
	while(valuation->current_values[j] > valuation->ranges[j]->upper_value){
		valuation->current_values[j] 	= valuation->ranges[j]->lower_value;
		if(j == 0)break;
		j--;
		valuation->current_values[j]++;
	}
	valuation->current_combination++;
}
bool automaton_indexes_valuation_has_next(automaton_indexes_valuation* valuation){
	return valuation->current_combination <= (valuation->total_combinations);
}
void automaton_indexes_valuation_set_to_label(automaton_parsing_tables* tables, automaton_indexes_valuation* valuation, automaton_indexes_syntax* from_indexes, automaton_indexes_syntax* to_indexes, char* label, char* target){
	if(valuation == NULL || from_indexes == NULL || to_indexes == NULL){
		strcpy(target, label);
		return;
	}
	uint32_t i, j;

	sprintf(target, "%s", label);
	char name[40];
	char *from_ident, *to_ident;
	for(i = 0; i < from_indexes->count; i++){
		for(j = 0; j < to_indexes->count; j++){
			if(from_indexes->indexes[i]->is_expr && (from_indexes->indexes[i]->expr->type == UPPER_IDENT_TERMINAL_TYPE_AUT ||
					from_indexes->indexes[i]->expr->type == IDENT_TERMINAL_TYPE_AUT)){
				from_ident	= from_indexes->indexes[i]->expr->string_terminal;
			}else if(from_indexes->indexes[i]->is_expr){
				sprintf(target, "%s_%d", target, automaton_expression_syntax_evaluate(tables, from_indexes->indexes[i]->expr, valuation));
			}else{from_ident	= from_indexes->indexes[i]->lower_ident;}
			if(to_indexes->indexes[j]->is_expr && (to_indexes->indexes[j]->expr->type == UPPER_IDENT_TERMINAL_TYPE_AUT ||
					to_indexes->indexes[j]->expr->type == IDENT_TERMINAL_TYPE_AUT)){
				to_ident	= to_indexes->indexes[j]->expr->string_terminal;
			}else if(to_indexes->indexes[j]->is_expr){
				sprintf(target, "%s_%d", target, automaton_expression_syntax_evaluate(tables, to_indexes->indexes[j]->expr, valuation));
			}else{to_ident	= to_indexes->indexes[j]->lower_ident;}
			if(strcmp(from_ident, to_ident) == 0){
				sprintf(target, "%s_%d", target, valuation->current_values[i]);
			}
		}
	}
}

void automaton_indexes_valuation_set_label(automaton_indexes_valuation* valuation, char* label, char* target){
	if(valuation == NULL){
		strcpy(target, label);
		return;
	}
	sprintf(target, "%s", label);
	uint32_t i;
	for(i = 0; i < valuation->count; i++){
		sprintf(target, "%s_%d", target, valuation->current_values[i]);
	}
}
#define INDEX_BUF_SIZE 800
void automaton_indexes_syntax_eval_strings(automaton_parsing_tables* tables, automaton_indexes_valuation* valuation, char*** a, int32_t* a_count, automaton_indexes_syntax* indexes){
	uint32_t i, j, k;
	int32_t *lower_index, *upper_index, *current_index, position;
	uint32_t total_combinations = 1;

	char buffer[INDEX_BUF_SIZE], buffer2[INDEX_BUF_SIZE];

	char** ret_value			= NULL;
	int32_t inner_count			= 0;

	int32_t effective_count	= indexes->count;
	/*
	int32_t effective_count	= 0;

	for(i = 0; i < indexes->count; i++){
		if(!(indexes->indexes[i]->is_expr)){
			effective_count++;
		}
	}

	if(effective_count == 0)return;*/

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
				total_combinations *= (uint32_t)(upper_index[j] - lower_index[j] + 1);
				current_index[j]	= lower_index[j];
			}
			j++;
		}else{
			if(valuation != NULL){
				for(k = 0; k < valuation->count; k++){
					if(strcmp(valuation->ranges[k]->name, indexes->indexes[i]->expr->string_terminal) == 0){
						current_index[j]	= lower_index[j] = upper_index[j]	= valuation->current_values[k];
						j++;
					}
				}

			}
		}
	}

	for(i = 0; i < total_combinations; i++){
		buffer[0] = '\0';
		for(j = 0; j < (uint32_t)effective_count; j++){
			if(strlen(buffer) > (INDEX_BUF_SIZE - 10) ){
				printf("[automaton_indexes_syntax_eval_strings] buffer overrun\n");
				exit(-1);
			}
			sprintf(buffer, "%s_%d", buffer, current_index[j]);
		}

		for(k = 0; k < (uint32_t)*a_count; k++){
			if(strlen(buffer2) > (INDEX_BUF_SIZE - 10) ){
				printf("[automaton_indexes_syntax_eval_strings] buffer overrun\n");
				exit(-1);
			}
			sprintf(buffer2, "%s%s", (*a)[k], buffer);

			aut_push_string_to_list(&ret_value, &inner_count, buffer2, &position, true, false);
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
	}else{
		//TODO: only considering integer expressions
		*lower_index	= index->expr->integer_terminal;
		*upper_index	= index->expr->integer_terminal;
	}
}


automaton_range* automaton_range_syntax_evaluate(automaton_parsing_tables *tables, char* name, automaton_expression_syntax *range_def_syntax){
	int32_t lower_value	= automaton_expression_syntax_evaluate(tables, range_def_syntax->first, NULL);
	int32_t upper_value	= automaton_expression_syntax_evaluate(tables, range_def_syntax->second, NULL);
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

bool automaton_statement_syntax_to_fluent(automaton_automata_context* ctx, automaton_fluent_syntax* fluent_def_syntax
		, automaton_parsing_tables* tables, automaton_alphabet* global_alphabet){
	uint32_t main_index	= automaton_parsing_tables_get_entry_index(tables, FLUENT_ENTRY_AUT, fluent_def_syntax->name);
	tables->fluent_entries[main_index]->solved					= true;
	tables->fluent_entries[main_index]->valuation_count			= 1;
	tables->fluent_entries[main_index]->valuation.fluent_value	= automaton_fluent_create_from_syntax(tables, fluent_def_syntax, global_alphabet);
	return false;
}

automaton_fluent* automaton_fluent_create_from_syntax(automaton_parsing_tables* tables, automaton_fluent_syntax* fluent_def_syntax
		, automaton_alphabet* global_alphabet){
	//TODO:implement fluents initial value
	automaton_fluent* fluent	= automaton_fluent_create(fluent_def_syntax->name, false);
	uint32_t i;
	if(fluent_def_syntax->initiating_set->is_ident || fluent_def_syntax->finishing_set->is_ident){
		//TODO: implement set by ref in fluents
		printf("[FATAL ERROR] fluent set by ref not implemented\n");
		exit(-1);
	}
	for(i =0; i < fluent_def_syntax->initiating_set->count; i++){
		if(fluent_def_syntax->initiating_set->labels_count[i] != 1){
			//TODO: implement concurrent fluents
			printf("[FATAL ERROR] concurrent labels in fluents not implement\n");
			exit(-1);
		}
	}
	for(i =0; i < fluent_def_syntax->finishing_set->count; i++){
		if(fluent_def_syntax->finishing_set->labels_count[i] != 1){
			//TODO: implement concurrent fluents
			printf("[FATAL ERROR] concurrent labels in fluents not implement\n");
			exit(-1);
		}
	}
	automaton_signal_event* sig_event;
	for(i = 0; i < fluent_def_syntax->initiating_set->count; i++){
		if(fluent_def_syntax->initiating_set->labels[i][0]->is_set){
			printf("[FATAL ERROR] fluent set by ref not implemented\n");
			exit(-1);
		}
		sig_event = automaton_signal_event_create(fluent_def_syntax->initiating_set->labels[i][0]->string_terminal, INPUT_SIG);
		automaton_fluent_add_starting_signal(fluent, global_alphabet, sig_event);
		automaton_signal_event_destroy(sig_event, true);
	}
	for(i = 0; i < fluent_def_syntax->finishing_set->count; i++){
		if(fluent_def_syntax->finishing_set->labels[i][0]->is_set){
			printf("[FATAL ERROR] fluent set by ref not implemented\n");
			exit(-1);
		}
		sig_event = automaton_signal_event_create(fluent_def_syntax->finishing_set->labels[i][0]->string_terminal, INPUT_SIG);
		automaton_fluent_add_ending_signal(fluent, global_alphabet, sig_event);
		automaton_signal_event_destroy(sig_event, true);
	}
	fluent->initial_valuation	= fluent_def_syntax->initial_value != 0;
	return fluent;
}
/*
 * if it is initial and input it will receive a valuation over X and it will store it on X
 * if it is initial and output it will receive a valuation over XY and it will store it on XY
 * if it is rho and input it will receive a valuation over XYX' and it will store it on the X' of a XYX' state
 * if it is rho and output it will receive a valuation over XYX'Y' and it will store it on XY
 */
void automaton_set_composed_valuation(bool* valuation, bool* partial_valuation, uint32_t valuation_offset,bool is_initial, bool is_input, uint32_t x_count, uint32_t y_count){
	uint32_t i;
	if(is_initial){
		if(is_input) for(i = 0; i < x_count; i++)valuation[i]	= partial_valuation[x_count * valuation_offset + i];
		else for(i = 0; i < (x_count + y_count); i++)valuation[i]	= partial_valuation[(x_count + y_count)* valuation_offset + i];
	}else{
		if(is_input) for(i = x_count + y_count; i < x_count * 2 + y_count; i++)valuation[i]	= partial_valuation[(x_count * 2+ y_count) * valuation_offset + i];
		else for(i = 0; i < (x_count + y_count); i++)valuation[i]	= partial_valuation[(x_count + y_count * 2)* valuation_offset + x_count + y_count + i];
	}
}
bool automaton_add_transition_from_valuations(obdd_mgr* mgr, automaton_automaton* automaton, bool from_input, uint32_t from_state, uint32_t to_state
		, bool* from_valuation, bool* to_valuation, uint32_t* x_alphabet, uint32_t* y_alphabet, uint32_t x_count
		, uint32_t y_count){
	uint32_t i, fluent_index, fluent_count	= automaton->context->liveness_valuations_count;
	automaton_transition* transition		= automaton_transition_create(from_state, to_state);
	automaton_signal_event* signal_event	= automaton_signal_event_create("", INPUT_SIG);
	char signal_name[255];
	if(!from_input){//X'Y' -> XYX'
		for(i = 0; i < x_count; i++){
			if(from_valuation[i] != to_valuation[x_count + y_count + i]){
				strcpy(signal_name, mgr->vars_dict->entries[x_alphabet[i]].key);
				strcat(signal_name, from_valuation[i]? SIGNAL_OFF_SUFFIX: SIGNAL_ON_SUFFIX);
				aut_dupstr(&(signal_event->name), signal_name);
				automaton_transition_add_signal_event(transition, automaton->context, signal_event);
			}
		}
	}else{//XYX' -> X'Y'
		signal_event->type	= OUTPUT_SIG;
		for(i = x_count; i < x_count + y_count; i++){
			if(from_valuation[i] != to_valuation[i]){
				strcpy(signal_name, mgr->vars_dict->entries[y_alphabet[i]].key);
				strcat(signal_name, from_valuation[i]? SIGNAL_OFF_SUFFIX: SIGNAL_ON_SUFFIX);
				aut_dupstr(&(signal_event->name), signal_name);
				automaton_transition_add_signal_event(transition, automaton->context, signal_event);
			}
		}
	}
	automaton_signal_event_destroy(signal_event, true);
	bool has_transition = automaton_automaton_has_transition(automaton, transition);
	if(!has_transition)
		automaton_automaton_add_transition(automaton, transition);
	automaton_transition_destroy(transition, true);
	//should add after adding transition since structure resizing may not have been triggered
	if(!has_transition)
		for(i = 0; i < fluent_count; i++){
			fluent_index	= GET_STATE_FLUENT_INDEX(fluent_count, to_state, i);
			//TODO:
			//evaluate each liveness formula on landing state to check if formula is satisfied and then set bit
			/*
			if(to_valuation[i])SET_FLUENT_BIT(automaton->valuations, fluent_index);
			else CLEAR_FLUENT_BIT(automaton->valuations, fluent_index);
			*/
		}
	return has_transition;
}
automaton_automaton* automaton_build_automaton_from_obdd(automaton_automata_context* ctx, char* name, obdd** env_theta_obdd, uint32_t env_theta_count, obdd** sys_theta_obdd, uint32_t sys_theta_count
		, obdd** env_rho_obdd, uint32_t env_rho_count, obdd** sys_rho_obdd, uint32_t sys_rho_count, automaton_parsing_tables* tables){
	//remember that if automaton was built from ltl its valuations should be added when building it
	//and should be kept when composing it, if several automata are to be composed from ltl their composed valuation
	//equals to the conjunction of the components' valuations
	obdd_mgr* mgr						= parser_get_obdd_mgr();
	obdd_state_tree* state_map			= obdd_state_tree_create(mgr->vars_dict->size);
	uint32_t local_alphabet_count		= (mgr->vars_dict->size - 2) * 2;
	uint32_t i, j, current_element 		= 0;
	uint32_t* local_alphabet			= malloc(sizeof(uint32_t) * local_alphabet_count);
	char current_dict_entry[255];
	parser_get_primed_variables();

	uint32_t x_count = 0, y_count = 0, x_p_count = 0, y_p_count = 0, x_y_count = 0, x_y_x_p_count = 0, x_p_y_p_count = 0, signals_count = 0;
	uint32_t alphabet_element_index;
	int32_t global_index;
	bool is_primed;
	char current_key[255];
	bool is_input;
	/**
	 * BUILD LOCAL ALPHABET
	 */
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

		strcpy(current_key, mgr->vars_dict->entries[i].key);
		strcat(current_key, SIGNAL_ON_SUFFIX);
		global_index	= automaton_alphabet_get_value_index(ctx->global_alphabet, current_key);
		is_input		= ctx->global_alphabet->list[global_index].type == INPUT_SIG;
		if(is_input){
			if(is_primed)x_p_count++; else x_count++;
		}else{
			if(is_primed)y_p_count++; else y_count++;
		}
		signals_count++;
		strcpy(current_dict_entry, mgr->vars_dict->entries[i].key);
		strcat(current_dict_entry, SIGNAL_ON_SUFFIX);
		local_alphabet[current_element++]	= automaton_alphabet_get_value_index(ctx->global_alphabet, current_dict_entry);
		strcpy(current_dict_entry, mgr->vars_dict->entries[i].key);
		strcat(current_dict_entry, SIGNAL_OFF_SUFFIX);
		local_alphabet[current_element++]	= automaton_alphabet_get_value_index(ctx->global_alphabet, current_dict_entry);
	}
	/**
	 * get x, y, x', y' alphabets
	 */
	uint32_t* x_alphabet	= malloc(sizeof(uint32_t) * x_count);
	uint32_t* y_alphabet	= malloc(sizeof(uint32_t) * y_count);
	uint32_t* x_y_alphabet	= malloc(sizeof(uint32_t) * (x_count + y_count));
	uint32_t* x_y_x_p_alphabet	= malloc(sizeof(uint32_t) * (x_count * 2 + y_count));
	uint32_t* x_p_alphabet	= malloc(sizeof(uint32_t) * x_p_count);
	uint32_t* y_p_alphabet	= malloc(sizeof(uint32_t) * y_p_count);
	uint32_t* x_p_y_p_alphabet	= malloc(sizeof(uint32_t) * (y_p_count + x_p_count));
	uint32_t* signals_alphabet	= malloc(sizeof(uint32_t) * signals_count);
	x_count = 0, y_count = 0, x_p_count = 0, y_p_count = 0,signals_count = 0;
	for(i = 0; i < mgr->vars_dict->size; i++){
		//avoid searching for the true/false variables
		if((strcmp(mgr->vars_dict->entries[i].key, TRUE_VAR) == 0) || (strcmp(mgr->vars_dict->entries[i].key, FALSE_VAR) == 0))
			continue;
		alphabet_element_index	= mgr->vars_dict->entries[i].value;
		is_primed				= false;
		for(j = 0; j < parser_primed_variables_count; j++)
			if(parser_primed_variables[j] == alphabet_element_index){
				is_primed	= true;
				break;
			}
		strcpy(current_key, mgr->vars_dict->entries[i].key);
		strcat(current_key, SIGNAL_ON_SUFFIX);
		global_index	= automaton_alphabet_get_value_index(ctx->global_alphabet, current_key);
		is_input		= ctx->global_alphabet->list[global_index].type == INPUT_SIG;
		if(is_input){
			if(is_primed){x_p_alphabet[x_p_count++]	= i; x_y_x_p_alphabet[x_y_x_p_count++] = i; }else x_alphabet[x_count++]	= i;
		}else{
			if(is_primed)y_p_alphabet[y_p_count++]	= i; else y_alphabet[y_count++]	= i;
		}
		signals_alphabet[signals_count++]	= i;
		if(is_primed){ x_p_y_p_alphabet[x_p_y_p_count++] = i;}
		else{ x_y_alphabet[x_y_count++] = i; x_y_x_p_alphabet[x_y_x_p_count++] = i;}
	}
	//get automaton
	automaton_automaton* ltl_automaton	= automaton_automaton_create(name, ctx, local_alphabet_count, local_alphabet, true, true);

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
	for(i = 0; i < env_theta_count; i++){
		if(i == 0){ env_theta_composed	= env_theta_obdd[i];
		}else{
			old_obdd	= env_theta_composed;
			env_theta_composed	= obdd_apply_and(env_theta_composed, env_theta_obdd[i]);
			if(i > 1)obdd_destroy(old_obdd);
		}
	}
	for(i = 0; i < sys_theta_count; i++){
		if(i == 0){ sys_theta_composed	= sys_theta_obdd[i];
		}else{
			old_obdd	= sys_theta_composed;
			sys_theta_composed	= obdd_apply_and(sys_theta_composed, sys_theta_obdd[i]);
			if(i > 1)obdd_destroy(old_obdd);
		}
	}
	for(i = 0; i < env_rho_count; i++){
		if(i == 0){ env_rho_composed	= env_rho_obdd[i];
		}else{
			old_obdd	= env_rho_composed;
			env_rho_composed	= obdd_apply_and(env_rho_composed, env_rho_obdd[i]);
			if(i > 1)obdd_destroy(old_obdd);
		}
	}
	for(i = 0; i < sys_rho_count; i++){
		if(i == 0){ sys_rho_composed	= sys_rho_obdd[i];
		}else{
			old_obdd	= sys_rho_composed;
			sys_rho_composed	= obdd_apply_and(sys_rho_composed, sys_rho_obdd[i]);
			if(i > 1)obdd_destroy(old_obdd);
		}
	}


	env_sys_theta_composed				= obdd_apply_and(env_theta_composed, sys_theta_composed);
	env_sys_rho_composed				= obdd_apply_and(env_rho_composed, sys_rho_composed);
	/**
	 * BEHAVIOUR CONSTRUCTION (RHO AND THETA)
	 */
	uint32_t current_valuations_count;
	obdd_composite_state* env_state = obdd_composite_state_create(0, x_count * 2 + y_count);
	obdd_composite_state* sys_state = obdd_composite_state_create(0, x_count + y_count);
	uint32_t fluent_count	= x_count + y_count;
	bool* current_valuations;
	obdd_state_tree* obdd_state_map	= obdd_state_tree_create(x_count * 2 + y_count);
	automaton_concrete_bucket_list* theta_env_bucket_list	= automaton_concrete_bucket_list_create(
			LTL_BUCKET_SIZE, obdd_composite_state_extractor, sizeof(uint32_t) + sizeof(bool) * x_count);
	automaton_concrete_bucket_list* theta_sys_bucket_list	= automaton_concrete_bucket_list_create(
			LTL_BUCKET_SIZE, obdd_composite_state_extractor, sizeof(uint32_t) + sizeof(bool) * (x_count + y_count));
	automaton_concrete_bucket_list* rho_env_bucket_list	= automaton_concrete_bucket_list_create(
			LTL_BUCKET_SIZE, obdd_composite_state_extractor, sizeof(uint32_t) + sizeof(bool) * x_count);
	automaton_concrete_bucket_list* rho_sys_bucket_list	= automaton_concrete_bucket_list_create(
			LTL_BUCKET_SIZE, obdd_composite_state_extractor, sizeof(uint32_t) + sizeof(bool) * (x_count + y_count));
	automaton_concrete_bucket_list* rho_env_processed_bucket_list	= automaton_concrete_bucket_list_create(
			LTL_BUCKET_SIZE, obdd_composite_state_extractor, sizeof(uint32_t) + sizeof(bool) * x_count);
	automaton_concrete_bucket_list* rho_sys_processed_bucket_list	= automaton_concrete_bucket_list_create(
			LTL_BUCKET_SIZE, obdd_composite_state_extractor, sizeof(uint32_t) + sizeof(bool) * (x_count + y_count));
	sys_state->state		= obdd_state_tree_get_key(obdd_state_map, sys_state->valuation, x_y_count);
	automaton_automaton_add_initial_state(ltl_automaton, sys_state->state);
	uint32_t fluent_index;
	for(i = 0; i < fluent_count; i++){
		fluent_index	= GET_STATE_FLUENT_INDEX(fluent_count, sys_state->state, i);
		CLEAR_FLUENT_BIT(ltl_automaton->valuations, fluent_index);
	}
	/**
	 * THETA INITIAL CONDITION
	 */
	//add initial env valuations
	current_valuations	= obdd_get_valuations(mgr, env_theta_composed, &current_valuations_count, x_alphabet, x_count);
#if DEBUG_LTL_AUTOMATON
	int32_t state_counter = 0;
	printf("Init env valuations\n");
	obdd_print_valuations(mgr, current_valuations, current_valuations_count, x_alphabet, x_count);
#endif
	bool* tmp_state_valuation = calloc((x_count * 2 + y_count), sizeof(bool));
	bool has_transition;
	for(i = 0; i < current_valuations_count; i++){
		//automaton_set_composed_valuation(env_state->valuation, (bool*)(current_valuations + x_count * i * sizeof(bool)), true, x_count, y_count);
		automaton_set_composed_valuation(env_state->valuation, current_valuations, i, true, true, x_count, y_count);
		env_state->state		= obdd_state_tree_get_key(obdd_state_map, env_state->valuation, x_y_x_p_count);
		has_transition	= automaton_add_transition_from_valuations(mgr, ltl_automaton, false, sys_state->state, env_state->state
				, sys_state->valuation, tmp_state_valuation, x_alphabet, y_alphabet, x_count, y_count);
		if(!has_transition){
#if DEBUG_LTL_AUTOMATON
			printf("(%d-[", sys_state->state);
			for(j = 0; j < x_y_x_p_count; j++)
				printf("%s", env_state->valuation[j] ? "1" : "0");
			state_counter++;
			printf("]->%d)%s", env_state->state, state_counter % 10 == 0 ? "\n" : " ");
			if(state_counter % 1000 == 0){
				printf("States processed for ltl: %d\n", state_counter);
				fflush(stdout);
			}
#endif
			automaton_concrete_bucket_add_entry(theta_env_bucket_list, env_state);
		}
	}
	free(current_valuations);

#if DEBUG_LTL_AUTOMATON
	state_counter = 0;
	current_valuations	= obdd_get_valuations(mgr, env_sys_theta_composed, &current_valuations_count, x_y_alphabet, x_y_count);
	printf("Init sys valuations\n");
	obdd_print_valuations(mgr, current_valuations, current_valuations_count, y_alphabet, y_count);
	free(current_valuations);
#endif
	/**
	 * We restrict robdd(theta_env && theta_sys) with the valuation for each pending state s_i in bucket_list
	 * and then ask for S_j = obdd_get_valuations, we build and add the transition between s_i and each s_j in S_j
	 * adding s_j to rho_bucket_list
	 */
	//obdd_node* obdd_node_restrict_vector(obdd_mgr* mgr, obdd_node* root, uint32_t* var_ids, bool* values, uint32_t count);
	uint32_t state_ptr = 0;
	obdd* obdd_current_state	= NULL;

	if(theta_env_bucket_list->composite_count > 0){
		do{
			automaton_concrete_bucket_pop_entry(theta_env_bucket_list, env_state);
			obdd_current_state	= obdd_restrict_vector(env_sys_theta_composed, x_y_x_p_alphabet, env_state->valuation, x_y_x_p_count);
			current_valuations	= obdd_get_valuations(mgr, obdd_current_state, &current_valuations_count, x_y_alphabet, x_y_count);
#if DEBUG_LTL_AUTOMATON
			obdd_print_valuations(mgr, current_valuations, current_valuations_count, x_y_alphabet, x_y_count);
#endif
			for(i = 0; i < current_valuations_count; i++){
				//automaton_set_composed_valuation(sys_state->valuation, (bool*)(current_valuations + x_y_count * i * sizeof(bool)), false, x_count, y_count);
				automaton_set_composed_valuation(sys_state->valuation, current_valuations, i, true, false, x_count, y_count);
				sys_state->state		= obdd_state_tree_get_key(obdd_state_map, sys_state->valuation, x_y_count);
				has_transition	= automaton_add_transition_from_valuations(mgr, ltl_automaton, true, env_state->state, sys_state->state
						, env_state->valuation, sys_state->valuation, x_alphabet, y_alphabet, x_count, y_count);
				if(!has_transition && !automaton_concrete_bucket_has_entry(theta_sys_bucket_list, sys_state)){
#if DEBUG_LTL_AUTOMATON
					printf("(%d-[", env_state->state);
					for(j = 0; j < x_count + y_count; j++)
						printf("%s", sys_state->valuation[j] ? "1" : "0");
					state_counter++;
					printf("]->%d)%s", sys_state->state, state_counter % 10 == 0 ? "\n" : " ");
					if(state_counter % 1000 == 0){
						printf("States processed for ltl: %d\n", state_counter);
						fflush(stdout);
					}
#endif
					automaton_concrete_bucket_add_entry(rho_sys_bucket_list, sys_state);
				}
			}
			obdd_destroy(obdd_current_state);
			free(current_valuations);
		}while(theta_env_bucket_list->composite_count > 0);
	}
#if DEBUG_LTL_AUTOMATON
	printf("Rho relation building\n");
#endif

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
	uint32_t rho_counter = 0, skipped = 0, evaluated = 0;
	//TODO: properly implement this
	do{
		if(rho_sys_bucket_list->composite_count > 0){
			do{
				rho_counter++;
				if(rho_counter == 500){
					printf("evaluated|processed|skipped:\t%d\t%d\t%d\t||\t", evaluated, rho_sys_processed_bucket_list->composite_count + rho_env_processed_bucket_list->composite_count, skipped);
					printf("pending S (S|E|P_S|P_E):\t%d\t%d\t%d\t%d\n", rho_sys_bucket_list->composite_count, rho_env_bucket_list->composite_count, rho_sys_processed_bucket_list->composite_count, rho_env_processed_bucket_list->composite_count);
					rho_counter	= 0;
				}
				automaton_concrete_bucket_pop_entry(rho_sys_bucket_list, sys_state);
				automaton_concrete_bucket_add_entry(rho_sys_processed_bucket_list, sys_state);
				for(i = 0; i < x_y_count; i++)env_state->valuation[i]	= sys_state->valuation[i];
				obdd_current_state	= obdd_restrict_vector(env_rho_composed, x_y_alphabet, sys_state->valuation, x_y_count);
				current_valuations	= obdd_get_valuations(mgr, obdd_current_state, &current_valuations_count, x_y_x_p_alphabet, x_y_x_p_count);
#if DEBUG_LTL_AUTOMATON
				printf("XY->X'\n");
				obdd_print_valuations(mgr, current_valuations, current_valuations_count, x_p_alphabet, x_p_count);
#endif
				for(i = 0; i < current_valuations_count; i++){
					automaton_set_composed_valuation(env_state->valuation, current_valuations, i, false, true, x_count, y_count);
					env_state->state		= obdd_state_tree_get_key(obdd_state_map, env_state->valuation, x_y_x_p_count);
					has_transition	= automaton_add_transition_from_valuations(mgr, ltl_automaton, false, sys_state->state, env_state->state
							, sys_state->valuation, env_state->valuation, x_alphabet, y_alphabet, x_count, y_count);
#if DEBUG_LTL_AUTOMATON
						printf("(%d-[", sys_state->state);
						for(j = 0; j < x_y_x_p_count; j++)
							printf("%s", env_state->valuation[j] ? "1" : "0");
						state_counter++;
						printf("]->%d)%s%s", env_state->state, has_transition? "" : "*", state_counter % 10 == 0 ? "\n" : " ");
						printf("\n");
						if(state_counter % 1000 == 0){
							printf("States processed for ltl: %d\n", state_counter);
							fflush(stdout);
						}
#endif
					if(!has_transition && !automaton_concrete_bucket_has_entry(rho_env_bucket_list, env_state) && !automaton_concrete_bucket_has_entry(rho_env_processed_bucket_list, env_state)){
						automaton_concrete_bucket_add_entry(rho_env_bucket_list, env_state);
					}else skipped++;
					evaluated++;
				}
				obdd_destroy(obdd_current_state);
				free(current_valuations);
			}while(rho_sys_bucket_list->composite_count > 0);
		}
		if(rho_env_bucket_list->composite_count > 0){
			do{
				rho_counter++;
				if(rho_counter == 500){
					printf("evaluated|processed|skipped:\t%d\t%d\t%d\t||\t", evaluated, rho_sys_processed_bucket_list->composite_count + rho_env_processed_bucket_list->composite_count, skipped);
					printf("pending E (S|E|P_S|P_E):\t%d\t%d\t%d\t%d\n", rho_sys_bucket_list->composite_count, rho_env_bucket_list->composite_count, rho_sys_processed_bucket_list->composite_count, rho_env_processed_bucket_list->composite_count);
					rho_counter	= 0;
				}
				automaton_concrete_bucket_pop_entry(rho_env_bucket_list, env_state);
				automaton_concrete_bucket_add_entry(rho_env_processed_bucket_list, env_state);
				obdd_current_state	= obdd_restrict_vector(env_sys_rho_composed, x_y_x_p_alphabet, env_state->valuation, x_y_x_p_count);
				current_valuations	= obdd_get_valuations(mgr, obdd_current_state, &current_valuations_count, signals_alphabet, signals_count);
#if DEBUG_LTL_AUTOMATON
				printf("X'->X'Y'\n");
				obdd_print_valuations(mgr, current_valuations, current_valuations_count, x_p_alphabet, x_p_count);
#endif
				for(i = 0; i < current_valuations_count; i++){
					automaton_set_composed_valuation(sys_state->valuation, current_valuations, i, false, false, x_count, y_count);
					sys_state->state		= obdd_state_tree_get_key(obdd_state_map, sys_state->valuation, x_y_count);
					has_transition	= automaton_add_transition_from_valuations(mgr, ltl_automaton, true, env_state->state, sys_state->state
							, env_state->valuation, sys_state->valuation, x_alphabet, y_alphabet, x_count, y_count);
#if DEBUG_LTL_AUTOMATON
						printf("(%d-[", env_state->state);
						for(j = 0; j < x_y_count; j++)
							printf("%s", sys_state->valuation[j] ? "1" : "0");
						state_counter++;
						printf("]->%d)%s%s", sys_state->state, has_transition? "" : "*", state_counter % 10 == 0 ? "\n" : " ");
						printf("\n");
						if(state_counter % 1000 == 0){
							printf("States processed for ltl: %d\n", state_counter);
							fflush(stdout);
						}
#endif
					if(!has_transition && !automaton_concrete_bucket_has_entry(rho_sys_bucket_list, sys_state) && !automaton_concrete_bucket_has_entry(rho_sys_processed_bucket_list, sys_state)){
						automaton_concrete_bucket_add_entry(rho_sys_bucket_list, sys_state);
					}else skipped++;
					evaluated++;
				}
				obdd_destroy(obdd_current_state);
				free(current_valuations);
			}while(rho_env_bucket_list->composite_count > 0);
		}
	}while(rho_sys_bucket_list->composite_count > 0);


	int32_t main_index					= automaton_parsing_tables_add_entry(tables, COMPOSITION_ENTRY_AUT, name, ltl_automaton);
	tables->composition_entries[main_index]->solved	= true;
	tables->composition_entries[main_index]->valuation_count			= 1;
	tables->composition_entries[main_index]->valuation.automaton_value	= ltl_automaton;

	if(env_theta_count > 1)obdd_destroy(env_theta_composed);
	if(sys_theta_count > 1)obdd_destroy(sys_theta_composed);
	if(env_rho_count > 1)obdd_destroy(env_rho_composed);
	if(sys_rho_count > 1)obdd_destroy(sys_rho_composed);
	obdd_destroy(env_sys_theta_composed); obdd_destroy(env_sys_rho_composed);
	free(env_state); free(sys_state); free(tmp_state_valuation);
	automaton_concrete_bucket_destroy(theta_env_bucket_list);automaton_concrete_bucket_destroy(theta_sys_bucket_list);
	automaton_concrete_bucket_destroy(rho_env_bucket_list);automaton_concrete_bucket_destroy(rho_sys_bucket_list);
	automaton_concrete_bucket_destroy(rho_env_processed_bucket_list);automaton_concrete_bucket_destroy(rho_sys_processed_bucket_list);
	free(x_alphabet); free(y_alphabet); free(x_p_alphabet); free(y_p_alphabet);free(x_y_alphabet); free(x_y_x_p_alphabet); free(x_p_y_p_alphabet); free(signals_alphabet);
	free(local_alphabet);
	obdd_state_tree_destroy(state_map);
	obdd_state_tree_destroy(obdd_state_map);
	//automaton_automaton_print(ltl_automaton, true, true, true, "", "");
	return ltl_automaton;
}

automaton_automata_context* automaton_automata_context_create_from_syntax(automaton_program_syntax* program, char* ctx_name, bool is_synchronous, bool print_fsp){
	automaton_parsing_tables* tables	= automaton_parsing_tables_create();
	automaton_automata_context* ctx		= malloc(sizeof(automaton_automata_context));

	//build look up tables
	uint32_t i;
	int32_t j;
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
	uint32_t fluent_count	= 0;
	for(i = 0; i < program->count; i++)
		if(program->statements[i]->type == FLUENT_AUT)
			fluent_count++;
	automaton_fluent** fluents				= malloc(sizeof(automaton_fluent*) * fluent_count);
	fluent_count = 0;
	uint32_t fluent_index;
	for(i = 0; i < program->count; i++){
		if(program->statements[i]->type == FLUENT_AUT){
			automaton_statement_syntax_to_fluent(ctx, program->statements[i]->fluent_def, tables, global_alphabet);
			fluent_index = automaton_parsing_tables_get_entry_index(tables, FLUENT_ENTRY_AUT, program->statements[i]->fluent_def->name);
			fluents[fluent_count++]			= tables->fluent_entries[fluent_index]->valuation.fluent_value;
		}
	}

	//get ltl rules
	uint32_t ltl_automata_count = 0;
	char** ltl_automata_names	= NULL;
	bool ltl_automata_found		= false;
	for(i = 0; i < program->count; i++)
		if(program->statements[i]->type == LTL_RULE_AUT){
			ltl_automata_found	= false;
			for(j = 0; j < (int32_t)ltl_automata_count; j++)
				if(strcmp(ltl_automata_names[j], program->statements[i]->ltl_rule_def->game_structure_name) == 0){
					ltl_automata_found	= true;
					break;
				}
			if(!ltl_automata_found){
				if(ltl_automata_names == NULL){
					ltl_automata_names	= malloc(sizeof(char*) * (ltl_automata_count + 1));
				}else{
					char** ptr	= realloc(ltl_automata_names, sizeof(char*) * (ltl_automata_count + 1));
					if(ptr == NULL){
						printf("Could not allocate memory\n");
						exit(-1);
					}else{
						ltl_automata_names	= ptr;
					}
				}
				ltl_automata_names[ltl_automata_count++]	= program->statements[i]->ltl_rule_def->game_structure_name;
			}
		}
	uint32_t *sys_theta_count	= malloc(sizeof(uint32_t) * ltl_automata_count);
	uint32_t *env_theta_count	= malloc(sizeof(uint32_t) * ltl_automata_count);
	uint32_t *sys_rho_count		= malloc(sizeof(uint32_t) * ltl_automata_count);
	uint32_t *env_rho_count		= malloc(sizeof(uint32_t) * ltl_automata_count);
	uint32_t liveness_formulas_count	= 0;
	char** liveness_formulas_names		= NULL;
	obdd** liveness_formulas			= NULL;
	for(i = 0; i < ltl_automata_count; i++){
		sys_theta_count[i]	= 0;	env_theta_count[i]	= 0;
		sys_rho_count[i]	= 0;	env_rho_count[i]	= 0;
	}
	bool is_env, is_theta;
	uint32_t ltl_automaton_index;
	for(i = 0; i < program->count; i++){
		if(program->statements[i]->type == LTL_FLUENT_AUT)
			liveness_formulas_count++;
		if(program->statements[i]->type == LTL_RULE_AUT){
			for(j = 0; j < (int32_t)ltl_automata_count; j++)
				if(strcmp(ltl_automata_names[j], program->statements[i]->ltl_rule_def->game_structure_name) == 0){
					ltl_automaton_index	= j;
					break;
				}
			is_env	= program->statements[i]->ltl_rule_def->is_env;
			is_theta= program->statements[i]->ltl_rule_def->is_theta;
			if(is_env){
				if(is_theta)	env_theta_count[j]++;
				else			env_rho_count[j]++;
			}else{
				if(is_theta)	sys_theta_count[j]++;
				else			sys_rho_count[j]++;
			}
		}
	}
	obdd ***sys_theta_obdd	= malloc(sizeof(obdd**) * ltl_automata_count);
	obdd ***env_theta_obdd	= malloc(sizeof(obdd**) * ltl_automata_count);
	obdd ***sys_rho_obdd	= malloc(sizeof(obdd**) * ltl_automata_count);
	obdd ***env_rho_obdd	= malloc(sizeof(obdd**) * ltl_automata_count);
	for(i = 0; i < ltl_automata_count; i++){
		sys_theta_obdd[i]	= malloc(sizeof(obdd*) * sys_theta_count[i]);
		env_theta_obdd[i]	= malloc(sizeof(obdd*) * env_theta_count[i]);
		sys_rho_obdd[i]		= malloc(sizeof(obdd*) * sys_rho_count[i]);
		env_rho_obdd[i]		= malloc(sizeof(obdd*) * env_rho_count[i]);
	}
	for(i = 0; i < ltl_automata_count; i++){
		sys_theta_count[i]	= 0;
		env_theta_count[i]	= 0;
		sys_rho_count[i]	= 0;
		env_rho_count[i]	= 0;
	}
	liveness_formulas		= malloc(sizeof(obdd*) * liveness_formulas_count);
	liveness_formulas_names	= malloc(sizeof(char*) * liveness_formulas_count);
	liveness_formulas_count	= 0;
	for(i = 0; i < program->count; i++){
		if(program->statements[i]->type == LTL_FLUENT_AUT){
			liveness_formulas[liveness_formulas_count]			= program->statements[i]->ltl_fluent_def->obdd;
			liveness_formulas_names[liveness_formulas_count++]	= program->statements[i]->ltl_fluent_def->name;
		}

		if(program->statements[i]->type == LTL_RULE_AUT){
			for(j = 0; j < (int32_t)ltl_automata_count; j++)
				if(strcmp(ltl_automata_names[j], program->statements[i]->ltl_rule_def->game_structure_name) == 0){
					ltl_automaton_index	= j;
					break;
				}
			is_env	= program->statements[i]->ltl_rule_def->is_env;
			is_theta= program->statements[i]->ltl_rule_def->is_theta;
			if(is_env){
				if(is_theta)	env_theta_obdd[ltl_automaton_index][env_theta_count[ltl_automaton_index]++]	= program->statements[i]->ltl_rule_def->obdd;
				else			env_rho_obdd[ltl_automaton_index][env_rho_count[ltl_automaton_index]++]		= program->statements[i]->ltl_rule_def->obdd;
			}else{
				if(is_theta)	sys_theta_obdd[ltl_automaton_index][sys_theta_count[ltl_automaton_index]++]	= program->statements[i]->ltl_rule_def->obdd;
				else			sys_rho_obdd[ltl_automaton_index][sys_rho_count[ltl_automaton_index]++]		= program->statements[i]->ltl_rule_def->obdd;
			}
		}
	}
	//get fluents
	automaton_automata_context_initialize(ctx, ctx_name, global_alphabet, fluent_count, fluents, liveness_formulas_count, liveness_formulas, liveness_formulas_names);
	free(fluents);
	automaton_alphabet_destroy(global_alphabet);
	//build automata from ltl
	for(i = 0; i < ltl_automata_count; i++){
		automaton_build_automaton_from_obdd(ctx, ltl_automata_names[i], env_theta_obdd[i], env_theta_count[i], sys_theta_obdd[i], sys_theta_count[i],
				env_rho_obdd[i], env_rho_count[i], sys_rho_obdd[i], sys_rho_count[i], tables);
	}
	obdd_mgr* mgr	= parser_get_obdd_mgr();
	obdd_mgr_destroy(mgr);
	free(parser_primed_variables);


	for(i = 0; i < ltl_automata_count; i++){
		free(sys_theta_obdd[i]);		free(env_theta_obdd[i]);
		free(sys_rho_obdd[i]);			free(env_rho_obdd[i]);
	}
	free(sys_theta_obdd);		free(env_theta_obdd);
	free(sys_rho_obdd);			free(env_rho_obdd);
	free(sys_theta_count);		free(env_theta_count);
	free(sys_rho_count);		free(env_rho_count);
	free(ltl_automata_names);

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
	//compute gr1 games
	int32_t main_index;
	automaton_gr1_game_syntax* gr1_game;
	automaton_automaton *game_automaton, *winning_region_automaton;
	char **assumptions, **guarantees;
	char set_name[255];
	int32_t assumptions_count = 0, guarantees_count = 0;

	for(i = 0; i < program->count; i++){
		if(program->statements[i]->type == GR_1_AUT){
			gr1_game		= program->statements[i]->gr1_game_def;
			main_index		= automaton_parsing_tables_get_entry_index(tables, COMPOSITION_ENTRY_AUT, gr1_game->composition_name);
			game_automaton	= tables->composition_entries[main_index]->valuation.automaton_value;
			//here we will merge fluent and liveness ltl valuations and restore them after synthesis
			bool was_merged	= false;
			uint32_t other_fluent_index;
			uint32_t k;
			automaton_bucket_list** old_inverted_valuations	= NULL;
			uint32_t* old_valuations						= NULL;
			uint32_t old_valuations_size					= 0;
			//merge context
			uint32_t old_fluents_count						= 0;
			automaton_fluent* old_fluents					= NULL;
			automaton_fluent current_fluent;
			current_fluent.ending_signals_count	= 0; current_fluent.ending_signals	= NULL;
			current_fluent.starting_signals_count	= 0; current_fluent.starting_signals	= NULL;
			if(game_automaton->liveness_valuations_size > 0){
				was_merged	= true;
				old_inverted_valuations	= game_automaton->inverted_valuations;
				old_valuations			= game_automaton->valuations;
				old_valuations_size		= game_automaton->valuations_size;
				old_fluents_count		= ctx->global_fluents_count;
				old_fluents				= ctx->global_fluents;

				ctx->global_fluents_count	+= game_automaton->liveness_valuations_size;
				ctx->global_fluents		= malloc(sizeof(automaton_fluent) * ctx->global_fluents_count);
				for(j = 0; j < old_fluents_count; j++)
					automaton_fluent_copy(&(old_fluents[j]), &(ctx->global_fluents[j]));
				for(j = old_fluents_count; j < ctx->global_fluents_count; j++){
					current_fluent.name	= liveness_formulas_names[j - old_fluents_count];
					automaton_fluent_copy(&current_fluent, &(ctx->global_fluents[j]));
				}
				uint32_t new_size					= GET_FLUENTS_ARR_SIZE(ctx->global_fluents_count, game_automaton->transitions_size);
				game_automaton->valuations 				= malloc(sizeof(uint32_t) * new_size);
				game_automaton->inverted_valuations		= malloc(sizeof(automaton_bucket_list*) * ctx->global_fluents_count);
				for(j = 0; j < old_fluents_count; j++){
					game_automaton->inverted_valuations[j]	= old_inverted_valuations[j];
				}
				for(j = old_fluents_count; j < ctx->global_fluents_count; j++){
					game_automaton->inverted_valuations[j]	= game_automaton->liveness_inverted_valuations[j - old_fluents_count];
				}
				for(j = 0; j < game_automaton->transitions_count; j++){
					for(k = 0; j < old_fluents_count; k++){
						fluent_index		= GET_STATE_FLUENT_INDEX(old_fluents_count, j, k);
						other_fluent_index	= GET_STATE_FLUENT_INDEX(ctx->global_fluents_count, j, k);
						if(TEST_FLUENT_BIT(old_valuations, fluent_index)){
							SET_FLUENT_BIT(game_automaton->valuations, other_fluent_index);
						}else{
							CLEAR_FLUENT_BIT(game_automaton->valuations, other_fluent_index);
						}
					}
					for(k = 0; k < liveness_formulas_count; k++){
						fluent_index		= GET_STATE_FLUENT_INDEX(liveness_formulas_count, j, k);
						other_fluent_index	= GET_STATE_FLUENT_INDEX(ctx->global_fluents_count, j, old_fluents_count + k);
						if(TEST_FLUENT_BIT(game_automaton->liveness_valuations, fluent_index)){
							SET_FLUENT_BIT(game_automaton->valuations, other_fluent_index);
						}else{
							CLEAR_FLUENT_BIT(game_automaton->valuations, other_fluent_index);
						}
					}
				}
			}
			sprintf(set_name, "Assumption %s", gr1_game->name);
			assumptions		= automaton_set_syntax_evaluate(tables, gr1_game->assumptions, &assumptions_count, set_name);
			sprintf(set_name, "Guarantees %s", gr1_game->name);
			guarantees		= automaton_set_syntax_evaluate(tables, gr1_game->guarantees, &guarantees_count, set_name);
			winning_region_automaton	= automaton_get_gr1_strategy(game_automaton, assumptions, assumptions_count
					, guarantees, guarantees_count);
			if(print_fsp){
				char buf[255];
				//automaton_automaton_print(tables->composition_entries[i]->valuation.automaton_value, true, true, true, "*\t", "*\t");
				sprintf(buf, "%s_%d_strat_%s.fsp", ctx_name, i, is_synchronous? "synch": "asynch");
				automaton_automaton_print_fsp(winning_region_automaton, buf);
			}
			//restore old liveness valuations and old context
			if(was_merged){
				free(ctx->global_fluents);
				free(game_automaton->valuations);
				free(game_automaton->inverted_valuations);
				was_merged	= false;
				game_automaton->inverted_valuations	= old_inverted_valuations;
				game_automaton->valuations			= old_valuations;
				game_automaton->valuations_size		= old_valuations_size;
				ctx->global_fluents_count			= old_fluents_count;
				ctx->global_fluents					= old_fluents;
			}
			if(winning_region_automaton != NULL)
				automaton_automaton_destroy(winning_region_automaton);
			for(j = 0; j < assumptions_count; j++)
				free(assumptions[j]);
			free(assumptions);
			for(j = 0; j < guarantees_count; j++)
				free(guarantees[j]);
			free(guarantees);
		}
	}

	if(print_fsp){
		char buf[255];

		for(i = 0; i < tables->composition_count; i++){
			if(tables->composition_entries[i]->solved){
				//automaton_automaton_print(tables->composition_entries[i]->valuation.automaton_value, true, true, true, "*\t", "*\t");
				sprintf(buf, "%s_%d_result_%s.fsp", ctx_name, i, is_synchronous? "synch": "asynch");
				automaton_automaton_print_fsp(tables->composition_entries[i]->valuation.automaton_value, buf);
			}
		}
	}
	free(liveness_formulas); free(liveness_formulas_names);
	automaton_parsing_tables_destroy(tables);
	return ctx;
}



