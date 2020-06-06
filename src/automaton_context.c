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
	char** ret_value			= NULL;
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
			aut_merge_string_lists(&ret_value, count, tmp_value, inner_count, true, false);
			for(k = 0; k < (uint32_t)inner_count;k++)free(tmp_value[k]);
			for(k = 0; k < (uint32_t)inner_count;k++)free(values[k]);
			free(tmp_value);
			free(values);
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
			aut_push_string_to_list(&labels_list, &labels_list_count, label_indexes, &label_position, true, false);
#if DEBUG_PARSE_STATES
		printf("%s ",labels_list[label_position]);
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
				automaton_indexes_syntax_eval_strings(tables,explicit_start_valuation
						,&explicit_start_valuations, &explicit_start_state_count, &explicit_start_state_size, &indexes_values, &ret_value, &count, state->label->indexes);
				for(j = 0; j < count; j++){free(ret_value[j]);free(indexes_values[j]);}
				free(ret_value); ret_value = NULL;
				free(indexes_values); indexes_values = NULL;

				explicit_start_states	= calloc(explicit_start_state_count, sizeof(uint32_t));
				for(j = 0; j < count; j++){
					automaton_indexes_valuation_set_from_label(tables, explicit_start_valuations[j], state->label->indexes, state->label->name, label_indexes);
					aut_push_string_to_list(&labels_list, &labels_list_count, label_indexes, &label_position, false, false);
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
				aut_push_string_to_list(&labels_list, &labels_list_count, state->label->name, &label_position, false, false);
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
						if(transition->condition != NULL){
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
												printf("\t\t[v] valuation %d of %d:", next_valuations_count - count + n, next_valuations_count);
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
												aut_push_string_to_list(&labels_list, &labels_list_count, label_indexes, &label_position, false, false);

												to_state	= (uint32_t)label_position;
#if DEBUG_PARSE_STATES
												printf("\t\t[<] to state reassigned as: %s(%d)\n", labels_list[label_position], label_position);
#endif
											}else{
												added_state++;
												snprintf(added_state_string, sizeof(added_state_string), "S___-%d", added_state);
												aut_push_string_to_list(&labels_list, &labels_list_count, added_state_string, &label_position, false, false);
												to_state	= (uint32_t)label_position;
#if DEBUG_PARSE_STATES
												printf("\t\t[<] to state reassigned as: %s(%d)\n", labels_list[label_position], to_state);
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
											printf("\t\t[v] valuation %d of %d:", next_valuations_count - 1, next_valuations_count);
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
											aut_push_string_to_list(&labels_list, &labels_list_count, added_state_string, &label_position, false, false);
											to_state	= (uint32_t)label_position;
#if DEBUG_PARSE_STATES
											printf("\t\t[<] to state reassigned as: %s(%d)\n", labels_list[label_position], to_state);
#endif
										}else{
											automaton_indexes_valuation_set_to_label(tables, current_valuations_count > 0 ? current_valuations[current_valuations_count -current_from_state_count + r]: NULL, next_valuations_count > 0 ? next_valuations[next_valuations_count - 1]: NULL
													, transition->to_state->indexes,transition->to_state->name, label_indexes);
											aut_push_string_to_list(&labels_list, &labels_list_count, label_indexes, &label_position, false, false);
											to_state	= (uint32_t)label_position;
#if DEBUG_PARSE_STATES
											printf("\t\t[<] to state reassigned as: %s(%d)\n", labels_list[label_position], label_position);
#endif
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
										next_from_state[next_from_state_count++]	= to_state;
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
										aut_push_string_to_list(&labels_list, &labels_list_count, label_indexes, &label_position, false, false);

										to_state	= (uint32_t)label_position;
#if DEBUG_PARSE_STATES
										printf("\t\t[<] to state reassigned as: %s(%d)\n", labels_list[label_position], label_position);
#endif
									}else{
										added_state++;
										snprintf(added_state_string, sizeof(added_state_string), "S___-%d", added_state);
										aut_push_string_to_list(&labels_list, &labels_list_count, added_state_string, &label_position, false, false);
										to_state	= (uint32_t)label_position;
#if DEBUG_PARSE_STATES
										printf("\t\t[<] to state reassigned as: %s(%d)\n", labels_list[label_position], to_state);
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
									next_from_state[next_from_state_count++]	= to_state;

								}
							}

						}
						for(s= 0; s < (int32_t)automaton_transition_count; s++){
							automaton_automaton_add_transition(automaton, current_automaton_transition[s]);
							automaton_transition_destroy(current_automaton_transition[s], true);
						}
						automaton_transition_count	= 0;
					}
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
						printf("\t\t\t[N] %d:", s);
						automaton_indexes_valuation_print(next_valuations[s], " ", " ");
					}
					printf("\t\t[<] to state reassigned as: %s(%d)\n", labels_list[label_position], to_state);
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
				}
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
			}
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
		for(i = 0; i < labels_list_count; i++)
			free(labels_list[i]);
		free(labels_list);
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
	valuation->current_values	= calloc(valuation->count, sizeof(uint32_t));
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
	if((next == NULL && current == NULL) || to_indexes == NULL){
		strcpy(target, label);
		return;
	}
	uint32_t i, j;
	automaton_indexes_valuation *valuation = automaton_indexes_valuation_merge(current, next);
	sprintf(target, "%s", label);
	char name[40];
	char *from_ident, *to_ident;
	if((next != NULL || current != NULL) && to_indexes != NULL){
		for(j = 0; j < to_indexes->count; j++){
			if(to_indexes->indexes[j]->is_expr && (to_indexes->indexes[j]->expr->type == UPPER_IDENT_TERMINAL_TYPE_AUT ||
					to_indexes->indexes[j]->expr->type == IDENT_TERMINAL_TYPE_AUT)){
				snprintf(target + strlen(target), sizeof(target), "_%d", automaton_indexes_valuation_get_value(valuation, to_indexes->indexes[j]->expr->string_terminal));
			}else if(to_indexes->indexes[j]->is_expr){
				snprintf(target + strlen(target), sizeof(target), "_%d", automaton_expression_syntax_evaluate(tables, to_indexes->indexes[j]->expr, valuation));
			}else{
				to_ident	= to_indexes->indexes[j]->lower_ident;
				snprintf(target + strlen(target), sizeof(target), "_%s", to_ident);
			}
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
	char** ret_value			= NULL;
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
	if(atom_valuation != NULL)automaton_indexes_valuation_destroy(atom_valuation);
	atom_valuation = NULL;

	(*next_valuations_count) += total_combinations;


	for(i = 0; i < (uint32_t)*a_count; i++){
		free((*a)[i]);
	}
	if(*a != NULL)
		free(*a);
	*a_count	= inner_count;
	(*a)		= ret_value;
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
void automaton_set_composed_valuation(bool* valuation, bool* partial_valuation, uint32_t valuation_offset, bool is_initial, bool is_input
		, uint32_t x_count, uint32_t y_count){
	uint32_t i;

	uint32_t offset_size	= (is_initial) ? (is_input? x_count : x_count + y_count) : (is_input? x_count : y_count);
	uint32_t left_offset	= is_initial? 0 : (is_input? x_count + y_count: x_count);
	for(i = 0; i < offset_size; i++)valuation[left_offset + i]	= partial_valuation[offset_size * valuation_offset + i];
}

uint64_t __global_signals_count = 0;
uint32_t __global_max_signals	= 0;
bool automaton_add_transition_from_valuations(obdd_mgr* mgr, automaton_automaton* automaton, uint32_t from_state, uint32_t to_state, bool* from_valuation,
		bool* to_valuation, bool* adjusted_valuation, bool is_initial, bool is_input, uint32_t x_count, uint32_t y_count, uint32_t* obdd_on_indexes, uint32_t* obdd_off_indexes,
		uint32_t* x_y_alphabet, uint32_t* x_y_x_p_alphabet, uint32_t* x_y_order){
	uint32_t i, j, fluent_index, fluent_count	= automaton->context->liveness_valuations_count;
	automaton_transition* transition		= automaton_transition_create(from_state, to_state);
	//TODO: optimize alphabet indexes computation
#if DEBUG_LTL_AUTOMATON
	uint32_t to_state_size	= is_input? (is_initial? x_count : x_count * 2 + y_count) : (x_count + y_count);
	uint32_t from_state_size	= !is_input? (is_initial? x_count+y_count : x_count * 2 + y_count) : (is_initial? x_count : x_count + y_count);
	printf("(");
	for(i = 0; i < from_state_size; i++)
		printf("%s", from_valuation[i]?"1":"0");
	printf("):%d-[", from_state);
#endif
	uint32_t signal_dict_index;
	uint32_t var_count = is_input ? x_count : y_count;
	uint32_t left_offset	= is_input ? 0 : x_count;
	uint32_t right_offset	= is_input ? (is_initial? 0 : x_count + y_count) : x_count;
	bool take_on;
#if DEBUG_LTL_AUTOMATON
	bool has_action = false;
#endif
	for(i = 0; i < var_count; i++){
		if(!is_initial){
			if(from_valuation[left_offset + i] != to_valuation[right_offset + i]){
#if DEBUG_LTL_AUTOMATON
				has_action 	= true;
#endif
				take_on				= from_valuation[left_offset + i]; //is_input? from_valuation[left_offset + i] : !from_valuation[left_offset + i];
				signal_dict_index	= (take_on? obdd_off_indexes[left_offset + i]: obdd_on_indexes[left_offset + i]);
				automaton_transition_add_signal_event_ID(transition, automaton->context, signal_dict_index, INPUT_SIG);
#if DEBUG_LTL_AUTOMATON
				printf("(%d:%s:%s)%s", signal_dict_index, automaton->context->global_alphabet->list[signal_dict_index].name,
						take_on? "OFF" : "ON", i < var_count - 1 ? "," : "");
#endif
			}
		}else{
#if DEBUG_LTL_AUTOMATON
			has_action 	= true;
#endif
			take_on		= to_valuation[right_offset + i];
			if(take_on){
				signal_dict_index	= (take_on? obdd_on_indexes[right_offset + i]: obdd_off_indexes[right_offset + i]);
				automaton_transition_add_signal_event_ID(transition, automaton->context, signal_dict_index, INPUT_SIG);
#if DEBUG_LTL_AUTOMATON
				printf("(%d:%s:%s)%s", signal_dict_index, automaton->context->global_alphabet->list[signal_dict_index].name,
						take_on? "ON" : "OFF", i < var_count - 1 ? "," : "");
#endif
			}

		}
	}
#if DEBUG_LTL_AUTOMATON
	printf("%s]->(", !has_action? "__tau__" : "");
	for(i = 0; i < to_state_size; i++)
		printf("%s", to_valuation[i]?"1":"0");
	printf(")%d",to_state);
#endif
	if(is_input){
		TRANSITION_SET_INPUT(transition);
	}else{
		TRANSITION_CLEAR_INPUT(transition);
	}
	bool has_transition = automaton_automaton_has_transition(automaton, transition);
	if(!has_transition){
		automaton_automaton_add_transition(automaton, transition);
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
		printf("% V:");
#endif
		for(i = 0; i < fluent_count; i++){
			fluent_index	= GET_STATE_FLUENT_INDEX(fluent_count, to_state, i);
			//evaluate each liveness formula on landing state to check if formula is satisfied and then set bit
			if(!is_initial){
				if(is_input){
					//TODO:
					//restrict x according to x' in to_valuation
					for(j = 0; j < x_count; j++)
						adjusted_valuation[j]	= to_valuation[x_count + y_count + j];
					for(j = 0; j < y_count; j++)
						adjusted_valuation[x_count + j]	= from_valuation[x_count + j];
					obdd_sat_vector	= obdd_satisfies_vector(automaton->context->liveness_valuations[i],
							x_y_alphabet, adjusted_valuation, x_count + y_count, x_y_order);

							//x_y_x_p_alphabet, to_valuation, x_count * 2 + y_count);
				}else{
					obdd_sat_vector	= obdd_satisfies_vector(automaton->context->liveness_valuations[i],
							x_y_alphabet, to_valuation, x_count + y_count, x_y_order);
				}
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
		obdd_composite_state* env_state, obdd_composite_state* sys_state, obdd_state_tree* obdd_state_map,
		uint32_t x_count, uint32_t y_count, uint32_t x_y_count, uint32_t x_y_x_p_count,
		uint32_t* x_y_alphabet, uint32_t* x_y_x_p_alphabet, uint32_t* x_y_order,
		uint32_t signals_count, bool* valuation, bool* hashed_valuation, bool* adjusted_valuation,
		automaton_bool_array_hash_table* x_y_hash_table, automaton_bool_array_hash_table* x_y_x_p_hash_table,
		uint32_t* obdd_on_signals_indexes, uint32_t* obdd_off_signals_indexes){
	uint32_t j;
	bool has_transition;
	//xy ->xy'
	//automaton_set_composed_valuation(env_state->valuation, valuations, i, false, true, x_y_x_p_alphabet, x_count, y_count);
	for(j = 0; j < x_y_count; j++)sys_state->valuation[j]	= valuation[j];
	for(j = 0; j < x_y_x_p_count; j++)env_state->valuation[j]	= valuation[j];
#if DEBUG_LTL_AUTOMATON
	printf("sys val.:");
	for(j = 0; j < x_y_count; j++)printf("%d\t",sys_state->valuation[j]);
	printf("\tenv val.:");
	for(j = 0; j < x_y_x_p_count; j++)printf("%d\t",env_state->valuation[j]);
	printf("\n");
#endif
	hashed_valuation		= automaton_bool_array_hash_table_add_or_get_entry(x_y_x_p_hash_table, env_state->valuation, true);
	env_state->state		= obdd_state_tree_get_key(obdd_state_map, env_state->valuation, x_y_x_p_count);
	sys_state->state		= obdd_state_tree_get_key(obdd_state_map, sys_state->valuation, x_y_count);
	for(j = 0; j < x_y_count; j++)adjusted_valuation[j] = 0;
	for(j = 0; j < x_count; j++)adjusted_valuation[j] = hashed_valuation[x_y_count + j];
#if DEBUG_LTL_AUTOMATON
	printf("Adjusted valuation:[");
	for(j = 0; j < x_y_count; j++)
		printf("%s", adjusted_valuation[j] ? "1" : "0");
	printf("]\n");
#endif
	has_transition	= automaton_add_transition_from_valuations(mgr, ltl_automaton, sys_state->state, env_state->state
			, env_state->valuation, hashed_valuation, adjusted_valuation, false, true
			, x_count, y_count
			, obdd_on_signals_indexes, obdd_off_signals_indexes, x_y_alphabet, x_y_x_p_alphabet
			, x_y_order);


#if DEBUG_LTL_AUTOMATON
	printf("(%d-[", sys_state->state);
	for(j = 0; j < x_y_x_p_count; j++)
		printf("%s", env_state->valuation[j] ? "1" : "0");
	printf("]->%d)%s", env_state->state, has_transition? "" : "*");
#endif
	if(!has_transition){
#if DEBUG_LTL_AUTOMATON
	printf("[ADDED]");
#endif
	}
	//xyx'->x'y'
	//automaton_set_composed_valuation(sys_state->valuation, valuations, i, false, false, signals_alphabet, x_count, y_count);
	//move Y'->XY
	for(j = 0; j < x_y_count; j++)sys_state->valuation[j]	= valuation[x_y_count  + j];
#if DEBUG_LTL_AUTOMATON
	printf("env val.:");
	for(j = 0; j < x_y_x_p_count; j++)printf("%d\t",env_state->valuation[j]);

	printf("\t sys val.:");
	for(j = 0; j < x_y_count; j++)printf("%d\t",sys_state->valuation[j]);
	printf("\n");
#endif
	//move X'->X
	//for(j =0; j < x_count; j++)env_state->valuation[j] = env_state->valuation[x_y_count + j];
	hashed_valuation		= automaton_bool_array_hash_table_add_or_get_entry(x_y_hash_table, sys_state->valuation, true);
	//before getting state for sys_valuation copy X' part form env state into X part of sys state
	//for(j = 0; j < x_count; j++)sys_state->valuation[j]	= env_state->valuation[x_y_count + j];
	sys_state->state		= obdd_state_tree_get_key(obdd_state_map, sys_state->valuation, x_y_count);
	has_transition	= automaton_add_transition_from_valuations(mgr, ltl_automaton, env_state->state, sys_state->state
			, env_state->valuation, hashed_valuation, adjusted_valuation, false, false
			, x_count, y_count
			, obdd_on_signals_indexes, obdd_off_signals_indexes, x_y_alphabet, x_y_x_p_alphabet, x_y_order);
#if DEBUG_LTL_AUTOMATON
	printf("(%d-[", env_state->state);
	for(j = 0; j < x_y_count; j++)
		printf("%s", sys_state->valuation[j] ? "1" : "0");
	printf("]->%d)%s", sys_state->state, has_transition? "" : "*");
#endif

	if(!has_transition){
#if DEBUG_LTL_AUTOMATON
		printf("[ADDED]");
#endif
	}
}

void automaton_add_transitions_from_valuations(obdd_mgr* mgr, obdd* root, automaton_automaton* automaton, uint32_t* valuations_count, uint32_t* valuation_img, uint32_t img_count,
		bool* dont_care_list, bool* partial_valuation, bool* initialized_values, bool* valuation_set, obdd_node** last_nodes,
		obdd_composite_state* env_state, obdd_composite_state* sys_state, obdd_state_tree* obdd_state_map, uint32_t x_count, uint32_t y_count, uint32_t x_y_count, uint32_t x_y_x_p_count,
		uint32_t* x_y_alphabet, uint32_t* x_y_x_p_alphabet, uint32_t* x_y_order, uint32_t signals_count, bool* hashed_valuation, bool* adjusted_valuation, automaton_bool_array_hash_table* x_y_hash_table,
		automaton_bool_array_hash_table* x_y_x_p_hash_table, uint32_t* obdd_on_signals_indexes, uint32_t* obdd_off_signals_indexes){
	int32_t i, j, dont_cares_count = 0, variable_index;
	uint32_t nodes_count;
	bool* valuation	= calloc(img_count, sizeof(bool));
	*valuations_count			= 0;
	uint32_t variables_count	= mgr->vars_dict->size - 2;
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

	//solve case where var_ID is 0 (obdd == true, retrieve all values for img)
	if(current_node->var_ID == mgr->false_obdd->root_obdd->var_ID){
		free(valuation);
		return;
	}
	if(current_node->var_ID == mgr->true_obdd->root_obdd->var_ID){
		dont_cares_count	= 1;
		for(i = 0; i < img_count; i++)
			dont_cares_count *= 2;
		//get new valuations according to dont care list
		uint32_t modulo	= dont_cares_count;

		#if DEBUG_OBDD_VALUATIONS
								printf("Partial Valuation\t<");
						for(i = 0; i < (int32_t)variables_count; i++)
							printf("%s", partial_valuation[i]? "1" : "0");
						printf(">\n");
						printf("Dont care list\t<");
						for(i = 0; i < (int32_t)variables_count; i++){
							variable_index	= -1;
							for(j = 0; j < (int32_t)img_count; j++){
								if(valuation_img[j] == (uint32_t)i + 2){
									variable_index	= valuation_img[j] - 2;
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
			for(i = 0; i < (int32_t)img_count; i++)
				valuation_set[i] = false;
			last_bit_index = -1;
			for(i = 0; i < (int32_t)variables_count; i++){
				variable_index	= -1;
				//only setting odd variables
				for(j = 0; j < (int32_t)img_count && variable_index < 0; j++){
					if(valuation_img[j] == (uint32_t)i + 2){
						variable_index	= valuation_img[j] - 2;
						img_index		= j;
						valuation_set[j]	= true;
					}
				}

				if(variable_index >= 0){
					if(dont_care_list[variable_index]){
						last_bit_index++;
						//set according to modulo division if current position was set to dont care
						valuation[img_index]	= (k & (0x1 << last_bit_index)) != 0;
					}else{
						//set to predefined value for this search
						valuation[img_index]	= partial_valuation[variable_index];
					}
				}
			}
			for(j = 0; j < (int32_t)img_count; j++){
				if(!valuation_set[j]){
					printf("Value not set for %s on valuation %d\n", dictionary_key_for_value(mgr->vars_dict, valuation_img[j]), *valuations_count + k );
					exit(-1);
				}
			}
			automaton_add_transition_from_obdd_valuation(mgr, automaton, env_state, sys_state, obdd_state_map,
					x_count, y_count, x_y_count, x_y_x_p_count,x_y_alphabet, x_y_x_p_alphabet, x_y_order, signals_count, valuation,
					hashed_valuation, adjusted_valuation, x_y_hash_table, x_y_x_p_hash_table, obdd_on_signals_indexes, obdd_off_signals_indexes);
#if DEBUG_OBDD_VALUATIONS
			printf("[");
			for(i = 0; i < (int32_t)img_count; i++){
				bool value = GET_VAR_IN_VALUATION((*valuations), img_count, *valuations_count + k, i);
				bool care_for_value = dont_care_list[valuation_img[i] - 2];
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
				//printf("Should not be exploring this branch\n");exit(-1);
			}else if(current_node->var_ID == mgr->true_obdd->root_obdd->var_ID){//found terminal
				//add valuations
				dont_cares_count	= 1;
				//count dont cares to get number of new valuations (only for variables belonging to the image)
				for(i = 0; i < (int32_t)variables_count; i++){
					if(dont_care_list[i]){
						variable_index	= -1;
						for(j = 0; j < (int32_t)img_count; j++)
							if(valuation_img[j] == (uint32_t)i + 2){
								variable_index	= valuation_img[j] - 2;
								break;
							}
						if(variable_index >= 0)
							dont_cares_count *= 2;
					}
				}

				uint32_t modulo	= dont_cares_count;

				#if DEBUG_OBDD_VALUATIONS
								printf("[T]erminals on node: %d (%d:%s) :\n", last_node_index, last_nodes[current_index]->var_ID,dictionary_key_for_value(mgr->vars_dict,last_nodes[current_index]->var_ID));

								for(i = 0; i <= (int32_t)current_index; i++)
									printf("%s", dont_care_list[i]? "?" : (partial_valuation[i]? "1" : "0"));
								for(i = variables_count - 1; i > last_node_index; i--)
									printf("x");
								printf(">\n");
								printf("Partial Valuation\t<");
								for(i = 0; i < (int32_t)variables_count; i++)
									printf("%s", partial_valuation[i]? "1" : "0");
								printf(">\n");
								printf("Dont care list\t<");
								for(i = 0; i < (int32_t)variables_count; i++){
									variable_index	= -1;
									for(j = 0; j < (int32_t)img_count; j++){
										if(valuation_img[j] == (uint32_t)i + 2){
											variable_index	= valuation_img[j] - 2;
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
					for(i = 0; i < (int32_t)img_count; i++)
						valuation_set[i] = false;
					last_bit_index = -1;
					for(i = 0; i < (int32_t)variables_count; i++){
						variable_index	= -1;
						//only setting odd variables
						for(j = 0; j < (int32_t)img_count && variable_index < 0; j++){
							if(valuation_img[j] == (uint32_t)i + 2){
								variable_index	= valuation_img[j] - 2;
								img_index		= j;
								valuation_set[j]	= true;
							}
						}

						if(variable_index >= 0){
							if(dont_care_list[variable_index]){
								last_bit_index++;
								//set according to modulo division if current position was set to dont care
								valuation[img_index]	= (k & (0x1 << last_bit_index)) != 0;
							}else{
								//set to predefined value for this search
								valuation[img_index]	= partial_valuation[variable_index];
							}
						}
					}
					for(j = 0; j < (int32_t)img_count; j++){
						if(!valuation_set[j]){
							printf("Value not set for %s on valuation %d\n", dictionary_key_for_value(mgr->vars_dict, valuation_img[j]), *valuations_count + k );
							exit(-1);
						}
					}
					automaton_add_transition_from_obdd_valuation(mgr, automaton, env_state, sys_state, obdd_state_map,
							x_count, y_count, x_y_count, x_y_x_p_count,x_y_alphabet, x_y_x_p_alphabet, x_y_order,signals_count, valuation,
							hashed_valuation, adjusted_valuation, x_y_hash_table, x_y_x_p_hash_table,
							obdd_on_signals_indexes, obdd_off_signals_indexes);
#if DEBUG_OBDD_VALUATIONS
					printf("[");
					for(i = 0; i < (int32_t)img_count; i++){
						bool value = GET_VAR_IN_VALUATION((*valuations), img_count, *valuations_count + k, i);
						bool care_for_value = dont_care_list[valuation_img[i] - 2];
						printf("%s", value ? (care_for_value ? "i" : "1") : (care_for_value ? "o" : "0"));
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
	uint32_t local_alphabet_count		= (mgr->vars_dict->size - 2) * 2;
	uint32_t i, j, current_element 		= 0;
	uint32_t* local_alphabet			= malloc(sizeof(uint32_t) * local_alphabet_count);
	char current_dict_entry[255];
	parser_get_primed_variables();

	uint32_t x_count = 0, y_count = 0, x_p_count = 0, y_p_count = 0, x_y_count = 0, x_y_x_p_count = 0, x_p_y_p_count = 0, signals_count = 0, not_x_p_count = 0, not_y_p_count =0;
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

	not_x_p_count = signals_count - x_p_count;
	not_y_p_count = signals_count - y_p_count;
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
	uint32_t* not_x_p_alphabet	= malloc(sizeof(uint32_t) * not_x_p_count);
	uint32_t* not_y_p_alphabet	= malloc(sizeof(uint32_t) * not_y_p_count);
	//set of duplicated alphabets respecting obdd original ordering for usage within the restrict function
	uint32_t* x_alphabet_o	= malloc(sizeof(uint32_t) * x_count);
	uint32_t* y_alphabet_o	= malloc(sizeof(uint32_t) * y_count);
	uint32_t* x_y_alphabet_o	= malloc(sizeof(uint32_t) * (x_count + y_count));
	uint32_t* x_y_x_p_alphabet_o	= malloc(sizeof(uint32_t) * (x_count * 2 + y_count));
	uint32_t* x_p_alphabet_o	= malloc(sizeof(uint32_t) * x_p_count);
	uint32_t* y_p_alphabet_o	= malloc(sizeof(uint32_t) * y_p_count);
	uint32_t* x_p_y_p_alphabet_o	= malloc(sizeof(uint32_t) * (y_p_count + x_p_count));
	uint32_t* signals_alphabet_o	= malloc(sizeof(uint32_t) * signals_count);
	uint32_t* not_x_p_alphabet_o	= malloc(sizeof(uint32_t) * not_x_p_count);
	uint32_t* not_y_p_alphabet_o	= malloc(sizeof(uint32_t) * not_y_p_count);
	x_count = 0, y_count = 0, x_p_count = 0, y_p_count = 0,signals_count = 0, x_y_count = 0, x_y_x_p_count = 0, not_x_p_count = 0, not_y_p_count = 0;
	for(i = 0; i < mgr->vars_dict->size; i++){//X
		//avoid searching for the true/false variables
		if((strcmp(mgr->vars_dict->entries[i].key, TRUE_VAR) == 0) || (strcmp(mgr->vars_dict->entries[i].key, FALSE_VAR) == 0))	continue;
		alphabet_element_index	= mgr->vars_dict->entries[i].value;
		is_primed				= false;
		for(j = 0; j < parser_primed_variables_count; j++)
			if(parser_primed_variables[j] == alphabet_element_index){is_primed	= true;	break;}
		strcpy(current_key, mgr->vars_dict->entries[i].key);strcat(current_key, SIGNAL_ON_SUFFIX);
		global_index	= automaton_alphabet_get_value_index(ctx->global_alphabet, current_key);
		is_input		= ctx->global_alphabet->list[global_index].type == INPUT_SIG;
		if(!is_primed){
			x_y_alphabet_o[x_y_count++] = i;
			if(is_input){x_alphabet_o[x_count++] = i;}else{y_alphabet_o[y_count++] = i;}
		}else{
			if(is_input){x_p_alphabet_o[x_p_count++] = i;}else{y_p_alphabet_o[y_p_count++] = i;}
		}
		if(!(is_primed && !is_input))x_y_x_p_alphabet_o[x_y_x_p_count++] = i;
		signals_alphabet_o[signals_count++]	= i;
		if(!is_primed || !is_input)not_x_p_alphabet_o[not_x_p_count++] = i;
		if(!is_primed || is_input)not_y_p_alphabet_o[not_y_p_count++] = i;
	}
	x_count = 0, y_count = 0, x_p_count = 0, y_p_count = 0,signals_count = 0, x_y_count = 0, x_y_x_p_count = 0, not_x_p_count = 0, not_y_p_count = 0;
	//this code seems repeated but should be kept this way since enforces
	//sequentialization of X Y X' Y' variables
	for(i = 0; i < mgr->vars_dict->size; i++){//X
		//avoid searching for the true/false variables
		if((strcmp(mgr->vars_dict->entries[i].key, TRUE_VAR) == 0) || (strcmp(mgr->vars_dict->entries[i].key, FALSE_VAR) == 0))	continue;
		alphabet_element_index	= mgr->vars_dict->entries[i].value;
		is_primed				= false;
		for(j = 0; j < parser_primed_variables_count; j++)
			if(parser_primed_variables[j] == alphabet_element_index){is_primed	= true;	break;}
		strcpy(current_key, mgr->vars_dict->entries[i].key);strcat(current_key, SIGNAL_ON_SUFFIX);
		global_index	= automaton_alphabet_get_value_index(ctx->global_alphabet, current_key);
		is_input		= ctx->global_alphabet->list[global_index].type == INPUT_SIG;
		if(is_input){if(is_primed){x_p_alphabet[x_p_count++]	= i; }else x_alphabet[x_count++]	= i;
		}else{if(is_primed)y_p_alphabet[y_p_count++]	= i; else y_alphabet[y_count++]	= i;}
		if(is_primed){ x_p_y_p_alphabet[x_p_y_p_count++] = i;}
		if(is_input && !is_primed){
			not_x_p_alphabet_o[not_x_p_count++] = i;
			not_y_p_alphabet_o[not_y_p_count++] = i;
			x_y_alphabet[x_y_count++] = i;
			x_y_x_p_alphabet[x_y_x_p_count++] = i;
			signals_alphabet[signals_count++]	= i;
			strcpy(current_dict_entry, mgr->vars_dict->entries[i].key);	strcat(current_dict_entry, SIGNAL_ON_SUFFIX);
			obdd_on_signals_indexes[obdd_on_count++]			= automaton_alphabet_get_value_index(ctx->global_alphabet, current_dict_entry);//dictionary_value_for_key(mgr->vars_dict, current_dict_entry);
			strcpy(current_dict_entry, mgr->vars_dict->entries[i].key);	strcat(current_dict_entry, SIGNAL_OFF_SUFFIX);
			obdd_off_signals_indexes[obdd_off_count++]		= automaton_alphabet_get_value_index(ctx->global_alphabet, current_dict_entry);//dictionary_value_for_key(mgr->vars_dict, current_dict_entry);
		}
	}
	for(i = 0; i < mgr->vars_dict->size; i++){//Y
			//avoid searching for the true/false variables
			if((strcmp(mgr->vars_dict->entries[i].key, TRUE_VAR) == 0) || (strcmp(mgr->vars_dict->entries[i].key, FALSE_VAR) == 0))	continue;
			alphabet_element_index	= mgr->vars_dict->entries[i].value;
			is_primed				= false;
			for(j = 0; j < parser_primed_variables_count; j++)
				if(parser_primed_variables[j] == alphabet_element_index){is_primed	= true;	break;}
			strcpy(current_key, mgr->vars_dict->entries[i].key);strcat(current_key, SIGNAL_ON_SUFFIX);
			global_index	= automaton_alphabet_get_value_index(ctx->global_alphabet, current_key);
			if(global_index < 0){
				printf("%s not found on signals's dictionary\n", current_key);
				exit(-1);
			}
			is_input		= ctx->global_alphabet->list[global_index].type == INPUT_SIG;
			if(!is_primed && !is_input){
				not_x_p_alphabet_o[not_x_p_count++] = i;
				not_y_p_alphabet_o[not_y_p_count++] = i;
				x_y_alphabet[x_y_count++] = i;
				x_y_x_p_alphabet[x_y_x_p_count++] = i;
				signals_alphabet[signals_count++]	= i;
				strcpy(current_dict_entry, mgr->vars_dict->entries[i].key);	strcat(current_dict_entry, SIGNAL_ON_SUFFIX);
				obdd_on_signals_indexes[obdd_on_count++]		= automaton_alphabet_get_value_index(ctx->global_alphabet, current_dict_entry);//dictionary_value_for_key(mgr->vars_dict, current_dict_entry);
				strcpy(current_dict_entry, mgr->vars_dict->entries[i].key);	strcat(current_dict_entry, SIGNAL_OFF_SUFFIX);
				obdd_off_signals_indexes[obdd_off_count++]		= automaton_alphabet_get_value_index(ctx->global_alphabet, current_dict_entry);//dictionary_value_for_key(mgr->vars_dict, current_dict_entry);
			}
		}
	for(i = 0; i < mgr->vars_dict->size; i++){//X'
		//avoid searching for the true/false variables
		if((strcmp(mgr->vars_dict->entries[i].key, TRUE_VAR) == 0) || (strcmp(mgr->vars_dict->entries[i].key, FALSE_VAR) == 0))	continue;
		alphabet_element_index	= mgr->vars_dict->entries[i].value;
		is_primed				= false;
		for(j = 0; j < parser_primed_variables_count; j++)
			if(parser_primed_variables[j] == alphabet_element_index){is_primed	= true;	break;}
		strcpy(current_key, mgr->vars_dict->entries[i].key);strcat(current_key, SIGNAL_ON_SUFFIX);
		global_index	= automaton_alphabet_get_value_index(ctx->global_alphabet, current_key);
		if(global_index < 0){
						printf("%s not found on signals's dictionary\n", current_key);
						exit(-1);
					}
		is_input		= ctx->global_alphabet->list[global_index].type == INPUT_SIG;
		if(is_primed && is_input){
			not_y_p_alphabet_o[not_y_p_count++] = i;
			x_y_x_p_alphabet[x_y_x_p_count++] = i;
			signals_alphabet[signals_count++]	= i;
		}
	}
	for(i = 0; i < mgr->vars_dict->size; i++){//Y'
		//avoid searching for the true/false variables
		if((strcmp(mgr->vars_dict->entries[i].key, TRUE_VAR) == 0) || (strcmp(mgr->vars_dict->entries[i].key, FALSE_VAR) == 0))	continue;
		alphabet_element_index	= mgr->vars_dict->entries[i].value;
		is_primed				= false;
		for(j = 0; j < parser_primed_variables_count; j++)
			if(parser_primed_variables[j] == alphabet_element_index){is_primed	= true;	break;}
		strcpy(current_key, mgr->vars_dict->entries[i].key);strcat(current_key, SIGNAL_ON_SUFFIX);
		global_index	= automaton_alphabet_get_value_index(ctx->global_alphabet, current_key);
		if(global_index < 0){
						printf("%s not found on signals's dictionary\n", current_key);
						exit(-1);
					}
		is_input		= ctx->global_alphabet->list[global_index].type == INPUT_SIG;
		if(is_primed && !is_input){
			not_x_p_alphabet_o[not_x_p_count++] = i;
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
	automaton_bool_array_hash_table *x_y_x_p_hash_table	= automaton_bool_array_hash_table_create(x_y_x_p_count);
	bool *hashed_valuation = NULL;


#if DEBUG_LTL_AUTOMATON
	printf("X alphabet\n[");
	for(i = 0; i < x_count; i++)
		printf("%s%s", mgr->vars_dict->entries[x_alphabet[i]].key, i == x_count - 1 ? "" : ",");
	printf("]\nY alphabet\n[");
	for(i = 0; i < y_count; i++)
		printf("%s%s", mgr->vars_dict->entries[y_alphabet[i]].key, i == y_count - 1 ? "" : ",");
	printf("]\nXY alphabet\n[");
	for(i = 0; i < x_y_count; i++)
		printf("%s%s", mgr->vars_dict->entries[x_y_alphabet[i]].key, i == x_y_count - 1 ? "" : ",");
	printf("]\nXYX' alphabet\n[");
	for(i = 0; i < x_y_x_p_count; i++)
		printf("%s%s", mgr->vars_dict->entries[x_y_x_p_alphabet[i]].key, i == x_y_x_p_count - 1 ? "" : ",");
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

	bool *adjusted_valuation			= calloc(x_y_count, sizeof(bool));

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
			if(x_y_alphabet[j] > last_var && (x_y_alphabet[j] < current_var || !var_initialized)){
				var_initialized	= true;
				current_var		= x_y_alphabet[j];
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
	obdd_composite_state* env_state = obdd_composite_state_create(0, x_count * 2 + y_count);
	obdd_composite_state* sys_state = obdd_composite_state_create(0, x_count + y_count);
	uint32_t fluent_count	= x_count + y_count;
	obdd_state_tree* obdd_state_map	= obdd_state_tree_create(x_count * 2 + y_count);
	automaton_concrete_bucket_list* theta_env_bucket_list	= automaton_concrete_bucket_list_create(
			LTL_BUCKET_SIZE, obdd_composite_state_extractor, sizeof(uint32_t) + sizeof(bool) * (x_count * 2 + y_count) );
	automaton_concrete_bucket_list* theta_sys_bucket_list	= automaton_concrete_bucket_list_create(
			LTL_BUCKET_SIZE, obdd_composite_state_extractor, sizeof(uint32_t) + sizeof(bool) * (x_count + y_count));
	automaton_concrete_bucket_list* rho_env_bucket_list	= automaton_concrete_bucket_list_create(
			LTL_BUCKET_SIZE, obdd_composite_state_extractor, sizeof(uint32_t) + sizeof(bool) * (x_count * 2 + y_count) );
	automaton_concrete_bucket_list* rho_sys_bucket_list	= automaton_concrete_bucket_list_create(
			LTL_BUCKET_SIZE, obdd_composite_state_extractor, sizeof(uint32_t) + sizeof(bool) * (x_count + y_count));
	automaton_concrete_bucket_list* rho_env_processed_bucket_list	= automaton_concrete_bucket_list_create(
			LTL_PROCESSED_BUCKET_SIZE, obdd_composite_state_extractor, sizeof(uint32_t) + sizeof(bool) * (x_count * 2 + y_count) );
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

	//NEW BODD CONSTRUCTION APPROACH
	bool* tmp_state_valuation = calloc((x_count * 2 + y_count), sizeof(bool));
	bool has_transition;

	obdd_get_valuations(mgr, env_theta_composed, &valuations, &valuations_size, &current_valuations_count, x_alphabet, x_count
			, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
#if DEBUG_LTL_AUTOMATON
	printf("Init env valuations\n");
	obdd_print_valuations_names(mgr, valuations, current_valuations_count, x_alphabet, x_count);
#endif

	for(i = 0; i < current_valuations_count; i++){
		automaton_set_composed_valuation(env_state->valuation, valuations, i, true, true, x_count, y_count);
		hashed_valuation		= automaton_bool_array_hash_table_add_or_get_entry(x_y_hash_table, env_state->valuation, true);
		env_state->state		= obdd_state_tree_get_key(obdd_state_map, env_state->valuation, x_count);
		has_transition	= automaton_add_transition_from_valuations(mgr, ltl_automaton, sys_state->state, env_state->state
				, sys_state->valuation, hashed_valuation, adjusted_valuation, true, true
				, x_count, y_count
				, obdd_on_signals_indexes, obdd_off_signals_indexes, x_y_alphabet, x_y_x_p_alphabet, x_y_order);
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
			automaton_concrete_bucket_add_entry(theta_env_bucket_list, env_state);
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

	if(theta_env_bucket_list->composite_count > 0){
		do{
			automaton_concrete_bucket_pop_entry(theta_env_bucket_list, env_state);
			hashed_valuation		= automaton_bool_array_hash_table_add_or_get_entry(x_hash_table, env_state->valuation, true);
			obdd_current_state	= obdd_restrict_vector(env_sys_theta_composed, x_alphabet, hashed_valuation, x_count);
			obdd_get_valuations(mgr, obdd_current_state, &valuations, &valuations_size, &current_valuations_count, x_y_alphabet, x_y_count
					, dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes);
#if DEBUG_LTL_AUTOMATON
			printf("%d-[", sys_state->state);
			for(j = 0; j < x_y_count; j++)
				printf("%s", env_state->valuation[j] ? "1" : "0");
			printf("]\n");
			obdd_print_valuations_names(mgr, valuations, current_valuations_count, x_y_alphabet, x_y_count);
#endif
			for(i = 0; i < current_valuations_count; i++){
				automaton_set_composed_valuation(sys_state->valuation, valuations, i, true, false, x_count, y_count);
				sys_state->state		= obdd_state_tree_get_key(obdd_state_map, sys_state->valuation, x_y_count);
				hashed_valuation		= automaton_bool_array_hash_table_add_or_get_entry(x_y_hash_table, sys_state->valuation, true);

				has_transition	= automaton_add_transition_from_valuations(mgr, ltl_automaton, env_state->state, sys_state->state
						, env_state->valuation, hashed_valuation, adjusted_valuation, true, false
						, x_count, y_count
						, obdd_on_signals_indexes, obdd_off_signals_indexes, x_y_alphabet, x_y_x_p_alphabet, x_y_order);
#if DEBUG_LTL_AUTOMATON
				printf("(%d-[", env_state->state);
					for(j = 0; j < x_count + y_count; j++)
						printf("%s", sys_state->valuation[j] ? "1" : "0");
					state_counter++;
					printf("]->%d)%s\n", sys_state->state,!has_transition && !automaton_concrete_bucket_has_entry(rho_sys_bucket_list, sys_state)? "*":"");
					if(state_counter % 1000 == 0){
						printf("States processed for ltl: %d\n", state_counter);
						fflush(stdout);
					}
#endif
				if(!has_transition && !automaton_concrete_bucket_has_entry(rho_sys_bucket_list, sys_state)){
					automaton_concrete_bucket_add_entry(rho_sys_bucket_list, sys_state);
				}
			}
			obdd_destroy(obdd_current_state);
#if DEBUG_LTL_AUTOMATON
			fflush(stdout);
#endif
		}while(theta_env_bucket_list->composite_count > 0);
	}
#define CNTR_LIMIT 200
#if VERBOSE || DEBUG_LTL_AUTOMATON
	printf("[#obdd nodes:val.size:val.count:bucket_count]\n");
#endif
	uint32_t rho_counter = 0, skipped = 0, evaluated = 0;
#if VERBOSE || DEBUG_LTL_AUTOMATON
	printf(ANSI_COLOR_RED "Building rho valuations\n" ANSI_COLOR_RESET);
#endif
	//NEW RHO BUILD APPROACH
	free(valuations);
#if APPLY_OBDD_REACHABLE
	obdd *tmp_obdd = obdd_reachable_states(env_sys_theta_composed, env_sys_rho_composed);
	obdd *and_obdd = obdd_apply_and(env_sys_rho_composed, tmp_obdd);
	obdd_destroy(tmp_obdd);
	obdd_destroy(env_sys_rho_composed);
	env_sys_rho_composed	= and_obdd;
#endif

	automaton_add_transitions_from_valuations(mgr, env_sys_rho_composed, ltl_automaton, &current_valuations_count, signals_alphabet, signals_count,
			dont_care_list, partial_valuation, initialized_values, valuation_set, last_nodes, env_state, sys_state, obdd_state_map, x_count, y_count, x_y_count, x_y_x_p_count,
			x_y_alphabet, x_y_x_p_alphabet, x_y_order, signals_count, hashed_valuation, adjusted_valuation, x_y_hash_table,
			x_y_x_p_hash_table, obdd_on_signals_indexes, obdd_off_signals_indexes);
#if VERBOSE || DEBUG_LTL_AUTOMATON
	//TODO:print cache table info
#endif
/*
	uint32_t deadlocks = 0;
	for(i = 0; i < ltl_automaton->transitions_count; i++)
		if(ltl_automaton->out_degree[i] == 0 && ltl_automaton->in_degree[i] > 0)deadlocks++;
	printf("\n\n %d DEADLOCKS before removal, %d states \n\n", deadlocks, ltl_automaton->transitions_count);
	automaton_automaton_remove_deadlocks(ltl_automaton);
	//automaton_compact_states(ltl_automaton);
	deadlocks = 0;
	for(i = 0; i < ltl_automaton->transitions_count; i++)
		if(ltl_automaton->out_degree[i] == 0 && ltl_automaton->in_degree[i] > 0)deadlocks++;
	printf("\n\n %d DEADLOCKS after removal, %d states \n\n", deadlocks, ltl_automaton->transitions_count);
*/


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
	free(adjusted_valuation);
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
	automaton_bool_array_hash_table_destroy(x_y_x_p_hash_table);

	free(env_state); free(sys_state); free(tmp_state_valuation);
	free(obdd_on_signals_indexes); free(obdd_off_signals_indexes);
	automaton_concrete_bucket_destroy(theta_env_bucket_list);automaton_concrete_bucket_destroy(theta_sys_bucket_list);
	automaton_concrete_bucket_destroy(rho_env_bucket_list);automaton_concrete_bucket_destroy(rho_sys_bucket_list);
	automaton_concrete_bucket_destroy(rho_env_processed_bucket_list);automaton_concrete_bucket_destroy(rho_sys_processed_bucket_list);
	free(x_alphabet); free(y_alphabet); free(x_p_alphabet); free(y_p_alphabet);free(x_y_alphabet); free(x_y_x_p_alphabet); free(x_p_y_p_alphabet); free(signals_alphabet);
	free(not_x_p_alphabet); free(not_y_p_alphabet);
	free(x_alphabet_o); free(y_alphabet_o); free(x_p_alphabet_o); free(y_p_alphabet_o);free(x_y_alphabet_o); free(x_y_x_p_alphabet_o); free(x_p_y_p_alphabet_o); free(signals_alphabet_o);
	free(not_x_p_alphabet_o);free(not_y_p_alphabet_o);
	free(local_alphabet);
	obdd_state_tree_destroy(state_map);
	obdd_state_tree_destroy(obdd_state_map);
	return ltl_automaton;
}

automaton_automata_context* automaton_automata_context_create_from_syntax(automaton_program_syntax* program, char* ctx_name,
		char* test_name, diagnosis_search_method is_diagnosis, char *results_filename, bool append_result){
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
	automaton_automata_context_initialize(ctx, ctx_name, global_alphabet, fluent_count, fluents, liveness_formulas_count, liveness_formulas, liveness_formulas_names);
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
	while(pending_statements){
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
			winning_region_automaton	= automaton_get_gr1_strategy(game_automaton, assumptions, assumptions_count
					, guarantees, guarantees_count, true);
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
			if(winning_region_automaton->transitions_count == 0 && is_diagnosis != 0){
				nonreal	= true;
				automaton_automaton_destroy(winning_region_automaton);
				if(is_diagnosis & DD_SEARCH)
					winning_region_automaton = automaton_get_gr1_unrealizable_minimization_dd(game_automaton, assumptions, assumptions_count, guarantees, guarantees_count,
							&steps, &steps_sizes, &steps_times, &steps_size);
				else if(is_diagnosis & LINEAR_SEARCH)
					winning_region_automaton = automaton_get_gr1_unrealizable_minimization(game_automaton, assumptions, assumptions_count, guarantees, guarantees_count,
							&steps, &steps_sizes, &steps_times, &steps_size);

				automaton_automaton_remove_unreachable_states(winning_region_automaton);
				results_minimization_states = winning_region_automaton->transitions_count;
				results_minimization_transitions	= winning_region_automaton->transitions_composite_count;
				results_minimization_controllable_transitions = 0;
				for(j = 0; j < results_minimization_states; j++){
					for(k = 0; k < winning_region_automaton->out_degree[j]; k++){
						if( !(TRANSITION_IS_INPUT(&(winning_region_automaton->transitions[j][k]))))
							results_minimization_controllable_transitions++;
					}
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
	/*
	 * 			for(i = 0; i < tables->composition_count; i++){
				if(tables->composition_entries[i]->solved &&
						strcmp(program->statem)){
						sprintf(buf, "%s_%s.fsp", ctx_name, tables->composition_entries[i]->valuation.automaton_value->name);
						automaton_automaton_print_fsp(tables->composition_entries[i]->valuation.automaton_value, buf);
						sprintf(buf, "%s_%s.rep", ctx_name, tables->composition_entries[i]->valuation.automaton_value->name);
						automaton_automaton_print_report(tables->composition_entries[i]->valuation.automaton_value, buf);
				}
			}
	 * */

	//PRINT RESULTS
	if(is_diagnosis != 0){
		experimental_results = fopen(results_filename, append_result?"a": "w");
		if (experimental_results == NULL){
			printf("Error opening file!\n");
			return false;
		}
		if(!append_result)
			fprintf(experimental_results, "name\trealizable\tltl_model_build_time\tmodel_build_time\tcomposition_time\t" \
					"synthesis_time\tdiagnosis_time\talphabet_size\tguarantees_count\t" \
					"assumptions_count\tplant_states\tplant_transitions\tplant_controllable_transitions\tminimization_states\t" \
					"minimization_transitions\tminimization_controllable_transitions\tsearch_method\t" \
					"diagnosis_steps\tdiagnosis_times\tdiagnosis_sizes\n");
		fprintf(experimental_results, "%s\t%s\t%ld.%06ld\t%ld.%06ld\t%ld.%06ld\t" \
				"%ld.%06ld\t%ld.%06ld\t%d\t%d\t" \
				"%d\t%d\t%d\t%d\t%d\t" \
				"%d\t%d\t%s\t" \
				"%d\t[",
				test_name, nonreal? "false":"true", tval_ltl_model_build_result.tv_sec, tval_ltl_model_build_result.tv_usec,
						tval_model_build_result.tv_sec, tval_model_build_result.tv_usec,
						tval_composition_result.tv_sec, tval_composition_result.tv_usec,
						tval_synthesis_result.tv_sec, tval_synthesis_result.tv_usec,
						tval_minimization_result.tv_sec, tval_minimization_result.tv_usec,
						ctx->global_alphabet->count, guarantees_count, assumptions_count,
						results_plant_states, results_plant_transitions, results_plant_controllable_transitions,
						results_minimization_states, results_minimization_transitions, results_minimization_controllable_transitions,
						is_diagnosis & DD_SEARCH ? "DD" : "linear",
						steps);
		for(i = 0; i < steps; i++){
			fprintf(experimental_results, "%ld.%06ld%s", steps_times[i].tv_sec, steps_times[i].tv_usec,
					i == steps - 1 ?"]\t[" : ",");
		}
		for(i = 0; i < steps; i++){
			fprintf(experimental_results, "%d%s", steps_sizes[i], i == steps - 1 ?"]\n" : ",");
		}
		if(steps == 0)fprintf(experimental_results,"]\t[]\n");
		fclose(experimental_results);
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




