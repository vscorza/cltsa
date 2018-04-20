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
automaton_parsing_table_entry* automaton_parsing_table_entry_create(automaton_parsing_table_entry_type type, char* value, int32_t index){
	automaton_parsing_table_entry* entry	= malloc(sizeof(automaton_parsing_table_entry));
	entry->type		= type;
	aut_dupstr(&(entry->value), value);
	entry->index	= index;
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

int32_t automaton_parsing_tables_add_entry(automaton_parsing_tables* tables, automaton_parsing_table_entry_type type, char* value){
	int32_t current_index;
	current_index	= automaton_parsing_tables_get_entry_index(tables, type, value);
	if(current_index >= 0)
		return current_index;
	automaton_parsing_table_entry* entry;
	switch(type){
	case LABEL_ENTRY_AUT:
		current_index	= tables->label_count;
		entry			= automaton_parsing_table_entry_create(type, value, current_index);
		aut_add_ptr_list((void***)&(tables->label_entries), (void*)entry, &(tables->label_count));
		break;
	case SET_ENTRY_AUT:
			current_index	= tables->set_count;
			entry			= automaton_parsing_table_entry_create(type, value, current_index);
			aut_add_ptr_list((void***)&(tables->set_entries), (void*)entry, &(tables->set_count));
			break;
	case FLUENT_ENTRY_AUT:
				current_index	= tables->fluent_count;
				entry			= automaton_parsing_table_entry_create(type, value, current_index);
				aut_add_ptr_list((void***)&(tables->fluent_entries), (void*)entry, &(tables->fluent_count));
				break;
	case RANGE_ENTRY_AUT:
					current_index	= tables->range_count;
					entry			= automaton_parsing_table_entry_create(type, value, current_index);
					aut_add_ptr_list((void***)&(tables->range_entries), (void*)entry, &(tables->range_count));
					break;
	case CONST_ENTRY_AUT:
					current_index	= tables->const_count;
					entry			= automaton_parsing_table_entry_create(type, value, current_index);
					aut_add_ptr_list((void***)&(tables->const_entries), (void*)entry, &(tables->const_count));
					break;
	case AUTOMATON_ENTRY_AUT:
					current_index	= tables->automaton_count;
					entry			= automaton_parsing_table_entry_create(type, value, current_index);
					aut_add_ptr_list((void***)&(tables->automaton_entries), (void*)entry, &(tables->automaton_count));
					break;
	case COMPOSITION_ENTRY_AUT:
					current_index	= tables->composition_count;
					entry			= automaton_parsing_table_entry_create(type, value, current_index);
					aut_add_ptr_list((void***)&(tables->composition_entries), (void*)entry, &(tables->composition_count));
					break;
	}
	return current_index;
}

automaton_automata_context* automaton_automata_context_create_from_syntax(automaton_program_syntax* program, char* ctx_name){
	automaton_parsing_tables* tables	= automaton_parsing_tables_create();
	automaton_automata_context* ctx	= malloc(sizeof(automaton_automata_context));
	aut_dupstr(&(ctx->name), ctx_name);



	automaton_parsing_tables_destroy(tables);
	return ctx;
}



