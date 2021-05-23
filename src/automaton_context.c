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
	tables->equivalence_count	= 0;
	tables->equivalence_entries	= NULL;
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
	automaton_parsing_tables_destroy_entry(tables->equivalence_entries, tables->equivalence_count);
	free(tables);
}
automaton_parsing_table_entry* automaton_parsing_table_entry_create(automaton_parsing_table_entry_type type, char* key, void* value, int32_t index){
	automaton_parsing_table_entry* entry	= calloc(1, sizeof(automaton_parsing_table_entry));
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
			entry->valuation.labels_value = NULL;
		}
		break;
	case COMPOSITION_ENTRY_AUT:
		if(entry->valuation.automaton_value != NULL){
			automaton_automaton_destroy(entry->valuation.automaton_value);
			entry->valuation.automaton_value = NULL;
		}
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
	case EQUIVALENCE_CHECK_ENTRY_AUT:
		for(i = 0; i < (int32_t)tables->equivalence_count; i++){
			if(strcmp(tables->equivalence_entries[i]->key, key) == 0)
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
	case EQUIVALENCE_CHECK_ENTRY_AUT:
					current_index	= tables->equivalence_count;
					entry			= automaton_parsing_table_entry_create(type, key, value, current_index);
					aut_add_ptr_list((void***)&(tables->equivalence_entries), (void*)entry, &(tables->equivalence_count));
					break;
	}
	return current_index;
}

void automaton_statement_syntax_to_table(automaton_statement_syntax* statement, automaton_parsing_tables* tables){
	automaton_expression_syntax* expr;
	automaton_fluent_syntax* fluent_def;
	automaton_set_def_syntax* set_def;
	automaton_composition_syntax* composition_def;
	automaton_equivalence_check_syntax* equivalence_def;
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
	case EQUIV_CHECK_AUT:
		equivalence_def	= statement->equivalence_check;
		if(equivalence_def->name != NULL){
			automaton_parsing_tables_add_entry(tables, EQUIVALENCE_CHECK_ENTRY_AUT, equivalence_def->name, (void*)equivalence_def);
		}
		break;
	default: break;
	//ASSERTION_AUT,
	//GOAL_AUT
	}
}
/**
 * Evaluates an expression against a valuation and returns the result as an int
 * @param tables the staging parsing structure
 * @param expr the expression to be evaluated
 * @param indexes_valuation the valuation that will map variables to values in expr
 * @param the integer value resulting from evaluating the expression against the given valuation
 */
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
		printf("Range type not implemented ");
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
	default:
		printf("Syntax expression could not be evaluated\n");
		exit(-1);
		break;
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


/**
 * Returns the values of a set by extension, given its syntax definition
 * @param tables staging parsing structrue
 * @param set set syntax's definition
 * @param count placeholder for the resulting set length
 * @param set_def_key set reference name within the parsing structure
 * @return a list of string representing the values of the required set
 */
char** automaton_set_syntax_evaluate(automaton_parsing_tables* tables, automaton_set_syntax* set, int32_t *count, char* set_def_key){
	int32_t index = -1;
	uint32_t i, j,k;
	char* current_entry;
	int32_t inner_count			= 0;
	char** inner_value			= NULL;
	bool is_set;
	automaton_indexes_syntax* indexes	= NULL;
	if(set == NULL)return NULL;
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
	automaton_string_list *ret_value			= automaton_string_list_create(true, false);
	//if proper set was not solved try to solve it
	index						= automaton_parsing_tables_get_entry_index(tables, SET_ENTRY_AUT, set_def_key);
	automaton_indexes_valuation **valuations	= NULL;
	uint32_t valuations_count					= 0;
	uint32_t valuations_size					= 0;
	for(i = 0; i < set->count; i++)for(j = 0; j < set->labels_count[i]; j++){
		automaton_label_syntax* label_syntax	= set->labels[i][j];
		if(!(label_syntax->is_set) && label_syntax->string_terminal == NULL)continue;//empty label (TAU)
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
			uint32_t **values = NULL;
			automaton_indexes_syntax_eval_strings(tables, NULL, &valuations, &valuations_count, &valuations_size, &values, &tmp_value, &inner_count, indexes);
			for(k = 0; k < valuations_count; k++)automaton_indexes_valuation_destroy(valuations[k]); free(valuations); valuations = NULL; valuations_count = 0;
			aut_merge_string_lists(ret_value, tmp_value, inner_count);
			for(k = 0; k < (uint32_t)inner_count;k++)free(tmp_value[k]);
			for(k = 0; k < (uint32_t)inner_count;k++)free(values[k]);
			free(tmp_value);
			free(values);
		}else{
			aut_merge_string_lists(ret_value, inner_value, inner_count);
		}
	}
	char** arr_value	= calloc(ret_value->count, sizeof(char*));
	for(k = 0; k < ret_value->count; k++){
		char *val	= calloc(strlen(ret_value->list[k]) + 1, sizeof(char));
		strcpy(val, ret_value->list[k]);
		arr_value[k]	= val;
	}
	*count	= ret_value->count;
	automaton_string_list_destroy(ret_value);
	//once solved, update set_def_entry
	if(index >= 0){
		tables->set_entries[index]->solved	= true;
		tables->set_entries[index]->valuation_count	 	= *count;
		tables->set_entries[index]->valuation.labels_value	= arr_value;
	}
	return arr_value;
}

/**
 * Recreates a global alphabet from the information in the automaton_parsing_tables structure
 * @param tables staging parsing table structure
 * @return the automaton_alphabet holding the global alphabet definitions
 */
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
				/*
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
				*/
			}
	}
	return global_alphabet;
}
/**
 * Augment current valuation with the ranges missing from incoming indexes
 * @param valuation the valuation to be augmented
 * @param tables staging parsing structure
 * @param indexes the indexes from where to take the ranges to augment the valuation
 */
void automaton_indexes_valuation_add_indexes(automaton_indexes_valuation* valuation, automaton_parsing_tables* tables, automaton_indexes_syntax* indexes){
	uint32_t i, j;
	char name[40];
	uint32_t new_count = 0;
	bool found = false;
	for(j = 0; j < indexes->count; j++){
		found = false;
		for(i  = 0; i < valuation->count; i++){
			if(indexes->indexes[j]->is_expr){
				if(indexes->indexes[j]->expr->type == INTEGER_TERMINAL_TYPE_AUT){
					sprintf(name, "%d", indexes->indexes[j]->expr->integer_terminal);
				}else{
					sprintf(name, "%s", indexes->indexes[j]->expr->string_terminal);
				}
			}else{
				sprintf(name, "%s", indexes->indexes[j]->lower_ident);
			}
			if(strcmp(valuation->ranges[i]->name, name)== 0){
				found = true;
				break;
			}
		}
		if(!found)
			new_count++;
	}
	new_count	+= valuation->count;
	int32_t* ptr	= realloc(valuation->current_values, sizeof(int32_t) * (new_count));
	if(ptr == NULL){
		printf("Could not allocate memory[automaton_indexes_valuation_add_indexes:1]\n");
		exit(-1);
	}else{
		valuation->current_values	= ptr;
	}
	automaton_range** ranges_ptr	= realloc(valuation->ranges, sizeof(automaton_range*) * (new_count));
	if(ptr == NULL){
		printf("Could not allocate memory[automaton_indexes_valuation_add_indexes:2]\n");
		exit(-1);
	}else{
		valuation->ranges	= ranges_ptr;
	}
	int32_t lower_index, upper_index;

	automaton_range* range;
	for(i = 0; i < indexes->count; i++){
		found = false;
		for(j = 0; j < valuation->count; j++){
			if(strcmp(valuation->ranges[j]->name, name)== 0){
				found = true;
				break;
			}
		}
		if(found)
			continue;
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
		valuation->current_values[valuation->count + i]	= lower_index;
		valuation->ranges[valuation->count + i]			= range;
		valuation->total_combinations	*= (upper_index - lower_index);

	}
	valuation->count	= new_count;
}

/**
 * Fix the index with name according to parameter to the value provided
 *
 * @param valuation current valuation to be fixed over a single variable
 * @param index_name the name of the variable to be fixed
 * @param value the value to be set on said variable
 */
void automaton_indexes_valuation_fix_index(automaton_indexes_valuation* valuation, char* index_name, int32_t value){
	uint32_t i;
	for(i = 0; i < valuation->count; i++){
		if(strcmp(valuation->ranges[i]->name, index_name) == 0){
			valuation->current_values[i] = value;
			return;
		}
	}
}

/**
 * Initializes a valuation object from a set of indexes
 * @param tables the parsing staging structure
 * @param indexes current indexes from wich to initialize the valuation
 * @return the valuation where each value is defined according to ranges or values defined in indexes
 */
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
				int32_t index			 	=
						automaton_parsing_tables_get_entry_index(tables, CONST_ENTRY_AUT,indexes->indexes[i]->expr->string_terminal);
				if(index != -1){
					sprintf(name, "%s", indexes->indexes[i]->expr->string_terminal);
					lower_index	= upper_index	= tables->const_entries[index]->valuation.int_value;
				}else{
					sprintf(name, "%s", indexes->indexes[i]->expr->string_terminal);
				}
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

bool automaton_statement_syntax_to_composition(automaton_automata_context* ctx, automaton_composition_syntax* composition_syntax
		, automaton_parsing_tables* tables, uint32_t main_index){
	int32_t i, index;
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
	automaton_synchronization_type* synch_type	= malloc(sizeof(automaton_synchronization_type) * composition_syntax->count);
	for(i = 0; i < (int32_t)composition_syntax->count; i++){
		//TODO: update transitions with prefixes/indexes
		index						= automaton_parsing_tables_get_entry_index(tables, COMPOSITION_ENTRY_AUT, composition_syntax->components[i]->ident);
		automata[i]					= tables->composition_entries[index]->valuation.automaton_value;
		switch(composition_syntax->components[i]->synch_type){
			case CONCURRENT_AUT: synch_type[i]	= CONCURRENT;break;
			case SYNCH_AUT: synch_type[i]		= SYNCHRONOUS;break;
			default: synch_type[i]				= ASYNCHRONOUS;break;
		}
	}
	//if is game build fluents and add to automata
	uint32_t composition_count	= composition_syntax->count;
	aut_context_log("composing.\n");
	automaton_automaton* automaton	= automaton_automata_compose(automata, synch_type, composition_count, composition_syntax->is_game
			, composition_syntax->name);//SYNCHRONOUS);
	tables->composition_entries[main_index]->solved	= true;
	tables->composition_entries[main_index]->valuation_count			= 1;
	tables->composition_entries[main_index]->valuation.automaton_value	= automaton;
	aut_context_log("done, %d states\n", automaton->transitions_count);
	free(automata);
	free(synch_type);
	return false;
}

/**
 * Adds an element to the local alphabet, respecting order, if element is not found in global alphabet report error
 * @param element_to_find string to find in the global alphabet array
 * @param ctx automata context over which current automata is initialized
 * @param local_alphabet_count a placeholder for the local alphabet count after adding the element (if not present)
 * @param local_alphabet the local alphabet where the element will be added (if needed)
 */
void automaton_statement_syntax_find_add_local_element(char *element_to_find, automaton_automata_context* ctx, uint32_t *local_alphabet_count, uint32_t **local_alphabet){
	int32_t element_global_index= -1;
	int32_t element_position	= (*local_alphabet_count);
	int32_t m;
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
	for(m = 0; m < (int32_t)(*local_alphabet_count); m++){
		if((*local_alphabet)[m] == (uint32_t)element_global_index){
			element_position	= -1;
			break;
		}
		if((*local_alphabet)[m] > (uint32_t)element_global_index){
			element_position	= m;
			break;
		}
	}
	if(element_position >= 0){
		uint32_t* new_alphabet	= malloc(sizeof(uint32_t) * ((*local_alphabet_count) + 1));
		for(m = 0; m < (int32_t)(*local_alphabet_count); m++){
			if(m < element_position)
				new_alphabet[m]	= (*local_alphabet)[m];
			else
				new_alphabet[m+1]	= (*local_alphabet)[m];
		}
		new_alphabet[element_position]	= (uint32_t)element_global_index;
		(*local_alphabet_count)++;
		if((*local_alphabet) != NULL) free((*local_alphabet));
		(*local_alphabet)	= new_alphabet;
	}

}

/**
 * Initialize the local alphabet from the automaton and valuations list for each state
 * @param ctx is the automata context where the automaton is being held
 * @param composition_syntax staging structure containing the automaton definition
 * @param tables the stating parsing structure holding overall references
 * @param local_alphabet_count placeholder for local alphabet count
 * @param local_alphabet place holder for the local alphabet as array of int, where each int points to the position of each element in the global alphabet (ordered)
 */
void automaton_statement_syntax_build_local_alphabet(automaton_automata_context* ctx, automaton_composition_syntax* composition_syntax
		, automaton_parsing_tables* tables, uint32_t *local_alphabet_count, uint32_t** local_alphabet){
	int32_t i,j,k,l,m,n,o;
	/** BUILD LOCAL ALPHABET **/
	int32_t		element_global_index;
	int32_t		element_position;
	char** ret_value				= NULL;
	uint32_t **indexes_values		= NULL;
	int32_t count					= 0;
	automaton_state_syntax* state;
	automaton_transition_syntax* transition;
	automaton_trace_label_syntax* trace_label;
	automaton_trace_label_atom_syntax* trace_label_atom;
	automaton_label_syntax* atom_label;
	uint32_t current_valuations_size = LIST_INITIAL_SIZE;
	uint32_t current_valuations_count = 0;
	automaton_indexes_valuation **current_valuations = calloc(current_valuations_size, sizeof(automaton_indexes_valuation*));
	char* element_to_find;

	for(i = 0; i < (int32_t)composition_syntax->count; i++){
			state	= composition_syntax->states[i];
			if(state->label->indexes != NULL){
				if(current_valuations_count >= (current_valuations_size - 1)){
					uint32_t new_size	= current_valuations_size * LIST_INCREASE_FACTOR;
					automaton_indexes_valuation** ptr = realloc(current_valuations, new_size * sizeof(automaton_indexes_valuation*));
					if(ptr == NULL){
						printf("Could not reallocate (*current_valuations)\n");
						exit(-1);
					}
					current_valuations_size	= new_size;
					current_valuations = ptr;
				}
				current_valuations[current_valuations_count++] 	= automaton_indexes_valuation_create_from_indexes(tables, state->label->indexes);
			}
			for(j = 0; j < (int32_t)state->transitions_count; j++){
				transition	= state->transitions[j];
				//TODO: take guards into consideration
				for(k = 0; k < (int32_t)transition->count; k++){
					trace_label	= transition->labels[k];
					for(l = 0; l < (int32_t)trace_label->count; l++){
						trace_label_atom	= trace_label->atoms[l];
						atom_label			= trace_label_atom->label;
						if(!atom_label->is_set && atom_label->string_terminal == NULL)continue; //tau
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

								automaton_indexes_syntax_eval_strings(tables, current_valuations_count > 0 ? current_valuations[current_valuations_count -1] : NULL
										, &current_valuations, &current_valuations_count, &current_valuations_size, &indexes_values, &ret_value, &count, atom_label->indexes);
								for(n = 0; n < count;n++)free(indexes_values[n]);
								free(indexes_values); indexes_values = NULL;
								for(n = 0; n < count; n++){
									automaton_statement_syntax_find_add_local_element(ret_value[n], ctx, local_alphabet_count, local_alphabet);

								}
							}else{
								automaton_statement_syntax_find_add_local_element(atom_label->string_terminal, ctx, local_alphabet_count, local_alphabet);
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
										automaton_indexes_syntax_eval_strings(tables, current_valuations_count > 0 ? current_valuations[current_valuations_count - 1] : NULL
												, &current_valuations, &current_valuations_count, &current_valuations_size, &indexes_values, &ret_value, &count, atom_label->set->labels[n][o]->indexes);
										for(n = 0; n < count;n++)free(indexes_values[n]);
										free(indexes_values); indexes_values = NULL;
										for(n = 0; n < count; n++){
											automaton_statement_syntax_find_add_local_element(ret_value[n], ctx, local_alphabet_count, local_alphabet);
										}
									}else{
										automaton_statement_syntax_find_add_local_element(atom_label->set->labels[n][o]->string_terminal, ctx, local_alphabet_count, local_alphabet);
									}
								}
							}
						}
					}
				}
			}
		}
		for(n = 0; n < current_valuations_count; n++){
			automaton_indexes_valuation_destroy(current_valuations[n]);
		}
		free(current_valuations);
		current_valuations = NULL;
		if(ret_value != NULL){
			for(n = 0; n < count; n++){
				free(ret_value[n]);
			}
			free(ret_value);
			ret_value = NULL;
			count = 0;
		}
		aut_context_log("local alphabet built with size %d\n", (*local_alphabet_count));
}
/**
 * Creates a new valuation constructed from the merging of the last valuation and the new indexes structure
 * @param tables staging parsing structure
 * @param previous_valuation the last element of the previous valuation
 * @param indexes label indexes over which the new valuation will be computed and added to the array
 * @param returns the newly created valuation
 */
automaton_indexes_valuation* automaton_statement_syntax_create_implicit_transition_valuation(automaton_parsing_tables *tables, automaton_indexes_valuation *previous_valuation, automaton_indexes_syntax *indexes){
	automaton_indexes_valuation *composed_valuation = NULL;
	if(previous_valuation != NULL){
		composed_valuation = automaton_indexes_valuation_clone(previous_valuation);
		automaton_indexes_valuation_add_indexes(composed_valuation, tables, indexes);
	}else{
		composed_valuation = automaton_indexes_valuation_create_from_indexes(tables, indexes);
	}
	return composed_valuation;
}

/**
 * Initializes an automaton from a composition staging structure
 * @param ctx automata context over which to create the automaton
 * @param composition_syntax staging structure containing the definition of the automaton
 * @param tables the overall parsing staging structure where the automaton will have its reference
 * @return true if there are more automata to be initialized
 */
bool automaton_statement_syntax_to_automaton(automaton_automata_context* ctx, automaton_composition_syntax* composition_syntax
		, automaton_parsing_tables* tables){
	int32_t main_index, index, i, j, k, l, m, n, o, p, r, s;
	main_index						= automaton_parsing_tables_get_entry_index(tables, COMPOSITION_ENTRY_AUT, composition_syntax->name);
	if(main_index >= 0)if(tables->composition_entries[main_index]->solved)	return false;
	//check whether composition syntax is a composition or a single automaton description
	if(composition_syntax->components != NULL){//MULTIPLE COMPONENTS (AUTOMATA)
		return automaton_statement_syntax_to_composition(ctx, composition_syntax, tables, main_index);
	}else{//SINGLE COMPONENT (AUTOMATON)
#if DEBUG_PARSE_STATES
		printf("[I] Parsing automaton %s\n", composition_syntax->name);
#endif
		uint32_t	local_alphabet_count	= 0;
		uint32_t*	local_alphabet		= NULL;
		uint32_t current_valuations_size = LIST_INITIAL_SIZE, current_valuations_count = 0;
		automaton_indexes_valuation** current_valuations	= calloc(current_valuations_size, sizeof(automaton_indexes_valuation*));
		uint32_t next_valuations_size = LIST_INITIAL_SIZE, next_valuations_count = 0;
		automaton_indexes_valuation** next_valuations	= calloc(next_valuations_size, sizeof(automaton_indexes_valuation*));

		automaton_statement_syntax_build_local_alphabet(ctx, composition_syntax, tables, &local_alphabet_count, &local_alphabet);
		/** CREATE AUTOMATON **/
		char** ret_value				= NULL;
		uint32_t **indexes_values		= NULL;
		int32_t count					= 0;
		automaton_state_syntax* state;
		automaton_transition_syntax* transition;
		automaton_trace_label_syntax* trace_label;
		automaton_trace_label_atom_syntax* trace_label_atom;
		automaton_label_syntax* atom_label;

		automaton_automaton* automaton	= automaton_automaton_create(composition_syntax->name, ctx, local_alphabet_count, local_alphabet, false, false);

		free(local_alphabet);
		//add transitions
		//map state label to int
		//char** labels_list	= NULL;
		automaton_string_list *labels_list	= automaton_string_list_create(false, false);
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
		uint32_t added_state	= composition_syntax->count;
		char label_indexes[255];
		label_indexes[0] = '\0';
		bool first_index_set	= false;
		//HANDLE REF STATES
		//initialize states list
#if DEBUG_PARSE_STATES
		printf("[Rs] Initializing ref states\n\t");
#endif
		for(i = 0; i < (int32_t)composition_syntax->count; i++){
			automaton_indexes_valuation *current_valuation;
			state	= composition_syntax->states[i];
			if(state->label->indexes != NULL)
				current_valuation 	= automaton_indexes_valuation_create_from_indexes(tables, state->label->indexes);
			else
				current_valuation	= NULL;
			bool first_run_from	= true;

			automaton_indexes_valuation_set_label(current_valuation, state->label->name, label_indexes);
			aut_push_string_to_list(labels_list, label_indexes, &label_position);
#if DEBUG_PARSE_STATES
		printf("%s ",labels_list->list[label_position]);
#endif
			if(current_valuation != NULL){
				automaton_indexes_valuation_destroy(current_valuation);
				current_valuation	= NULL;
			}
		}
#if DEBUG_PARSE_STATES
		printf("\n");
#endif
		//TODO: solve aliases in states e.g. S[i:R] = S_p[i], S_p[i:R] = (s->S_2[i]).
		//update ref states
		uint32_t explicit_start_state_count = 0, explicit_start_state_size= 0;
		uint32_t *explicit_start_states = NULL;
		automaton_indexes_valuation **explicit_start_valuations = NULL;
		automaton_indexes_valuation *explicit_start_valuation = NULL;


		for(i = 0; i < (int32_t)composition_syntax->count; i++){
			state	= composition_syntax->states[i];
			if(state->ref != NULL){
				automaton_indexes_valuation *current_valuation = NULL;
				if(state->label->indexes != NULL){
					current_valuation 	= automaton_indexes_valuation_create_from_indexes(tables, state->label->indexes);
				}
				label_indexes[0] = '\0';
				automaton_indexes_valuation_set_label(current_valuation, state->label->name, label_indexes);

				aut_push_string_to_list(labels_list, label_indexes, &label_position);
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
				aut_push_string_to_list(labels_list, label_indexes, &label_position);
				to_state	= (uint32_t) label_position;
				if(current_valuation != NULL){
					automaton_indexes_valuation_destroy(current_valuation);
					current_valuation	= NULL;
				}

				if(!first_state_set){
					automaton_automaton_add_initial_state(automaton, to_state);
					first_state_set	= true;
				}else{
					//partial incomplete solution to state to state ref: (S[i:R] = S_p[i].) converted to (S[i:R] = (<> -> S_p[i]).
					//create empty transitions
					current_automaton_transition[0]	= automaton_transition_create(current_from_state[0], to_state);
					automaton_automaton_add_transition(automaton, current_automaton_transition[0]);
					automaton_transition_destroy(current_automaton_transition[0], true);
				}
				continue;
			}
		}

		//STATE ITERATION
		int32_t		element_global_index = -1;
		char* element_to_find;
		char added_state_string[255];

		for(i = 0; i < (int32_t)composition_syntax->count; i++){
			state	= composition_syntax->states[i];
			if(state->ref != NULL){//do not process ref states at this loop
				continue;
			}
			if(state->label->indexes != NULL){
				if(explicit_start_valuation != NULL){
					automaton_indexes_valuation_destroy(explicit_start_valuation);
					explicit_start_valuation = NULL;
				}
				if(explicit_start_valuations != NULL){
					for(j = 0; j < explicit_start_state_count; j++)automaton_indexes_valuation_destroy(explicit_start_valuations[j]);
					free(explicit_start_valuations);
				}
				if(explicit_start_states != NULL)free(explicit_start_states);
				explicit_start_valuation 	= automaton_indexes_valuation_create_from_indexes(tables, state->label->indexes);
				ret_value	= malloc(sizeof(char*));
				aut_dupstr(&(ret_value[0]),  state->label->name);
				count 		= 1;
				//main load
				automaton_indexes_syntax_eval_strings(tables,explicit_start_valuation
						,&explicit_start_valuations, &explicit_start_state_count, &explicit_start_state_size, &indexes_values, &ret_value, &count, state->label->indexes);
				for(j = 0; j < count; j++){free(ret_value[j]);free(indexes_values[j]);}
				free(ret_value); ret_value = NULL;
				free(indexes_values); indexes_values = NULL;

				explicit_start_states	= calloc(explicit_start_state_count, sizeof(uint32_t));
				labels_list->sorted	= false;
				for(j = 0; j < count; j++){
					automaton_indexes_valuation_set_from_label(tables, explicit_start_valuations[j], state->label->indexes, state->label->name, label_indexes);
					aut_push_string_to_list(labels_list, label_indexes, &label_position);
					//explicit from state
					explicit_start_states[j]	= (uint32_t)label_position;
					if(!first_state_set){
						automaton_automaton_add_initial_state(automaton, label_position);
						first_state_set	= true;
					}
				}
				automaton_indexes_valuation_destroy(explicit_start_valuation); explicit_start_valuation	= NULL;
			}else{
				explicit_start_valuation	= NULL;
				explicit_start_state_count	= explicit_start_state_size	= 1;
				explicit_start_valuations	= calloc(explicit_start_state_count, sizeof(automaton_indexes_valuation*));
				explicit_start_states	= calloc(explicit_start_state_count, sizeof(uint32_t));
				aut_push_string_to_list(labels_list, state->label->name, &label_position);
				//explicit from state
				explicit_start_states[0]	= (uint32_t)label_position;
				if(!first_state_set){
					automaton_automaton_add_initial_state(automaton, label_position);
					first_state_set	= true;
				}
			}
			//aut_context_log("\t(%s):", label_indexes);
			//FROM-STATE PIPED-TRANSITIONS ITERATION
			for(j = 0; j < (int32_t)state->transitions_count; j++){
				if(current_valuations != NULL || current_valuations_count > 0){
					for(k = 0; k < current_valuations_count; k++){
						automaton_indexes_valuation_destroy(current_valuations[k]);
					}
					if(current_valuations != NULL){
						free(current_valuations);
						current_valuations = NULL;
					}
				}
				current_valuations_count = current_valuations_size = current_from_state_size = current_from_state_count	= explicit_start_state_count;
				if(current_from_state != NULL)free(current_from_state);
				current_from_state	= calloc(current_from_state_size, sizeof(uint32_t));
				current_valuations = calloc(current_valuations_size, sizeof(automaton_indexes_valuation*));
				for(k = 0; k < current_from_state_count; k++){
					current_from_state[k]	= explicit_start_states[k];
					current_valuations[k]	= explicit_start_valuations[k] == NULL ? NULL : automaton_indexes_valuation_clone(explicit_start_valuations[k]);
#if DEBUG_PARSE_STATES
					printf("\t[V] index %i start state %d ", k, current_from_state[k]);
					if(current_valuations[k] != NULL){
						automaton_indexes_valuation_print(current_valuations[k], " ", " ");
					}else{
						printf("no valuation\n");
					}
#endif
				}
				transition	= state->transitions[j];

#if DEBUG_PARSE_STATES
				printf("\t[T] piped transition %i \n", j);
#endif
				//IMPLICIT TRANSITIONS ITERATION
				for(k = 0; k < (int32_t)transition->count; k++){
#if DEBUG_PARSE_STATES
					printf("\t\t[t] implicit transition %d\n", k);
#endif
					bool explicit_to_state	= k >= (((int32_t)transition->count) - 1);
					//COMPUTE TO-STATE VALUE

					for(r = 0; r < (int32_t)current_from_state_count; r++){
#if DEBUG_PARSE_STATES
						printf("\t\t[>] current from state %d (%d)\n", current_from_state[r], current_from_state_count);
#endif
						if(transition->condition != NULL && k == 0){
							//if(!automaton_expression_syntax_evaluate(tables, transition->condition, current_valuations_count > 0 ? current_valuations[current_valuations_count - 1]: NULL)){
							if(!automaton_expression_syntax_evaluate(tables, transition->condition, current_valuations_count > 0 ? current_valuations[current_valuations_count -current_from_state_count + r]: NULL)){
								continue;
							}
						}
						trace_label	= transition->labels[k];
						//SET ITERATION ( s_i = ({a,b,c} -> S_j).
						first_index_set = false;
						for(l = 0; l < (int32_t)trace_label->count; l++){

							trace_label_atom	= trace_label->atoms[l];
							atom_label			= trace_label_atom->label;
							if(!atom_label->is_set && atom_label->string_terminal == NULL){
#if DEBUG_PARSE_STATES
								printf("\t\t[A] tau transition\n");
#endif
								//continue;//tau
							}
							if(!atom_label->is_set){
								//process set ( Alphabet -> ...)
								if(atom_label->indexes != NULL){
#if DEBUG_PARSE_STATES
									if(current_valuations_count > 0){
										printf("\t\t\t[M] Merging implicit valuation with:");
										if(current_valuations[current_valuations_count -current_from_state_count + r] != NULL)
											automaton_indexes_valuation_print(current_valuations[current_valuations_count -current_from_state_count + r], " ", " ");
										else{
											printf("no valuation\n");
										}
									}
#endif
									automaton_indexes_valuation *implicit_valuation = automaton_statement_syntax_create_implicit_transition_valuation(tables,current_valuations_count > 0 ? current_valuations[current_valuations_count -current_from_state_count + r] : NULL
											, atom_label->indexes);
									if(!first_index_set){
										//remove explicit transition
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
									//when strings are evaluated over a valuation the new valuations should replace the current one
									//then inside the count cycle should take from current_valuations[current_valuations_count - count + n]
									automaton_indexes_syntax_eval_strings(tables,implicit_valuation
											,&next_valuations, &next_valuations_count, &next_valuations_size, &indexes_values, &ret_value, &count, atom_label->indexes);
#if DEBUG_PARSE_STATES
									if(next_valuations_count > 0){
										printf("\t\t[v] next valuation %d of %d\n", next_valuations_count - count, next_valuations_count);
										automaton_indexes_valuation_print(next_valuations[next_valuations_count - count], " ", " ");
									}else{
										printf("\t\t[v] No next valuation\n");
									}
#endif
									automaton_indexes_valuation_destroy(implicit_valuation);
									implicit_valuation = NULL;
									for(n = 0; n < count; n++){
										element_to_find		= ret_value[n];
										element_global_index= -1;
										for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++){
											if(strcmp(ctx->global_alphabet->list[m].name, element_to_find) == 0){
												element_global_index = m;
												break;
											}
										}
										if(element_global_index >= 0){
											//TODO: set label_indexes according to current valuation values
#if DEBUG_PARSE_STATES
											if(next_valuations_count > 0){
												printf("\t\t[v] next valuation %d of %d:", next_valuations_count - count + n, next_valuations_count);
												automaton_indexes_valuation_print(next_valuations[next_valuations_count - count + n], " ", " ");
											}
											if(current_valuations_count > 0){
												printf("\t\t[v] current valuation %d of %d:", current_valuations_count -current_from_state_count + r, current_valuations_count);
												if(current_valuations[current_valuations_count -current_from_state_count + r] != NULL){
													automaton_indexes_valuation_print(current_valuations[current_valuations_count -current_from_state_count + r], " ", " ");
												}else{
													printf("no valuation\n");
												}
											}
#endif

											if(explicit_to_state){
												automaton_indexes_valuation_set_to_label(tables, current_valuations_count > 0 ? current_valuations[current_valuations_count -current_from_state_count + r]: NULL, next_valuations_count > 0 ? next_valuations[next_valuations_count - count + n]: NULL
														, transition->to_state->indexes,transition->to_state->name, label_indexes);
												aut_push_string_to_list(labels_list, label_indexes, &label_position);

												to_state	= (uint32_t)label_position;
#if DEBUG_PARSE_STATES
												printf("\t\t[<] to state reassigned as: %s(%d)\n", labels_list->list[label_position], label_position);
#endif
											}else{
												added_state++;
												snprintf(added_state_string, sizeof(added_state_string), "S___-%d", added_state);
												aut_push_string_to_list(labels_list, added_state_string, &label_position);
												to_state	= (uint32_t)label_position;
#if DEBUG_PARSE_STATES
												printf("\t\t[<] to state reassigned as: %s(%d)\n", labels_list->list[label_position], to_state);
#endif

											}
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
											automaton_transition_add_signal_event(current_automaton_transition[automaton_transition_count - 1], ctx, &(ctx->global_alphabet->list[element_global_index]));
#if DEBUG_PARSE_STATES
											printf("\t\t\t[s] signal added: %s\n", ctx->global_alphabet->list[element_global_index].name);
#endif
											//automaton_automaton_add_transition(automaton, automaton_transition[automaton_transition_count - 1]);
										}else{
											printf("Element not found:%s\nAlphabet:", element_to_find);
											for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++)
												printf("%s%s", ctx->global_alphabet->list[m].name, m == ((int32_t)ctx->global_alphabet->count - 1) ? "": ",");
											printf("\n");
											fflush(stdout);
											exit(-1);
										}
										next_from_state[next_from_state_count++]	= to_state;
									}
									for(n = 0; n < count;n++){
										free(indexes_values[n]);
										free(ret_value[n]);
									}
									free(indexes_values); indexes_values = NULL;
									free(ret_value); ret_value = NULL;

									to_state	= (uint32_t)label_position;
								}else{
									if(!first_index_set){
										first_index_set	= true;
									}
									element_global_index= -1;
									//atom_label->string_terminal == NULL && !atom_label->is_set && atom_label->indexes == NULL
									//is satisfied by the tau transition (used in digital systems for no change in signals)
									if(atom_label->string_terminal != NULL){
										for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++){
											if(strcmp(ctx->global_alphabet->list[m].name, atom_label->string_terminal) == 0){
												element_global_index = m;
												break;
											}
										}
									}
									if(element_global_index >= 0 || atom_label->string_terminal == NULL){
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
#if DEBUG_PARSE_STATES
										if(next_valuations_count > 0){
											printf("\t\t[v] next valuation %d of %d:", next_valuations_count - 1, next_valuations_count);
											automaton_indexes_valuation_print(next_valuations[next_valuations_count - 1], " ", " ");
										}
										if(current_valuations_count > 0){
											printf("\t\t[v] current valuation %d of %d:", current_valuations_count -current_from_state_count + r, current_valuations_count);
											if(current_valuations[current_valuations_count -current_from_state_count + r] != NULL){
												automaton_indexes_valuation_print(current_valuations[current_valuations_count -current_from_state_count + r], " ", " ");
											}else{
												printf("no valuation\n");
											}
										}
#endif
										if(!explicit_to_state){
											added_state++;
											snprintf(added_state_string, sizeof(added_state_string), "S___-%d", added_state);
											aut_push_string_to_list(labels_list, added_state_string, &label_position);
											to_state	= (uint32_t)label_position;
#if DEBUG_PARSE_STATES
											printf("\t\t[<] to state reassigned as: %s(%d)\n", labels_list->list[label_position], to_state);
#endif
										}else{
											automaton_indexes_valuation_set_to_label(tables, current_valuations_count > 0 ? current_valuations[current_valuations_count -current_from_state_count + r]: NULL, next_valuations_count > 0 ? next_valuations[next_valuations_count - 1]: NULL
													, transition->to_state->indexes,transition->to_state->name, label_indexes);
											aut_push_string_to_list(labels_list, label_indexes, &label_position);
											to_state	= (uint32_t)label_position;
#if DEBUG_PARSE_STATES
											printf("\t\t[<] to state reassigned as: %s(%d)\n", labels_list->list[label_position], label_position);
#endif
										}
										current_automaton_transition[automaton_transition_count++]	= automaton_transition_create(current_from_state[r], to_state);

										if(next_from_state_count >= (next_from_state_size - 1)){
											next_from_state_size *= LIST_INCREASE_FACTOR;
											uint32_t* new_next_from	= realloc(next_from_state, sizeof(uint32_t) * next_from_state_size);
											if(new_next_from == NULL){printf("Could not allocate memory [new_next_from:1]\n");exit(-1);}
											next_from_state	= new_next_from;
										}
										next_from_state[next_from_state_count++]	= to_state;

										if(!explicit_to_state){
											if(next_valuations_size == 0){
												next_valuations_size = LIST_INITIAL_SIZE;
												next_valuations_count	= 0;
												next_valuations	= calloc(next_valuations_size, sizeof(automaton_indexes_valuation*));
											}
											if(next_valuations_count >= (next_valuations_size - 1)){
												next_valuations_size	*= LIST_INCREASE_FACTOR;
												automaton_indexes_valuation** new_next_valuations	=
														realloc(next_valuations, sizeof(automaton_indexes_valuation*) * next_valuations_size);
												if(new_next_valuations == NULL){printf("Could not reallocate memory[next_valuations:1]\n");exit(-1);}
												next_valuations	= new_next_valuations;
											}
											next_valuations[next_valuations_count++] = current_valuations[current_valuations_count -current_from_state_count + r] != NULL?
													automaton_indexes_valuation_clone(current_valuations[current_valuations_count -current_from_state_count + r]) : NULL;
										}
										//do not add signal if transition is tau
										if(atom_label->string_terminal != NULL)
											automaton_transition_add_signal_event(current_automaton_transition[automaton_transition_count - 1], ctx, &(ctx->global_alphabet->list[element_global_index]));
#if DEBUG_PARSE_STATES
										printf("\t\t\t[s] signal added: %s\n", ctx->global_alphabet->list[element_global_index].name);
#endif
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
								for(n = 0; n < (int32_t)(atom_label->set->count); n++){
#if DEBUG_PARSE_STATES
									if(current_valuations_count > 0){
										printf("\t\t[v] current valuation %d of %d:", current_valuations_count -current_from_state_count + r, current_valuations_count);
										if(current_valuations[current_valuations_count -current_from_state_count + r] != NULL){
											automaton_indexes_valuation_print(current_valuations[current_valuations_count -current_from_state_count + r], " ", " ");
										}else{
											printf("no valuation\n");
										}
									}

#endif
									if(explicit_to_state){
										automaton_indexes_valuation_set_to_label(tables, current_valuations_count > 0 ? current_valuations[current_valuations_count -current_from_state_count + r]: NULL, next_valuations_count > 0 ? next_valuations[next_valuations_count - count + n]: NULL
												, transition->to_state->indexes,transition->to_state->name, label_indexes);
										aut_push_string_to_list(labels_list, label_indexes, &label_position);

										to_state	= (uint32_t)label_position;
#if DEBUG_PARSE_STATES
										printf("\t\t[<] to state reassigned as: %s(%d)\n", labels_list->list[label_position], label_position);
#endif
									}else{
										added_state++;
										snprintf(added_state_string, sizeof(added_state_string), "S___-%d", added_state);
										aut_push_string_to_list(labels_list, added_state_string, &label_position);
										to_state	= (uint32_t)label_position;
#if DEBUG_PARSE_STATES
										printf("\t\t[<] to state reassigned as: %s(%d)\n", labels_list->list[label_position], to_state);
#endif

									}
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
											//TODO: solve indexes on concurrent specs
											automaton_indexes_syntax_eval_strings(tables,current_valuations_count > 0 ? current_valuations[current_valuations_count -current_from_state_count + r] : NULL
													, &next_valuations, &next_valuations_count, &next_valuations_size, &indexes_values, &ret_value, &count, atom_label->set->labels[n][o]->indexes);
#if DEBUG_PARSE_STATES
											if(next_valuations_count > 0){
												printf("\t\t[v] next valuation %d of %d\n", next_valuations_count - count + n, next_valuations_count);
												automaton_indexes_valuation_print(next_valuations[next_valuations_count - count + n], " ", " ");
											}else{
												printf("\t\t[v] No next valuation\n");
											}
#endif
											for(p = 0; p < count; p++){
												element_to_find		= ret_value[p];
												element_global_index= -1;
												for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++){
													if(strcmp(ctx->global_alphabet->list[m].name, element_to_find) == 0){
														element_global_index = m;
														break;
													}
												}
												if(element_global_index >= 0){
													automaton_transition_add_signal_event(current_automaton_transition[automaton_transition_count - 1], ctx, &(ctx->global_alphabet->list[element_global_index]));
#if DEBUG_PARSE_STATES
													printf("\t\t\t[s] signal added: %s\n", ctx->global_alphabet->list[element_global_index].name);
#endif
												}
											}
											for(p = 0; p < count;p++)free(indexes_values[p]);
											free(indexes_values); indexes_values = NULL;
										}else{
											element_to_find		= atom_label->set->labels[n][o]->string_terminal;
											element_global_index= -1;
											for(m = 0; m < (int32_t)ctx->global_alphabet->count; m++){
												if(strcmp(ctx->global_alphabet->list[m].name, element_to_find) == 0){
													element_global_index = m;
													break;
												}
											}
											if(element_global_index >= 0){
												automaton_transition_add_signal_event(current_automaton_transition[automaton_transition_count - 1], ctx, &(ctx->global_alphabet->list[element_global_index]));
#if DEBUG_PARSE_STATES
												printf("\t\t\t[s] signal added: %s\n", ctx->global_alphabet->list[element_global_index].name);
#endif
											}
										}
									}
									if(next_from_state_count >= (next_from_state_size - 1)){
										next_from_state_size *= LIST_INCREASE_FACTOR;
										uint32_t* new_next_from	= realloc(next_from_state, sizeof(uint32_t) * next_from_state_size);
										if(new_next_from == NULL){printf("Could not allocate memory [new_next_from:2]\n");exit(-1);}
										next_from_state	= new_next_from;
									}
									next_from_state[next_from_state_count++]	= to_state;
									if(!explicit_to_state){
										if(next_valuations_size == 0){
											next_valuations_size = LIST_INITIAL_SIZE;
											next_valuations_count	= 0;
											next_valuations	= calloc(next_valuations_size, sizeof(automaton_indexes_valuation*));
										}
										if(next_valuations_count >= (next_valuations_size - 1)){
											next_valuations_size	*= LIST_INCREASE_FACTOR;
											automaton_indexes_valuation** new_next_valuations	=
													realloc(next_valuations, sizeof(automaton_indexes_valuation*) * next_valuations_size);
											if(new_next_valuations == NULL){printf("Could not reallocate memory[next_valuations:2]\n");exit(-1);}
											next_valuations	= new_next_valuations;
										}
										next_valuations[next_valuations_count++] = current_valuations[current_valuations_count -current_from_state_count + r] != NULL?
												automaton_indexes_valuation_clone(current_valuations[current_valuations_count -current_from_state_count + r]) : NULL;
									}
								}
							}

						}
						for(s= 0; s < (int32_t)automaton_transition_count; s++){
							automaton_automaton_add_transition(automaton, current_automaton_transition[s]);
							automaton_transition_destroy(current_automaton_transition[s], true);
						}
						automaton_transition_count	= 0;
					}//end of r:current from state
					free(current_from_state);
					current_from_state_size		= next_from_state_size;
					current_from_state_count	= 0;
					current_from_state			= malloc(sizeof(uint32_t) * current_from_state_size);
					for(s = 0; s < (int32_t)next_from_state_count; s++){
						current_from_state[current_from_state_count++]	= next_from_state[s];
					}
#if DEBUG_PARSE_STATES
					printf("\t\t[W] next valuations\n");
					for(s = 0; s < next_valuations_count; s++){
						if(next_valuations[s] == NULL)continue;
						printf("\t\t\t[N] %d:", s);
						automaton_indexes_valuation_print(next_valuations[s], " ", " ");
					}
					printf("\t\t[<] to state reassigned as: %s(%d)\n", labels_list->list[label_position], to_state);
#endif
					for(s = 0; s < current_valuations_count; s++)
						if(current_valuations[s] != NULL){
							automaton_indexes_valuation_destroy(current_valuations[s]);
							current_valuations[s]	= NULL;
						}
					free(current_valuations);
					current_valuations = next_valuations; current_valuations_count = next_valuations_count; current_valuations_size = next_valuations_size;
					next_valuations = NULL; next_valuations_count = 0; next_valuations_size = 0;
					next_from_state_count	= 0;
				}//end of k:implicit transition
				for(s = 0; s < current_valuations_count; s++)
					if(current_valuations[s] != NULL){
						automaton_indexes_valuation_destroy(current_valuations[s]);
						current_valuations[s]	= NULL;
					}
				free(current_valuations);
				current_valuations = NULL;
				current_valuations_count = current_valuations_size = 0;
				for(s = 0; s < next_valuations_count; s++)
					if(next_valuations[s] != NULL){
						automaton_indexes_valuation_destroy(next_valuations[s]);
						next_valuations[s]	= NULL;
					}
				free(next_valuations);
			}//end of j:piped transition
			for(s = 0; s < explicit_start_state_count; s++){
				if(explicit_start_valuations[s] != NULL)
					automaton_indexes_valuation_destroy(explicit_start_valuations[s]);
			}
			free(explicit_start_valuations);explicit_start_valuations	= NULL;
			free(explicit_start_states);explicit_start_states			= NULL;
			explicit_start_state_count	= explicit_start_state_size		= 0;
			//automaton_indexes_valuation_increase(current_valuations_count > 0 ? current_valuations[current_valuations_count - 1]: NULL);


			while(current_valuations_count > 0){
				automaton_indexes_valuation_destroy(current_valuations[--current_valuations_count]);
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
		automaton_string_list_destroy(labels_list);
		if(current_from_state != NULL){
			free(current_from_state);
			current_from_state = NULL;
		}
		free(next_from_state);
		free(current_automaton_transition);
		if(indexes_values != NULL){
			free(indexes_values);
			indexes_values = NULL;
		}
		for(i = 0; i < current_valuations_count; i++){
			automaton_indexes_valuation_destroy(current_valuations[i]);
		}
		free(current_valuations);
		current_valuations = NULL;
		current_valuations_count = 0;
		//set entry in table
		tables->composition_entries[main_index]->solved	= true;
		tables->composition_entries[main_index]->valuation_count			= 1;
		tables->composition_entries[main_index]->valuation.automaton_value	= automaton;
		return false;
	}
	return true;
}

bool automaton_statement_syntax_to_constant(automaton_expression_syntax* const_def_syntax
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
/**
 * Merges two valuations into one
 * @param first the first valuation to be merged
 * @param second the second valuation to be merged
 * @return the valuation that results from merging the input instances without repeating ranges (first instance is prioritized)
 */
automaton_indexes_valuation *automaton_indexes_valuation_merge(automaton_indexes_valuation *first, automaton_indexes_valuation *second){
	if(first == NULL)return automaton_indexes_valuation_clone(second);
	if(second == NULL)return automaton_indexes_valuation_clone(first);
	uint32_t i, j, k, count = second->count;
	bool found = false;
	//find range diff amount
	for(i = 0; i < first->count; i++){
		found = false;
		for(j = 0; j < second->count; j++){
			if(strcmp(first->ranges[i]->name, second->ranges[j]->name) == 0){
				found = true;
				break;
			}
		}
		if(found)count--;
	}
	automaton_indexes_valuation *valuation = automaton_indexes_valuation_create();
	valuation->count	= first->count + count;
	valuation->ranges	= calloc(valuation->count, sizeof(automaton_range*));
	valuation->total_combinations = first->total_combinations;
	valuation->current_values	= calloc(valuation->count, sizeof(int32_t));
	for(i = 0; i < first->count; i++){
		valuation->ranges[i]	= automaton_range_clone(first->ranges[i]);
		valuation->current_values[i]	= first->current_values[i];
	}
	for(k = 0; k < count; k++){
		for(i = 0; i < second->count; i++){
			found = false;
			for(j = 0; j < first->count; j++){
				if(strcmp(first->ranges[j]->name, second->ranges[i]->name) == 0){
					found = true;
					break;
				}
			}
			if(!found){
				valuation->ranges[first->count + k]	= automaton_range_clone(second->ranges[i]);
				valuation->current_values[first->count + k]	= second->current_values[i];
				valuation->total_combinations *= second->ranges[i]->upper_value - second->ranges[i]->lower_value;
			}
		}
	}
	return valuation;
}

/**
 * Defines an explicit state according to the valuations defined from the original from state and through the implicit states
 * @param tables staging parsing structure
 * @param current current indexes valuation
 * @param next next indexes valuation
 * @param to_indexes indexes from the terminal to state
 * @param label original label to be suffixed
 * @param target placeholder for the suffixed label
 */
void automaton_indexes_valuation_set_to_label(automaton_parsing_tables* tables, automaton_indexes_valuation* current, automaton_indexes_valuation* next
		, automaton_indexes_syntax* to_indexes, char* label, char* target){
	if(/*(next == NULL && current == NULL) ||*/ to_indexes == NULL){
		strcpy(target, label);
		return;
	}
	uint32_t i, j;
	sprintf(target, "%s", label);
	char name[40];
	char *from_ident, *to_ident;
	automaton_indexes_valuation *valuation	= NULL;
	if(next == NULL && current == NULL){
		valuation	= automaton_indexes_valuation_create();
	}else{
		valuation = automaton_indexes_valuation_merge(current, next);
	}
	for(j = 0; j < to_indexes->count; j++){
		if(to_indexes->indexes[j]->is_expr && (to_indexes->indexes[j]->expr->type == UPPER_IDENT_TERMINAL_TYPE_AUT ||
				to_indexes->indexes[j]->expr->type == IDENT_TERMINAL_TYPE_AUT)){
			int32_t index			 	= automaton_parsing_tables_get_entry_index(tables, CONST_ENTRY_AUT, to_indexes->indexes[j]->expr->string_terminal);
			if(index != -1){
				snprintf(target + strlen(target), sizeof(target), "_%d", tables->const_entries[index]->valuation.int_value);
			}else{
				snprintf(target + strlen(target), sizeof(target), "_%d", automaton_indexes_valuation_get_value(valuation, to_indexes->indexes[j]->expr->string_terminal));
			}
		}else if(to_indexes->indexes[j]->is_expr){
			snprintf(target + strlen(target), sizeof(target), "_%d", automaton_expression_syntax_evaluate(tables, to_indexes->indexes[j]->expr, valuation));
		}else{
			to_ident	= to_indexes->indexes[j]->lower_ident;
			snprintf(target + strlen(target), sizeof(target), "_%s", to_ident);
		}
	}

	automaton_indexes_valuation_destroy(valuation);
}

/**
 * Defines an explicit state according to the valuations defined from the original from state
 * @param tables staging parsing structure
 * @param current current indexes valuation
 * @param to_indexes indexes from the terminal to state
 * @param label original label to be suffixed
 * @param target placeholder for the suffixed label
 */
void automaton_indexes_valuation_set_from_label(automaton_parsing_tables* tables, automaton_indexes_valuation* current
		, automaton_indexes_syntax* to_indexes, char* label, char* target){
	if(current == NULL || to_indexes == NULL){
		strcpy(target, label);
		return;
	}
	uint32_t i, j;
	sprintf(target, "%s", label);
	char name[40];
	char *from_ident, *to_ident;
	if(current != NULL && to_indexes != NULL){
		for(j = 0; j < to_indexes->count; j++){
			if(to_indexes->indexes[j]->is_expr && (to_indexes->indexes[j]->expr->type == UPPER_IDENT_TERMINAL_TYPE_AUT ||
					to_indexes->indexes[j]->expr->type == IDENT_TERMINAL_TYPE_AUT)){
				snprintf(target + strlen(target), sizeof(target), "_%d", automaton_indexes_valuation_get_value(current, to_indexes->indexes[j]->expr->string_terminal));
			}else if(to_indexes->indexes[j]->is_expr){
				snprintf(target + strlen(target), sizeof(target), "_%d", automaton_expression_syntax_evaluate(tables, to_indexes->indexes[j]->expr, current));
			}else{
				to_ident	= to_indexes->indexes[j]->lower_ident;
				//snprintf(target + strlen(target), sizeof(target), "_%d", to_ident);
				snprintf(target + strlen(target), sizeof(target), "_%d", automaton_indexes_valuation_get_value(current, to_ident));
			}
		}
	}
}
/**
 * Append indexes values to label and return the result on target.
 * @param valuation current valuation for indexes to be applied on the label
 * @param label value to be prefixed in the computed new value
 * @param target string where the result will be set
 */
void automaton_indexes_valuation_set_label(automaton_indexes_valuation* valuation, char* label, char* target){
	if(valuation == NULL){
		strcpy(target, label);
		return;
	}
	sprintf(target, "%s", label);
	uint32_t i;
	for(i = 0; i < valuation->count; i++){
		snprintf(target + strlen(target), sizeof(target), "_%d", valuation->current_values[i]);
	}
}
#define INDEX_BUF_SIZE 800
/**
 * Adds to the provided set of strings a the suffixes constructed from combinations of indexes
 *
 * @param tables staging structures from the parser
 * @param last_valuation last valuation being evaluated
 * @param next_valuations new valuations computed for variables in the indexes
 * @param next_valuations_count number of new valuations for variables in the indexes
 * @param next_valuations_size size of the new valuations structure
 * @param values valuation to be applied on the indexes
 * @param a the array of strings to be suffixed, it will also be the place where the result is stored
 * @param a_count the number of input strings to be suffixed, it will also be the place where the number of suffixed strings are stored
 * @param indexes the indexes over which to apply the valuation
 */
void automaton_indexes_syntax_eval_strings(automaton_parsing_tables* tables, automaton_indexes_valuation* last_valuation
		, automaton_indexes_valuation*** next_valuations, uint32_t* next_valuations_count
		, uint32_t* next_valuations_size, uint32_t*** values, char*** a, int32_t* a_count, automaton_indexes_syntax* indexes){
	uint32_t i, j, k;
	int32_t *lower_index, *upper_index, *current_index, position;
	uint32_t total_combinations = 1;
	char buffer[INDEX_BUF_SIZE], buffer2[INDEX_BUF_SIZE];
	for(i = 0; i < INDEX_BUF_SIZE; i++){buffer[i] = 0; buffer2[i] = 0;}
	automaton_string_list *ret_value	= automaton_string_list_create(true, false);
	int32_t inner_count			= 0;
	int32_t effective_count	= indexes->count;
	lower_index	= malloc(sizeof(int32_t) * effective_count);
	upper_index	= malloc(sizeof(int32_t) * effective_count);
	current_index	= malloc(sizeof(int32_t) * effective_count);
	j = 0;

	//compute total combinations and initialize indexes
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
			if(last_valuation != NULL){
				for(k = 0; k < last_valuation->count; k++){
					if(strcmp(last_valuation->ranges[k]->name, indexes->indexes[i]->expr->string_terminal) == 0){
						current_index[j]	= lower_index[j] = upper_index[j]	= last_valuation->current_values[k];
						j++;
					}
				}
			}
		}
	}

	automaton_indexes_valuation *atom_valuation	= NULL;
	//resize valuations as needed
	if(last_valuation != NULL){
		atom_valuation = automaton_indexes_valuation_clone(last_valuation);
		//automaton_indexes_valuation_add_range()
		//automaton_indexes_valuation_destroy(last_valuation);
		if(*next_valuations_size <= *next_valuations_count + total_combinations){
			*next_valuations_size = (*next_valuations_count + total_combinations);
			automaton_indexes_valuation** ptr	= realloc(*next_valuations, sizeof(automaton_indexes_valuation*) * (*next_valuations_size));
			if(ptr == NULL){
				printf("Could not allocate memory[automaton_indexes_syntax_eval_strings:1]\n");
				exit(-1);
			}else{
				*next_valuations	= ptr;
			}
		}
	}else{
		atom_valuation	= automaton_indexes_valuation_create_from_indexes(tables, indexes);
		if(*next_valuations == NULL){
			*next_valuations_size	= total_combinations;
			*next_valuations	= calloc(*next_valuations_size, sizeof(automaton_indexes_valuation*));
		}else if(*next_valuations_size < total_combinations){
			*next_valuations_size	= total_combinations;
			automaton_indexes_valuation** ptr	= realloc(*next_valuations, sizeof(automaton_indexes_valuation*) * (*next_valuations_size));
			if(ptr == NULL){
				printf("Could not allocate memory[automaton_indexes_syntax_eval_strings:2]\n");
				exit(-1);
			}else{
				*next_valuations	= ptr;
			}
		}
	}
	//append new strings and valuations
	uint32_t **new_values	= malloc(total_combinations * sizeof(uint32_t*));
	automaton_indexes_valuation *current_valuation = NULL;
	for(i = 0; i < total_combinations; i++){
		new_values[i]	= malloc(effective_count * sizeof(uint32_t));
		(*next_valuations)[*next_valuations_count + i]	= automaton_indexes_valuation_clone(atom_valuation);
		current_valuation	= (*next_valuations)[*next_valuations_count + i];

		buffer[0] = '\0';
		for(j = 0; j < (uint32_t)effective_count; j++){
			if(strlen(buffer) > (INDEX_BUF_SIZE - 10) ){
				printf("[automaton_indexes_syntax_eval_strings] buffer overrun\n");
				exit(-1);
			}
			snprintf(buffer + strlen(buffer), sizeof(buffer), "_%d", current_index[j]);
			new_values[i][j]	= current_index[j];
			for(k = 0; k < current_valuation->count; k++){
				if((indexes->indexes[j]->is_expr && strcmp(current_valuation->ranges[k]->name, indexes->indexes[j]->expr->string_terminal) == 0)
						|| (!(indexes->indexes[j]->is_expr) && strcmp(current_valuation->ranges[k]->name, indexes->indexes[j]->lower_ident) == 0)){
					current_valuation->current_values[k]	= current_index[j];
					break;
				}
			}

		}
		for(k = 0; k < (uint32_t)*a_count; k++){
			if(strlen(buffer2) > (INDEX_BUF_SIZE - 10) ){
				printf("[automaton_indexes_syntax_eval_strings] buffer overrun\n");
				exit(-1);
			}
			sprintf(buffer2, "%s%s", (*a)[k], buffer);

			aut_push_string_to_list(ret_value, buffer2, &position);
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
	if(atom_valuation != NULL)automaton_indexes_valuation_destroy(atom_valuation);
	atom_valuation = NULL;

	(*next_valuations_count) += total_combinations;


	for(i = 0; i < (uint32_t)*a_count; i++){
		free((*a)[i]);
	}
	if(*a != NULL)
		free(*a);
	*a_count	= ret_value->count;
	//copy ret_value->list to a
	*a			= calloc(*a_count, sizeof(char*));
	for(i = 0; i < *a_count; i++){
		(*a)[i]	= calloc(strlen(ret_value->list[i]) + 1, sizeof(char));
		strcpy((*a)[i], ret_value->list[i]);
	}
	automaton_string_list_destroy(ret_value);
	if(*values != NULL){
		free(*values);
		*values = NULL;
	}
	*values = new_values;
	free(lower_index);
	free(upper_index);
	free(current_index);
}
/**
 * Evaluates index provided as input and sets its upper and lower bounds on lower_index and upper_index.
 * @param tables the staging parsing structures
 * @param index the current index to be observed
 * @param lower_index will hold the index lowest value
 * @param upper_index will hold the index upmost value
 */
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
	uint32_t i, j, k, l;
	if(fluent_def_syntax->initiating_set->is_ident || fluent_def_syntax->finishing_set->is_ident){
		//TODO: implement set by ref in fluents
		printf("[FATAL ERROR] fluent set by ref not implemented\n");
		exit(-1);
	}
	automaton_signal_event** sig_events;
	automaton_set_syntax* current_set;
	automaton_signal_event* current_event;
	for(i = 0; i < fluent_def_syntax->initiating_set->count; i++){
		if(fluent_def_syntax->initiating_set->labels[i][0]->is_set){
			current_set = fluent_def_syntax->initiating_set->labels[i][0]->set;
		}else{
			current_set	= fluent_def_syntax->initiating_set;
		}
		if(current_set->is_ident){
			//TODO: implement set by ref in fluents
			printf("[FATAL ERROR] fluent set by ref not implemented\n");
			exit(-1);
		}
		for(k = 0; k < current_set->count; k++){
			for(j = 0; j < current_set->labels_count[k]; j++){
				if(current_set->labels[k][j]->is_set){
					if(current_set->labels[k][j]->set->is_ident){
						printf("[FATAL ERROR] fluent set by ref not implemented\n");exit(-1);
					}
					sig_events	= calloc(current_set->labels[k][j]->set->labels_count[0], sizeof(automaton_signal_event*));
					for(l = 0; l < current_set->labels[k][j]->set->labels_count[0]; l++){
						sig_events[j] = automaton_signal_event_create(current_set->labels[k][j]->set->labels[0][l]->string_terminal, INPUT_SIG);
					}
					automaton_fluent_add_starting_signals(fluent, global_alphabet, current_set->labels[k][j]->set->labels_count[0], sig_events);
					for(l = 0; l < current_set->labels[k][j]->set->labels_count[0]; l++){
						automaton_signal_event_destroy(sig_events[l], true);
					}
					free(sig_events);
				}else{
					current_event = automaton_signal_event_create(current_set->labels[k][j]->string_terminal, INPUT_SIG);
					automaton_fluent_add_starting_signals(fluent, global_alphabet, 1, &current_event);
					automaton_signal_event_destroy(current_event, true);
				}
			}
		}
	}
	for(i = 0; i < fluent_def_syntax->finishing_set->count; i++){
		if(fluent_def_syntax->finishing_set->labels[i][0]->is_set){
			current_set = fluent_def_syntax->finishing_set->labels[i][0]->set;
		}else{
			current_set	= fluent_def_syntax->finishing_set;
		}
		if(current_set->is_ident){
			//TODO: implement set by ref in fluents
			printf("[FATAL ERROR] fluent set by ref not implemented\n");
			exit(-1);
		}
		for(k = 0; k < current_set->count; k++){
			for(j = 0; j < current_set->labels_count[k]; j++){
				if(current_set->labels[k][j]->is_set){
					if(current_set->labels[k][j]->set->is_ident){
						printf("[FATAL ERROR] fluent set by ref not implemented\n");exit(-1);
					}
					sig_events	= calloc(current_set->labels[k][j]->set->labels_count[0], sizeof(automaton_signal_event*));
					for(l = 0; l < current_set->labels[k][j]->set->labels_count[0]; l++){
						sig_events[j] = automaton_signal_event_create(current_set->labels[k][j]->set->labels[0][l]->string_terminal, INPUT_SIG);
					}
					automaton_fluent_add_ending_signals(fluent, global_alphabet, current_set->labels[k][j]->set->labels_count[0], sig_events);
					for(l = 0; l < current_set->labels[k][j]->set->labels_count[0]; l++){
						automaton_signal_event_destroy(sig_events[l], true);
					}
					free(sig_events);
				}else{
					current_event = automaton_signal_event_create(current_set->labels[k][j]->string_terminal, INPUT_SIG);
					automaton_fluent_add_ending_signals(fluent, global_alphabet, 1, &current_event);
					automaton_signal_event_destroy(current_event, true);
				}
			}
		}
	}
	fluent->initial_valuation	= fluent_def_syntax->initial_value != 0;
	return fluent;
}


automaton_automata_context* automaton_automata_context_create_from_syntax(automaton_program_syntax* program, char* ctx_name,
		char* test_name, diagnosis_search_method is_diagnosis, char *results_filename, char* steps_filename, bool append_result){
	automaton_parsing_tables* tables	= automaton_parsing_tables_create();
	automaton_automata_context* ctx		= malloc(sizeof(automaton_automata_context));
	char buf[250];
	FILE *experimental_results			= NULL;
	struct timeval tval_before = {0,0}, tval_after = {0,0}, tval_ltl_model_build_result = {0,0},
		tval_model_build_result = {0,0}, tval_composition_result = {0,0}, tval_synthesis_result = {0,0},
		tval_minimization_result = {0,0};
	uint32_t results_minimization_steps = 0, results_alphabet_size = 0, results_guarantees_count = 0,
		results_assumptions_count = 0, results_plant_states = 0, results_plant_transitions = 0,
		results_minimization_states = 0, results_minimization_transitions = 0,
		results_plant_controllable_transitions = 0, results_minimization_controllable_transitions = 0;


#if VERBOSE
	printf("Creating automaton %s\n", ctx_name);
#endif
	//build look up tables
	uint32_t i;
	int32_t j;
	for(i = 0; i < program->count; i++){
		automaton_statement_syntax_to_table(program->statements[i], tables);
	}
	//build constants and ranges
	for(i = 0; i < program->count; i++){
		if(program->statements[i]->type == CONST_AUT)
			automaton_statement_syntax_to_constant(program->statements[i]->const_def, tables);
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
	//get set of state fluents
	automaton_vstates_fluent_syntax *vstates_fluent	= NULL;
	uint32_t vstates_fluent_count	= 0;
	char** vstates_fluent_names		= NULL;
	bool fluent_found	= false;
	for(i = 0; i < program->count; i++){
		if(program->statements[i]->type == VSTATES_FLUENT_AUT){
			if(vstates_fluent_names	== NULL){
				vstates_fluent_count++;
				vstates_fluent_names	= calloc(1, sizeof(char*));
				aut_dupstr(&(vstates_fluent_names[0]), program->statements[i]->vstates_syntax->name);
			}else{
				fluent_found	= false;
				for(j = 0; j < vstates_fluent_count; j++){
					if(strcmp(vstates_fluent_names[j], program->statements[i]->vstates_syntax->name) == 0){
						fluent_found	= true;
						break;
					}
				}
				if(!fluent_found){
					vstates_fluent_count++;
					char** ptr = realloc(vstates_fluent_names, vstates_fluent_count, sizeof(char*));
					if(ptr == NULL){
						printf("Could not allocate memory for vstates_fluent_names\n");
						exit(-1);
					}
					vstates_fluent_names	= ptr;
					aut_dupstr(&(vstates_fluent_names[vstates_fluent_count - 1]), program->statements[i]->vstates_syntax->name);

				}
			}
		}
	}


	for(i = 0; i < program->count; i++){
		if(program->statements[i]->type == VSTATES_FLUENT_AUT){
			aut_dupstr(&(vstates_fluent_names[i]), program->statements[i]->vstates_syntax->name);

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
						printf("Could not allocate memory[automaton_automata_context_create_from_syntax:1]\n");
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
	automaton_automata_context_initialize(ctx, ctx_name, global_alphabet, fluent_count, fluents, liveness_formulas_count, liveness_formulas,
			vstates_fluent_count, vstates_fluent_names, liveness_formulas_names);
	for(i = 0; i < vstates_fluent_count; i++)free(vstates_fluent_names[i]);
	free(vstates_fluent_names);
	free(fluents);
	automaton_alphabet_destroy(global_alphabet);
#if VERBOSE
	printf("Building LTL automata\n");
	fflush(stdout);
#endif
	gettimeofday(&tval_before, NULL);
	//build automata from ltl
	automaton_automaton* obdd_automaton;
	for(i = 0; i < ltl_automata_count; i++){
		obdd_automaton	= automaton_build_automaton_from_obdd(ctx, ltl_automata_names[i], env_theta_obdd[i], env_theta_count[i], sys_theta_obdd[i], sys_theta_count[i],
				env_rho_obdd[i], env_rho_count[i], sys_rho_obdd[i], sys_rho_count[i], tables);
#if VERBOSE
		printf(".");
#endif
#if DEBUG_OBDD_DEADLOCK
		automaton_automaton_print_traces_to_deadlock(obdd_automaton, DEADLOCK_TRACE_COUNT);
#endif
		fflush(stdout);
	}



	for(i = 0; i < ltl_automata_count; i++){
		free(sys_theta_obdd[i]);		free(env_theta_obdd[i]);
		free(sys_rho_obdd[i]);			free(env_rho_obdd[i]);
	}
	free(sys_theta_obdd);		free(env_theta_obdd);
	free(sys_rho_obdd);			free(env_rho_obdd);
	free(sys_theta_count);		free(env_theta_count);
	free(sys_rho_count);		free(env_rho_count);
	free(ltl_automata_names);
	gettimeofday(&tval_after, NULL);
	timersub(&tval_after, &tval_before, &tval_ltl_model_build_result);

#if VERBOSE
	printf("\nBuilding LTS automata\n");
	fflush(stdout);
#endif
	gettimeofday(&tval_before, NULL);
	int32_t main_index;
	//import automata
	for(i = 0; i < program->count; i++)
		if(program->statements[i]->type == IMPORT_AUT){
			automaton_automaton *loaded_automaton	= automaton_automaton_load_report(ctx, program->statements[i]->import_def->filename);
			if(loaded_automaton->name != NULL)
				free(loaded_automaton->name);
			loaded_automaton->name	= NULL;
			aut_dupstr(&(loaded_automaton->name), program->statements[i]->import_def->name);
			main_index = automaton_parsing_tables_add_entry(tables, COMPOSITION_ENTRY_AUT, loaded_automaton->name,
					loaded_automaton);

			tables->composition_entries[main_index]->solved	= true;
			tables->composition_entries[main_index]->valuation_count			= 1;
			tables->composition_entries[main_index]->valuation.automaton_value	= loaded_automaton;
		}

	//build automata
	bool pending_statements	= true;
	uint32_t current_count = 0;
	while(pending_statements && current_count < program->count){
		pending_statements	= false;
		for(i = 0; i < program->count; i++){
			if(program->statements[i]->type == COMPOSITION_AUT){
				pending_statements = automaton_statement_syntax_to_automaton(ctx, program->statements[i]->composition_def, tables) || pending_statements;
#if VERBOSE
				printf(".");
				fflush(stdout);
#endif
			}
		}
		current_count++;
	}
	gettimeofday(&tval_after, NULL);
	timersub(&tval_after, &tval_before, &tval_model_build_result);
	//compute gr1 games
	gettimeofday(&tval_before, NULL);
	automaton_gr1_game_syntax* gr1_game;
	automaton_automaton *game_automaton, *winning_region_automaton, *dd_automaton;
	char **assumptions, **guarantees;
	char set_name[255];
	int32_t assumptions_count = 0, guarantees_count = 0;
	bool nonreal	= false;
	uint32_t minimization_steps	= 0;
#if VERBOSE
	printf("\nSolving GR1\n");
	fflush(stdout);
#endif
	uint32_t steps_size	= 32;
	uint32_t steps	= 0;
	uint32_t *steps_sizes = calloc(steps_size, sizeof(uint32_t));
	struct timeval *steps_times	= calloc(steps_size, sizeof(struct timeval));
	for(i = 0; i < program->count; i++){
		assumptions_count = 0; guarantees_count = 0;
		minimization_steps = 0;
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
			current_fluent.ending_signals_element_count	= NULL; current_fluent.starting_signals_element_count = NULL;
			current_fluent.starting_signals_count	= 0; current_fluent.starting_signals	= NULL;
			//TODO:this needs to be done when building the game, not afterwards, when liveness data is lost, ends with restoration involving
			//was_merged and old values
			if(ctx->liveness_valuations_count > 0){
				was_merged	= true;
				old_inverted_valuations	= game_automaton->inverted_valuations;
				old_valuations			= game_automaton->valuations;
				old_valuations_size		= game_automaton->valuations_size;
				old_fluents_count		= ctx->global_fluents_count;
				old_fluents				= ctx->global_fluents;

				ctx->global_fluents_count	+= ctx->liveness_valuations_count;
				ctx->global_fluents		= calloc(ctx->global_fluents_count, sizeof(automaton_fluent));
				for(j = 0; j < old_fluents_count; j++)
					automaton_fluent_copy(&(old_fluents[j]), &(ctx->global_fluents[j]));
				for(j = old_fluents_count; j < ctx->global_fluents_count; j++){
					current_fluent.name	= liveness_formulas_names[j - old_fluents_count];
					automaton_fluent_copy(&current_fluent, &(ctx->global_fluents[j]));
				}
				uint32_t new_size					= GET_FLUENTS_ARR_SIZE(ctx->global_fluents_count, game_automaton->transitions_size);
				game_automaton->valuations_size			= new_size;
				game_automaton->valuations 				= calloc(new_size,  sizeof(uint32_t));
				game_automaton->inverted_valuations		= malloc(sizeof(automaton_bucket_list*) * ctx->global_fluents_count);
				for(j = 0; j < (int32_t)old_fluents_count; j++){
					game_automaton->inverted_valuations[j]	= old_inverted_valuations[j];
				}
				for(j = (int32_t)old_fluents_count; j < (int32_t)ctx->global_fluents_count; j++){
					game_automaton->inverted_valuations[j]	= game_automaton->liveness_inverted_valuations[j - old_fluents_count];
				}
				for(j = 0; j < (int32_t)game_automaton->transitions_count; j++){
					for(k = 0; k < old_fluents_count; k++){
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
#if VERBOSE
			printf("[Synthesizing] %s over %s\n", gr1_game->name, game_automaton->name);
#endif
			winning_region_automaton	= automaton_get_gr1_strategy(game_automaton, assumptions, assumptions_count
					, guarantees, guarantees_count, true);
			results_assumptions_count	= assumptions_count;
			results_guarantees_count	= guarantees_count;
			free(winning_region_automaton->name);
			winning_region_automaton->name	= NULL;
			aut_dupstr(&(winning_region_automaton->name), gr1_game->name);
			nonreal	= false;
			results_plant_states = game_automaton->transitions_count;
			results_plant_transitions	= game_automaton->transitions_composite_count;
			results_plant_controllable_transitions = 0;
			for(j = 0; j < results_plant_states; j++){
				for(k = 0; k < game_automaton->out_degree[j]; k++){
					if( !(TRANSITION_IS_INPUT(&(game_automaton->transitions[j][k]))))
						results_plant_controllable_transitions++;
				}
			}
#if VERBOSE

			/*if(winning_region_automaton->transitions_count == 0){
			printf("[Unreal.] %s over %s is NOT REALIZABLE\n", gr1_game->name, game_automaton->name);
		}else{
			printf("[Real.] %s over %s is REALIZABLE\n", gr1_game->name, game_automaton->name);
		}		*/
			if((winning_region_automaton->transitions_count != 0
					&& winning_region_automaton->out_degree[winning_region_automaton->initial_states[0]] > 0)){
				printf("[Real.] %s over %s is REALIZABLE\n", gr1_game->name, game_automaton->name);
			}else{
				printf("[Unreal.] %s over %s is NOT REALIZABLE\n", gr1_game->name, game_automaton->name);
			}
#endif
			if(winning_region_automaton->transitions_count == 0 && is_diagnosis != 0){
#if VERBOSE
				printf("[Diagnosing] %s over %s\n", gr1_game->name, game_automaton->name);
#endif
				nonreal	= true;
				automaton_automaton_destroy(winning_region_automaton);
				if(is_diagnosis & DD_SEARCH)
					winning_region_automaton = automaton_get_gr1_unrealizable_minimization_dd(game_automaton, assumptions, assumptions_count, guarantees, guarantees_count,
							&steps, &steps_sizes, &steps_times, &steps_size);
				else if(is_diagnosis & LINEAR_SEARCH)
					winning_region_automaton = automaton_get_gr1_unrealizable_minimization(game_automaton, assumptions, assumptions_count, guarantees, guarantees_count,
							&steps, &steps_sizes, &steps_times, &steps_size);

				automaton_automaton_remove_unreachable_states(winning_region_automaton);
			}
			results_minimization_states = winning_region_automaton->transitions_count;
			results_minimization_transitions	= winning_region_automaton->transitions_composite_count;
			results_minimization_controllable_transitions = 0;
			for(j = 0; j < results_minimization_states; j++){
				for(k = 0; k < winning_region_automaton->out_degree[j]; k++){
					if( !(TRANSITION_IS_INPUT(&(winning_region_automaton->transitions[j][k]))))
						results_minimization_controllable_transitions++;
				}
			}
			main_index = automaton_parsing_tables_add_entry(tables, COMPOSITION_ENTRY_AUT, gr1_game->name, winning_region_automaton);

			uint32_t namelen	= strlen(gr1_game->name) + 12;
			char *name = calloc(namelen, sizeof(char));
			snprintf(name, namelen, "%s_%s", gr1_game->name, nonreal? "diag" : "strat");
			free(winning_region_automaton->name);
			winning_region_automaton->name	= name;
			tables->composition_entries[main_index]->solved	= true;
			tables->composition_entries[main_index]->valuation_count			= 1;
			tables->composition_entries[main_index]->valuation.automaton_value	= winning_region_automaton;

			if(nonreal){
				//clear everything game related from diagnosis
				for(j = 0; j < winning_region_automaton->context->global_fluents_count; j++)
					automaton_bucket_destroy(winning_region_automaton->inverted_valuations[j]);
				if(winning_region_automaton->valuations_size > 0){
					free(winning_region_automaton->inverted_valuations);winning_region_automaton->inverted_valuations = NULL;
					free(winning_region_automaton->valuations);winning_region_automaton->valuations = NULL;
					winning_region_automaton->valuations_size = 0;
				}
				if(winning_region_automaton->liveness_valuations_size > 0){
					for(j = 0; j < winning_region_automaton->context->liveness_valuations_count; j++)
						automaton_bucket_destroy(winning_region_automaton->liveness_inverted_valuations[j]);
					free(winning_region_automaton->liveness_inverted_valuations); winning_region_automaton->liveness_inverted_valuations = NULL;
					free(winning_region_automaton->liveness_valuations); winning_region_automaton->liveness_valuations = NULL;
					winning_region_automaton->liveness_valuations_size = 0;
				}
				winning_region_automaton->is_game = false;
			}

			//restore old liveness valuations and old context
			if(was_merged){
				for(j = 0; j < ctx->global_fluents_count; j++)
					automaton_fluent_destroy(&(ctx->global_fluents[j]), false);
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
			for(j = 0; j < assumptions_count; j++)
				free(assumptions[j]);
			free(assumptions);
			for(j = 0; j < guarantees_count; j++)
				free(guarantees[j]);
			free(guarantees);
#if VERBOSE
			printf(".");
#endif
			fflush(stdout);
		}
	}
	gettimeofday(&tval_after, NULL);
	if(nonreal){
		timersub(&tval_after, &tval_before, &tval_minimization_result);
	}else{
		timersub(&tval_after, &tval_before, &tval_synthesis_result);
	}
	//build pending automat if needed for compositions needing gr1 solving
	//build automata
	pending_statements	= true;
	while(pending_statements && current_count < program->count){
		pending_statements	= false;
		for(i = 0; i < program->count; i++){
			if(program->statements[i]->type == COMPOSITION_AUT){
				pending_statements = automaton_statement_syntax_to_automaton(ctx, program->statements[i]->composition_def, tables) || pending_statements;
#if VERBOSE
				printf(".");
				fflush(stdout);
#endif
			}
		}
		current_count++;
	}
	//run equivalence checks
#if VERBOSE
	printf("==========\n TESTS \n==========\n");
#endif
	automaton_equivalence_check_syntax* equiv_check; int32_t left_index, right_index; automaton_automaton *left_automaton, * right_automaton;
	for(i = 0; i < program->count; i++){
		if(program->statements[i]->type == EQUIV_CHECK_AUT){
			equiv_check		= program->statements[i]->equivalence_check;
			left_index		= automaton_parsing_tables_get_entry_index(tables, COMPOSITION_ENTRY_AUT, equiv_check->left);
			right_index		= automaton_parsing_tables_get_entry_index(tables, COMPOSITION_ENTRY_AUT, equiv_check->right);
			if(left_index < 0 || right_index < 0){
				printf("Incorrect components for check %s (%s == %s)\n", equiv_check->name, equiv_check->left, equiv_check->right);
				continue;
			}
			left_automaton	= tables->composition_entries[left_index]->valuation.automaton_value;
			right_automaton	= tables->composition_entries[right_index]->valuation.automaton_value;
			main_index		= automaton_parsing_tables_get_entry_index(tables, EQUIVALENCE_CHECK_ENTRY_AUT, equiv_check->name);
			if(main_index < 0){
				printf("Incorrect name for check %s (%s == %s)\n", equiv_check->name, equiv_check->left, equiv_check->right);
				continue;
			}
			bool are_equivalent	= automaton_automata_are_equivalent(left_automaton, right_automaton);
			if((strstr(equiv_check->name, "FATAL") != NULL && !are_equivalent)
					|| (strstr(equiv_check->name, "FAIL") != NULL && are_equivalent)){
				printf(ANSI_COLOR_RED);
				printf("X");
				printf(ANSI_COLOR_RESET);
			}else{
				printf(ANSI_COLOR_GREEN);
				printf("■");
				printf(ANSI_COLOR_RESET);
			}
			printf("\t [%s] \t (%s==%s)\n", equiv_check->name, equiv_check->left, equiv_check->right
					, are_equivalent? "true" : "false");
			tables->equivalence_entries[main_index]->valuation.bool_value = are_equivalent;
			fflush(stdout);
		}
	}
	//export automata
	char fsp_name[1024];
	for(j = 0; j < program->count; j++){
		if(program->statements[j]->type == EXPORT_AUT){
			for(i = 0; i < tables->composition_count; i++){
				if(tables->composition_entries[i]->solved &&
						strncmp(program->statements[j]->import_def->name,
								tables->composition_entries[i]->valuation.automaton_value->name,
								strlen(program->statements[j]->import_def->name))== 0){
						automaton_automaton_print_report(tables->composition_entries[i]->valuation.automaton_value,
								program->statements[j]->import_def->filename);
						sprintf(fsp_name, "%s.fsp", program->statements[j]->import_def->filename);
						automaton_automaton_print_fsp(tables->composition_entries[i]->valuation.automaton_value,
								fsp_name);
				}
			}
		}
	}

	//PRINT RESULTS
	if(is_diagnosis != 0){
		uint32_t target_length	= strlen(results_filename);
		char *target_name	= malloc(sizeof(char) * (target_length + 255));
		sprintf(target_name, "%s.csv", results_filename);
		experimental_results = fopen(target_name, append_result?"a": "w");
		if (experimental_results != NULL){
			if(!append_result)
				fprintf(experimental_results, "name,realizable,ltl_model_build_time,model_build_time,composition_time," \
						"synthesis_time,diagnosis_time,alphabet_size,guarantees_count," \
						"assumptions_count,plant_states,plant_transitions,plant_controllable_transitions,minimization_states," \
						"minimization_transitions,minimization_controllable_transitions,search_method," \
						"diagnosis_steps\n");
			fprintf(experimental_results, "%s,%s,%ld.%06ld,%ld.%06ld,%ld.%06ld," \
					"%ld.%06ld,%ld.%06ld,%d,%d," \
					"%d,%d,%d,%d,%d," \
					"%d,%d,%s," \
					"%d\n",
					test_name, nonreal? "false":"true", tval_ltl_model_build_result.tv_sec, tval_ltl_model_build_result.tv_usec,
							tval_model_build_result.tv_sec, tval_model_build_result.tv_usec,
							tval_composition_result.tv_sec, tval_composition_result.tv_usec,
							tval_synthesis_result.tv_sec, tval_synthesis_result.tv_usec,
							tval_minimization_result.tv_sec, tval_minimization_result.tv_usec,
							ctx->global_alphabet->count, results_guarantees_count, results_assumptions_count,
							results_plant_states, results_plant_transitions, results_plant_controllable_transitions,
							results_minimization_states, results_minimization_transitions, results_minimization_controllable_transitions,
							is_diagnosis & DD_SEARCH ? "DD" : "linear",
							steps);
			sprintf(target_name, "%s.csv", steps_filename);
			FILE *experimental_steps_results = fopen(target_name, "w");
			if (experimental_steps_results != NULL){
				fprintf(experimental_steps_results, "step,size,time\n");
				for(i = 0; i < steps; i++){
					fprintf(experimental_steps_results, "%d,%d,%ld.%06ld\n", i, steps_sizes[i], steps_times[i].tv_sec, steps_times[i].tv_usec);
				}
				fclose(experimental_steps_results);
			}
			fclose(experimental_results);

		}
		free(target_name);
	}
	free(steps_times); free(steps_sizes);
	free(liveness_formulas); free(liveness_formulas_names);
	automaton_parsing_tables_destroy(tables);

	if(parser_primed_variables != NULL){
		free(parser_primed_variables);
		parser_primed_variables = NULL;
		free(parser_primed_original_variables);
		parser_primed_original_variables = NULL;
	}
#if VERBOSE
	printf("\nDONE\n");
#endif
	return ctx;
}




