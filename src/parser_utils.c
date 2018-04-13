#include "parser_utils.h"
#include "assert.h"
void aut_dupstr(char** dst, char* src){
	if(src != NULL){
		*dst = malloc(strlen(src) + 1);
		strcpy(*dst, src);
	}else{*dst = NULL;}
}
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
automaton_set_syntax* automaton_set_syntax_create_concurrent(char* string_terminal){
	automaton_set_syntax* set	= malloc(sizeof(automaton_set_syntax));
	set->is_ident	= false;
	set->count			= 1;
	set->labels_count	= malloc(sizeof(uint32_t) * set->count);
	set->labels_count[0]= 1;
	set->labels			= malloc(sizeof(automaton_label_syntax**) * set->count);
	set->labels[0][0]	= automaton_label_syntax_create(false, NULL, string_terminal);
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
	set->labels[0][set->labels_count[0] - 1]	= automaton_label_syntax_create(false, NULL, string_terminal);
	free(set->labels[0][i]);
	return set;
}
automaton_label_syntax* automaton_label_syntax_create(bool is_set, automaton_set_syntax* set, char* string_terminal){
	automaton_label_syntax* label	= malloc(sizeof(automaton_label_syntax));
	label->is_set	= is_set;
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
	set->is_ident	= false;
	set->count			= 1;
	set->labels_count	= malloc(sizeof(uint32_t) * set->count);
	set->labels_count[0]= 1;
	set->labels			= malloc(sizeof(automaton_label_syntax*) * set->count);
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
	set->labels[0][set->labels_count[0] - 1]	= label;
	free(set->labels[0][i]);
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
automaton_fluent_syntax* automaton_fluent_syntax_create(char* name, automaton_set_syntax* initiating_set, automaton_set_syntax* finishing_set){
	automaton_fluent_syntax* fluent	= malloc(sizeof(automaton_fluent_syntax));
	fluent->initiating_set	= initiating_set;
	fluent->finishing_set	= finishing_set;
	aut_dupstr(&(fluent->name), name);
	return fluent;
}
automaton_index_syntax* automaton_index_syntax_create(bool is_expr, bool is_range, automaton_expression_syntax* expr, char* lower_ident
		, char* upper_ident){
	automaton_index_syntax* index	= malloc(sizeof(automaton_index_syntax));

	if(is_expr){
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
	composition->name	= NULL;
	composition->components	= NULL;
	composition->count	= states->count;
	composition->states	= states->states;
	return composition;
}
automaton_composition_syntax* automaton_composition_syntax_create_from_ref(char* name, automaton_components_syntax* components){
	automaton_composition_syntax* composition	= malloc(sizeof(automaton_composition_syntax));
	aut_dupstr(&(composition->name), name);
	composition->components	= components->components;
	composition->count	= components->count;
	composition->states	= NULL;
	return composition;
}
automaton_components_syntax* automaton_components_syntax_create(automaton_component_syntax* component){
	automaton_components_syntax* components	= malloc(sizeof(automaton_components_syntax));
	components->count	= 1;
	components->components	= malloc(sizeof(automaton_component_syntax*) * components->count);
	components->components[0]	= component;
	return components;
}
automaton_components_syntax* automaton_components_syntax_add_component(automaton_components_syntax* components, automaton_component_syntax* component){
	components->count++;
	automaton_component_syntax** new_components	= malloc(sizeof(automaton_component_syntax*) * components->count);
	uint32_t i;
	for(i = 0; i < (components->count -1); i++)new_components[i] = components->components[i];
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
	return component;
}
