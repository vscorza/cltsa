#include "parser_utils.h"
#include "assert.h"

parser_obdd_mgr = NULL;

automaton_expression_syntax* automaton_expression_syntax_create(automaton_expression_type_syntax type, automaton_expression_syntax* first
		, automaton_expression_syntax* second, char* string_terminal, int32_t integer_terminal, automaton_expression_operator_syntax op){
	automaton_expression_syntax* expr	= malloc(sizeof(automaton_expression_syntax));
	expr->type	= type;
	expr->first	= first;
	expr->second= second;
	aut_dupstr(&(expr->string_terminal), string_terminal);
	expr->integer_terminal	= integer_terminal;
	expr->op				= op;
	return expr;
}
automaton_set_syntax* automaton_set_syntax_create(bool is_ident, uint32_t count, uint32_t* labels_count,
		automaton_label_syntax*** labels, char* string_terminal){
	automaton_set_syntax* set	= malloc(sizeof(automaton_set_syntax));
	set->is_ident	= is_ident;
	if(count > 0){
		uint32_t i,j;
		set->count			= count;
		set->labels_count	= malloc(sizeof(uint32_t) * count);
		for(i = 0; i < count; i++) set->labels_count[i]	= labels_count[i];
		set->labels			= malloc(sizeof(automaton_label_syntax**) * count);
		for(i = 0; i < count; i++){
			set->labels[i]	= malloc(sizeof(automaton_label_syntax) * labels_count[i]);
			for(j = 0; j < labels_count[i]; j++){
				(set->labels[i])[j]	= labels[i][j];
			}
		}
	}else{
		set->count 			= 0;
		set->labels_count	= NULL;
		set->labels			= NULL;
	}
	aut_dupstr(&(set->string_terminal), string_terminal);
	return set;
}
void automaton_program_add_obdd_primed_variables(){
	dictionary *dict	= parser_get_obdd_mgr()->vars_dict;
	int32_t i, old_size = dict->size;
	char prime_name[255];
	for (i = 2; i < old_size; i++){
		strcpy(prime_name, dict->entries[i].key);
		strcat(prime_name, SIGNAL_PRIME_SUFFIX);
		parser_add_primed_variables(dictionary_add_entry(dict, prime_name));
	}
}
automaton_set_syntax* automaton_set_syntax_create_concurrent(char* string_terminal){
	automaton_set_syntax* set	= malloc(sizeof(automaton_set_syntax));
	set->is_ident	= false;
	set->count			= 1;
	set->labels_count	= malloc(sizeof(uint32_t) * set->count);
	set->labels_count[0]= 1;
	set->labels			= malloc(sizeof(automaton_label_syntax**) * set->count);
	set->labels[0]		= malloc(sizeof(automaton_label_syntax*));
	set->labels[0][0]	= automaton_label_syntax_create(false, NULL, string_terminal, NULL);
	set->string_terminal= NULL;
	return set;
}
automaton_set_syntax* automaton_set_syntax_concat_concurrent(automaton_set_syntax* set, char* string_terminal){
	set->labels_count[0]++;
	automaton_label_syntax** new_labels	= malloc(sizeof(automaton_label_syntax*) * set->labels_count[0]);
	uint32_t i;
	if(set->labels_count[0] > 1){
		for(i = 0; i < (set->labels_count[0] - 1); i++){
			new_labels[i]	= set->labels[0][i];
		}
	}
	new_labels[set->labels_count[0] - 1]	= automaton_label_syntax_create(false, NULL, string_terminal, NULL);
	free(set->labels[0]);
	set->labels[0] = new_labels;
	return set;
}
automaton_label_syntax* automaton_label_syntax_create(bool is_set, automaton_set_syntax* set, char* string_terminal, automaton_indexes_syntax* indexes){
	automaton_label_syntax* label	= malloc(sizeof(automaton_label_syntax));
	label->is_set	= is_set;
	label->indexes	= indexes;
	if(label->is_set){
		label->set				= set;
		label->string_terminal	= NULL;
	}else{
		label->set				= NULL;
		aut_dupstr(&(label->string_terminal), string_terminal);
	}
	return label;
}
automaton_set_syntax* automaton_set_syntax_create_from_label(automaton_label_syntax* label){
	automaton_set_syntax* set	= malloc(sizeof(automaton_set_syntax));
	set->is_ident		= false;
	set->count			= 1;
	set->labels_count	= malloc(sizeof(uint32_t) * set->count);
	set->labels_count[0]= 1;
	set->labels			= malloc(sizeof(automaton_label_syntax**) * set->count);
	set->labels[0]		= malloc(sizeof(automaton_label_syntax*) * set->labels_count[0]);
	set->labels[0][0]	= label;
	set->string_terminal= NULL;
	return set;
}
automaton_set_syntax* automaton_set_syntax_concat_labels(automaton_set_syntax* set, automaton_label_syntax* label){
	set->labels_count[0]++;
	automaton_label_syntax** new_labels	= malloc(sizeof(automaton_label_syntax*) * set->labels_count[0]);
	uint32_t i;
	if(set->labels_count[0] > 1){
		for(i = 0; i < (set->labels_count[0] - 1); i++){
			new_labels[i]	= set->labels[0][i];
		}
	}
	new_labels[set->labels_count[0] - 1]	= label;
	free(set->labels[0]);
	set->labels[0]	= new_labels;
	return set;
}
automaton_set_syntax* automaton_set_syntax_create_from_ident(char* ident){
	return automaton_set_syntax_create(true, 0, NULL, NULL, ident);
}
automaton_set_def_syntax* automaton_set_def_syntax_create(automaton_set_syntax* set, char* name){
	automaton_set_def_syntax* set_def	= malloc(sizeof(automaton_set_syntax));
	set_def->set		= set;
	aut_dupstr(&(set_def->name), name);
	return set_def;
}
automaton_fluent_syntax* automaton_fluent_syntax_create(char* name, automaton_set_syntax* initiating_set, automaton_set_syntax* finishing_set, uint32_t initial_value){
	automaton_fluent_syntax* fluent	= malloc(sizeof(automaton_fluent_syntax));
	fluent->initiating_set	= initiating_set;
	fluent->finishing_set	= finishing_set;
	fluent->initial_value 	= initial_value;
	aut_dupstr(&(fluent->name), name);
	return fluent;
}
automaton_index_syntax* automaton_index_syntax_create(bool is_expr, bool is_range, automaton_expression_syntax* expr, char* lower_ident
		, char* upper_ident){
	automaton_index_syntax* index	= malloc(sizeof(automaton_index_syntax));

	if(is_expr){
		index->is_expr		= true;
		index->expr			= expr;
		index->is_range		= false;
		index->lower_ident	= NULL;
		index->upper_ident	= NULL;
	}else{
		index->is_expr		= false;
		aut_dupstr(&(index->lower_ident), lower_ident);
		if(is_range){
			index->is_range	= true;
			index->expr		= expr;
			index->upper_ident	= NULL;
		}else{
			index->is_range	= false;
			index->expr		= NULL;
			aut_dupstr(&(index->upper_ident), upper_ident);
		}
	}
	return index;
}
automaton_indexes_syntax* automaton_indexes_syntax_create(automaton_index_syntax* first_index){
	automaton_indexes_syntax* indexes	= malloc(sizeof(automaton_index_syntax));
	indexes->count		= 1;
	indexes->indexes	= malloc(sizeof(automaton_index_syntax*) * indexes->count);
	indexes->indexes[0]	= first_index;
	return indexes;
}
automaton_indexes_syntax* automaton_indexes_syntax_add_index(automaton_indexes_syntax* indexes, automaton_index_syntax* index){
	indexes->count++;
	automaton_index_syntax** new_indexes	= malloc(sizeof(automaton_index_syntax*) * indexes->count);
	uint32_t i;
	for(i = 0; i < (indexes->count-1); i++) new_indexes[i]	= indexes->indexes[i];
	new_indexes[indexes->count-1]	= index;
	free(indexes->indexes);
	indexes->indexes	= new_indexes;
	return indexes;
}
automaton_trace_label_atom_syntax* automaton_trace_label_atom_syntax_create_from_index(automaton_index_syntax* first_index){
	automaton_trace_label_atom_syntax* label_atom	= malloc(sizeof(automaton_trace_label_atom_syntax));
	label_atom->label	= NULL;
	label_atom->indexes	= automaton_indexes_syntax_create(first_index);
	return label_atom;
}
automaton_trace_label_atom_syntax* automaton_trace_label_atom_syntax_create(automaton_label_syntax* label, automaton_indexes_syntax* indexes){
	automaton_trace_label_atom_syntax* label_atom	= malloc(sizeof(automaton_trace_label_atom_syntax));
	label_atom->label	= label;
	label_atom->indexes	= indexes;
	return label_atom;
}
automaton_trace_label_syntax* automaton_trace_label_syntax_create(automaton_trace_label_atom_syntax* first_atom){
	automaton_trace_label_syntax* trace_label		= malloc(sizeof(automaton_trace_label_syntax));
	trace_label->count	= 1;
	trace_label->atoms	= malloc(sizeof(automaton_trace_label_atom_syntax*) * trace_label->count);
	trace_label->atoms[0]	= first_atom;
	return trace_label;
}
automaton_trace_label_syntax* automaton_trace_label_syntax_add_atom(automaton_trace_label_syntax* trace_label
		, automaton_trace_label_atom_syntax* atom){
	trace_label->count++;
	automaton_trace_label_atom_syntax** new_atoms	= malloc(sizeof(automaton_trace_label_atom_syntax*) * trace_label->count);
	uint32_t i;
	for(i = 0; i < (trace_label->count -1); i++) new_atoms[i]	= trace_label->atoms[i];
	new_atoms[trace_label->count - 1]	= atom;
	free(trace_label->atoms);
	trace_label->atoms	= new_atoms;
	return trace_label;
}
automaton_transition_syntax* automaton_transition_syntax_finish(automaton_expression_syntax* condition, automaton_transition_syntax* trace
		, automaton_state_label_syntax* state){
	trace->condition	= condition;
	trace->to_state		= state;
	return trace;
}
automaton_transition_syntax* automaton_transition_syntax_create_from_trace(automaton_trace_label_syntax* trace){
	automaton_transition_syntax* transition	= malloc(sizeof(automaton_transition_syntax));
	transition->count	= 1;
	transition->condition	= NULL;
	transition->to_state	= NULL;
	transition->labels		= malloc(sizeof(automaton_trace_label_syntax*));
	transition->labels[0]	= trace;
	return transition;
}
automaton_transition_syntax* automaton_transition_syntax_add_trace(automaton_transition_syntax* transition, automaton_trace_label_syntax* trace){
	transition->count++;
	automaton_trace_label_syntax** new_labels		= malloc(sizeof(automaton_trace_label_syntax*) * transition->count);
	uint32_t i;
	for(i = 0; i < (transition->count -1); i++) new_labels[i]	= transition->labels[i];
	new_labels[transition->count -1]	= trace;
	free(transition->labels);
	transition->labels	= new_labels;
	return transition;
}
automaton_transitions_syntax* automaton_transitions_syntax_create(automaton_transition_syntax* transition){
	automaton_transitions_syntax* transitions	= malloc(sizeof(automaton_transitions_syntax));
	transitions->count	= 1;
	transitions->transitions	= malloc(sizeof(automaton_transitions_syntax*) * transitions->count);
	transitions->transitions[0]	= transition;
	return transitions;
}
automaton_transitions_syntax* automaton_transitions_syntax_add_transition(automaton_transitions_syntax* transitions, automaton_transition_syntax* transition){
	transitions->count++;
	automaton_transition_syntax** new_transitions	= malloc(sizeof(automaton_transition_syntax*) * transitions->count);
	uint32_t i;
	for(i = 0; i < (transitions->count - 1); i++)new_transitions[i] = transitions->transitions[i];
	new_transitions[transitions->count - 1]	= transition;
	free(transitions->transitions);
	transitions->transitions = new_transitions;
	return transitions;
}
automaton_state_syntax* automaton_state_syntax_create(bool is_ref, automaton_state_label_syntax* label, automaton_state_label_syntax* ref
		,automaton_transitions_syntax* transitions){
	automaton_state_syntax* state	= malloc(sizeof(automaton_state_syntax));
	state->label	= label;
	if(is_ref){
		state->is_ref	= true;
		state->ref		= ref;
		state->transitions_count	= 0;
		state->transitions			= NULL;
	}else{
		state->is_ref	= false;
		state->ref		= NULL;
		state->transitions_count	= transitions->count;
		state->transitions			= transitions->transitions;
	}
	return state;
}
automaton_state_label_syntax* automaton_state_label_syntax_create(char* name, automaton_indexes_syntax* indexes){
	automaton_state_label_syntax* state_label	= malloc(sizeof(automaton_state_label_syntax));
	aut_dupstr(&(state_label->name), name);
	state_label->indexes	= indexes;
	return state_label;
}
automaton_states_syntax* automaton_states_syntax_create(automaton_state_syntax* state){
	automaton_states_syntax* states	= malloc(sizeof(automaton_states_syntax));
	states->count	= 1;
	states->states	= malloc(sizeof(automaton_state_syntax*) * states->count);
	states->states[0]	= state;
	return states;
}
automaton_states_syntax* automaton_states_syntax_add_state(automaton_states_syntax* states, automaton_state_syntax* state){
	states->count++;
	automaton_state_syntax** new_states	= malloc(sizeof(automaton_state_syntax*) * states->count);
	uint32_t i;
	for(i = 0; i < (states->count - 1); i++)new_states[i] = states->states[i];
	new_states[states->count - 1]	= state;
	free(states->states);
	states->states = new_states;
	return states;
}
automaton_composition_syntax* automaton_composition_syntax_create_from_states(automaton_states_syntax* states){
	automaton_composition_syntax* composition	= malloc(sizeof(automaton_composition_syntax));
	aut_dupstr(&(composition->name), states->states[0]->label->name);
	composition->components	= NULL;
	composition->count	= states->count;
	composition->states	= states->states;
	composition->is_game	= false;
	return composition;
}
automaton_composition_syntax* automaton_composition_syntax_create_from_ref(char* name, automaton_components_syntax* components, bool is_game){
	automaton_composition_syntax* composition	= malloc(sizeof(automaton_composition_syntax));
	aut_dupstr(&(composition->name), name);
	composition->components	= components->components;
	composition->count	= components->count;
	composition->states	= NULL;
	composition->is_game= is_game;

	return composition;
}
automaton_gr1_game_syntax* automaton_gr1_game_syntax_create(char* name, char* composition_name
		, automaton_set_syntax* assumptions, automaton_set_syntax* goals){
	automaton_gr1_game_syntax* gr1_game	= malloc(sizeof(automaton_gr1_game_syntax));
	aut_dupstr(&(gr1_game->name), name);
	aut_dupstr(&(gr1_game->composition_name), composition_name);
	gr1_game->assumptions	= assumptions;
	gr1_game->guarantees	= goals;
	return gr1_game;
}
automaton_components_syntax* automaton_components_syntax_create(automaton_component_syntax* component){
	automaton_components_syntax* components		= malloc(sizeof(automaton_components_syntax));
	components->count							= 1;
	components->components						= malloc(sizeof(automaton_component_syntax*) * components->count);
	components->components[0]					= component;
	return components;
}

automaton_components_syntax* automaton_components_syntax_add_component(automaton_components_syntax* components, automaton_component_syntax* component, automaton_synchronization_type_syntax type){
	components->count++;
	automaton_component_syntax** new_components	= malloc(sizeof(automaton_component_syntax*) * components->count);
	uint32_t i;
	for(i = 0; i < (components->count -1); i++)new_components[i] = components->components[i];
	component->synch_type	= type;
	new_components[components->count -1]	= component;
	free(components->components);
	components->components	= new_components;
	return components;
}
automaton_component_syntax* automaton_component_syntax_create(char* ident, char* prefix, automaton_index_syntax* index, automaton_indexes_syntax* indexes){
	automaton_component_syntax* component	= malloc(sizeof(automaton_component_syntax));
	aut_dupstr(&(component->ident), ident);
	aut_dupstr(&(component->prefix), prefix);
	component->index	= index;
	component->indexes	= indexes;
	component->synch_type		= ASYNCH_AUT;
	return component;
}
automaton_program_syntax* automaton_program_syntax_create(automaton_statement_syntax* first_statement){
	automaton_program_syntax* program	= malloc(sizeof(automaton_program_syntax));
	program->count	= 1;
	program->statements	= malloc(sizeof(automaton_statement_syntax) * program->count);
	program->statements[0]	= first_statement;
	return program;
}
automaton_program_syntax* automaton_program_syntax_add_statement(automaton_program_syntax* program, automaton_statement_syntax* statement){
	program->count++;
	automaton_statement_syntax** new_statements	= malloc(sizeof(automaton_statement_syntax*) * program->count);
	uint32_t i;
	for(i = 0; i < (program->count -1); i++)new_statements[i] = program->statements[i];
	new_statements[program->count -1]	= statement;
	free(program->statements);
	program->statements	= new_statements;
	return program;
}
automaton_statement_syntax* automaton_statement_syntax_create(automaton_statement_type_syntax type, automaton_composition_syntax* composition_def,
		automaton_expression_syntax* range_def, automaton_expression_syntax* const_def, automaton_fluent_syntax* fluent_def,
		automaton_set_def_syntax* set_def, automaton_gr1_game_syntax* gr1_game_def, ltl_rule_syntax* ltl_rule, ltl_fluent_syntax* ltl_fluent){
	automaton_statement_syntax* statement	= malloc(sizeof(automaton_statement_syntax));
	statement->type	= type;
	statement->composition_def	= composition_def;
	statement->range_def		= range_def;
	statement->const_def		= const_def;
	statement->fluent_def		= fluent_def;
	statement->set_def			= set_def;
	statement->gr1_game_def		= gr1_game_def;
	statement->ltl_rule_def		= ltl_rule;
	statement->ltl_fluent_def	= ltl_fluent;
	return statement;
}

ltl_rule_syntax* ltl_rule_syntax_create(bool is_theta, bool is_env, char* name, char* game_structure_name, obdd* obdd){
	ltl_rule_syntax* ltl_rule	= malloc(sizeof(ltl_rule_syntax));
	ltl_rule->is_theta			= is_theta;
	ltl_rule->is_env			= is_env;
	aut_dupstr(&(ltl_rule->name), name);
	aut_dupstr(&(ltl_rule->game_structure_name), game_structure_name);
	ltl_rule->obdd				= obdd;
	return ltl_rule;
}

ltl_fluent_syntax* automaton_ltl_fluent_syntax_create(char* name, obdd* obdd){
	ltl_fluent_syntax* ltl_fluent	= malloc(sizeof(ltl_fluent_syntax));
	aut_dupstr(&(ltl_fluent->name), name);
	ltl_fluent->obdd			= obdd;
	return ltl_fluent;
}

void automaton_program_syntax_destroy(automaton_program_syntax* program){
	uint32_t i;
	for(i = 0; i < program->count; i++)	automaton_statement_syntax_destroy(program->statements[i]);
	free(program->statements);
	free(program);
}
void automaton_statement_syntax_destroy(automaton_statement_syntax* statement){
	switch (statement->type){
	case IMPORT_AUT: break;
	case MENU_AUT: break;
	case CONST_AUT: automaton_expression_syntax_destroy(statement->const_def);break;
	case RANGE_AUT: automaton_expression_syntax_destroy(statement->range_def);break;
	case FLUENT_AUT: automaton_fluent_syntax_destroy(statement->fluent_def); break;
	case ASSERTION_AUT: break;
	case SET_AUT: automaton_set_def_syntax_destroy(statement->set_def);break;
	case COMPOSITION_AUT: automaton_composition_syntax_destroy(statement->composition_def);break;
	case GR_1_AUT: automaton_gr1_game_syntax_destroy(statement->gr1_game_def);break;
	case LTL_RULE_AUT: ltl_rule_syntax_destroy(statement->ltl_rule_def);break;
	case LTL_FLUENT_AUT: ltl_fluent_syntax_destroy(statement->ltl_fluent_def); break;
	case GOAL_AUT: break;
	}
	free(statement);
}
void automaton_components_syntax_destroy(automaton_components_syntax* components){
	uint32_t i;
	for(i = 0; i < components->count; i++)automaton_component_syntax_destroy(components->components[i]);
	free(components->components);
	free(components);
}
void automaton_component_syntax_destroy(automaton_component_syntax* component){
	if(component->ident != NULL)free(component->ident);
	if(component->index != NULL)automaton_index_syntax_destroy(component->index);
	if(component->indexes != NULL)automaton_indexes_syntax_destroy(component->indexes);
	if(component->prefix != NULL)free(component->prefix);
	free(component);
}
void automaton_composition_syntax_destroy(automaton_composition_syntax* composition){
	uint32_t i;
	if(composition->name != NULL) free(composition->name);
	if(composition->components != NULL){
		for(i = 0; i < composition->count; i++)	automaton_component_syntax_destroy(composition->components[i]);
		free(composition->components);
	}else if(composition->states != NULL){
		for(i = 0; i < composition->count; i++)	automaton_state_syntax_destroy(composition->states[i]);
		free(composition->states);
	}
	free(composition);
}

void automaton_gr1_game_syntax_destroy(automaton_gr1_game_syntax* gr1_game){
	if(gr1_game->name != NULL)
		free(gr1_game->name);
	if(gr1_game->composition_name != NULL)
		free(gr1_game->composition_name);
	if(gr1_game->assumptions != NULL)
		automaton_set_syntax_destroy(gr1_game->assumptions);
	if(gr1_game->guarantees != NULL)
		automaton_set_syntax_destroy(gr1_game->guarantees);
	free(gr1_game);
}
void automaton_states_syntax_destroy(automaton_states_syntax* states){
	uint32_t i;
	for(i = 0; i < states->count; i++)automaton_state_syntax_destroy(states->states[i]);
	free(states->states);
	free(states);
}
void automaton_state_syntax_destroy(automaton_state_syntax* state){
	if(state->label != NULL)		automaton_state_label_syntax_destroy(state->label);
	if(state->ref != NULL)		automaton_state_label_syntax_destroy(state->ref);
	uint32_t i;
	for(i = 0; i < state->transitions_count; i++)automaton_transition_syntax_destroy(state->transitions[i]);
	if(state->transitions != NULL)		free(state->transitions);
	free(state);
}
void automaton_state_label_syntax_destroy(automaton_state_label_syntax* state_label){
	if(state_label->name != NULL) free(state_label->name);
	if(state_label->indexes != NULL) automaton_indexes_syntax_destroy(state_label->indexes);
	free(state_label);
}
void automaton_transitions_syntax_destroy(automaton_transitions_syntax* transitions){
	uint32_t i;
	for(i = 0; i < transitions->count; i++)automaton_transition_syntax_destroy(transitions->transitions[i]);
	free(transitions->transitions);
	free(transitions);
}
void automaton_transition_syntax_destroy(automaton_transition_syntax* transition){
	uint32_t i;
	if(transition->condition != NULL)automaton_expression_syntax_destroy(transition->condition);
	for(i = 0; i < transition->count; i++)automaton_trace_label_syntax_destroy(transition->labels[i]);
	free(transition->labels);
	if(transition->to_state != NULL)automaton_state_label_syntax_destroy(transition->to_state);
	free(transition);
}
void automaton_trace_label_syntax_destroy(automaton_trace_label_syntax* trace_label){
	uint32_t i;
	for(i = 0; i < trace_label->count;i++)automaton_trace_label_atom_syntax_destroy(trace_label->atoms[i]);
	free(trace_label->atoms);
	free(trace_label);
}
void automaton_trace_label_atom_syntax_destroy(automaton_trace_label_atom_syntax* trace_label_atom){
	if(trace_label_atom->label != NULL)automaton_label_syntax_destroy(trace_label_atom->label);
	if(trace_label_atom->indexes != NULL)automaton_indexes_syntax_destroy(trace_label_atom->indexes);
	free(trace_label_atom);
}
void automaton_label_syntax_destroy(automaton_label_syntax* label){
	if(label->string_terminal != NULL)free(label->string_terminal);
	if(label->set != NULL)automaton_set_syntax_destroy(label->set);
	if(label->indexes != NULL)automaton_indexes_syntax_destroy(label->indexes);
	free(label);
}
void automaton_indexes_syntax_destroy(automaton_indexes_syntax* indexes){
	uint32_t i;
	for(i = 0; i < indexes->count; i++) automaton_index_syntax_destroy(indexes->indexes[i]);
	free(indexes->indexes);
	free(indexes);
}
void automaton_index_syntax_destroy(automaton_index_syntax* index){
	if(index->expr != NULL)automaton_expression_syntax_destroy(index->expr);
	if(index->lower_ident != NULL)free(index->lower_ident);
	if(index->upper_ident != NULL)free(index->upper_ident);
	free(index);
}
void automaton_fluent_syntax_destroy(automaton_fluent_syntax* fluent){
	if(fluent->name != NULL) free(fluent->name);
	if(fluent->initiating_set != NULL)automaton_set_syntax_destroy(fluent->initiating_set);
	if(fluent->finishing_set != NULL)automaton_set_syntax_destroy(fluent->finishing_set);
	free(fluent);
}
void automaton_set_syntax_destroy(automaton_set_syntax* set){
	if(set->string_terminal != NULL) free(set->string_terminal);
	uint32_t i,j;
	for(i = 0; i < set->count; i++){
		for(j = 0; j < set->labels_count[i]; j++)automaton_label_syntax_destroy(set->labels[i][j]);
		free(set->labels[i]);
	}
	free(set->labels_count);
	free(set->labels);
	free(set);
}
void automaton_set_def_syntax_destroy(automaton_set_def_syntax* set_def){
	if(set_def->name != NULL)free(set_def->name);
	if(set_def->set != NULL)automaton_set_syntax_destroy(set_def->set);
	free(set_def);
}
void automaton_expression_syntax_destroy(automaton_expression_syntax* expr){
	if(expr->first != NULL)automaton_expression_syntax_destroy(expr->first);
	if(expr->second != NULL)automaton_expression_syntax_destroy(expr->second);
	if(expr->string_terminal != NULL)free(expr->string_terminal);
	free(expr);
}
bool automaton_syntax_is_reserved(char* token){
	uint32_t KEYWORDS_LENGTH	= 3;
	const char *keywords[KEYWORDS_LENGTH];
	keywords[0] = "set";
	keywords[1] = "const";
	keywords[2] = "range";
	uint32_t i;
	for(i = 0; i < KEYWORDS_LENGTH; i++){
		if(strcmp(keywords[i], token) == 0)
			return true;
	}
	return false;
}


obdd_mgr* parser_get_obdd_mgr(){
	static obdd_mgr* parser_obdd_mgr	= NULL;
	if(parser_obdd_mgr == NULL)
		parser_obdd_mgr	= obdd_mgr_create();
	return parser_obdd_mgr;
}

uint32_t* parser_primed_variables	= NULL;
uint32_t parser_primed_variables_size	= 0;
uint32_t parser_primed_variables_count	= 0;

uint32_t* parser_get_primed_variables(){
	if(parser_primed_variables == NULL){
		parser_primed_variables_size	= LIST_INITIAL_SIZE;
		parser_primed_variables_count	= 0;
		parser_primed_variables			= malloc(sizeof(uint32_t) * parser_primed_variables_size);
	}
	return parser_primed_variables;
}

void parser_add_primed_variables(uint32_t primed_variable){
	int32_t last_less_than		= -1;
	int32_t i;
	parser_get_primed_variables();
	for(i = 0; i < (int32_t)parser_primed_variables_count; i++){
		if(parser_primed_variables[i] < primed_variable)last_less_than	= i;
		if(parser_primed_variables[i] == primed_variable)	return;
	}
	if((parser_primed_variables_count + 1) == parser_primed_variables_size){
		uint32_t new_size		= parser_primed_variables_size * LIST_INCREASE_FACTOR;
		uint32_t* ptr	= realloc(parser_primed_variables, new_size);
		if(ptr == NULL){
			printf("Could not allocate more space for primed variables list\n");
			exit(-1);
		}
		parser_primed_variables			= ptr;
		parser_primed_variables_size	= new_size;
	}
	for(i = parser_primed_variables_count; i > last_less_than && i > 0; i--){
		parser_primed_variables[i]		= parser_primed_variables[i - 1];
	}
	parser_primed_variables[last_less_than + 1]	= primed_variable;
	parser_primed_variables_count++;
}

void ltl_rule_syntax_destroy(ltl_rule_syntax* ltl_rule){
	if(ltl_rule->game_structure_name != NULL)free(ltl_rule->game_structure_name);
	if(ltl_rule->name != NULL)free(ltl_rule->name);
	if(ltl_rule->obdd && ltl_rule->obdd->mgr != NULL)
		obdd_destroy(ltl_rule->obdd);
	free(ltl_rule);
}

void ltl_fluent_syntax_destroy(ltl_fluent_syntax* ltl_fluent){
	if(ltl_fluent->name != NULL)free(ltl_fluent->name);
	obdd_destroy(ltl_fluent->obdd);
	free(ltl_fluent);
}
