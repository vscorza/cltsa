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
int32_t automaton_parsing_tables_get_entry_index(automaton_parsing_tables* tables, automaton_parsing_table_entry_type type, char* value){
	int32_t current_index = -1;
	int32_t i;
	automaton_parsing_table_entry* entry;
	switch(type){
	case LABEL_ENTRY_AUT:
		for(i = 0; i < (int32_t)tables->label_count; i++){
			if(strcmp(tables->label_entries[i]->value, value) == 0)
				return i;
		}
		break;
	case SET_ENTRY_AUT:
		for(i = 0; i < (int32_t)tables->set_count; i++){
			if(strcmp(tables->set_entries[i]->value, value) == 0)
				return i;
		}
		break;
	case FLUENT_ENTRY_AUT:
		for(i = 0; i < (int32_t)tables->fluent_count; i++){
			if(strcmp(tables->fluent_entries[i]->value, value) == 0)
				return i;
		}
		break;
	case RANGE_ENTRY_AUT:
		for(i = 0; i < (int32_t)tables->range_count; i++){
			if(strcmp(tables->range_entries[i]->value, value) == 0)
				return i;
		}
		break;
	case CONST_ENTRY_AUT:
		for(i = 0; i < (int32_t)tables->const_count; i++){
			if(strcmp(tables->const_entries[i]->value, value) == 0)
				return i;
		}
		break;
	case AUTOMATON_ENTRY_AUT:
		for(i = 0; i < (int32_t)tables->automaton_count; i++){
			if(strcmp(tables->automaton_entries[i]->value, value) == 0)
				return i;
		}
		break;
	case COMPOSITION_ENTRY_AUT:
		for(i = 0; i < (int32_t)tables->composition_count; i++){
			if(strcmp(tables->composition_entries[i]->value, value) == 0)
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
/*
typedef struct automaton_label_syntax_str{
	bool	is_set;
	struct automaton_set_syntax_str* set;
	char* string_terminal;
}automaton_label_syntax;
typedef struct automaton_set_syntax_str{
	bool is_ident;
	uint32_t count;
	uint32_t* labels_count;
	struct automaton_label_syntax_str*** labels;
	char* string_terminal;
}automaton_set_syntax;
typedef struct automaton_set_def_syntax_str{
	struct automaton_set_syntax_str* set;
	char* name;
}automaton_set_def_syntax;
*/
char** automaton_set_syntax_evaluate(automaton_parsing_tables* tables, automaton_set_syntax* set, int32_t *count){
	int32_t index;
	uint32_t i, j;
	char* current_entry;
	char** ret_value			= NULL;
	int32_t inner_count			= 0;
	char** inner_value			= NULL;
	bool is_set;
	index						= automaton_parsing_tables_get_entry_index(tables, SET_ENTRY_AUT, set->string_terminal);
	if(tables->set_entries[index]->solved)
		return tables->set_entries[index]->valuation.labels_value;
	//search until proper set def is found
	while(set->is_ident){
		index	= automaton_parsing_tables_get_entry_index(tables, SET_ENTRY_AUT, set->string_terminal);
		set		= ((automaton_set_def_syntax*)tables->set_entries[index]->value)->set;
	}
	if(tables->set_entries[index]->solved)
		return tables->set_entries[index]->valuation.labels_value;
	//if proper set was not solved try to solve it
	for(i = 0; i < set->count; i++)for(j = 0; j < set->labels_count[i]; j++){
		is_set		= set->labels[i][j]->is_set;
		if(!is_set){
			current_entry	= set->labels[i][j]->string_terminal;
			while(set->is_ident){
				index	= automaton_parsing_tables_get_entry_index(tables, SET_ENTRY_AUT, set->string_terminal);
				set		= ((automaton_set_def_syntax*)tables->set_entries[index]->value)->set;
			}
			if(!tables->set_entries[index]->solved){
				inner_value	= automaton_set_syntax_evaluate(tables, set, &inner_count);
			}else{
				inner_count	= tables->set_entries[index]->valuation_count;
				inner_value	= tables->set_entries[index]->valuation.labels_value;
			}
			aut_merge_string_lists(&ret_value, count, inner_value, inner_count, true, false);
		}else{
			//TODO:parse current set and merge
			aut_merge_string_lists(&ret_value, count, &(set->labels[i][j]->string_terminal), 1, true, false);
		}
	}
	//once solved, update set_def_entry
	tables->set_entries[index]->solved	= true;
	tables->const_entries[index]->valuation_count	 	= *count;
	tables->set_entries[index]->valuation.labels_value	= ret_value;
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
	int32_t global_count;
	int32_t controllable_count;
	char** global_values		= automaton_set_syntax_evaluate(tables, ((automaton_set_def_syntax*)tables->set_entries[global_index]->value)->set
			, &global_count);
	char** controllable_values	= automaton_set_syntax_evaluate(tables, ((automaton_set_def_syntax*)tables->set_entries[controllable_index]->value)->set
			, &controllable_count);

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
/*
typedef struct automaton_component_syntax_str{
	char* ident;	char* prefix;
	automaton_index_syntax* index;	automaton_indexes_syntax* indexes;
}automaton_component_syntax;
typedef struct automaton_components_syntax_str{
	uint32_t count;	struct automaton_component_syntax_str** components;
}automaton_components_syntax;
typedef struct automaton_composition_syntax_str{
	char* name;	uint32_t count; 	struct automaton_state_syntax_str** states;
	struct automaton_component_syntax_str** components;
}automaton_composition_syntax;
typedef struct automata_context_str{
	char*				name;	automaton_alphabet*	global_alphabet;
	uint32_t			global_fluents_count;	automaton_fluent*	global_fluents;
} automaton_automata_context;
typedef struct automaton_str{
	char*					name;
	automaton_automata_context*		context;	uint32_t				local_alphabet_count;
	uint32_t*				local_alphabet;	uint32_t				states_count;
	uint32_t				transitions_size;	uint32_t				transitions_count;
	uint32_t				max_out_degree;	uint32_t*				out_degree;
	automaton_transition**	transitions;			// S -> list of transitions (s,s')
	uint32_t*				in_degree;	automaton_transition**	inverted_transitions;
	uint32_t				initial_states_count;	uint32_t*				initial_states;
	uint32_t				valuations_size;	uint32_t				valuations_count;
	automaton_valuation*	valuations;
} automaton_automaton;
*/
bool automaton_statement_syntax_to_automaton(automaton_automata_context* ctx, automaton_composition_syntax* composition_syntax
		, automaton_parsing_tables* tables){
	//check whether composition syntax is a composition or a single automaton description
	if(composition_syntax->components != NULL){

	}else{
		//automaton_automaton_create(name, ctx, local_alphabet_count, local_alphabet))
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
	ctx->global_alphabet	= automaton_parsing_tables_get_global_alphabet(tables);
	//build automata
	bool pending_automata	= true;
	while(pending_automata){
		pending_automata	= false;
		for(i = 0; i < program->count; i++){
			if(program->statements[i]->type == COMPOSITION_AUT)
				pending_automata = pending_automata || automaton_statement_syntax_to_automaton(ctx, program->statements[i]->composition_def, tables);
		}
	}
	automaton_alphabet* global_alphabet	= automaton_parsing_tables_get_global_alphabet(tables);
	automaton_parsing_tables_destroy(tables);
	return ctx;
}



