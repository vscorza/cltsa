#include "automaton.h"
#include "assert.h"

/** CLONING AND COPYING FUNCTIONS **/
automaton_signal_event* automaton_signal_event_clone(automaton_signal_event* source){
	automaton_signal_event* copy	= malloc(sizeof(automaton_signal_event));
	automaton_signal_event_copy(source, copy);
	return copy;
}
void automaton_signal_event_copy(automaton_signal_event* source,automaton_signal_event* target){
	target->name						= malloc(sizeof(char) * (strlen(source->name) + 1));
	strcpy(target->name, source->name);
	target->type						= source->type;
}
automaton_alphabet* automaton_alphabet_clone(automaton_alphabet* source){
	automaton_alphabet* copy		= malloc(sizeof(automaton_alphabet));
	automaton_alphabet_copy(source, copy);
	return copy;
}
void automaton_alphabet_copy(automaton_alphabet* source,automaton_alphabet* target){
	target->count						= source->count;
	target->size 						= source->size;
	target->list						= calloc(target->size, sizeof(automaton_signal_event));
	uint32_t i;
	for(i = 0; i < target->count; i++){
		automaton_signal_event_copy(&(source->list[i]), &(target->list[i]));
	}
}
automaton_transition* automaton_transition_clone(automaton_transition* source){
	automaton_transition* copy		= calloc(1, sizeof(automaton_transition));
	automaton_transition_copy(source, copy);
	return copy;
}
void automaton_transition_copy(automaton_transition* source, automaton_transition* target){
	target->state_from				= source->state_from;
	target->state_to				= source->state_to;
	target->signals_count			= source->signals_count;
	target->signals_size			= source->signals_size;
	target->is_input				= source->is_input;
	uint32_t i;
	for(i = 0; i < FIXED_SIGNALS_COUNT; i++)
		target->signals[i]			= source->signals[i];
	if(target->signals_size > FIXED_SIGNALS_COUNT){
		if(target->other_signals != NULL){free(target->other_signals);target->other_signals = NULL;}
		target->other_signals					= calloc((target->signals_size - FIXED_SIGNALS_COUNT), sizeof(signal_t) );
		for(i = 0; i < (target->signals_size - FIXED_SIGNALS_COUNT); i++){
			target->other_signals[i]			= source->other_signals[i];
		}
	}else{
		target->other_signals		= NULL;
	}
}
automaton_fluent* automaton_fluent_clone(automaton_fluent* source){
	automaton_fluent* copy			= malloc(sizeof(automaton_fluent));
	automaton_fluent_copy(source, copy);
	return copy;
}
void automaton_fluent_copy(automaton_fluent* source, automaton_fluent* target){
	target->name 					= malloc(sizeof(char) * (strlen(source->name) + 1));
	strcpy(target->name, source->name);
	uint32_t i, j;
	if(target->starting_signals_count > 0){
		for(i = 0; i < target->starting_signals_count; i++){
			free(target->starting_signals[i]);
		}
		free(target->starting_signals);
		free(target->starting_signals_element_count);
	}
	target->starting_signals_count	= source->starting_signals_count;
	target->starting_signals		= calloc(target->starting_signals_count, sizeof(signal_t*));
	target->starting_signals_element_count = calloc(target->starting_signals_count, sizeof(uint32_t));
	for(i = 0; i < target->starting_signals_count; i++){
		target->starting_signals_element_count[i]	= source->starting_signals_element_count[i];
		target->starting_signals[i]	= calloc(target->starting_signals_element_count[i], sizeof(signal_t));
		for(j = 0; j < target->starting_signals_element_count[i]; j++){
			target->starting_signals[i][j]	= source->starting_signals[i][j];
		}
	}
	if(target->ending_signals_count > 0){
		for(i = 0; i < target->ending_signals_count; i++){
			free(target->ending_signals[i]);
		}
		free(target->ending_signals);
		free(target->ending_signals_element_count);
	}
	target->ending_signals_count	= source->ending_signals_count;
	target->ending_signals		= calloc(target->ending_signals_count, sizeof(signal_t*));
	target->ending_signals_element_count = calloc(target->ending_signals_count, sizeof(uint32_t));
	for(i = 0; i < target->ending_signals_count; i++){
		target->ending_signals_element_count[i]	= source->ending_signals_element_count[i];
		target->ending_signals[i]	= calloc(target->ending_signals_element_count[i], sizeof(signal_t));
		for(j = 0; j < target->ending_signals_element_count[i]; j++){
			target->ending_signals[i][j]	= source->ending_signals[i][j];
		}
	}

	target->initial_valuation		= source->initial_valuation;
}
automaton_valuation* automaton_valuation_clone(automaton_valuation* source){
	automaton_valuation*	copy	= malloc(sizeof(automaton_valuation));
	automaton_valuation_copy(source, copy);
	return copy;
}
void automaton_valuation_copy(automaton_valuation* source, automaton_valuation* target){
	target->state					= source->state;
	target->active_fluents_count			= source->active_fluents_count;
	target->active_fluents			= malloc(sizeof(uint32_t) * target->active_fluents_count);
	uint32_t i;
	for(i = 0; i < target->active_fluents_count; i++){
		target->active_fluents[i]	= source->active_fluents[i];	
	}
}
automaton_automata_context* automaton_automata_context_clone(automaton_automata_context* source){
	automaton_automata_context*	copy	= malloc(sizeof(automaton_automata_context));
	automaton_automata_context_copy(source, copy);
	return copy;
}
void automaton_automata_context_copy(automaton_automata_context* source, automaton_automata_context* target){
	target->name					= malloc(sizeof(char) * (strlen(source->name) + 1));
	strcpy(target->name, source->name);
	target->global_alphabet			= automaton_alphabet_clone(source->global_alphabet);
	target->global_fluents_count	= source->global_fluents_count;
	target->global_fluents			= malloc(sizeof(automaton_fluent) * target->global_fluents_count);
	uint32_t i;
	for(i = 0; i < target->global_fluents_count; i++){
		automaton_fluent_copy(&(source->global_fluents[i]), &(target->global_fluents[i]));	
	}
	target->liveness_valuations_count	= source->liveness_valuations_count;
	target->liveness_valuations			= malloc(sizeof(obdd*) * target->liveness_valuations_count);
	target->liveness_valuations_names	= malloc(sizeof(char*) * target->liveness_valuations_count);
	for(i = 0; i < target->liveness_valuations_count; i++){
		target->liveness_valuations[i]	= obdd_clone(source->liveness_valuations[i]);
		aut_dupstr(&(target->liveness_valuations_names[i]), source->liveness_valuations_names[i]);
	}
}
automaton_automaton* automaton_automaton_clone(automaton_automaton* source){
	automaton_automaton* copy		= malloc(sizeof(automaton_automaton));
	automaton_automaton_copy(source, copy);
	return copy;
}
void automaton_automaton_copy(automaton_automaton* source, automaton_automaton* target){
	uint32_t i, j, in_degree, out_degree, in_size, out_size;
	target->name					= malloc(sizeof(char) * (strlen(source->name) + 1));
	target->is_game					= source->is_game;
	target->built_from_ltl			= source->built_from_ltl;
	strcpy(target->name, source->name);
	target->context					= source->context;
	target->local_alphabet_count	= source->local_alphabet_count;
	target->local_alphabet			= malloc(sizeof(uint32_t) * target->local_alphabet_count);
	for(i = 0; i < target->local_alphabet_count; i++){ target->local_alphabet[i]	= source->local_alphabet[i]; }
	target->transitions_composite_count	= source->transitions_composite_count;
	target->transitions_count		= source->transitions_count;
	target->transitions_size		= source->transitions_size;
	target->in_degree				= malloc(sizeof(uint32_t) * target->transitions_size);
	target->in_size					= malloc(sizeof(uint32_t) * target->transitions_size);
	target->max_out_degree			= source->max_out_degree;
	target->max_concurrent_degree	= source->max_concurrent_degree;
	target->is_controllable			= malloc(sizeof(bool) * target->transitions_size);
	target->out_degree				= malloc(sizeof(uint32_t) * target->transitions_size);
	target->out_size				= malloc(sizeof(uint32_t) * target->transitions_size);
	target->transitions				= malloc(sizeof(automaton_transition*) * target->transitions_size);
	target->inverted_transitions	= malloc(sizeof(automaton_transition*) * target->transitions_size);
	for(i = 0; i < target->transitions_size; i++){
		target->is_controllable[i]	= source->is_controllable[i];
		target->in_degree[i]	= source->in_degree[i];
		target->in_size[i]		= source->in_size[i];
		in_degree				= target->in_degree[i];
		in_size					= target->in_size[i];
		if(in_size > 0){
			target->inverted_transitions[i]	= calloc(in_size, sizeof(automaton_transition));
			for(j = 0; j < in_degree; j++){
				automaton_transition_copy(&(source->inverted_transitions[i][j]), &(target->inverted_transitions[i][j]));
			}
		}
		target->out_degree[i]	= source->out_degree[i];
		target->out_size[i]		= source->out_size[i];
		out_degree				= target->out_degree[i];
		out_size				= target->out_size[i];
		if(out_size > 0){
			target->transitions[i]	= calloc(out_size, sizeof(automaton_transition));
			for(j = 0; j < out_degree; j++){
				automaton_transition_copy(&(source->transitions[i][j]), &(target->transitions[i][j]));
			}
		}
	}
	target->initial_states_count	= source->initial_states_count;
	target->initial_states			= malloc(sizeof(uint32_t) * target->initial_states_count);
	for(i = 0; i < target->initial_states_count; i++){
		target->initial_states[i]	= source->initial_states[i];
	}
	target->is_game					= source->is_game;
	if(source->is_game){
		target->valuations_size			= source->valuations_size;
		target->valuations				= malloc(sizeof(uint32_t) * target->valuations_size);
		for(i = 0; i < target->valuations_size; i++){
			target->valuations[i]		= source->valuations[i];
		}
		target->liveness_valuations_size	= source->liveness_valuations_size;
		target->liveness_valuations			= calloc(target->liveness_valuations_size, sizeof(uint32_t));
		for(i = 0; i < target->liveness_valuations_size; i++){
				target->liveness_valuations[i]		= source->liveness_valuations[i];
			}
		target->inverted_valuations						= calloc(target->context->global_fluents_count, sizeof(automaton_bucket_list*));
		for(i = 0; i < target->context->global_fluents_count; i++){
			target->inverted_valuations[i]			= automaton_bucket_list_clone(source->inverted_valuations[i]);
		}
		target->liveness_inverted_valuations			= calloc(target->context->liveness_valuations_count, sizeof(automaton_bucket_list*));
		for(i = 0; i < target->context->liveness_valuations_count; i++){
			target->liveness_inverted_valuations[i]	= automaton_bucket_list_clone(source->liveness_inverted_valuations[i]);
		}
	}
}
automaton_automata* automaton_automata_clone(automaton_automata* source){
	automaton_automata* copy		= malloc(sizeof(automaton_automata));
	automaton_automata_copy(source, copy);
	return copy;
}
void automaton_automata_copy(automaton_automata* source, automaton_automata* target){
	target->automata_count			= source->automata_count;
	target->automata_list			= malloc(sizeof(automaton_automaton) * target->automata_count);
	uint32_t i,j;
	for(i = 0; i < target->automata_count; i++){
		target->automata_list[i]	= source->automata_list[i];	
	}
	target->states_explored_count	= source->states_explored_count;
	target->states_explored_size	= source->states_explored_size;
	target->states_explored			= malloc(sizeof(uint32_t) * target->automata_count * target->states_explored_size);
	target->states_decomposed		= malloc(sizeof(uint32_t) * target->states_explored_size);
	target->states_valuations		= malloc(sizeof(automaton_valuation) * target->states_explored_size);
	for(i = 0; i < target->states_explored_count; i++){
		for(j = 0; j < target->automata_count; j++){
			target->states_explored[i][j]	= source->states_explored[i][j];	
		}
		target->states_decomposed[i]	= source->states_decomposed[i];
		automaton_valuation_copy(&(source->states_valuations[i]),&(target->states_valuations[i]));
	}
}
automaton_range* automaton_range_clone(automaton_range* source){
	automaton_range* copy	= malloc(sizeof(automaton_range));
	automaton_range_copy(source, copy);
	return copy;
}
void automaton_range_copy(automaton_range* source, automaton_range* target){
	target->name					= malloc(sizeof(char) * (strlen(source->name) + 1));
	strcpy(target->name, source->name);
	target->lower_value				= source->lower_value;
	target->upper_value				= source->upper_value;
}
automaton_indexes_valuation* automaton_indexes_valuation_clone(automaton_indexes_valuation* source){
	automaton_indexes_valuation* copy	= malloc(sizeof(automaton_indexes_valuation));
	automaton_indexes_valuation_copy(source, copy);
	return copy;
}
void automaton_indexes_valuation_copy(automaton_indexes_valuation* source, automaton_indexes_valuation* target){
	target->count					= source->count;
	uint32_t i;
	target->current_values			= malloc(sizeof(int32_t) * target->count);
	for(i = 0; i < target->count; i++){
		target->current_values[i]	= source->current_values[i];
	}
	target->ranges					= malloc(sizeof(automaton_range*) * target->count);
	for(i = 0; i < target->count; i++){
		target->ranges[i]			= automaton_range_clone(source->ranges[i]);
	}
	target->current_combination		= source->current_combination;
	target->total_combinations		= source->total_combinations;
}
/** PRINTING FUNCTIONS **/
void automaton_signal_type_print(automaton_signal_type type, char* prefix, char* suffix){
	if(prefix != NULL)
		printf("%s", prefix);
	switch(type){
		case INPUT_SIG: printf("[INPUT]"); break;
		case OUTPUT_SIG: printf("[OUTPUT]"); break;
		case INTERNAL_SIG: printf("[INTERNAL]"); break;
		default: printf("[UNKNOWN]"); break;
	}
	if(suffix != NULL)
		printf("%s", suffix);
}
void automaton_signal_event_print(automaton_signal_event* signal_event, char* prefix, char* suffix){
	if(prefix != NULL)
		printf("%s", prefix);
	printf("%s", signal_event->name);
	automaton_signal_type_print(signal_event->type, "\t,","");
	if(suffix != NULL)
		printf("%s", suffix);
}
void automaton_alphabet_print(automaton_alphabet* alphabet, char* prefix, char* suffix){
	if(prefix != NULL)
		printf("%s", prefix);
	uint32_t i;
	if(prefix == NULL)
		prefix = "\t";
	if(suffix == NULL)
		suffix = "";
	char	prefix2[255];
	strcpy(prefix2, prefix);
	strcat(prefix2, "\t");
	printf("Alphabet:\n");
	for(i = 0; i < alphabet->count; i++){
		automaton_signal_event_print(&(alphabet->list[i]), prefix2, (i < (alphabet->count - 1)? "\n":""));
	}
	if(suffix != NULL)
		printf("%s", suffix);
}

void automaton_transition_print(automaton_transition* transition, automaton_automata_context* ctx, char* prefix, char* suffix){
	if(prefix != NULL)
		printf("%s", prefix);
	uint32_t i;
	printf("(%d {", transition->state_from);
	for(i = 0; i < transition->signals_count; i++){
		signal_t sig = i < FIXED_SIGNALS_COUNT ? transition->signals[i] : transition->other_signals[i-FIXED_SIGNALS_COUNT];
		printf("%s%s", ctx->global_alphabet->list[sig].name, (i < (transition->signals_count -1)? "," : ""));
	}
	printf("}-> %d)", transition->state_to);
	if(suffix != NULL)
		printf("%s", suffix);
}

void automaton_fluent_print(automaton_fluent* fluent, automaton_automata_context* ctx, char* prefix, char* suffix){
	if(prefix != NULL)
		printf("%s", prefix);
	uint32_t i,j;

	automaton_alphabet*	alphabet	= ctx->global_alphabet;

	printf("Fluent %s:<[", fluent->name);
	for(i = 0; i < fluent->starting_signals_count; i++){
		printf("{");
		for(j = 0; j < fluent->starting_signals_element_count[i]; j++){
			printf("%s%s", alphabet->list[fluent->starting_signals[i][j]].name, (j < (fluent->starting_signals_element_count[i] -1)? "," : ""));
		}
		printf("}%s", i < fluent->starting_signals_count - 1 ? ",":"");
	}
	printf("],[");
	for(i = 0; i < fluent->ending_signals_count; i++){
		printf("{");
		for(j = 0; j < fluent->ending_signals_element_count[i]; j++){
			printf("%s%s", alphabet->list[fluent->ending_signals[i][j]].name, (j < (fluent->ending_signals_element_count[i] -1)? "," : ""));
		}
		printf("}%s", i < fluent->ending_signals_count - 1 ? ",":"");
	}
	printf("]> initially %s", (fluent->initial_valuation? "true":"false"));
	if(suffix != NULL)
		printf("%s", suffix);
}

void automaton_valuation_print(automaton_valuation* valuation, automaton_automata_context* ctx, char* prefix, char* suffix){
	if(prefix != NULL)
		printf("%s", prefix);
	uint32_t i;

	automaton_fluent* fluents		= ctx->global_fluents;

	printf("(%d, <", valuation->state);
	for(i = 0; i < valuation->active_fluents_count; i++){
		printf("%s%s", fluents[valuation->active_fluents[i]].name, (i < (valuation->active_fluents_count -1)? "," : ""));
	}
	printf(">)");
	if(suffix != NULL)
		printf("%s", suffix);	
}

void automaton_automata_context_print(automaton_automata_context* ctx, char* prefix, char* suffix){
	if(prefix != NULL)
		printf("%s", prefix);
	uint32_t i;
	if(prefix == NULL)
		prefix = "\t";
	if(suffix == NULL)
		suffix = "";
	char	prefix2[255];
	strcpy(prefix2, prefix);
	strcat(prefix2, "\t");
	printf("Ctx.%s:\n", ctx->name);
	automaton_alphabet_print(ctx->global_alphabet, prefix2, "\n");
	printf("%sFluents:\n", prefix2);
	for(i = 0; i < ctx->global_fluents_count; i++){
		automaton_fluent_print(&(ctx->global_fluents[i]), ctx, prefix2, "\n");
	}
	printf("%sLiveness Valuations:\n", prefix2);
	char *buff = calloc(1024, sizeof(char));
	for(i = 0; i < ctx->liveness_valuations_count; i++){
		printf("%s:\n", ctx->liveness_valuations_names[i]);
		obdd_print(ctx->liveness_valuations[i], buff, 1024);
		printf("%s", buff);
		buff[0] = '\0';
	}
	free(buff);
	if(suffix != NULL)
		printf("%s", suffix);	
}


void automaton_automaton_print(automaton_automaton* current_automaton, bool print_ctx, bool print_alphabet, bool print_valuations, char* prefix, char* suffix){
	if(prefix != NULL)
		printf("%s", prefix);
	uint32_t i, j;
	if(prefix == NULL)
		prefix = "\t";
	if(suffix == NULL)
		suffix = "";
	char	prefix2[255];
	strcpy(prefix2, prefix);
	strcat(prefix2,  "\t");
	printf("Aut.%s:\n", current_automaton->name);
	automaton_automata_context* ctx		= current_automaton->context;
	if(print_ctx){ automaton_automata_context_print(current_automaton->context, prefix2, "\n");}
	if(print_alphabet){
		printf("%sLocal Alphabet:{", prefix2);
		for(i = 0; i < current_automaton->local_alphabet_count; i++){
			printf("%s[%d]%s", ctx->global_alphabet->list[current_automaton->local_alphabet[i]].name
					, current_automaton->local_alphabet[i],(i < (current_automaton->local_alphabet_count -1)? "," : ""));
		}
		printf("}\n");
	}
	if(print_valuations && current_automaton->is_game){
		printf("%sValuations:\n%s", prefix2, prefix2);
		for(j = 0; j < ctx->global_fluents_count; j++){
			printf(ctx->global_fluents[j].name);
			if(j < (ctx->global_fluents_count - 1))printf(",");
		}
		printf("\n%s", prefix2);
		uint32_t fluent_index;
		bool satisfies_valuation;
		for(i = 0; i < current_automaton->transitions_count; i ++){
			printf("s_%d:", i);
			for(j = 0; j < ctx->global_fluents_count; j++){
				fluent_index		= GET_STATE_FLUENT_INDEX(ctx->global_fluents_count, i, j);
				satisfies_valuation	= TEST_FLUENT_BIT(current_automaton->valuations, fluent_index);
				printf(satisfies_valuation? "1" : "0");
			}
			printf(" ");
			for(j = 0; j < ctx->liveness_valuations_count; j++){
				fluent_index		= GET_STATE_FLUENT_INDEX(ctx->liveness_valuations_count, i, j);
				satisfies_valuation	= TEST_FLUENT_BIT(current_automaton->liveness_valuations, fluent_index);
				printf(satisfies_valuation? "1" : "0");
			}
			printf("\n%s", prefix2);
		}
		printf("\n");
		printf("%sInv. Valuations:\n", prefix2);
		for(i = 0; i < current_automaton->context->global_fluents_count; i++){
			printf("%sFluent\t%s:", prefix2, current_automaton->context->global_fluents[i].name);
			//TODO: print inverted fluent bucket
			/*
			for(j = 0; j < current_automaton->inverted_valuations_counts[i]; j++){
				printf("%d%s", current_automaton->inverted_valuations[i][j], j < (current_automaton->inverted_valuations_counts[i] -1 )? ",":"");
			}
			*/
			printf("\n");
		}

	}
	printf("%sTransitions:\n", prefix2);
	for(i = 0; i < current_automaton->transitions_count; i++){
		for(j = 0; j < current_automaton->out_degree[i]; j++)
		automaton_transition_print(&(current_automaton->transitions[i][j]), ctx, prefix2, "\n");
	}
	printf("%sInitial States:{", prefix2);
	for(i = 0; i < current_automaton->initial_states_count; i++){
		printf("%d%s", current_automaton->initial_states[i], (i < (current_automaton->initial_states_count -1)? "," : ""));
	}
	printf("}\n");
	if(suffix != NULL)
		printf("%s", suffix);		
}

bool automaton_automaton_print_fsp(automaton_automaton* current_automaton, char* filename){
	FILE *f = fopen(filename, "w");
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    return false;
	}

	fprintf(f, "/* AUTO-GENERATED_FILE\n\tAutomaton %s */\n", current_automaton->name);
	automaton_automata_context* ctx		= current_automaton->context;
	uint32_t i,j,k;
	for(i = 0; i < current_automaton->initial_states_count; i++){
		fprintf(f, "A = S_%d,\n", current_automaton->initial_states[i]);
	}
	bool first_line = true;
	automaton_transition* current_transition;
	for(i = 0; i < current_automaton->transitions_count; i++){
		if(first_line){
			first_line = false;
		}else if(current_automaton->out_degree[i] > 0){
			fprintf(f, ",\n");
		}
		for(j = 0; j < current_automaton->out_degree[i]; j++){
			current_transition	= &(current_automaton->transitions[i][j]);
			if(j == 0){fprintf(f, "S_%d = (", current_transition->state_from);}
			if(current_transition->signals_count == 0)fprintf(f, AUT_TAU_CONSTANT);
			if(current_transition->signals_count > 1)fprintf(f, "<");
			for(k = 0; k < current_transition->signals_count; k++){
				signal_t sig	= k < FIXED_SIGNALS_COUNT ? current_transition->signals[k] : current_transition->other_signals[k - FIXED_SIGNALS_COUNT];
				fprintf(f,"%s%s", ctx->global_alphabet->list[sig].name, (k < (current_transition->signals_count -1)? "," : ""));
			}
			if(current_transition->signals_count > 1)fprintf(f, ">");
			fprintf(f,"-> S_%d", current_transition->state_to);
			if(j < (current_automaton->out_degree[i] - 1))fprintf(f, "|");
			if(j == (current_automaton->out_degree[i] - 1))fprintf(f, ")");
		}
	}
	fprintf(f, ".");
	fclose(f);
	return true;
}

bool automaton_automaton_print_dot(automaton_automaton* current_automaton, char* filename){
	FILE *f = fopen(filename, "w");
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    return false;
	}

	fprintf(f, "/* AUTO-GENERATED_FILE\n\tAutomaton %s */\n", current_automaton->name);
	fprintf(f, "digraph %s {\n\tgraph [fontname=Arial,fontsize=10,layout = circo, overlap=false,splines=true];\n\t node [shape = circle, label=\"STOP\", fontsize=14] STOP; \n\t node [shape = point ]; qi \n", current_automaton->name);

	automaton_automata_context* ctx		= current_automaton->context;
	uint32_t i,j,k;
	for(i = 0; i < current_automaton->transitions_count; i++){
		fprintf(f, "\tnode [shape = circle, label=\"S_%d\", fontsize=14] S_%d\n", i, i);
	}
	for(i = 0; i < current_automaton->initial_states_count; i++){
		fprintf(f, "\tqi -> S_%d;\n", current_automaton->initial_states[i]);
	}
	automaton_transition* current_transition;
	for(i = 0; i < current_automaton->transitions_count; i++){
		if(current_automaton->out_degree[i] <= 0){
			fprintf(f, "\tS_%d  -> STOP;\n", i);

		}
		for(j = 0; j < current_automaton->out_degree[i]; j++){
			current_transition	= &(current_automaton->transitions[i][j]);
			fprintf(f, "\tS_%d -> S_%d [ label = \"", current_transition->state_from, current_transition->state_to);
			if(current_transition->signals_count == 0)fprintf(f, "%s", AUT_TAU_CONSTANT);
			for(k = 0; k < current_transition->signals_count; k++){
				signal_t sig	= k < FIXED_SIGNALS_COUNT ? current_transition->signals[k] : current_transition->other_signals[k - FIXED_SIGNALS_COUNT];
				fprintf(f,"%s%s", ctx->global_alphabet->list[sig].name, (k < (current_transition->signals_count -1)? "," : ""));
			}
			fprintf(f, "\"];\n");
		}
	}
	fprintf(f, "}\n");
	fclose(f);
	return true;
}

void automaton_range_print(automaton_range* range, char* prefix, char* suffix){
	if(prefix != NULL)
		printf("%s", prefix);
	uint32_t i, j;
	if(prefix == NULL)
		prefix = "\t";
	if(suffix == NULL)
		suffix = "";
	char	prefix2[255];
	strcpy(prefix2, prefix);
	strcat(prefix2,  "\t");
	printf("Range.%s:{lower:%d,upper:%d}\n", range->name, range->lower_value, range->upper_value);
	if(suffix != NULL)
		printf("%s", suffix);
}
void automaton_indexes_valuation_print(automaton_indexes_valuation* valuation, char* prefix, char* suffix){
	if(prefix != NULL)
		printf("%s", prefix);
	uint32_t i;
	if(prefix == NULL)
		prefix = "\t";
	if(suffix == NULL)
		suffix = "";
	char	prefix2[255];
	strcpy(prefix2, prefix);
	strcat(prefix2,  "\t");
	printf("Indexes valuation:{[");
	for(i = 0; i < valuation->count; i++){
		printf("%d:%s%s", valuation->current_values[i], valuation->ranges[i]->name, i < (valuation->count - 1) ? "," : "");
	}
	printf(")]}\n");
	if(suffix != NULL)
		printf("%s", suffix);
}
/** SERIALIZING FUNCTIONS **/
//<name,type>
void automaton_signal_event_serialize_report(FILE *f, automaton_signal_event *evt){
	fprintf(f, "%s%s%s%d%s", AUT_SER_OBJ_START, evt->name, AUT_SER_SEP, evt->type, AUT_SER_OBJ_END);
}
//<name,starting_count,[f_1,..,f_n],ending_count,[f'_1,..,f'_m],initially>
void automaton_fluent_serialize_report(FILE *f, automaton_fluent *fluent){
	fprintf(f, "%s%s%s%d%s%s", AUT_SER_OBJ_START, fluent->name, AUT_SER_SEP, fluent->starting_signals_count, AUT_SER_SEP, AUT_SER_ARRAY_START);
	uint32_t i,j;
	for(i = 0; i < fluent->starting_signals_count; i++){
		fprintf(f, "%d%s%s", fluent->starting_signals_element_count[i], AUT_SER_SEP, AUT_SER_ARRAY_START);
		for(j = 0; j < fluent->starting_signals_element_count[i]; j++)
			fprintf(f, "%d%s", fluent->starting_signals[i][j], j == (fluent->starting_signals_element_count[i] - 1)? "" :  AUT_SER_SEP);
		fprintf(f, "%s%s", AUT_SER_ARRAY_END, i == (fluent->starting_signals_count - 1) ? "" : AUT_SER_SEP, AUT_SER_ARRAY_START);
	}
	fprintf(f, "%s%s%d%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, fluent->ending_signals_count, AUT_SER_SEP, AUT_SER_ARRAY_START);
	for(i = 0; i < fluent->ending_signals_count; i++){
		fprintf(f, "%d%s%s", fluent->ending_signals_element_count[i], AUT_SER_SEP, AUT_SER_ARRAY_START);
		for(j = 0; j < fluent->ending_signals_element_count[i]; j++)
			fprintf(f, "%d%s", fluent->ending_signals[i][j], j == (fluent->ending_signals_element_count[i] - 1)? "" :  AUT_SER_SEP);
		fprintf(f, "%s%s", AUT_SER_ARRAY_END, i == (fluent->ending_signals_count - 1) ? "" : AUT_SER_SEP, AUT_SER_ARRAY_START);
	}
	fprintf(f, "%s%s%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, fluent->initial_valuation? "1" : "0", AUT_SER_OBJ_END);
}
//<name,alphabet,fluents_count,[f_1.name,..,f_n.name],liveness_count,[l_1.name,..,l_m.name]>
void automaton_automata_context_serialize_report(FILE *f, automaton_automata_context *ctx){
	fprintf(f, "%s%s%s", AUT_SER_OBJ_START, ctx->name, AUT_SER_SEP);
	automaton_alphabet_serialize_report(f, ctx->global_alphabet);
	fprintf(f, "%s%d%s%s", AUT_SER_SEP, ctx->global_fluents_count, AUT_SER_SEP, AUT_SER_ARRAY_START);
	uint32_t i;
	for(i = 0; i < ctx->global_fluents_count; i++){
		fprintf(f, "%s%s", ctx->global_fluents[i].name, i == (ctx->global_fluents_count - 1)? "" :  AUT_SER_SEP);
	}
	fprintf(f, "%s%s%d%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, ctx->liveness_valuations_count, AUT_SER_SEP, AUT_SER_ARRAY_START);
	for(i = 0; i < ctx->liveness_valuations_count; i++){
		fprintf(f, "%s%s", ctx->liveness_valuations_names[i], i == (ctx->liveness_valuations_count - 1)? "" :  AUT_SER_SEP);
	}
	fprintf(f, "%s%s", AUT_SER_ARRAY_END, AUT_SER_OBJ_END);
}
//<count,[sig_1,..,sig_count]>
void automaton_alphabet_serialize_report(FILE *f, automaton_alphabet *alphabet){
	fprintf(f, "%s%d%s%s", AUT_SER_OBJ_START, alphabet->count, AUT_SER_SEP, AUT_SER_ARRAY_START);
	uint32_t i;
	for(i = 0; i < alphabet->count; i++){
		automaton_signal_event_serialize_report(f, &(alphabet->list[i]));
		fprintf(f, "%s", i == (alphabet->count - 1)? "" :  AUT_SER_SEP);
	}
	fprintf(f, "%s%s", AUT_SER_ARRAY_END, AUT_SER_OBJ_END);
}
//<from,to,sig_count,[s_1.idx,..,s_N.idx],is_input>
void automaton_transition_serialize_report(FILE *f, automaton_transition *transition){
	fprintf(f, "%s%d%s%d%s%d%s%s", AUT_SER_OBJ_START, transition->state_from, AUT_SER_SEP, transition->state_to, AUT_SER_SEP, transition->signals_count, AUT_SER_SEP, AUT_SER_ARRAY_START);
	uint32_t i;
	for(i = 0; i < transition->signals_count; i++){
		fprintf(f, "%d%s", ((i >= FIXED_SIGNALS_COUNT) ? (transition->other_signals[i - FIXED_SIGNALS_COUNT]) : transition->signals[i]), i == (transition->signals_count - 1)? "" :  AUT_SER_SEP);
	}
		fprintf(f, "%s%s%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, transition->is_input? "1" :"0",AUT_SER_OBJ_END);
}
/*
 * <name,ctx,local_alphabet_count,[sig_1.idx,..,sig_N.idx],trans_count,[trans_1,..,trans_M],init_count,[init_i,..,init_K],[[val_s_0_f_0,..,val_s_0_f_L],..,[val_s_T_f_0,..,val_s_T_f_L]]
,[[val_s_0_l_0,..,val_s_0_l_R],..,[val_s_T_l_0,..,val_s_T_l_R]]>
 */
void automaton_automaton_serialize_report(FILE *f, automaton_automaton *automaton){
	fprintf(f, "%s%s%s", AUT_SER_OBJ_START, automaton->name, AUT_SER_SEP);
	automaton_automata_context_serialize_report(f, automaton->context);
	fprintf(f, "%s%d%s%s", AUT_SER_SEP, automaton->local_alphabet_count, AUT_SER_SEP, AUT_SER_ARRAY_START);
	uint32_t i, j;
	uint32_t fluent_index;
	uint32_t fluent_count	= automaton->context->global_fluents_count;
	uint32_t liveness_count	= automaton->context->liveness_valuations_count;
	for(i = 0; i < automaton->local_alphabet_count; i++){
		fprintf(f, "%d%s", automaton->local_alphabet[i], i == (automaton->local_alphabet_count - 1)? "" :  AUT_SER_SEP);
	}
	fprintf(f, "%s%s%d%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, automaton->transitions_count, AUT_SER_SEP, AUT_SER_ARRAY_START);
	uint32_t current_count	= 0;
	bool first_transition	= true;
	for(i = 0; i < automaton->transitions_count; i++){
		for(j = 0; j < automaton->out_degree[i]; j++){
			if(&(automaton->transitions[i][j]) != NULL){
				if(first_transition)first_transition = false;
				else fprintf(f, "%s",AUT_SER_SEP);
				automaton_transition_serialize_report(f, &(automaton->transitions[i][j]));
			}else{
				printf("ERROR: Transition was null at [%d][%d]\n", i, j);
			}
			current_count++;
		}
	}
	fprintf(f, "%s%s%d%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, automaton->initial_states_count, AUT_SER_SEP, AUT_SER_ARRAY_START);
	for(i = 0; i < automaton->initial_states_count; i++){
		fprintf(f, "%d%s", automaton->initial_states[i], i == (automaton->initial_states_count - 1)? "" :  AUT_SER_SEP);
	}
	fprintf(f, "%s%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, AUT_SER_ARRAY_START);
	for(i = 0; i < automaton->transitions_count; i++){
		fprintf(f, "%s", AUT_SER_ARRAY_START);
		for(j = 0; j < fluent_count; j++){
			if(automaton->is_game){
				fluent_index	= GET_STATE_FLUENT_INDEX(fluent_count, i, j);
				fprintf(f, "%s%s", TEST_FLUENT_BIT(automaton->valuations, fluent_index) ? "1" : "0", j == (fluent_count - 1)? "" :  AUT_SER_SEP);
			}else{
				fprintf(f, "%s%s", "0", j == (fluent_count - 1)? "" :  AUT_SER_SEP);
			}
		}
		fprintf(f, "%s%s", AUT_SER_ARRAY_END, i == (automaton->transitions_count - 1) ? "" : AUT_SER_SEP);
	}
	fprintf(f, "%s%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, AUT_SER_ARRAY_START);
	for(i = 0; i < automaton->transitions_count; i++){
		fprintf(f, "%s", AUT_SER_ARRAY_START);
		for(j = 0; j < liveness_count; j++){
			if(automaton->is_game){
				fluent_index	= GET_STATE_FLUENT_INDEX(liveness_count, i, j);
				fprintf(f, "%s%s", TEST_FLUENT_BIT(automaton->liveness_valuations, fluent_index) ? "1" : "0", j == (liveness_count - 1)? "" :  AUT_SER_SEP);
			}else{
				fprintf(f, "%s%s", "0", j == (fluent_count - 1)? "" :  AUT_SER_SEP);
			}

		}
		fprintf(f, "%s%s", AUT_SER_ARRAY_END, i == (automaton->transitions_count - 1) ? "" : AUT_SER_SEP);
	}

	fprintf(f, "%s%s", AUT_SER_ARRAY_END, AUT_SER_OBJ_END);
}
void automaton_ranking_alphabet_serialize_report(FILE *f, automaton_alphabet *alphabet, uint32_t max_delta){
	fprintf(f, "%s%d%s%s", AUT_SER_OBJ_START, alphabet->count + max_delta + 1, AUT_SER_SEP, AUT_SER_ARRAY_START);
	uint32_t i;
	for(i = 0; i < alphabet->count; i++){
		automaton_signal_event_serialize_report(f, &(alphabet->list[i]));
		fprintf(f, "%s", AUT_SER_SEP);
	}
	fprintf(f, "%sinf%s0%s%s", AUT_SER_OBJ_START, AUT_SER_SEP, AUT_SER_OBJ_END, AUT_SER_SEP);
	for(i = 0; i < max_delta; i++){
		fprintf(f, "%sd%d%s0%s", AUT_SER_OBJ_START, i + 1,AUT_SER_SEP, AUT_SER_OBJ_END);
		fprintf(f, "%s", i == (max_delta - 1)? "" :  AUT_SER_SEP);
	}
	fprintf(f, "%s%s", AUT_SER_ARRAY_END, AUT_SER_OBJ_END);
}
void automaton_ranking_automata_context_serialize_report(FILE *f, automaton_automata_context *ctx, uint32_t max_delta){
	fprintf(f, "%s%s%s", AUT_SER_OBJ_START, ctx->name, AUT_SER_SEP);
	automaton_ranking_alphabet_serialize_report(f, ctx->global_alphabet, max_delta);
	fprintf(f, "%s%d%s%s", AUT_SER_SEP, ctx->global_fluents_count, AUT_SER_SEP, AUT_SER_ARRAY_START);
	uint32_t i;
	for(i = 0; i < ctx->global_fluents_count; i++){
		fprintf(f, "%s%s", ctx->global_fluents[i].name, i == (ctx->global_fluents_count - 1)? "" :  AUT_SER_SEP);
	}
	fprintf(f, "%s%s%d%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, ctx->liveness_valuations_count, AUT_SER_SEP, AUT_SER_ARRAY_START);
	for(i = 0; i < ctx->liveness_valuations_count; i++){
		fprintf(f, "%s%s", ctx->liveness_valuations_names[i], i == (ctx->liveness_valuations_count - 1)? "" :  AUT_SER_SEP);
	}
	fprintf(f, "%s%s", AUT_SER_ARRAY_END, AUT_SER_OBJ_END);
}
void automaton_ranking_transition_serialize_report(FILE *f, automaton_transition *transition,uint32_t ranking_value){
	fprintf(f, "%s%d%s%d%s%d%s%s", AUT_SER_OBJ_START, transition->state_from, AUT_SER_SEP, transition->state_to, AUT_SER_SEP, 1 /*transition->signals_count + 1*/, AUT_SER_SEP, AUT_SER_ARRAY_START);
	uint32_t i;
	/*
	for(i = 0; i < transition->signals_count; i++){
		fprintf(f, "%d%s", ((i >= FIXED_SIGNALS_COUNT) ? (transition->other_signals[i - FIXED_SIGNALS_COUNT]) : transition->signals[i]),AUT_SER_SEP);
	}
	*/
	fprintf(f,"%d", ranking_value);
	fprintf(f, "%s%s%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, transition->is_input? "1" :"0",AUT_SER_OBJ_END);
}
bool automaton_ranking_print_report(automaton_automaton *automaton,
		automaton_concrete_bucket_list** ranking_list, uint32_t* max_delta, uint32_t guarantee_count){
	uint32_t name_len = strlen(automaton->name) + 30;
	char *filename = calloc(name_len, sizeof(char));
	FILE *f = NULL;
	uint32_t r;
	for(r = 0; r < guarantee_count; r++){
		snprintf(filename, name_len, "results/%s_ranking_%d.rep", automaton->name, r);
		f = fopen(filename, "w");

		if (f == NULL)
		{
			printf("Error opening file!\n");
			return false;
		}

		fprintf(f, "%s%s_ranking_%d%s", AUT_SER_OBJ_START, automaton->name, r, AUT_SER_SEP);
		automaton_ranking_automata_context_serialize_report(f, automaton->context, max_delta[r]);
		fprintf(f, "%s%d%s%s", AUT_SER_SEP, automaton->local_alphabet_count + max_delta[r], AUT_SER_SEP, AUT_SER_ARRAY_START);
		uint32_t i, j;
		uint32_t fluent_index;
		uint32_t fluent_count	= automaton->context->global_fluents_count;
		uint32_t liveness_count	= automaton->context->liveness_valuations_count;
		for(i = 0; i < automaton->local_alphabet_count; i++){
			fprintf(f, "%d%s", automaton->local_alphabet[i],AUT_SER_SEP);
		}
		fprintf(f, "%d%s", automaton->local_alphabet_count + 1, AUT_SER_SEP);
		for(i = 0; i < max_delta[r]; i++){
			fprintf(f, "%d%s", automaton->local_alphabet_count + 1 + i, i == (max_delta[r] - 1)? "" :  AUT_SER_SEP);
		}
		fprintf(f, "%s%s%d%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, automaton->transitions_count, AUT_SER_SEP, AUT_SER_ARRAY_START);
		uint32_t current_count	= 0;
		int32_t ranking_value;
		bool first_transition	= true;
		for(i = 0; i < automaton->transitions_count; i++){
			for(j = 0; j < automaton->out_degree[i]; j++){
				if(&(automaton->transitions[i][j]) != NULL){
					if(first_transition)first_transition = false;
					else fprintf(f, "%s",AUT_SER_SEP);
					ranking_value = ((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[r],automaton->transitions[i][j].state_to))->value;
					automaton_ranking_transition_serialize_report(f, &(automaton->transitions[i][j]),
							ranking_value == RANKING_INFINITY? automaton->local_alphabet_count :automaton->local_alphabet_count + ranking_value + 1);
				}else{
					printf("ERROR: Transition was null at [%d][%d]\n", i, j);
				}
				current_count++;
			}
		}
		fprintf(f, "%s%s%d%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, automaton->initial_states_count, AUT_SER_SEP, AUT_SER_ARRAY_START);
		for(i = 0; i < automaton->initial_states_count; i++){
			fprintf(f, "%d%s", automaton->initial_states[i], i == (automaton->initial_states_count - 1)? "" :  AUT_SER_SEP);
		}
		fprintf(f, "%s%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, AUT_SER_ARRAY_START);
		for(i = 0; i < automaton->transitions_count; i++){
			fprintf(f, "%s", AUT_SER_ARRAY_START);
			for(j = 0; j < fluent_count; j++){
				if(automaton->is_game){
					fluent_index	= GET_STATE_FLUENT_INDEX(fluent_count, i, j);
					fprintf(f, "%s%s", TEST_FLUENT_BIT(automaton->valuations, fluent_index) ? "1" : "0", j == (fluent_count - 1)? "" :  AUT_SER_SEP);
				}else{
					fprintf(f, "%s%s", "0", j == (fluent_count - 1)? "" :  AUT_SER_SEP);
				}
			}
			fprintf(f, "%s%s", AUT_SER_ARRAY_END, i == (automaton->transitions_count - 1) ? "" : AUT_SER_SEP);
		}
		fprintf(f, "%s%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, AUT_SER_ARRAY_START);
		for(i = 0; i < automaton->transitions_count; i++){
			fprintf(f, "%s", AUT_SER_ARRAY_START);
			for(j = 0; j < liveness_count; j++){
				if(automaton->is_game){
					fluent_index	= GET_STATE_FLUENT_INDEX(liveness_count, i, j);
					fprintf(f, "%s%s", TEST_FLUENT_BIT(automaton->liveness_valuations, fluent_index) ? "1" : "0", j == (liveness_count - 1)? "" :  AUT_SER_SEP);
				}else{
					fprintf(f, "%s%s", "0", j == (fluent_count - 1)? "" :  AUT_SER_SEP);
				}

			}
			fprintf(f, "%s%s", AUT_SER_ARRAY_END, i == (automaton->transitions_count - 1) ? "" : AUT_SER_SEP);
		}

		fprintf(f, "%s%s", AUT_SER_ARRAY_END, AUT_SER_OBJ_END);
	}
	free(filename);
	return true;
}
bool automaton_automaton_print_report(automaton_automaton *automaton, char *filename){
	FILE *f = fopen(filename, "w");
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    return false;
	}
	automaton_automaton_serialize_report(f, automaton);
	fclose(f);
	return true;
}
/** CREATE FUNCTIONS **/
automaton_signal_event* automaton_signal_event_create(char* name, automaton_signal_type type){ 
	automaton_signal_event* signal_event	= malloc(sizeof(automaton_signal_event));
	automaton_signal_event_initialize(signal_event, name, type);
	return signal_event;
}
automaton_alphabet* automaton_alphabet_create(){
	automaton_alphabet* alphabet	= malloc(sizeof(automaton_alphabet));
	automaton_alphabet_initialize(alphabet);
	return alphabet;
}
automaton_transition* automaton_transition_create(uint32_t from_state, uint32_t to_state){
	automaton_transition* transition	= calloc(1, sizeof(automaton_transition));
	automaton_transition_initialize(transition, from_state, to_state);
	return transition;
}
automaton_fluent* automaton_fluent_create(char* name, bool initial_valuation){
	automaton_fluent* fluent	= malloc(sizeof(automaton_fluent));
	automaton_fluent_initialize(fluent, name, initial_valuation);
	return fluent;	
}
automaton_valuation* automaton_valuation_create(uint32_t state){
	automaton_valuation* valuation		= malloc(sizeof(automaton_valuation));
	automaton_valuation_initialize(valuation, state);
	return valuation;
}
automaton_automata_context* automaton_automata_context_create(char* name, automaton_alphabet* alphabet, uint32_t fluents_count, automaton_fluent** fluents, uint32_t liveness_valuations_count
		, obdd** liveness_valuations, char** liveness_valuations_names){
	automaton_automata_context* ctx		= malloc(sizeof(automaton_automata_context));
	automaton_automata_context_initialize(ctx, name, alphabet, fluents_count, fluents, liveness_valuations_count, liveness_valuations, liveness_valuations_names);
	return ctx;
}
automaton_automaton* automaton_automaton_create(char* name, automaton_automata_context* ctx, uint32_t local_alphabet_count, uint32_t* local_alphabet, bool is_game, bool built_from_ltl){
	automaton_automaton* automaton		= malloc(sizeof(automaton_automaton));
	automaton_automaton_initialize(automaton, name, ctx, local_alphabet_count, local_alphabet, is_game, built_from_ltl);
	return automaton;
}
automaton_range* automaton_range_create(char* name, uint32_t lower_value, uint32_t upper_value){
	automaton_range* range	= malloc(sizeof(automaton_range));
	automaton_range_initialize(range, name, lower_value, upper_value);
	return range;
}
automaton_indexes_valuation* automaton_indexes_valuation_create(){
	automaton_indexes_valuation* valuation	= malloc(sizeof(automaton_indexes_valuation));
	valuation->count			= 0;
	valuation->current_values	= NULL;
	valuation->ranges			= NULL;
	valuation->current_combination	= 0;
	valuation->total_combinations	= 0;
	return valuation;
}
/** INIT FUNCTIONS **/
void automaton_signal_event_initialize(automaton_signal_event* signal_event, char* name, automaton_signal_type type){
	signal_event->name		= malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(signal_event->name, name);
	signal_event->type		= type;
}
void automaton_alphabet_initialize(automaton_alphabet* alphabet){
	alphabet->size					= LIST_INITIAL_SIZE;
	alphabet->count					= 0;
	alphabet->list					= malloc(sizeof(automaton_signal_event)* alphabet->size);
}
void automaton_transition_initialize(automaton_transition* transition, uint32_t from_state, uint32_t to_state){
	transition->state_from	= from_state;
	transition->state_to	= to_state;
	transition->signals_count	= 0;
	transition->signals_size	= FIXED_SIGNALS_COUNT;
	transition->other_signals	= NULL;
	transition->is_input		= false;
}
void automaton_fluent_initialize(automaton_fluent* fluent, char* name, bool initial_valuation){
	fluent->name	= malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(fluent->name, name);
	fluent->starting_signals_count	= 0;
	fluent->starting_signals_element_count = NULL;
	fluent->starting_signals		= NULL;
	fluent->ending_signals_count	= 0;
	fluent->ending_signals_element_count	= NULL;
	fluent->ending_signals			= NULL;
	fluent->initial_valuation		= initial_valuation;
}
void automaton_valuation_initialize(automaton_valuation* valuation, uint32_t state){
	valuation->state	= state;
	valuation->active_fluents_count	= 0;
	valuation->active_fluents		= NULL;
}
void automaton_automata_context_initialize(automaton_automata_context* ctx, char* name, automaton_alphabet* alphabet, uint32_t fluents_count, automaton_fluent** fluents, uint32_t liveness_valuations_count, obdd** liveness_valuations
		, char** liveness_valuations_names){
	ctx->name					= malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(ctx->name, name);
	ctx->global_alphabet		= automaton_alphabet_clone(alphabet);
	ctx->global_fluents_count	= fluents_count;
	ctx->global_fluents			= calloc(ctx->global_fluents_count, sizeof(automaton_fluent));
	uint32_t i;
	for(i = 0; i < ctx->global_fluents_count; i++){
		automaton_fluent_copy(fluents[i], &(ctx->global_fluents[i]));
	}
	ctx->liveness_valuations_count	= liveness_valuations_count;
	ctx->liveness_valuations		= malloc(sizeof(obdd*) * liveness_valuations_count);
	ctx->liveness_valuations_names		= malloc(sizeof(char*) * liveness_valuations_count);
	for(i = 0; i < ctx->liveness_valuations_count; i++){
		ctx->liveness_valuations[i]	= obdd_clone(liveness_valuations[i]);
		aut_dupstr(&(ctx->liveness_valuations_names[i]), liveness_valuations_names[i]);
	}
}
void automaton_automaton_initialize(automaton_automaton* automaton, char* name, automaton_automata_context* ctx, uint32_t local_alphabet_count, uint32_t* local_alphabet, bool is_game, bool built_from_ltl){
	automaton->built_from_ltl			= built_from_ltl;
	automaton->name						= malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(automaton->name, name);
	automaton->context					= ctx;
	automaton->local_alphabet			= malloc(sizeof(uint32_t) * local_alphabet_count);
	automaton->local_alphabet_count		= 0;
	uint32_t i, j, current_entry;
	int32_t new_index;
	bool found;
	//insert ordered
	for(i = 0; i < local_alphabet_count; i++){
		new_index	= -1;
		found	= false;
		for(j = 0; j < i; j++){
			if(automaton->local_alphabet[j] == local_alphabet[i]){
				found	= true;
				break;
			}
			if(local_alphabet[i] < automaton->local_alphabet[j]){
				new_index	= j;
				break;
			}
		}
		//keep global alphabet ordered
		if(new_index > -1 && !found){
			for(j = (automaton->local_alphabet_count); j > (uint32_t)new_index; j--){
				automaton->local_alphabet[j]	= automaton->local_alphabet[j - 1];
			}
		}else{
			new_index	= automaton->local_alphabet_count;
		}
		if(!found){
			automaton->local_alphabet[new_index]	= local_alphabet[i];
			automaton->local_alphabet_count++;
		}
		//for(j = 0; j < automaton->local_alphabet_count; j++){printf("%d.", automaton->local_alphabet[j]);	}printf("\n");
	}
	automaton->transitions_size			= LIST_INITIAL_SIZE;
	automaton->transitions_count		= 0;
	automaton->transitions_composite_count	= 0;
	automaton->max_out_degree			= 0;
	automaton->max_concurrent_degree	= 0;
	automaton->is_controllable			= malloc(sizeof(bool) * automaton->transitions_size);
	automaton->out_degree				= malloc(sizeof(uint32_t) * automaton->transitions_size);
	automaton->out_size					= malloc(sizeof(uint32_t) * automaton->transitions_size);
	automaton->in_degree				= malloc(sizeof(uint32_t) * automaton->transitions_size);
	automaton->in_size					= malloc(sizeof(uint32_t) * automaton->transitions_size);
	automaton->transitions				= malloc(sizeof(automaton_transition*) * automaton->transitions_size);
	automaton->inverted_transitions		= malloc(sizeof(automaton_transition*) * automaton->transitions_size);
	for(i = 0; i < automaton->transitions_size; i++){
		automaton->is_controllable[i]		= true;
		automaton->out_degree[i]			= 0;
		automaton->in_degree[i]				= 0;
		automaton->in_size[i]				= TRANSITIONS_INITIAL_SIZE;
		automaton->out_size[i]				= TRANSITIONS_INITIAL_SIZE;
		automaton->transitions[i]			= calloc(automaton->out_size[i], sizeof(automaton_transition));
		for(j = 0; j < automaton->out_size[i]; j++)
			automaton_transition_initialize(&(automaton->transitions[i][j]), 0, 0);
		automaton->inverted_transitions[i]	= calloc(automaton->in_size[i],sizeof(automaton_transition));
		for(j = 0; j < automaton->in_size[i]; j++)
			automaton_transition_initialize(&(automaton->inverted_transitions[i][j]), 0, 0);
	}
	automaton->initial_states_count		= 0;
	automaton->initial_states			= NULL;
	automaton->is_game					= is_game;
	automaton->liveness_valuations_size = 0;
	if(is_game){
		if(automaton->context->global_fluents_count > 0){
			automaton->valuations_size			= GET_FLUENTS_ARR_SIZE(automaton->context->global_fluents_count, automaton->transitions_size);
			automaton->valuations 				= calloc(automaton->valuations_size, sizeof(uint32_t));
			automaton->inverted_valuations		= malloc(sizeof(automaton_bucket_list*) * automaton->context->global_fluents_count);
			for(i = 0; i < automaton->context->global_fluents_count; i++){
				automaton->inverted_valuations[i]	= automaton_bucket_list_create(FLUENT_BUCKET_SIZE);
			}
		}else{
			automaton->valuations_size	= 0;
			automaton->valuations		= NULL;
			automaton->inverted_valuations	= NULL;
		}
		if(automaton->context->liveness_valuations_count > 0){
			automaton->liveness_valuations_size			= GET_FLUENTS_ARR_SIZE(automaton->context->liveness_valuations_count, automaton->transitions_size);
			//automaton->liveness_valuations_size			= new_size;
			automaton->liveness_valuations 				= calloc(automaton->liveness_valuations_size, sizeof(uint32_t) );
			automaton->liveness_inverted_valuations		= malloc(sizeof(automaton_bucket_list*) * automaton->context->liveness_valuations_count);
			for(i = 0; i < automaton->context->liveness_valuations_count; i++){
				automaton->liveness_inverted_valuations[i]	= automaton_bucket_list_create(FLUENT_BUCKET_SIZE);
			}
		}else{
			automaton->liveness_valuations_size	= 0;
			automaton->liveness_valuations	= NULL;
			automaton->liveness_inverted_valuations	= NULL;
		}
	}
}
void automaton_range_initialize(automaton_range* range, char* name, uint32_t lower_value, uint32_t upper_value){
	if(name != NULL){
		range->name			= malloc(sizeof(char) * (strlen(name) + 1));
		strcpy(range->name, name);
	}
	range->lower_value	= lower_value;
	range->upper_value	= upper_value;
}
/** DESTROY FUNCTIONS **/
void automaton_signal_event_destroy(automaton_signal_event* signal_event, bool freeBase){
	free(signal_event->name);
	signal_event->name	= NULL;
	if(freeBase)
		free(signal_event);
}
void automaton_alphabet_destroy(automaton_alphabet* alphabet){
	uint32_t i;
	for(i = 0; i < alphabet->count; i++){
		automaton_signal_event_destroy(&(alphabet->list[i]), false);
	}
	free(alphabet->list);
	alphabet->list	= NULL;
	alphabet->count	= 0;
	alphabet->size	= 0;
	free(alphabet);
}
void automaton_transition_destroy(automaton_transition* transition, bool freeBase){
	if(transition->other_signals != NULL){
		free(transition->other_signals);
		transition->other_signals			= NULL;
	}
	transition->state_from		= 0;
	transition->state_to		= 0;
	transition->signals_count	= 0;
	transition->signals_size	= 4;
	if(freeBase)
		free(transition);
}
void automaton_fluent_destroy(automaton_fluent* fluent, bool freeBase){
	free(fluent->name);
	uint32_t i;
	for(i = 0; i < fluent->starting_signals_count; i++){free(fluent->starting_signals[i]);}
	free(fluent->starting_signals);
	for(i = 0; i < fluent->ending_signals_count; i++){free(fluent->ending_signals[i]);}
	free(fluent->ending_signals);
	fluent->name					= NULL;
	fluent->starting_signals_count	= 0;
	free(fluent->starting_signals_element_count);
	fluent->starting_signals_element_count	= NULL;
	fluent->starting_signals		= NULL;
	free(fluent->ending_signals_element_count);
	fluent->ending_signals_element_count	= NULL;
	fluent->ending_signals_count	= 0;
	fluent->ending_signals			= NULL;
	fluent->initial_valuation		= false;
	if(freeBase)
		free(fluent);
}
void automaton_valuation_destroy(automaton_valuation* valuation){
	free(valuation->active_fluents);
	valuation->active_fluents_count	= 0;
	valuation->state				= 0;
	valuation->active_fluents		= NULL;
	free(valuation);
}
void automaton_automata_context_destroy(automaton_automata_context* ctx){
	free(ctx->name);
	automaton_alphabet_destroy(ctx->global_alphabet);
	uint32_t i;
	for(i = 0; i < ctx->global_fluents_count; i++){
		automaton_fluent_destroy(&(ctx->global_fluents[i]), false);
	}
	if(ctx->global_fluents != NULL)
		free(ctx->global_fluents);
	for(i = 0; i < ctx->liveness_valuations_count; i++){
		obdd_destroy(ctx->liveness_valuations[i]);
		free(ctx->liveness_valuations_names[i]);
	}
	if(ctx->liveness_valuations != NULL)
		free(ctx->liveness_valuations);
	if(ctx->liveness_valuations_names != NULL)
		free(ctx->liveness_valuations_names);
	ctx->liveness_valuations	= NULL;
	ctx->liveness_valuations_names	= NULL;
	ctx->liveness_valuations_count	= 0;
	ctx->name					= NULL;
	ctx->global_alphabet		= NULL;
	ctx->global_fluents_count	= 0;
	ctx->global_fluents			= NULL;
	free(ctx);
}
void automaton_automaton_destroy(automaton_automaton* automaton){
	free(automaton->name);
	free(automaton->local_alphabet);
	uint32_t i,j;
	for(i = 0; i < automaton->transitions_size; i++){
		for(j = 0; j < automaton->out_degree[i]; j++){
			automaton_transition_destroy(&(automaton->transitions[i][j]), false);
		}
		for(j = 0; j < automaton->in_degree[i]; j++){
			automaton_transition_destroy(&(automaton->inverted_transitions[i][j]), false);
		}
		free(automaton->transitions[i]);
		free(automaton->inverted_transitions[i]);
	}
	free(automaton->is_controllable);
	free(automaton->out_degree);
	free(automaton->transitions);
	free(automaton->in_degree);
	free(automaton->inverted_transitions);
	free(automaton->in_size);
	free(automaton->out_size);
	if(automaton->initial_states_count > 0)
		free(automaton->initial_states);
	if(automaton->is_game){
		for(i = 0; i < automaton->context->global_fluents_count; i++)
			automaton_bucket_destroy(automaton->inverted_valuations[i]);
		if(automaton->valuations_size > 0){

			free(automaton->inverted_valuations);
			free(automaton->valuations);
		}
		if(automaton->liveness_valuations_size > 0){
			for(i = 0; i < automaton->context->liveness_valuations_count; i++)
				automaton_bucket_destroy(automaton->liveness_inverted_valuations[i]);
		}
		free(automaton->liveness_inverted_valuations);
		free(automaton->liveness_valuations);
	}
	automaton->name			= NULL;
	automaton->context		= NULL;
	automaton->local_alphabet_count	= 0;
	automaton->local_alphabet		= NULL;
	automaton->transitions_size		= 0;
	automaton->transitions_count	= 0;
	automaton->max_out_degree		= 0;
	automaton->max_concurrent_degree= 0;
	automaton->out_degree			= NULL;
	automaton->transitions			= NULL;
	automaton->in_degree			= NULL;
	automaton->inverted_transitions	= NULL;
	automaton->initial_states_count	= 0;
	automaton->initial_states		= NULL;
	automaton->valuations_size		= 0;
	automaton->valuations			= NULL;
	automaton->inverted_transitions	= NULL;
	automaton->liveness_valuations	= NULL;
	automaton->liveness_inverted_valuations	= NULL;
	free(automaton);
}
void automaton_range_destroy(automaton_range* range){
	if(range->name != NULL)
		free(range->name);
	range->lower_value	= 0;
	range->upper_value	= 0;
	free(range);
}
void automaton_indexes_valuation_destroy(automaton_indexes_valuation* valuation){
	free(valuation->current_values);
	uint32_t i;
	for(i = 0; i < valuation->count; i++){
		if(valuation->ranges[i] != NULL)
			automaton_range_destroy(valuation->ranges[i]);
	}

	valuation->current_values = NULL;
	if(valuation->ranges != NULL)
		free(valuation->ranges);
	valuation->ranges = NULL;
	free(valuation);
}
/** INDEXES VALUATION **/
bool automaton_indexes_valuation_has_range(automaton_indexes_valuation* valuation, automaton_range* range){
	uint32_t i;
	for(i = 0; i < valuation->count; i++){
		if(strcmp(valuation->ranges[i]->name, range->name))
			return true;
	}
	return false;
}
bool automaton_indexes_valuation_add_range(automaton_indexes_valuation* valuation, automaton_range* range){
	if(automaton_indexes_valuation_has_range(valuation, range)){ return false;}
	uint32_t new_count	= valuation->count + 1;
	int32_t* new_values	= malloc(sizeof(int32_t) * new_count);
	automaton_range** new_ranges	= malloc(sizeof(automaton_range*) * new_count);
	uint32_t i;
	for(i = 0; i < valuation->count; i++){
		new_values[i]	= valuation->current_values[i];
		new_ranges[i]	= valuation->ranges[i];
	}
	new_values[valuation->count]	= range->lower_value;
	new_ranges[valuation->count]	= range;
	free(valuation->current_values);
	free(valuation->ranges);
	valuation->current_values	= new_values;
	valuation->ranges			= new_ranges;
	return true;
}
/**
	 * Returns the index's value with name according to parameter provided
	 *
	 * @param valuation current valuation to be fixed over a single variable
	 * @param index_name the name of the variable to be fixed
	 * @return the value of the given index
	 */

int32_t automaton_indexes_valuation_get_value(automaton_indexes_valuation* valuation, char* range_name){
	uint32_t i;
	for(i = 0; i < valuation->count; i++){
		if(strcmp(valuation->ranges[i]->name, range_name) == 0){
			return valuation->current_values[i];
		}
	}
	printf("No index found with name %s \n", range_name);
	exit(-1);
}
void automaton_indexes_valuation_set_value(automaton_indexes_valuation* valuation, char* range_name, int32_t value){
	uint32_t i;
		for(i = 0; i < valuation->count; i++){
			if(strcmp(valuation->ranges[i]->name, range_name))
				valuation->current_values[i]	= value;
		}
}
/** ALPHABET **/
bool automaton_alphabet_has_signal_event(automaton_alphabet* alphabet, automaton_signal_event* signal_event){ 
	uint32_t i;
	for(i = 0; i < alphabet->count; i++){
		if(strcmp(alphabet->list[i].name, signal_event->name) == 0)
			return true;
	}
	return false;
}
bool automaton_alphabet_add_signal_event(automaton_alphabet* alphabet, automaton_signal_event* signal_event){ 
	if(automaton_alphabet_has_signal_event(alphabet, signal_event))
		return true;
	//search for position to insert event in order
	int32_t i, signal_ordered_index	= -1;
	int32_t old_count	= (int32_t)alphabet->count;
	for(i = 0; i < old_count; i++){
		if(strcmp(signal_event->name, alphabet->list[i].name) <= 0){
			signal_ordered_index	= i;
			break;
		}
	}
	//resize array if needed keeping proper order
	if(alphabet->count == alphabet->size){
		uint32_t new_size	= alphabet->size * LIST_INCREASE_FACTOR;
		automaton_signal_event* new_list	= malloc(sizeof(automaton_signal_event) * new_size);
		for(i = 0; i < old_count; i++){
			if(signal_ordered_index <= i && signal_ordered_index >= 0){
				automaton_signal_event_copy(&(alphabet->list[i]), &(new_list[i+1]));
			}else{
				automaton_signal_event_copy(&(alphabet->list[i]), &(new_list[i]));
			}
			automaton_signal_event_destroy(&(alphabet->list[i]), false);
		}
		free(alphabet->list);
		alphabet->list	= new_list;
		alphabet->size	= new_size;
	}else if(signal_ordered_index > -1){
		for(i = (alphabet->count) - 1; i >= signal_ordered_index; i--){
			automaton_signal_event_copy(&(alphabet->list[i]), &(alphabet->list[i + 1]));
			automaton_signal_event_destroy(&(alphabet->list[i]), false);
		}
	}
	if(signal_ordered_index == -1){
		signal_ordered_index	= alphabet->count;
	}
	automaton_signal_event_copy(signal_event, &(alphabet->list[signal_ordered_index]));
	alphabet->count++;
	return false;
}
int32_t automaton_alphabet_get_value_index(automaton_alphabet* alphabet, char* signal_name){
	int32_t i;
	for(i = 0; i < (int32_t)alphabet->count; i++){
		if(strcmp(alphabet->list[i].name, signal_name) == 0)
			return i;
	}
	return -1;
}
signal_t automaton_alphabet_get_signal_index(automaton_alphabet* alphabet, automaton_signal_event* signal_event){
	signal_t i;
	for(i = 0; i < alphabet->count; i++){
		if(strcmp(alphabet->list[i].name, signal_event->name) == 0)
			return i;
	}
	return 0;
}
/** TRANSITION **/
bool automaton_transition_has_signal_event(automaton_transition* transition, automaton_automata_context* ctx, automaton_signal_event* signal_event){ 
	uint32_t signal_index	= automaton_alphabet_get_signal_index(ctx->global_alphabet, signal_event);
	return automaton_transition_has_signal_event_ID(transition, ctx, signal_index);
}

bool automaton_transition_has_signal_event_ID(automaton_transition* transition, automaton_automata_context* ctx, uint32_t signal_index){
	uint32_t i;
	for(i = 0; i < transition->signals_count; i++){
		if( i < FIXED_SIGNALS_COUNT){
			if(transition->signals[i] == signal_index)
				return true;
		}else{
			if(transition->other_signals[i-FIXED_SIGNALS_COUNT] == signal_index)
				return true;
		}
	}
	return false;
}


bool automaton_transition_add_signal_event(automaton_transition* transition, automaton_automata_context* ctx, automaton_signal_event* signal_event){ 
	uint32_t signal_index	= automaton_alphabet_get_signal_index(ctx->global_alphabet, signal_event);
	return automaton_transition_add_signal_event_ID(transition, ctx, signal_index, signal_event->type);
}

bool automaton_transition_add_signal_event_ID(automaton_transition* transition, automaton_automata_context* ctx, uint32_t signal_index, automaton_signal_type signal_type){
	if(automaton_transition_has_signal_event_ID(transition, ctx, signal_index))
		return true;
	if(signal_type == INPUT_SIG)
		transition->is_input	= true;
	//signals should preserve order within the transition
	int32_t i, signal_ordered_index	= -1;
	int32_t old_count	= (int32_t)transition->signals_count;

	for(i = 0; i < old_count; i++){
		if(i < FIXED_SIGNALS_COUNT){
			if(signal_index < transition->signals[i]){
				signal_ordered_index	= i;
				break;
			}
		}else{
			if(signal_index < transition->other_signals[i-FIXED_SIGNALS_COUNT]){
				signal_ordered_index	= i;
				break;
			}
		}
	}
	uint32_t new_count		= transition->signals_count + 1;
	if(new_count > transition->signals_size){
		if(transition->signals_size == FIXED_SIGNALS_COUNT){//should add new list
			transition->other_signals	= calloc(FIXED_SIGNALS_COUNT, sizeof(signal_t));
			transition->signals_size	+= FIXED_SIGNALS_COUNT;
		}else{//should update list
			uint32_t old_size			= transition->signals_size - FIXED_SIGNALS_COUNT;
			uint32_t new_size			= old_size * SIGNALS_INCREASE_FACTOR;
			signal_t* new_signals		= calloc( new_size, sizeof(signal_t));
			for(i = 0; i < (int32_t)old_size; i++)
				new_signals[i]			= transition->other_signals[i];
			free(transition->other_signals);
			transition->other_signals	= new_signals;
			transition->signals_size	= FIXED_SIGNALS_COUNT + new_size;
		}
	}

	//if new index was not found within existing range then there is no need to update previous entries
	if(signal_ordered_index != -1){
		for(i = old_count -1; i >= 0; i--){
			if(signal_ordered_index <= i){
				if(i < FIXED_SIGNALS_COUNT){
					if((i+1) < FIXED_SIGNALS_COUNT){
						transition->signals[i+1]	= transition->signals[i];
					}else{
						transition->other_signals[i+1-FIXED_SIGNALS_COUNT]	= transition->signals[i];
					}
				}else{
					transition->other_signals[i+1-FIXED_SIGNALS_COUNT]	= transition->other_signals[i-FIXED_SIGNALS_COUNT];
				}
			}
		}
	}
	if(signal_ordered_index == -1){
		signal_ordered_index	= transition->signals_count;
	}
	if(signal_ordered_index < FIXED_SIGNALS_COUNT){
		transition->signals[signal_ordered_index]	= signal_index;
	}else{
		transition->other_signals[signal_ordered_index-FIXED_SIGNALS_COUNT]	= signal_index;
	}
	transition->signals_count++;
	return false;
}
/** FLUENT **/
bool automaton_fluent_has_starting_signals(automaton_fluent* fluent, automaton_alphabet* alphabet, uint32_t signals_count, automaton_signal_event** signal_events){
	uint32_t i,j,k;
	signal_t signal_index;
	bool found = false;
	for(i = 0; i < fluent->starting_signals_count; i++){
		if(fluent->starting_signals_element_count[i] != signals_count)continue;
		for(k = 0; k < signals_count; k++){
			signal_index	= automaton_alphabet_get_signal_index(alphabet, signal_events[k]);
			found = false;
			for(j = 0; j < fluent->starting_signals_element_count[i]; j++){
				if(fluent->starting_signals[i][j] == signal_index){
					found = true;
					break;
				}
			}
			if(!found)return false;
		}
		return true;
	}
	return false;
}
bool automaton_fluent_add_starting_signals(automaton_fluent* fluent, automaton_alphabet* alphabet
		, uint32_t signals_count, automaton_signal_event** signal_events){
	if(automaton_fluent_has_starting_signals(fluent, alphabet, signals_count, signal_events))
		return true;
	uint32_t i;
	uint32_t new_count = fluent->starting_signals_count +1;
	signal_t** ptr	= realloc(fluent->starting_signals, sizeof(signal_t*) * new_count);
	if(ptr == NULL){printf("Could not allocate memory\n");exit(-1);	}
	else fluent->starting_signals	= ptr;
	uint32_t* i_ptr	= realloc(fluent->starting_signals_element_count, sizeof(uint32_t) * new_count);
	if(ptr == NULL){printf("Could not allocate memory\n");exit(-1);}
	else fluent->starting_signals_element_count	= i_ptr;
	signal_t *new_signals	= calloc(signals_count, sizeof(signal_t));
	for(i = 0; i < signals_count; i++){
		new_signals[i]	= automaton_alphabet_get_signal_index(alphabet, signal_events[i]);
	}
	fluent->starting_signals[fluent->starting_signals_count]	= new_signals;
	fluent->starting_signals_element_count[fluent->starting_signals_count]	= signals_count;
	fluent->starting_signals_count++;
	return false;
}
bool automaton_fluent_has_ending_signals(automaton_fluent* fluent, automaton_alphabet* alphabet, uint32_t signals_count, automaton_signal_event** signal_events){
	uint32_t i,j,k;
	signal_t signal_index;
	bool found = false;
	for(i = 0; i < fluent->ending_signals_count; i++){
		if(fluent->ending_signals_element_count[i] != signals_count)continue;
		for(k = 0; k < signals_count; k++){
			signal_index	= automaton_alphabet_get_signal_index(alphabet, signal_events[k]);
			found = false;
			for(j = 0; j < fluent->ending_signals_element_count[i]; j++){
				if(fluent->ending_signals[i][j] == signal_index){
					found = true;
					break;
				}
			}
			if(!found)return false;
		}
		return true;
	}
	return false;
}
bool automaton_fluent_add_ending_signals(automaton_fluent* fluent, automaton_alphabet* alphabet
		, uint32_t signals_count, automaton_signal_event** signal_events){
	if(automaton_fluent_has_ending_signals(fluent, alphabet, signals_count, signal_events))
		return true;
	uint32_t i;
	uint32_t new_count = fluent->ending_signals_count +1;
	signal_t** ptr	= realloc(fluent->ending_signals, sizeof(signal_t*) * new_count);
	if(ptr == NULL){printf("Could not allocate memory\n");exit(-1);	}
	else fluent->ending_signals	= ptr;
	uint32_t* i_ptr	= realloc(fluent->ending_signals_element_count, sizeof(uint32_t) * new_count);
	if(ptr == NULL){printf("Could not allocate memory\n");exit(-1);}
	else fluent->ending_signals_element_count	= i_ptr;
	signal_t *new_signals	= calloc(signals_count, sizeof(signal_t));
	for(i = 0; i < signals_count; i++){
		new_signals[i]	= automaton_alphabet_get_signal_index(alphabet, signal_events[i]);
	}
	fluent->ending_signals[fluent->ending_signals_count]	= new_signals;
	fluent->ending_signals_element_count[fluent->ending_signals_count]	= signals_count;
	fluent->ending_signals_count++;
	return false;
}
/** VALUATION **/
bool automaton_valuation_has_fluent(automaton_valuation* valuation, automaton_automata_context* ctx, automaton_fluent* fluent){
	uint32_t i;
	uint32_t fluent_index	= automaton_automata_context_get_fluent_index(ctx, fluent);
	for(i = 0; i < valuation->active_fluents_count; i++){
		if(valuation->active_fluents[i] == fluent_index)
			return true;
	}
	return false;	
}
bool automaton_valuation_add_fluent(automaton_valuation* valuation, automaton_automata_context* ctx, automaton_fluent* fluent){
	if(automaton_valuation_has_fluent(valuation, ctx, fluent))
		return true;
	uint32_t i;
	uint32_t fluent_index	= automaton_automata_context_get_fluent_index(ctx, fluent);
	uint32_t* new_fluents	= malloc(sizeof(uint32_t) * (valuation->active_fluents_count + 1));
	for(i = 0; i < valuation->active_fluents_count; i++)
		new_fluents[i]	= valuation->active_fluents[i];
	free(valuation->active_fluents);
	new_fluents[valuation->active_fluents_count]	= fluent_index;
	valuation->active_fluents	= new_fluents;
	valuation->active_fluents_count++;
	return false;
}
/** AUTOMATA_CONTEXT **/
uint32_t automaton_automata_context_get_fluent_index(automaton_automata_context* ctx, automaton_fluent* fluent){
	uint32_t i;
	for(i = 0; i < ctx->global_fluents_count; i++){
		if(strcmp(ctx->global_fluents[i].name, fluent->name) == 0)
			return i;
	}
	return 0;
}
/** AUTOMATON **/
bool automaton_automaton_has_state(automaton_automaton* current_automaton, uint32_t state){
	return current_automaton->transitions_count > state
			&& (current_automaton->out_degree[state] > 0 || current_automaton->in_degree[state] > 0);
}
uint32_t automaton_automaton_get_in_degree(automaton_automaton* current_automaton, uint32_t state){
	return current_automaton->in_degree[state];
}
uint32_t automaton_automaton_get_out_degree(automaton_automaton* current_automaton, uint32_t state){
	return current_automaton->out_degree[state];
}
automaton_transition* automaton_automaton_get_transitions(automaton_automaton* current_automaton, uint32_t state){
	return current_automaton->transitions[state];
}
bool automaton_automaton_has_transition(automaton_automaton* current_automaton, automaton_transition* transition){
	uint32_t from_state	= transition->state_from;
	uint32_t to_state	= transition->state_to;
	if(!automaton_automaton_has_state(current_automaton, from_state) || !automaton_automaton_has_state(current_automaton, to_state))
		return false;
	automaton_transition* current_transitions	= current_automaton->transitions[from_state];
	uint32_t out_degree	= current_automaton->out_degree[from_state];
	uint32_t i, j, k;
	if(out_degree == 0)
		return false;
	bool mismatch = false;
	for(i = 0; i < out_degree; i++){
		if(current_transitions[i].signals_count != transition->signals_count)
			continue;
		if(current_transitions[i].state_to == to_state){
			if(transition->signals_count == 0 && current_transitions[i].signals_count == 0){
				return true;
			}else{
				mismatch = false;
				for(j = 0; j < transition->signals_count; j++){
					signal_t sig_j	= j < FIXED_SIGNALS_COUNT? transition->signals[j] : transition->other_signals[j-FIXED_SIGNALS_COUNT];
					signal_t sig_k	= j < FIXED_SIGNALS_COUNT? current_transitions[i].signals[j] : current_transitions[i].other_signals[j - FIXED_SIGNALS_COUNT];
					if(sig_j != sig_k)mismatch = true;
				}
				if(!mismatch)return true;
			}
		}
	}
	return false;
}
void automaton_automaton_resize_to_state(automaton_automaton* current_automaton, uint32_t state){
	if(state < current_automaton->transitions_size) return;
	uint32_t i,j;
	uint32_t old_size	= current_automaton->transitions_size;
	uint32_t next_size	= old_size;
	while(state >= next_size) next_size *= LIST_INCREASE_FACTOR;
	//printf("%d\t%d\t%d\n", state, next_size, current_automaton->transitions_composite_count);
	bool* next_is_controllable			= malloc(sizeof(bool) * next_size);
	uint32_t* next_out_degree			= malloc(sizeof(uint32_t) * next_size);
	uint32_t* next_out_size				= malloc(sizeof(uint32_t) * next_size);
	uint32_t* next_in_degree			= malloc(sizeof(uint32_t) * next_size);
	uint32_t* next_in_size				= malloc(sizeof(uint32_t) * next_size);
	automaton_transition** next_trans	= malloc(sizeof(automaton_transition*) * next_size);
	automaton_transition** next_inv_trans= malloc(sizeof(automaton_transition*) * next_size);
	for(i = 0; i < current_automaton->transitions_size; i++){
		next_is_controllable[i]			= current_automaton->is_controllable[i];
		next_out_degree[i]				= current_automaton->out_degree[i];
		next_in_degree[i]				= current_automaton->in_degree[i];
		next_in_size[i]					= current_automaton->in_size[i];
		next_out_size[i]				= current_automaton->out_size[i];
		next_trans[i]					= current_automaton->transitions[i];
		next_inv_trans[i]				= current_automaton->inverted_transitions[i];
	}
	for(i = current_automaton->transitions_size; i < next_size; i++){
		next_is_controllable[i]			= true;
		next_out_degree[i]				= 0;
		next_in_degree[i]				= 0;
		next_in_size[i]					= TRANSITIONS_INITIAL_SIZE;
		next_out_size[i]				= TRANSITIONS_INITIAL_SIZE;
		next_trans[i]					= calloc(next_out_size[i], sizeof(automaton_transition));
		for(j = 0; j < next_out_size[i]; j++){
			automaton_transition_initialize(&(next_trans[i][j]), 0, 0);
		}
		next_inv_trans[i]				= calloc(next_in_size[i], sizeof(automaton_transition));
		for(j = 0; j < next_in_size[i]; j++){
			automaton_transition_initialize(&(next_inv_trans[i][j]), 0, 0);
		}
	}

	free(current_automaton->is_controllable);
	free(current_automaton->out_degree);
	free(current_automaton->in_degree);
	free(current_automaton->out_size);
	free(current_automaton->in_size);
	free(current_automaton->transitions);
	free(current_automaton->inverted_transitions);

	current_automaton->is_controllable	= next_is_controllable;
	current_automaton->out_degree	= next_out_degree;
	current_automaton->in_degree	= next_in_degree;
	current_automaton->out_size		= next_out_size;
	current_automaton->in_size		= next_in_size;
	current_automaton->transitions	= next_trans;
	current_automaton->inverted_transitions	= next_inv_trans;
	current_automaton->transitions_size		= next_size;

	uint32_t old_valuations_size, new_valuations_size;
	if(current_automaton->is_game){
		uint32_t* new_valuations;
		if(current_automaton->context->global_fluents_count > 0){
			old_valuations_size						= GET_FLUENTS_ARR_SIZE(current_automaton->context->global_fluents_count, old_size);
			current_automaton->valuations_size		= GET_FLUENTS_ARR_SIZE(current_automaton->context->global_fluents_count, current_automaton->transitions_size);
			new_valuations 							= calloc(current_automaton->valuations_size, sizeof(uint32_t));
			for(i = 0; i < old_valuations_size; i++){
				new_valuations[i]					= current_automaton->valuations[i];
			}
			free(current_automaton->valuations);
			current_automaton->valuations			= new_valuations;
		}
		if(current_automaton->context->liveness_valuations_count > 0){
			old_valuations_size						= GET_FLUENTS_ARR_SIZE(current_automaton->context->liveness_valuations_count, old_size);
			current_automaton->liveness_valuations_size	= GET_FLUENTS_ARR_SIZE(current_automaton->context->liveness_valuations_count, current_automaton->transitions_size);
			new_valuations 							= malloc(sizeof(uint32_t) * current_automaton->liveness_valuations_size);
			for(i = 0; i < old_valuations_size; i++){
				new_valuations[i]					= current_automaton->liveness_valuations[i];
			}
			free(current_automaton->liveness_valuations);
			current_automaton->liveness_valuations	= new_valuations;
		}
	}

}
bool automaton_automaton_add_transition(automaton_automaton* current_automaton, automaton_transition* transition){
	if(automaton_automaton_has_transition(current_automaton, transition)) return false;
	uint32_t i;
	uint32_t from_state	= transition->state_from;
	uint32_t to_state	= transition->state_to;
	automaton_automaton_resize_to_state(current_automaton, from_state);
	automaton_automaton_resize_to_state(current_automaton, to_state);
	if((from_state + 1) > current_automaton->transitions_count){
		current_automaton->transitions_count = from_state + 1;
	}
	if((to_state + 1) > current_automaton->transitions_count){
		current_automaton->transitions_count = to_state + 1;
	}
	uint32_t	old_out_degree	= current_automaton->out_degree[from_state];
	uint32_t	old_in_degree	= current_automaton->in_degree[to_state];
	uint32_t	old_out_size	= current_automaton->out_size[from_state];
	uint32_t	old_in_size		= current_automaton->in_size[to_state];

	if(old_out_degree  >= old_out_size){
		current_automaton->out_size[from_state]	= (old_out_size * LIST_INCREASE_FACTOR);
		automaton_transition* new_out	= calloc(current_automaton->out_size[from_state], sizeof(automaton_transition));
		for(i = 0; i < old_out_degree; i++){
				automaton_transition_copy(&(current_automaton->transitions[from_state][i]), &(new_out[i]));
				automaton_transition_destroy(&(current_automaton->transitions[from_state][i]), false);
		}
		for(i = old_out_degree; i < current_automaton->out_size[from_state]; i++){
			automaton_transition_initialize(&(new_out[i]), 0, 0);
		}
		free(current_automaton->transitions[from_state]);
		current_automaton->transitions[from_state]	= new_out;
	}
	if(transition->is_input)current_automaton->is_controllable[from_state]	= false;
	automaton_transition_copy(transition, &(current_automaton->transitions[from_state][old_out_degree]));
	if(old_in_degree  >= old_in_size){
		current_automaton->in_size[to_state]	= (old_in_size * LIST_INCREASE_FACTOR);
		automaton_transition* new_in	= calloc(current_automaton->in_size[to_state], sizeof(automaton_transition));
		for(i = 0; i < old_in_degree; i++){
			automaton_transition_copy(&(current_automaton->inverted_transitions[to_state][i]), &(new_in[i]));
			automaton_transition_destroy(&(current_automaton->inverted_transitions[to_state][i]), false);
		}
		for(i = old_in_degree; i < current_automaton->in_size[to_state]; i++){
			automaton_transition_initialize(&(new_in[i]), 0, 0);
		}
		free(current_automaton->inverted_transitions[to_state]);
		current_automaton->inverted_transitions[to_state]	= new_in;
	}

	automaton_transition_copy(transition, &(current_automaton->inverted_transitions[to_state][old_in_degree]));

	current_automaton->out_degree[from_state]++;
	current_automaton->in_degree[to_state]++;
	if(current_automaton->max_out_degree < current_automaton->out_degree[from_state])
		current_automaton->max_out_degree	= current_automaton->out_degree[from_state];
	if(current_automaton->max_concurrent_degree < transition->signals_count)
		current_automaton->max_concurrent_degree	= transition->signals_count;
	current_automaton->transitions_composite_count++;
	return true;
}

bool automaton_automaton_remove_transition(automaton_automaton* current_automaton, automaton_transition* transition){
	uint32_t from_state	= transition->state_from; uint32_t to_state	= transition->state_to;
	automaton_transition *other_transition;
	int32_t index		= -1, inverse_index	= -1;
	uint32_t i, j, k;
	bool transition_found = true;
	for(i = 0; i < current_automaton->out_degree[from_state]; i++){//find transition index in automaton
		other_transition	= &(current_automaton->transitions[from_state][i]);
		if(other_transition->is_input != transition->is_input)continue;
		if(other_transition->signals_count != transition->signals_count
				|| other_transition->state_to != to_state)continue;
		transition_found	= true;
		for(j = 0; j < transition->signals_count; j++)
			if(GET_TRANSITION_SIGNAL(transition, j) != GET_TRANSITION_SIGNAL(other_transition,j)){
				transition_found = false; break;
			}
		if(transition_found){
			index = i;
			break;
		}
	}
	if(!transition_found)
		return false;
	for(i = 0; i < current_automaton->in_degree[to_state]; i++){//find inverse index in automaton
		other_transition	= &(current_automaton->inverted_transitions[to_state][i]);
		if(other_transition->is_input != transition->is_input)continue;
		if(other_transition->signals_count != transition->signals_count
				|| other_transition->state_from != from_state)continue;
		transition_found	= true;
		for(j = 0; j < transition->signals_count; j++)
			if(GET_TRANSITION_SIGNAL(transition, j) != GET_TRANSITION_SIGNAL(other_transition,j)){
				transition_found = false; break;
			}
		if(transition_found){
			inverse_index = i;
			break;
		}
	}
	if(!transition_found)
		return false;
	//remove from transitions
	if(index > -1){
		automaton_transition_destroy(&(current_automaton->transitions[from_state][index]), false);
		for(i = index; i < current_automaton->out_degree[from_state]-1; i++){
			memcpy(&(current_automaton->transitions[from_state][i]),&(current_automaton->transitions[from_state][i+1])
						, sizeof(automaton_transition));
		}
		current_automaton->out_degree[from_state]--;
	}
	//remove from inverted transitions
	if(inverse_index > -1){
		automaton_transition_destroy(&(current_automaton->inverted_transitions[to_state][inverse_index]), false);
		for(i = inverse_index; i < current_automaton->in_degree[to_state]-1; i++){
			memcpy(&(current_automaton->inverted_transitions[to_state][i]),&(current_automaton->inverted_transitions[to_state][i+1])
						, sizeof(automaton_transition));
		}
		current_automaton->in_degree[to_state]--;
	}
	//update structures
	//current_automaton->transitions_count--;
	current_automaton->transitions_composite_count--;
	return true;
}

bool automaton_automaton_add_initial_state(automaton_automaton* current_automaton, uint32_t state){
	//if(!automaton_automaton_has_state(current_automaton, state))return true;
	uint32_t i;
	for(i = 0; i < current_automaton->initial_states_count; i++){
		if(current_automaton->initial_states[i] == state)
			return true;
	}
	uint32_t* new_initial_states	= malloc(sizeof(uint32_t) * (current_automaton->initial_states_count + 1));
	for(i = 0; i < current_automaton->initial_states_count; i++){
		new_initial_states[i]	= current_automaton->initial_states[i];
	}
	new_initial_states[current_automaton->initial_states_count]	= state;
	if(current_automaton->initial_states_count > 0){
		free(current_automaton->initial_states);
	}
	current_automaton->initial_states_count++;
	current_automaton->initial_states	= new_initial_states;
	return false;
}
uint32_t automaton_ranking_key_extractor(void* ranking){return ((automaton_ranking*)ranking)->state;}
automaton_ranking* automaton_ranking_create_infinity(uint32_t state, int32_t assumption_to_satisfy){
	automaton_ranking* ranking	= malloc(sizeof(automaton_ranking));
	ranking->state			= state;
	ranking->value			= RANKING_INFINITY;
	ranking->assumption_to_satisfy	= assumption_to_satisfy;
	return ranking;
}
automaton_ranking* automaton_ranking_create(uint32_t state, int32_t assumption_to_satisfy){
	automaton_ranking* ranking	= malloc(sizeof(automaton_ranking));
	ranking->state			= state;
	ranking->value			= 0;
	ranking->assumption_to_satisfy		= assumption_to_satisfy;
	return ranking;
}
void automaton_ranking_destroy(automaton_ranking*  ranking){
	ranking->state			= 0;
	ranking->assumption_to_satisfy	= 0;
	free(ranking);
}
uint32_t automaton_pending_state_extractor(void* pending_state){return ((automaton_pending_state*)pending_state)->state;}
uint32_t automaton_pending_state_ranking_extractor(void* pending_state){return ((automaton_pending_state*)pending_state)->value;}
automaton_pending_state* automaton_pending_state_create(uint32_t state, int32_t goal_to_satisfy, int32_t assumption_to_satisfy,
		int32_t value, uint32_t timestamp){
	automaton_pending_state* pending	= malloc(sizeof(automaton_pending_state));
	pending->state			= state;
	pending->goal_to_satisfy= goal_to_satisfy;
	pending->assumption_to_satisfy	= assumption_to_satisfy;
	pending->value			= value;
	pending->timestamp		= timestamp;
	return pending;
}
int32_t automaton_pending_state_compare(void* left_pending_state, void* right_pending_state){
	automaton_pending_state* left	= (automaton_pending_state*)left_pending_state;
	automaton_pending_state* right= (automaton_pending_state*)right_pending_state;

	if(left->assumption_to_satisfy > right->assumption_to_satisfy){
		return 1;
	}else if(left->value > right->value){
		return 1;
	}else if(left->value < right->value){
		return -1;
	}else if(left->timestamp > right->timestamp){
		return 1;
	}else if(left->timestamp < right->timestamp){
		return -1;
	}else{
		return 0;
	}
}
void automaton_pending_state_copy(void* target_input, void* source_input){
	automaton_pending_state* target	= (automaton_pending_state*)target_input;
	automaton_pending_state* source	= (automaton_pending_state*)source_input;
	target->state				= source->state;
	target->goal_to_satisfy		= source->goal_to_satisfy;
	target->assumption_to_satisfy	= source->assumption_to_satisfy;
	target->value				= source->value;
	target->timestamp			= source->timestamp;
}
uint32_t automaton_int_extractor(void* value){return *((uint32_t*)value);}
int32_t automaton_int_compare(void* left_int, void* right_int){
	uint32_t left	= *((uint32_t*)left_int);
	uint32_t right	= *((uint32_t*)right_int);

	if(left > right){
		return 1;
	}else if(left < right){
		return -1;
	}else{
		return 0;
	}
}
void automaton_pending_state_destroy(automaton_pending_state*  pending_state){
	free(pending_state);
}


bool _print_ranking;
/**
 * best_j(v)
 */
automaton_ranking* automaton_state_best_successor_ranking(automaton_automaton* game_automaton, uint32_t state, automaton_concrete_bucket_list** ranking
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t* guarantees_indexes){
	if(game_automaton->out_degree[state] == 0)return NULL;
	uint32_t fluent_count		= game_automaton->context->global_fluents_count;
	uint32_t fluent_index		= GET_STATE_FLUENT_INDEX(fluent_count, state, guarantees_indexes[current_guarantee]);
	bool satisfies_guarantee	= TEST_FLUENT_BIT(game_automaton->valuations, fluent_index);
	bool is_controllable		= game_automaton->is_controllable[state];
	int32_t min_value			= RANKING_INFINITY;
	automaton_ranking* min_ranking	= NULL;
	int32_t max_value			= RANKING_INFINITY;
	automaton_ranking* max_ranking	= NULL;
	automaton_ranking* current_value;
	uint32_t i, j, to_state;
	j = satisfies_guarantee? (current_guarantee + 1) % guarantee_count : current_guarantee;
	//TODO: apply SIMD
	for(i = 0; i < game_automaton->out_degree[state]; i++){
		//if transition is controllable in non-controllable state, then skip
		if(!is_controllable && !game_automaton->transitions[state][i].is_input)continue;
		to_state	= game_automaton->transitions[state][i].state_to;
		//if(to_state == state)continue;
		current_value	= ((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking[j], to_state));
		//TODO:check this next line, is it right to retrieve NULL when initializing [Deadlock]?
		if(current_value == NULL)continue;
		if(min_ranking == NULL){
			min_value = max_value = current_value->value;
			min_ranking = max_ranking = current_value;
		}
		if(is_controllable){
			if(automaton_ranking_lt(current_value, min_ranking)){
			//if((min_value >= current_value->value) || (current_value->value != RANKING_INFINITY && min_value == RANKING_INFINITY)){
				min_value 	= current_value->value;
				min_ranking	= current_value;
			}
		}else{
			if(automaton_ranking_gt(current_value, max_ranking)){
			//if((max_value <= current_value->value) || (current_value->value == RANKING_INFINITY && min_value != RANKING_INFINITY)){
				max_value 	= current_value->value;
				max_ranking	= current_value;
			}
		}
	}
	automaton_ranking* return_value = is_controllable ? min_ranking : max_ranking;
#if DEBUG_SYNTHESIS
	if(_print_ranking)
		if(return_value == NULL){
			printf("NO PROPER PREDECESSOR FOR RANKING\n");
		}
#endif
	return return_value;
}
//return query for: left > right
bool automaton_ranking_gt(automaton_ranking* left, automaton_ranking* right){
	if(left->value == RANKING_INFINITY)return right->value != RANKING_INFINITY;
	if(right->value == RANKING_INFINITY)return false;
	return (left->value > right->value)
			|| (left->value == right->value && left->assumption_to_satisfy > right->assumption_to_satisfy);
}
bool automaton_ranking_eq(automaton_ranking* left, automaton_ranking* right){
	if(left->value == RANKING_INFINITY) return right->value == RANKING_INFINITY;
	return (left->value == right->value) && (left->assumption_to_satisfy == right->assumption_to_satisfy);
}
bool automaton_ranking_lt(automaton_ranking* left, automaton_ranking* right){
	return automaton_ranking_gt(right, left);
}
bool automaton_state_is_stable(automaton_automaton* game_automaton, uint32_t state, automaton_concrete_bucket_list** ranking
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t assumptions_count
		, uint32_t* guarantees_indexes, uint32_t* assumptions_indexes, int32_t first_assumption_index){
	uint32_t i, j, to_state;
	automaton_ranking concrete_ranking;
	/**
	 * r_j(v)
	 */
	automaton_ranking*	current_ranking	= ((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking[current_guarantee], state));
	//TODO: check if this fix is hiding an error
	if(current_ranking == NULL){
		printf("[%d]!! had no ranking defined for %d, set as INF.\n", state, current_guarantee);
		concrete_ranking.state	= state; concrete_ranking.assumption_to_satisfy	= 0;
		concrete_ranking.value	= RANKING_INFINITY;
		automaton_concrete_bucket_add_entry(ranking[current_guarantee], &concrete_ranking);
		current_ranking			= ((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking[current_guarantee], state));
	}
	/**
	 * v in Q_j
	 */
	uint32_t fluent_count		= game_automaton->context->global_fluents_count;
	uint32_t fluent_index		= GET_STATE_FLUENT_INDEX(fluent_count, state, guarantees_indexes[current_guarantee]);//guarantees_indexes[current_guarantee]);
	bool satisfies_guarantee	= TEST_FLUENT_BIT(game_automaton->valuations, fluent_index);
	/**
	 * v in P_i
	 */
	int32_t current_value		=  current_ranking->value;
	//TODO: check this against definition
	if(current_value == RANKING_INFINITY)return true;
	/**
	 * r_j(w)
	 */
	j = current_guarantee;
	automaton_ranking* sr		= automaton_state_best_successor_ranking(game_automaton, state, ranking, j
			, guarantee_count, guarantees_indexes);

	fluent_index				= GET_STATE_FLUENT_INDEX(fluent_count, state, assumptions_indexes[current_ranking->assumption_to_satisfy]);
	bool satisfies_assumption	= TEST_FLUENT_BIT(game_automaton->valuations, fluent_index);
	j = satisfies_guarantee? (current_guarantee + 1) % guarantee_count : current_guarantee;
	if(sr == NULL){
#if DEBUG_SYNTHESIS
	if(_print_ranking)
		printf("[S ] %d <%d:_, _> No best successor\n", state, j);
#endif
		return true;
	}
#if DEBUG_SYNTHESIS
	if(_print_ranking)
		printf("[S ] %d <%d:%d,%d> best succ. for %d",sr->state,j, sr->value, sr->assumption_to_satisfy,state);
#endif
	/**
	 * is ranking good?
	 */
	bool is_stable = false;
 	if(satisfies_guarantee){
		if(sr->value == RANKING_INFINITY)
			is_stable = current_value == RANKING_INFINITY;
		else
			is_stable = (current_ranking->value == INITIAL_RANKING_VALUE && current_ranking->assumption_to_satisfy == first_assumption_index);
	}else{
		if(satisfies_assumption)
			is_stable = automaton_ranking_gt(current_ranking, sr);
		else
			is_stable = automaton_ranking_gt(current_ranking, sr) || automaton_ranking_eq(current_ranking, sr);
	}
#if DEBUG_SYNTHESIS
 	if(_print_ranking)
 		printf(" (%s)\n", is_stable? "stable" : "unstable");
#endif
	return is_stable;
}

void automaton_add_unstable_predecessors(automaton_automaton* game_automaton, automaton_max_heap* pending_list, automaton_concrete_bucket_list* key_list
		, uint32_t state, automaton_concrete_bucket_list** ranking
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t assumptions_count
		, uint32_t* guarantees_indexes, uint32_t* assumptions_indexes, int32_t first_assumption_index
		, uint32_t timestamp){
	uint32_t i;
	automaton_transition* current_transition;
	automaton_pending_state current_pending_state;
	automaton_pending_state *existing_pending_state;
	automaton_ranking* current_ranking;

	for(i = 0; i < game_automaton->in_degree[state]; i++){
		current_transition	= &(game_automaton->inverted_transitions[state][i]);
		//TODO:check if we should add another structure to answer inclusion queries (heap of state_from synched with pending_state)
		//if(!automaton_state_is_stable(game_automaton, current_transition->state_from, ranking, current_guarantee, guarantee_count
		//		, assumptions_count, guarantees_indexes, assumptions_indexes, first_assumption_index) || current_transition->is_input){
#if DEBUG_SYNTHESIS
	if(_print_ranking)
		printf("[->] %d <%d:_, _> Pushing into pending pred. for state %d\n", current_transition->state_from, current_guarantee, state);
#endif
			//check if entry exists in the pending structure, if so update previous value
			current_ranking							= ((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking[current_guarantee], current_transition->state_from));
			if(current_ranking == NULL)
				continue;
			if(automaton_concrete_bucket_has_key(key_list, current_transition->state_from)){
				existing_pending_state					= automaton_concrete_bucket_get_entry(key_list, current_transition->state_from);
				existing_pending_state->value			= current_ranking->value;
				existing_pending_state->assumption_to_satisfy			= current_ranking->assumption_to_satisfy;
				existing_pending_state->timestamp		= timestamp;
				existing_pending_state->goal_to_satisfy	= current_guarantee;
			}else{
				current_pending_state.goal_to_satisfy	= current_guarantee;
				current_pending_state.state = current_transition->state_from;
				current_pending_state.assumption_to_satisfy	= current_ranking->assumption_to_satisfy;
				current_pending_state.value				= current_ranking->value;
				current_pending_state.timestamp			= timestamp;
				void* new_entry							= automaton_max_heap_add_entry(pending_list, &current_pending_state);
				automaton_concrete_bucket_add_entry(key_list, new_entry);
			}
		//}
	}
}
void automaton_ranking_increment(automaton_automaton* game_automaton, automaton_concrete_bucket_list** ranking, automaton_ranking* current_ranking, uint32_t ref_state, uint32_t* max_delta
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t assumptions_count
		, uint32_t* guarantees_indexes, uint32_t* assumptions_indexes, uint32_t first_assumption_index, automaton_ranking* target_ranking){
	automaton_automata_context* ctx		= game_automaton->context;
	uint32_t i;
	bool satisfies_goal, satisfies_assumption;
	//set default values
	target_ranking->state					= ref_state;
	target_ranking->assumption_to_satisfy	= current_ranking->assumption_to_satisfy;
	target_ranking->value					= current_ranking->value;
	//infinity is preserved
	if(current_ranking->value == RANKING_INFINITY){
		return;
	}

	uint32_t fluent_count				= ctx->global_fluents_count;
	uint32_t fluent_index				= GET_STATE_FLUENT_INDEX(fluent_count, ref_state, guarantees_indexes[current_guarantee]);
	satisfies_goal						= TEST_FLUENT_BIT(game_automaton->valuations, fluent_index);
	//goal satisfaction resets ranking
	if(satisfies_goal){
		target_ranking->value					= INITIAL_RANKING_VALUE;
		target_ranking->assumption_to_satisfy	= first_assumption_index;
		return;
	}
	//assumption being satisfied increments ranking
	fluent_index				= GET_STATE_FLUENT_INDEX(fluent_count, ref_state, assumptions_indexes[current_ranking->assumption_to_satisfy]);
	satisfies_assumption		= TEST_FLUENT_BIT(game_automaton->valuations, fluent_index);
	if(satisfies_assumption){
		if(current_ranking->assumption_to_satisfy < (int32_t)(assumptions_count - 1)){
			target_ranking->assumption_to_satisfy++;
		}else if(current_ranking->value < (int32_t)(max_delta[current_guarantee])){
			target_ranking->value++;
			target_ranking->assumption_to_satisfy = first_assumption_index;
		}else{
			target_ranking->value 		= RANKING_INFINITY;
		}
	}
}

void automaton_ranking_update(automaton_automaton* game_automaton, automaton_concrete_bucket_list** ranking, automaton_ranking* current_ranking, uint32_t* max_delta
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t assumptions_count
		, uint32_t* guarantees_indexes, uint32_t* assumptions_indexes, uint32_t first_assumption_index){
	automaton_ranking incr_ranking;
	/**
	 * get best_j(v)
	 */
	automaton_ranking* best_ranking		= automaton_state_best_successor_ranking(game_automaton, current_ranking->state, ranking, current_guarantee, guarantee_count, guarantees_indexes);
#if DEBUG_SYNTHESIS
	if(_print_ranking){
		uint32_t fluent_index		= GET_STATE_FLUENT_INDEX(game_automaton->context->global_fluents_count,
				current_ranking->state, guarantees_indexes[current_guarantee]);
		bool satisfies_guarantee	= TEST_FLUENT_BIT(game_automaton->valuations, fluent_index);
		printf("[B ] %d <%d:%d, %d> best succ <%d, %d>", current_ranking->state, current_guarantee, current_ranking->value
				, current_ranking->assumption_to_satisfy, best_ranking->value, best_ranking->assumption_to_satisfy);
		if(satisfies_guarantee){
			printf("satisfies %d\n", current_guarantee);
		}else{
			printf("\n");
		}
	}
#endif
	/**
	 * get incr_j_v(best_j(v))
	 */
	automaton_ranking_increment(game_automaton, ranking, best_ranking, current_ranking->state, max_delta, current_guarantee, guarantee_count, assumptions_count
			, guarantees_indexes, assumptions_indexes, first_assumption_index, &incr_ranking);
#if DEBUG_SYNTHESIS
	if(_print_ranking)
		printf("[In] %d <%d:%d, %d> incr <%d, %d>\n", current_ranking->state, current_guarantee, current_ranking->value
			, current_ranking->assumption_to_satisfy, incr_ranking.value, incr_ranking.assumption_to_satisfy);
#endif

	/**
	 * update to max between current ranking and the next
	 */
#if DEBUG_SYNTHESIS
	uint32_t old_value	= current_ranking->value;
	uint32_t old_ass	= current_ranking->assumption_to_satisfy;
#endif
	if(automaton_ranking_gt(&incr_ranking, current_ranking)){
		current_ranking->value			= incr_ranking.value;
		current_ranking->assumption_to_satisfy	= incr_ranking.assumption_to_satisfy;
	}
#if DEBUG_SYNTHESIS
	if(_print_ranking)
	printf("[U ] %d <%d:%d, %d> update <%d, %d> was <%d, %d>\n", current_ranking->state, current_guarantee, current_ranking->value
			, current_ranking->assumption_to_satisfy, incr_ranking.value, incr_ranking.assumption_to_satisfy
			, old_value, old_ass);
#endif
}

automaton_automaton* automaton_get_gr1_strategy(automaton_automaton* game_automaton, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count, bool print_ranking){
	clock_t begin = clock();
	_print_ranking	= print_ranking;
	uint32_t pending_processed	= 0;
	//TODO: preallocate pending states and rankings
	automaton_automata_context* ctx				= game_automaton->context;
	uint32_t i, j ,k, l, m;
	automaton_concrete_bucket_list** ranking_list	= malloc(sizeof(automaton_concrete_bucket_list*) * guarantees_count);
	automaton_max_heap* pending_list	= automaton_max_heap_create(sizeof(automaton_pending_state)
			, automaton_pending_state_compare);
	automaton_concrete_bucket_list** key_lists		= calloc(guarantees_count, sizeof(automaton_concrete_bucket_list*));
	for(i = 0; i < guarantees_count; i++)
				key_lists[i]					= automaton_concrete_bucket_list_create(RANKING_BUCKET_SIZE, automaton_pending_state_extractor, sizeof(automaton_pending_state));
	uint32_t current_state;
	//these are the indexes from the global fluents list, should not be used in ranking_list
	uint32_t* assumptions_indexes				= malloc(sizeof(uint32_t) * assumptions_count);
	uint32_t* guarantees_indexes				= malloc(sizeof(uint32_t) * guarantees_count);
	int32_t first_assumption_index				= 0;
	uint32_t fluent_count						= ctx->global_fluents_count;
	uint32_t fluent_index;
	//get assumptions and guarantees indexes
	for(i = 0; i < assumptions_count; i++){
		for(j = 0; j < ctx->global_fluents_count; j++){
			if(strcmp(assumptions[i], ctx->global_fluents[j].name) == 0){
				assumptions_indexes[i]	= j;
				/*if (i == 0)
					first_assumption_index	= (int32_t)j;*/
				break;
			}
		}
	}
	for(i = 0; i < guarantees_count; i++){
		for(j = 0; j < ctx->global_fluents_count; j++){
			if(strcmp(guarantees[i], ctx->global_fluents[j].name) == 0){
				guarantees_indexes[i]	= j;
				break;
			}
		}
	}
	//get max_l(|fi_l - gamma_g|)
	automaton_bucket_list *bigger_bucket_list, *smaller_bucket_list;
	uint32_t* max_delta	= malloc(sizeof(uint32_t) * guarantees_count);
	uint32_t current_delta;
#if DEBUG_SYNTHESIS
	if(_print_ranking)
		printf("!!COMPUTING INFINITY!!\n");
#endif
	for(l = 0; l < guarantees_count; l++){
		max_delta[l]	= 0;
		for(k = 0; k < assumptions_count; k++){
			if(game_automaton->inverted_valuations[assumptions_indexes[k]]->composite_count >
				game_automaton->inverted_valuations[guarantees_indexes[l]]->composite_count){
				bigger_bucket_list	= game_automaton->inverted_valuations[assumptions_indexes[k]];
				smaller_bucket_list	= game_automaton->inverted_valuations[guarantees_indexes[l]];
			}else{
				smaller_bucket_list	= game_automaton->inverted_valuations[assumptions_indexes[k]];
				bigger_bucket_list	= game_automaton->inverted_valuations[guarantees_indexes[l]];
			}
			current_delta	= 0;
			for(i = 0; i < bigger_bucket_list->count; i++)
				for(j = 0; j < bigger_bucket_list->bucket_count[i]; j++)
					if(!automaton_bucket_has_entry(smaller_bucket_list, bigger_bucket_list->buckets[i][j]))
						current_delta++;
			max_delta[l]	= current_delta > max_delta[l] ? current_delta : max_delta[l];
#if DEBUG_SYNTHESIS
	if(_print_ranking)
		printf("Infinity_%d:%d", l, max_delta[l]);
#endif
		}
	}
#if DEBUG_SYNTHESIS
	if(_print_ranking){
		printf("\n");
		automaton_automaton_print(game_automaton, false, false, true, "", "\n");
	}
#endif
	//initialize transitions ranking
	automaton_ranking concrete_ranking;
	automaton_pending_state concrete_pending_state;
	for(i = 0; i < guarantees_count; i++)
		ranking_list[i]	= automaton_concrete_bucket_list_create(RANKING_BUCKET_SIZE, automaton_ranking_key_extractor, sizeof(automaton_ranking));
	for(i = 0; i < game_automaton->transitions_count; i++){
		for(j = 0; j < guarantees_count; j++){
			if(game_automaton->out_degree[i] == 0){
				//state is deadlock
				concrete_ranking.state	= i; concrete_ranking.assumption_to_satisfy	= 0;
				concrete_ranking.value	= RANKING_INFINITY;
				automaton_concrete_bucket_add_entry(ranking_list[j], &concrete_ranking);
			}else{
				//rank_g(state) = (0, 1)
				concrete_ranking.state	= i; concrete_ranking.assumption_to_satisfy	= 0;
				concrete_ranking.value	= 0;
				automaton_concrete_bucket_add_entry(ranking_list[j], &concrete_ranking);
				//g falsifies one guarantee and satisfies ass_1 then add to pending
				fluent_index	= GET_STATE_FLUENT_INDEX(fluent_count, i, assumptions_indexes[first_assumption_index]);
				if(TEST_FLUENT_BIT(game_automaton->valuations, fluent_index)){
					for(l = 0; l < guarantees_count; l++){
						fluent_index	= GET_STATE_FLUENT_INDEX(fluent_count, i, guarantees_indexes[l]);
						if(!TEST_FLUENT_BIT(game_automaton->valuations, fluent_index)){
#if DEBUG_SYNTHESIS
	if(_print_ranking)
		printf("[Init] Pushing into pending (%d, %d)\n", i, guarantees_indexes[j]);
#endif
							concrete_pending_state.state 	= i; concrete_pending_state.goal_to_satisfy	= j;//TODO: check this, was ...goal_to_satisfy = guarantees_indexes[j];
							concrete_pending_state.value	= 0; concrete_pending_state.timestamp		= 0;
							concrete_pending_state.assumption_to_satisfy = 0;
							void* new_entry					= automaton_max_heap_add_entry(pending_list, &concrete_pending_state);
							automaton_concrete_bucket_add_entry(key_lists[j], new_entry);
							break;
						}
					}
				}
			}
		}

	}
	//add unstable pred.
	for(i = 0; i < game_automaton->transitions_count; i++){
		for(j = 0; j < guarantees_count; j++){
			if(game_automaton->out_degree[i] == 0){
#if DEBUG_SYNTHESIS
	if(_print_ranking)
		printf("[Deadlock] Adding unstable pred for %d\n", i);
#endif
				automaton_add_unstable_predecessors(game_automaton, pending_list, key_lists[j], i, ranking_list, /*guarantees_indexes[j] <- WAS*/ j, guarantees_count
						, assumptions_count, guarantees_indexes, assumptions_indexes, assumptions_indexes[first_assumption_index], pending_processed);
			}
		}
	}



	char strategy_name[255];
	sprintf(strategy_name, "%s_strategy", game_automaton->name);
	//stabilization
	automaton_pending_state current_pending_state;
	automaton_ranking* current_ranking;
	automaton_automaton* strategy = NULL;
#if DEBUG_SYNTHESIS
	if(_print_ranking){
		printf("!!STABILIZING GR1 GAME FOR %s!!\n", strategy_name);
		printf("[Action] State <ranking index:ranking value, assumption to satisfy> ...\n");
	}
#endif
	automaton_pending_state* state_location;
	uint32_t goal_to_satisfy;
	while(pending_list->count > 0){
		//current_pending_state	= (automaton_pending_state*)automaton_ptr_bucket_pop_entry(pending_list);
		automaton_max_heap_pop_entry(pending_list, &current_pending_state);

		goal_to_satisfy	= current_pending_state.goal_to_satisfy;

		state_location	= (automaton_pending_state*)automaton_concrete_bucket_get_entry(key_lists[goal_to_satisfy]
																								 , current_pending_state.state);
		//TODO:check, why are we getting a NULL at state location
		if(state_location != NULL)
			automaton_concrete_bucket_remove_entry(key_lists[goal_to_satisfy], state_location);
		current_ranking			= (automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[current_pending_state.goal_to_satisfy], current_pending_state.state);
#if DEBUG_SYNTHESIS
		if(_print_ranking)
			printf("[<-] %d <%d:%d, %d> Pop from pending with ranking\n", current_pending_state.state, current_pending_state.goal_to_satisfy, current_ranking->value, current_ranking->assumption_to_satisfy);
#endif
		if(current_ranking->value == RANKING_INFINITY){
			pending_processed++;
			continue;
		}

		if(automaton_state_is_stable(game_automaton, current_pending_state.state, ranking_list
				, current_pending_state.goal_to_satisfy, guarantees_count, assumptions_count
				, guarantees_indexes, assumptions_indexes, first_assumption_index)){
			pending_processed++;
			continue;
		}

		automaton_ranking_update(game_automaton, ranking_list,
				current_ranking
				//(automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[current_pending_state.goal_to_satisfy], current_pending_state.state)
				, max_delta, current_pending_state.goal_to_satisfy, guarantees_count, assumptions_count
				, guarantees_indexes, assumptions_indexes, first_assumption_index);
#if DEBUG_SYNTHESIS
	if(_print_ranking)
		printf("[Pr] %d <%d:%d, %d> Adding unstable preds.\n", current_pending_state.state, current_pending_state.goal_to_satisfy, current_ranking->value, current_ranking->assumption_to_satisfy);
#endif
		automaton_add_unstable_predecessors(game_automaton, pending_list, key_lists[goal_to_satisfy], current_pending_state.state
				, ranking_list, current_pending_state.goal_to_satisfy, guarantees_count
				, assumptions_count, guarantees_indexes, assumptions_indexes, first_assumption_index, pending_processed);
		pending_processed++;
#if PRINT_PARTIAL_SYNTHESIS
		if((pending_processed % 900000) == 0){
			printf("Partial Synthesis has [%09d] pending states and [%012d] processed states, running for [%08f]s\n",
					pending_list->count, pending_processed, (double)(clock() - begin) / CLOCKS_PER_SEC);

		}
#endif
	}
#if DEBUG_SYNTHESIS || DEBUG_STRATEGY_BUILD
	if(_print_ranking){
		printf("!!RANKING STABILIZATION ACHIEVED FOR %s!!\n", strategy_name);
		for(i = 0; i < game_automaton->transitions_count; i++){
			//if(game_automaton->out_degree[i] <= 0) continue;
			printf("[S %d:", i);
			for(j = 0; j < guarantees_count; j++){
				current_ranking	= ((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[j], i));
				printf("(g:%d,<%d,%d>)", j, current_ranking->value, current_ranking->assumption_to_satisfy);
			}
			printf("]\n");
		}
		printf("\tpending:%d\n", pending_list->count);
	}
#endif
#if VERBOSE
	printf("Synthesis processed [%09d] states, run for [%08f]s\n", pending_processed, (double)(clock() - begin) / CLOCKS_PER_SEC);
	//build strategy
#endif
	strategy	= automaton_automaton_create(strategy_name, game_automaton->context, game_automaton->local_alphabet_count, game_automaton->local_alphabet, false, false);



	bool is_winning = true;
	bool one_option = false;
	//check if all rankings have the initial state
	for(i = 0; i < guarantees_count; i++){
		one_option	= false;
		for(j = 0; j < game_automaton->out_degree[game_automaton->initial_states[0]]; j++){
#if DEBUG_STRATEGY_BUILD
			printf("[CHK] RANKING FOR %d: %d\n", game_automaton->transitions[game_automaton->initial_states[0]][j].state_to,
					((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[i], game_automaton->transitions[game_automaton->initial_states[0]][j].state_to))->value);
#endif
			if(((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[i]
																					 , game_automaton->transitions[game_automaton->initial_states[0]][j].state_to))->value != RANKING_INFINITY){

				one_option = true;
			}
		}
		if(!one_option){
			is_winning = false;
			//break;
		}
	}
	if(is_winning){
		//keep winning states
		uint32_t new_count;
		void** new_list;
		automaton_ranking *current_ranking, *succ_ranking;
		automaton_transition* current_transition;
		int32_t b;
		//build strategy automaton out of ranking_list
		//define map between strategy compose state (s x goal) to strategy automaton state (s)
		uint32_t** strategy_maps			= malloc(sizeof(uint32_t*) * guarantees_count);
		bool** strategy_maps_is_set	= malloc(sizeof(bool*) * guarantees_count);
		uint32_t last_strategy_state	= 0;
		uint32_t strategy_from_state, strategy_to_state;
		uint32_t succ_guarantee;
		bool may_increase, is_controllable;
		automaton_transition* strategy_transition	= automaton_transition_create(0, 0);
		for(i = 0; i < guarantees_count; i++){
			strategy_maps[i]		= malloc(sizeof(uint32_t) * game_automaton->transitions_count);
			strategy_maps_is_set[i]	= malloc(sizeof(bool) * game_automaton->transitions_count);
			for(j = 0; j < game_automaton->transitions_count; j++)
				strategy_maps_is_set[i][j]	= false;
		}

		for(i = 0; i < guarantees_count; i++){
			for(j = 0; j < ranking_list[i]->count; j++){
				for(b = (int32_t)ranking_list[i]->bucket_count[j] - 1; b >= 0; b--){

					current_ranking = (automaton_ranking*)GET_CONCRETE_BUCKET_LIST_ENTRY(ranking_list[i], j, b);
					if(!strategy_maps_is_set[i][current_ranking->state]){
						strategy_maps[i][current_ranking->state]	= last_strategy_state++;
						strategy_maps_is_set[i][current_ranking->state]	= true;
					}
					//v satisfies current guarantee v in Q_j
					may_increase	= automaton_bucket_has_entry(game_automaton->inverted_valuations[guarantees_indexes[i]], current_ranking->state);

					//is_controllable	= game_automaton->is_controllable[current_ranking->state];
					is_controllable = true;
					//TODO: check, was this, but changed in order to ensure strategy swap
					//succ_guarantee	= may_increase? ((i + 1) % guarantees_count) : i;
					succ_guarantee	= may_increase && is_controllable ? ((i + 1) % guarantees_count) : i;
					for(l = 0; l < game_automaton->out_degree[current_ranking->state] && is_controllable; l++){
						current_transition	= &(game_automaton->transitions[current_ranking->state][l]);
						for(m = 0; m < current_transition->signals_count; m++){
							automaton_signal_event * evt =
									&ctx->global_alphabet->list[GET_TRANSITION_SIGNAL(current_transition,m)];
							if(evt->type == INPUT_SIG){
								is_controllable = false;
								break;
							}
						}
					}
					for(l = 0; l < game_automaton->out_degree[current_ranking->state]; l++){
						current_transition	= &(game_automaton->transitions[current_ranking->state][l]);
						succ_ranking		=  (automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[succ_guarantee], current_transition->state_to);
						//TODO: check this condition
						if(succ_ranking == NULL){
							continue;
						}
						//check if succ ranking is better than current
						if(!is_controllable
						||
						(may_increase && succ_ranking->value != RANKING_INFINITY)
						||
						(
							!may_increase
							&&
							(
								(
									(
										current_ranking->value > succ_ranking->value
										||
										(
											current_ranking->value == RANKING_INFINITY
											&& succ_ranking->value != RANKING_INFINITY
										)
									)
									||
									(
										current_ranking->value == succ_ranking->value
										&&
										current_ranking->assumption_to_satisfy > succ_ranking->assumption_to_satisfy
									)
								)
								||
								(
									current_ranking->assumption_to_satisfy == succ_ranking->assumption_to_satisfy
									&&
									current_ranking->value == succ_ranking->value
									&&
									!automaton_bucket_has_entry(game_automaton->inverted_valuations[assumptions_indexes[current_ranking->assumption_to_satisfy]], current_ranking->state)
								)
							)
						))
						/*(!is_controllable || ((may_increase && succ_ranking->value != RANKING_INFINITY)
							||(!may_increase && (
								((current_ranking->value > succ_ranking->value || (current_ranking->value == RANKING_INFINITY && succ_ranking->value != RANKING_INFINITY))||
										(current_ranking->value == succ_ranking->value && current_ranking->assumption_to_satisfy > succ_ranking->assumption_to_satisfy)
								)||(current_ranking->assumption_to_satisfy == succ_ranking->assumption_to_satisfy
								&& current_ranking->value == succ_ranking->value
								&& !automaton_bucket_has_entry(game_automaton->inverted_valuations[assumptions_indexes[current_ranking->assumption_to_satisfy]], current_ranking->state))))))
						*/
						{
							if(!strategy_maps_is_set[i][current_ranking->state]){
								strategy_maps[i][current_ranking->state]	= last_strategy_state++;
								strategy_maps_is_set[i][current_ranking->state]	= true;
							}
							if(!strategy_maps_is_set[succ_guarantee][succ_ranking->state]){
								strategy_maps[succ_guarantee][succ_ranking->state]	= last_strategy_state++;
								strategy_maps_is_set[succ_guarantee][succ_ranking->state] = true;
							}
							bool is_input	= false;
							free(strategy_transition->other_signals);
							strategy_transition->other_signals = NULL;
							automaton_transition_copy(current_transition, strategy_transition);
							strategy_transition->state_from	= strategy_maps[i][current_ranking->state];
							strategy_transition->state_to	= strategy_maps[succ_guarantee][succ_ranking->state];
							/*
							strategy_transition->signals_size	= current_transition->signals_size;
							strategy_transition->signals_count	= current_transition->signals_count;
							for(m = 0; m < FIXED_SIGNALS_COUNT; m++){
								strategy_transition->signals[m]		= current_transition->signals[m];
							}
							strategy_transition->other_signals	= current_transition->other_signals;
							*/
							for(m = 0; m < current_transition->signals_count; m++){
								automaton_signal_event * evt = &ctx->global_alphabet->list[GET_TRANSITION_SIGNAL(current_transition,m)];
								if(evt->type == INPUT_SIG){
									is_input = true;
									break;
								}
							}
							if((!is_controllable && (is_input || current_transition->is_input)) || (succ_ranking->value != RANKING_INFINITY && !(!is_controllable && !is_input))){
#if DEBUG_STRATEGY_BUILD
								printf("(g:%d)[ADD] strategy transition: ", i);
								automaton_transition_print(strategy_transition, strategy->context, "", "\n");
#endif
								automaton_automaton_add_transition(strategy, strategy_transition);

							}
#if DEBUG_STRATEGY_BUILD
							else{
								printf("(g:%d)[INF] strategy transition: ", i);
								automaton_transition_print(strategy_transition, strategy->context, "", "\n");
							}
#endif

							//add initial state if not already added
							if(strategy->initial_states_count == 0 && current_ranking->state == game_automaton->initial_states[0]){
#if DEBUG_STRATEGY_BUILD
								printf("(g:%d)[INI] Initial state added as %d\n",i, strategy_maps[i][current_ranking->state]);
#endif
								automaton_automaton_add_initial_state(strategy, strategy_maps[i][current_ranking->state]);
							}
						}
					}
				}
			}
		}
		for(i = 0; i < guarantees_count; i++){
			free(strategy_maps[i]);
			free(strategy_maps_is_set[i]);
		}
		free(strategy_maps);
		free(strategy_maps_is_set);
		//we dont want to call destroy since signals are owned by original transition
		free(strategy_transition);

	}
	if(print_ranking){
		automaton_automaton *clone = automaton_automaton_clone(game_automaton);
		automaton_ranking_print_report(clone, ranking_list, max_delta, guarantees_count);
		uint32_t clone_len	= strlen(clone->name)+30;
		char *clone_name	= calloc(clone_len, sizeof(char));
		snprintf(clone_name, clone_len, "%s_minimized", clone->name);
		free(clone->name);
		clone->name	= clone_name;
		automaton_automaton_remove_deadlocks(clone);
		automaton_ranking_print_report(clone, ranking_list, max_delta, guarantees_count);
		automaton_automaton_destroy(clone);
	}
	//destroy structures
	free(max_delta);
	for(i = 0; i < guarantees_count; i++){
		automaton_concrete_bucket_destroy(ranking_list[i]);
	}
	free(ranking_list);
	automaton_max_heap_destroy(pending_list);
	for(i = 0; i < guarantees_count; i++)
		automaton_concrete_bucket_destroy(key_lists[i]);
	free(key_lists);
	free(assumptions_indexes);
	free(guarantees_indexes);
	automaton_automaton_remove_unreachable_states(strategy);
#if DEBUG_STRATEGY_BUILD
	printf("Game\n=====\n");
	automaton_automaton_print(game_automaton, false, false, false, "", "\n");

	printf("Built Strategy\n=====\n");
	automaton_automaton_print(strategy, false, false, false, "", "\n");
#endif
	return strategy;

}

bool automaton_is_gr1_realizable(automaton_automaton* game_automaton, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count){
	automaton_automaton* strategy = automaton_get_gr1_strategy(game_automaton, assumptions, assumptions_count,
			guarantees, guarantees_count, false);
	bool is_realizable = (strategy->transitions_count != 0
			&& strategy->out_degree[strategy->initial_states[0]] > 0);
	automaton_automaton_destroy(strategy);
	return is_realizable;
}

automaton_automaton* automaton_get_gr1_unrealizable_minimization(automaton_automaton* game_automaton, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count){

	//candidate transitions lists
	uint32_t t_size = LIST_INITIAL_SIZE, t_count = 0;
	uint32_t *t_states	= calloc(t_size, sizeof(uint32_t)), *t_indexes = calloc(t_size, sizeof(uint32_t));
	//transitions to remove lists
	uint32_t r_size = LIST_INITIAL_SIZE, r_count = 0;
	uint32_t *r_states	= calloc(r_size, sizeof(uint32_t)), *r_indexes = calloc(r_size, sizeof(uint32_t));

	int32_t i,j,k, from_step = 0;
	uint32_t new_size; uint32_t *ptr;

	//get a master copy of the game, remove controllable transitions from mixed states,
		//initialize list of non controllable transitions
	automaton_automaton *master	= automaton_automaton_clone(game_automaton);
	automaton_automaton_add_initial_state(master, game_automaton->initial_states[0]);
	automaton_transition *current_transition	= NULL;
	for(i = 0; i < master->transitions_count; i++){
		for(j = master->out_degree[i] - 1; j >= 0; j--){
			current_transition = &(master->transitions[i][j]);
			if(!master->is_controllable[i]){
				if(!current_transition->is_input)
					automaton_automaton_remove_transition(master, current_transition);
			}
		}
	}
	for(i = 0; i < master->transitions_count; i++){
		for(j = master->out_degree[i] - 1; j >= 0; j--){
			current_transition = &(master->transitions[i][j]);
			if(!master->is_controllable[i]){
				if(current_transition->is_input){
					if(t_count == t_size){
						new_size	= t_size * LIST_INCREASE_FACTOR;
						ptr	= realloc(t_states, new_size * sizeof(uint32_t));
						if(ptr == NULL){printf("Could not allocate memory\n"); exit(-1);}
						t_states	= ptr;
						ptr	= realloc(t_indexes, new_size * sizeof(uint32_t));
						if(ptr == NULL){printf("Could not allocate memory\n"); exit(-1);}
						t_indexes	= ptr;					t_size		= new_size;
					}
					t_states[t_count]	= i;	t_indexes[t_count++]	= j;
				}
			}
		}
	}
	automaton_automaton_remove_deadlocks(master);
	automaton_automaton_update_valuations(master);
	automaton_automaton *minimization	= automaton_automaton_clone(master);

	bool minimized	= false;
	uint32_t steps = 0;
	while(t_count > 0){
		//get next candidate
		current_transition	= &(master->transitions[t_states[t_count - 1]][t_indexes[t_count - 1]]); t_count--;
		//do not consider missing transitions or transitions that would induce deadlocks
		while(minimization->out_degree[current_transition->state_from] == 1){
			if(t_count == 0){
				minimized = true;
				break;
			}

			steps++;
			current_transition	= &(master->transitions[t_states[t_count - 1]][t_indexes[t_count - 1]]); t_count--;
		}
		if(minimized){
			automaton_automaton_destroy(minimization);
			minimization = automaton_automaton_clone(master);
			for(i =0 ; i < r_count; i++){
				current_transition	= &(master->transitions[r_states[i]][r_indexes[i]]);
				automaton_automaton_remove_transition(minimization, current_transition);
			}
			break;
		}
		//evaluate next reduction
		if(r_count == r_size){
			new_size	= r_size * LIST_INCREASE_FACTOR;
			ptr	= realloc(r_states, new_size * sizeof(uint32_t));
			if(ptr == NULL){printf("Could not allocate memory\n"); exit(-1);}
			r_states	= ptr;
			ptr	= realloc(r_indexes, new_size * sizeof(uint32_t));
			if(ptr == NULL){printf("Could not allocate memory\n"); exit(-1);}
			r_indexes	= ptr;					r_size		= new_size;
		}
		r_states[r_count]	= t_states[t_count];	r_indexes[r_count++]	= t_indexes[t_count];
		steps++;
		automaton_automaton_remove_transition(minimization, current_transition);
		automaton_automaton_remove_deadlocks(minimization);
		automaton_automaton_update_valuations(minimization);

		if(automaton_is_gr1_realizable(minimization, assumptions, assumptions_count,
				guarantees, guarantees_count) || minimization->out_degree[minimization->initial_states[0]] == 0){
			automaton_automaton_add_transition(minimization, current_transition);
			r_count--;
			automaton_automaton_destroy(minimization);
			minimization = automaton_automaton_clone(master);
			for(i =0 ; i < r_count; i++){
				current_transition	= &(master->transitions[r_states[i]][r_indexes[i]]);
				automaton_automaton_remove_transition(minimization, current_transition);
			}
			automaton_automaton_remove_deadlocks(minimization);
			automaton_automaton_update_valuations(minimization);
#if DEBUG_UNREAL
			printf("Minimizing [%d,%d,R,%d]\n", t_count, r_count,steps);
#endif
		}else{
			/*
			automaton_automaton_destroy(last_unrealizable);
			last_unrealizable = automaton_automaton_clone(minimization);
			*/
			automaton_automaton_remove_deadlocks(minimization);
			automaton_automaton_update_valuations(minimization);
			from_step = steps;
#if DEBUG_UNREAL
			printf("Minimizing [%d,%d,N,%d]\n", t_count, r_count, from_step);
#endif
		}
		fflush(stdout);
	}



	if(automaton_is_gr1_realizable(minimization, assumptions, assumptions_count,
				guarantees, guarantees_count) || minimization->out_degree[minimization->initial_states[0]] == 0){
		automaton_automaton_destroy(minimization);
		minimization = automaton_automaton_clone(master);
		for(i =0 ; i < r_count; i++){
			current_transition	= &(master->transitions[r_states[i]][r_indexes[i]]);
			automaton_automaton_remove_transition(minimization, current_transition);
		}
		automaton_automaton_remove_deadlocks(minimization);
		automaton_automaton_update_valuations(minimization);
		printf("[%d,%d,R,%d]\n", t_count, r_count,steps);
		printf("[%d,%d]Returning restored from %d\n", t_count, r_count, from_step);
	}else{
		printf("[%d,%d]Returning last from %d\n", t_count, r_count, from_step);
	}

	free(t_states); free(t_indexes);
	free(r_states); free(r_indexes);
	//print rankings
	automaton_automaton* strategy = automaton_get_gr1_strategy(minimization, assumptions, assumptions_count,
			guarantees, guarantees_count, true);
	automaton_automaton_destroy(strategy);

	automaton_automaton_destroy(master);
	return minimization;
}

void automaton_automaton_print_traces_to_deadlock(automaton_automaton* automaton, uint32_t max_traces){
	uint32_t count;
	uint32_t printed_traces = 0;
	uint32_t *sizes;
	uint32_t **traces	= automaton_automaton_traces_to_deadlock(automaton, automaton->initial_states[0], &count, &sizes);
	if(traces == NULL)return;
	uint32_t i, j, k,l;
	uint32_t current_state, next_state;
	automaton_transition *current_transition;
	if(count > 0){
		printf("Traces to deadlock in %s\n==============\n", automaton->name);
		for(i = 0; i < count; i++){
			for(j = 0; j < sizes[i]; j++){
				current_state	= traces[i][j];
				if(j < (sizes[i] - 1)){
					printf("[%d]<", current_state);
					next_state	= traces[i][j + 1];
					for(k = 0; k < automaton->out_degree[current_state]; k++){
						current_transition	= &(automaton->transitions[current_state][k]);
						if(current_transition->state_to == next_state){
							for(l = 0; l < current_transition->signals_count; l++){
								signal_t sig = l < FIXED_SIGNALS_COUNT ? current_transition->signals[l] : current_transition->other_signals[l-FIXED_SIGNALS_COUNT];
								printf("%s%s", automaton->context->global_alphabet->list[sig].name, (l < (current_transition->signals_count -1)? "," : ""));
							}
						}
					}
					printf(">");
				}else{
					printf("[%d](x)\n", current_state);
				}
			}
			printed_traces++;
			if(printed_traces > max_traces && max_traces != 0)
				break;
		}
	}
	free(sizes);
	for(i = 0; i < count; i++)free(traces[i]);
	free(traces);
}

uint32_t** automaton_automaton_traces_to_deadlock(automaton_automaton* automaton, uint32_t initial_state, uint32_t *count, uint32_t **sizes){
	if(initial_state >= automaton->transitions_count){
		printf("Initial state %d higher than transitions count %d\n", initial_state, automaton->transitions_count);
		*count 				= 0;
		return NULL;
	}

	*count 				= 0;
	uint32_t* distances	= calloc(automaton->transitions_count, sizeof(uint32_t));
	bool* visited		= calloc(automaton->transitions_count, sizeof(bool));
	uint32_t* visited_through = calloc(automaton->transitions_count, sizeof(uint32_t));
	uint32_t* deadlocks = calloc(automaton->transitions_count, sizeof(uint32_t));
	automaton_concrete_bucket_list* pending_list		= automaton_concrete_bucket_list_create(DISTANCE_BUCKET_SIZE, automaton_int_extractor, sizeof(uint32_t));
	int32_t i, j;
	uint32_t min_distance;
	automaton_transition* current_transition;
	uint32_t current_state;
	//add initial states (excluding incoming state)
	visited[initial_state]		= true;
	for(i = 0; i < automaton->out_degree[initial_state]; i++){
		current_transition	= &(automaton->transitions[initial_state][i]);
		if(current_transition->state_to != initial_state && !visited[current_transition->state_to]){
			automaton_concrete_bucket_add_entry(pending_list, &(current_transition->state_to));
			visited_through[current_transition->state_to]	= initial_state;
		}
	}
	//update distances
	while(pending_list->composite_count > 0){
		automaton_concrete_bucket_pop_entry(pending_list, &current_state);
		if(visited[current_state])continue;
		visited[current_state]	= true;
		min_distance			= 0;
		for(i = 0; i < automaton->in_degree[current_state]; i++){
			current_transition	= &(automaton->inverted_transitions[current_state][i]);
			if(!visited[current_transition->state_from])continue;
			if(min_distance == 0 || (current_transition->state_from + 1 < min_distance))min_distance	= distances[current_transition->state_from] + 1;
		}
		distances[current_state]	= min_distance;
		if(automaton->out_degree[current_state] == 0){
			deadlocks[*count]	= current_state;
			*count += 1;
		}
		for(i = 0; i < automaton->out_degree[current_state]; i++){
			current_transition	= &(automaton->transitions[current_state][i]);
			if(!visited[current_transition->state_to]){
				automaton_concrete_bucket_add_entry(pending_list, &(current_transition->state_to));
				visited_through[current_transition->state_to]	= current_state;
			}
		}

	}

	automaton_concrete_bucket_destroy(pending_list);

	uint32_t **deadlock_traces = calloc(*count, sizeof(uint32_t*));
	uint32_t *local_sizes	= calloc(*count,sizeof(uint32_t));
	uint32_t current_length;
	uint32_t last_state;
	for(i = 0; i < *count; i++){
		current_length	= distances[deadlocks[i]] + 1;
		local_sizes[i] 	= current_length;
		last_state		= deadlocks[i];
		deadlock_traces[i]	= calloc((current_length), sizeof(uint32_t));
		deadlock_traces[i][current_length - 1]	= last_state;
		for(j = current_length - 2; j >= 0 ; j--){
			last_state = deadlock_traces[i][j]	=  visited_through[last_state];
		}
	}
	free(visited);
	free(visited_through);
	free(deadlocks);
	free(distances);
	*sizes	= local_sizes;
	return deadlock_traces;
}


uint32_t* automaton_automaton_distance_to_state(automaton_automaton* automaton, uint32_t state){
	uint32_t* distances	= calloc(automaton->transitions_count, sizeof(uint32_t));
	bool* visited		= calloc(automaton->transitions_count, sizeof(bool));
	automaton_concrete_bucket_list* pending_list		= automaton_concrete_bucket_list_create(DISTANCE_BUCKET_SIZE, automaton_int_extractor, sizeof(uint32_t));
	uint32_t i;
	uint32_t min_distance;
	automaton_transition* current_transition;
	uint32_t current_state;
	//add initial states (excluding incoming state)
	visited[state]		= true;
	for(i = 0; i < automaton->out_degree[state]; i++){
		current_transition	= &(automaton->transitions[state][i]);
		if(current_transition->state_to != state && !visited[current_transition->state_to]){

			automaton_concrete_bucket_add_entry(pending_list, &(current_transition->state_to));
		}
	}
	//update distances
	while(pending_list->composite_count > 0){
		automaton_concrete_bucket_pop_entry(pending_list, &current_state);
		if(current_state >= automaton->transitions_count)continue;
		if(visited[current_state])continue;
		visited[current_state]	= true;
		min_distance			= 0;
		for(i = 0; i < automaton->in_degree[current_state]; i++){
			current_transition	= &(automaton->inverted_transitions[current_state][i]);
			if(!visited[current_transition->state_from])continue;
			if(min_distance == 0 || (current_transition->state_from + 1 < min_distance))min_distance	= distances[current_transition->state_from] + 1;
		}
		distances[current_state]	= min_distance;

		for(i = 0; i < automaton->out_degree[current_state]; i++){
			current_transition	= &(automaton->transitions[current_state][i]);
			if(!visited[current_transition->state_to]){
				automaton_concrete_bucket_add_entry(pending_list, &(current_transition->state_to));
			}
		}

	}

	automaton_concrete_bucket_destroy(pending_list);
	free(visited);
	return distances;
}

void automaton_automaton_remove_unreachable_states(automaton_automaton* automaton){
	if(automaton->initial_states_count < 1)return;
	if(automaton->transitions_count < 1)return;
	uint32_t* distances	= automaton_automaton_distance_to_state(automaton, automaton->initial_states[0]);
	uint32_t i,j;
	bool unreachable_found = false;
	do{
		unreachable_found = false;
		for(i = 0; i < automaton->transitions_count; i++){
			if(i == automaton->initial_states[0])continue;
			if((automaton->in_degree[i] != 0) && (distances[i] != 0))continue;
			if(automaton->in_degree[i] == 0 && automaton->out_degree[i] == 0)continue;
			unreachable_found = true;
			for(j = 0; j < automaton->out_degree[i]; j++){
				automaton_automaton_remove_transition(automaton, &(automaton->transitions[i][j]));
			}
		}
	}while(unreachable_found);
	free(distances);
}

void automaton_automaton_remove_deadlocks(automaton_automaton* automaton){
	if(automaton->initial_states_count < 1)return;
	uint32_t i, j;
	bool deadlock_found = false;
	do{
		deadlock_found = false;
		for(i = 0; i < automaton->transitions_count; i++){
			if(automaton->out_degree[i] == 0 && automaton->in_degree[i] > 0){
				deadlock_found = true;
				for(j = 0; j < automaton->in_degree[i]; j++){
					automaton_automaton_remove_transition(automaton, &(automaton->inverted_transitions[i][j]));
				}
			}
		}
	}while(deadlock_found);
	automaton_automaton_remove_unreachable_states(automaton);
}

void automaton_automaton_update_valuations(automaton_automaton* automaton){
	uint32_t i,j;
	for(i = 0; i < automaton->transitions_count; i++){
		if(automaton->in_degree[i] == 0 && automaton->out_degree[i] == 0){
			for(j = 0; j < automaton->context->global_fluents_count; j++){
				if(automaton_bucket_has_entry(automaton->inverted_valuations[j], i)){
					automaton_bucket_remove_entry(automaton->inverted_valuations[j], i);
				}
			}
		}
	}
}

uint32_t automaton_automata_get_composite_state(uint32_t states_count, uint32_t* states){
	return 0;
}

void automaton_automata_add_ordered_union_of_signals(automaton_automata_context* ctx, uint32_t* signals_union, uint32_t* signals_union_count, automaton_transition* left_transition
		, automaton_transition* right_transition, automaton_transition* target_transition){
	signal_t current_signal, current_other_signal;
	uint32_t n, o;
	*signals_union_count = n = o = 0;
	while(n < left_transition->signals_count && o < right_transition->signals_count){
		current_signal		= n < FIXED_SIGNALS_COUNT ? left_transition->signals[n] : left_transition->other_signals[n-FIXED_SIGNALS_COUNT];
		current_other_signal= o < FIXED_SIGNALS_COUNT ? right_transition->signals[o] : right_transition->other_signals[o-FIXED_SIGNALS_COUNT];
		if(current_signal == current_other_signal){
			signals_union[*signals_union_count]	= current_signal;
			*signals_union_count = *signals_union_count + 1;
			n++; o++;
		}else if(current_signal < current_other_signal){
			signals_union[*signals_union_count]	= current_signal;
			*signals_union_count = *signals_union_count + 1;
			n++;
		}else{
			signals_union[*signals_union_count]	= current_other_signal;
			*signals_union_count = *signals_union_count + 1;
			o++;
		}
	}
	while(n < (left_transition->signals_count)){
		signals_union[*signals_union_count]	= n < FIXED_SIGNALS_COUNT ? left_transition->signals[n] : left_transition->other_signals[n-FIXED_SIGNALS_COUNT];
		*signals_union_count = *signals_union_count + 1;
		n++;
	}
	while(o < (right_transition->signals_count)){
		signals_union[*signals_union_count]	= o < FIXED_SIGNALS_COUNT ? right_transition->signals[o] : right_transition->other_signals[o-FIXED_SIGNALS_COUNT];
		*signals_union_count = *signals_union_count + 1;
		o++;
	}
	for(n = 0; n < *signals_union_count; n++){
		automaton_transition_add_signal_event(target_transition, ctx, &(ctx->global_alphabet->list[signals_union[n]]));
	}
}

/**
 * Gets the union of alphabets in the composition and a map of labels in the composition to global indexes
 * @param ctx automata composition context
 * @param automata an array of automata to be composed
 * @param automata_count the number of automata to be composed
 * @param alphabet_count placeholder for the size of the composition local alphabet
 * @param compose_to_global an array of uint that maps composition local entries to the global alphabet
 * @return an array of uint representing the composition alphabet
 */
uint32_t* automaton_automata_get_union_alphabet(automaton_automata_context **ctx, automaton_automaton** automata, uint32_t automata_count
	, uint32_t *alphabet_count){
	uint32_t alphabet_size	= 0;
	uint32_t i, j, k, l;
	for(i = 0; i < automata_count; i++){
		if(automata[i]->initial_states_count != 1)
			return NULL;
		if(*ctx == NULL){
			*ctx = automata[i]->context;
		}else if(*ctx != automata[i]->context){
			return NULL;
		}
		alphabet_size += automata[i]->local_alphabet_count;
	}
	uint32_t *alphabet	= malloc(sizeof(uint32_t) * alphabet_size);
	// alphabet <- Union of alphabets
	int32_t new_index	= -1;
	uint32_t current_entry;
	bool overlaps 		= false;
	for(i = 0; i < automata_count; i++){
		for(j = 0; j < automata[i]->local_alphabet_count; j++){
			//compute signal dependency
			overlaps = false;
			for(k = 0; k < automata_count; k++){
				if(k != i){
					for(l = 0; l < automata[k]->local_alphabet_count; l++){
						if(automata[i]->local_alphabet[j] == automata[k]->local_alphabet[l]){
							overlaps = true; break;
						}
					}
				}
				if(overlaps) break;
			}
			//get local signal global index
			new_index	= -1;
			bool found	= false;
			current_entry	= automata[i]->local_alphabet[j];
			for(k = 0; k < *alphabet_count; k++){
				if(alphabet[k] == current_entry){
					found	= true;
					break;
				}
				if(alphabet[k] > current_entry){
					new_index	= k;
					break;
				}
			}
			//keep global alphabet ordered
			if(new_index > -1 && !found){
				for(k = (*alphabet_count); k > (uint32_t)new_index; k--){
					alphabet[k]	= alphabet[k - 1];
				}
			}else{
				new_index	= *alphabet_count;
			}
			if(!found){
				alphabet[new_index]	= current_entry;
				(*alphabet_count)++;
			}
			//test for alphabet ordering
		}
	}
	return alphabet;
}

void automaton_automata_add_pending_transitions(automaton_automaton **automata, uint32_t automata_count, uint32_t from_state, uint32_t *partial_states, bool *partial_set_states
	, uint32_t *current_state, automaton_transition** pending, uint32_t *pending_count, uint32_t max_degree_sum){
	uint32_t i, j, k;
	uint32_t current_out_degree			= 0;
	for(i = 0; i < automata_count; i++){
		current_out_degree		= automata[i]->out_degree[current_state[i]];
		for(j = 0; j < current_out_degree; j++){
			//add new transition to p_sigs and partial_state
			automaton_transition* starting_transition					= automaton_transition_clone(&(automata[i]->transitions[current_state[i]][j]));
			starting_transition->state_from								= from_state;
			for(k = 0; k < automata_count; k++){
				if(k == i){
					partial_states[*pending_count * automata_count + k]	= starting_transition->state_to;
					partial_set_states[*pending_count * automata_count + k]	= true;
				}else{
					partial_states[*pending_count * automata_count + k]	= current_state[k];
					partial_set_states[*pending_count * automata_count + k]	= false;
				}
			}
			pending[(*pending_count)++]			= starting_transition;
			if(*pending_count >= max_degree_sum){
				printf("[FATAL ERROR] WRONG BOUNDARY AT PENDING COUNT\n");
				exit(-1);
			}
		}
	}
}
/**
 * Evaluates whether and how two transitions coming from two automata to be composed overlap according to their elements and alphabets
 * @param pending_label the first label (set of boolean variables indicating presence of alphabet's elements) related to a pending transition in the composition algorithm
 * @param current_label the second label (set of boolean variables indicating presence of alphabet's elements) related to transition under evaluation in the composition algorithm
 * @param independencies a list of independent elements, those that appear in one automaton but not the other
 * @param alphabet_count the size of the global alphabet
 * @param local_alphabet the local alphabet as a set of ints (indexes in the global alphabet)
 * @param local_alphabet_count the size of the local alphabet
 * @param labels_overlap a boolean placeholder that determines whether the two labels overlap or not (share elements of the alphabet)
 * @param pending_overlaps_current a boolean placeholder that determines whether the pending transition shares elements with the current transition's alphabet
 * @param current_overlaps_pending a boolean placeholder that determines whether the current transition shares elements with the pending transition's alphabet
 * @param check_independency determines whether the mismatch should be checked when labels overlap (only applied when at the last automaton)
 * @return true if there is a mismatch between labels and the merged transition should not be added
 */
bool automaton_automata_transition_mismatch(uint32_t *pending_label, uint32_t *current_label, uint32_t *independencies, uint32_t alphabet_count
		, uint32_t *local_alphabet, uint32_t local_alphabet_count
		, bool *labels_overlap, bool *pending_overlaps_current, bool *current_overlaps_pending, bool *empty_intersection
		, bool check_independency){
	uint32_t i;
	*pending_overlaps_current = false;
	*current_overlaps_pending = false;
	*labels_overlap = false;
	*empty_intersection = true;
	for(i = 0; i < alphabet_count; i++){
		if(pending_label[i] && ((pending_label[i] > 0) == (current_label[i] > 0))){
			*labels_overlap = true;
			*pending_overlaps_current = true;
			*current_overlaps_pending = true;
			break;
		}
	}
	if(!*labels_overlap){
		for(i = 0; i < local_alphabet_count; i++){
			if(pending_label[local_alphabet[i]]){
				*pending_overlaps_current = true;
				break;
			}
		}
		for(i = 0; i < alphabet_count; i++){
			if(current_label[i] && independencies[i] > 1){
				*current_overlaps_pending = true;
				break;
			}
		}
	}

	for(i = 0; i < alphabet_count; i++){
			if(independencies[i] > 1){
				if(!*labels_overlap && ((pending_label[i] > 0) != (current_label[i] > 0))){
					return true;
				}else if(check_independency && ((pending_label[i] + current_label[i]) != independencies[i])){
					return true;
				}else if (pending_label[i]){
					*empty_intersection = false;
				}
			}
	}
	return false;
}



void automaton_automata_merge_transition_alphabet_into_bool(bool *tmp_alphabet, automaton_transition *t){
	uint32_t i;
	for(i = 0; i < t->signals_count; i++){
		uint32_t sig = GET_TRANSITION_SIGNAL(t, i);
		tmp_alphabet[sig] = true;
	}
}
void automaton_automata_transition_alphabet_to_bool(bool *tmp_alphabet, automaton_transition *t, uint32_t alphabet_count){
	uint32_t i;

	for(i = 0; i < alphabet_count; i++)tmp_alphabet[i] = false;
	automaton_automata_merge_transition_alphabet_into_bool(tmp_alphabet, t);
}
void automaton_automata_bool_to_transition_alphabet(bool *tmp_alphabet, automaton_transition *t, uint32_t alphabet_count){
	uint32_t i, signals_count = 0, tail_count = 0;
	signal_t *other_signals;
	for(i = 0; i < alphabet_count; i++)if(tmp_alphabet[i])signals_count++;
	if(signals_count > FIXED_SIGNALS_COUNT && signals_count > t->signals_size){
		tail_count	= signals_count - FIXED_SIGNALS_COUNT;
		other_signals = malloc(sizeof(signal_t) * tail_count);
		if(t->other_signals != NULL){
			free(t->other_signals);
			t->other_signals	= NULL;
		}
		t->other_signals = other_signals;
		t->signals_size		= signals_count;
	}

	uint32_t signal_index = 0;
	for(i = 0; i < alphabet_count; i++){
		if(tmp_alphabet[i]){
			if(signal_index < FIXED_SIGNALS_COUNT){
				t->signals[signal_index] = i;
			}else{
				t->other_signals[signal_index-FIXED_SIGNALS_COUNT] = i;
			}
			signal_index++;
		}
	}
	t->signals_count	= signals_count;

}

/**
 * Increments the indexes array as a counter, if the max value is reached the returned value is true
 * @param idxs an array of the current indexes of the components' transitions to be evaluated
 * @param idxs_sizes the outgoing degree for the current component's state plus one for the case where the transition
 * 	is not applied
 * @param idxs_skip the list of transitions to be skipped when incrementing (if one transition shares alphabet with an
 * 	automaton whose state has no transition bringing that element skip it altogether)
 * @param automata_count the number of automata being composed
 * @return true if the max value for the indexes is reached, false otherwise
 */
bool automaton_automata_compose_increment_idxs(uint32_t *idxs, uint32_t *idxs_sizes, bool** idxs_skip, uint32_t automata_count){
	int32_t i = automata_count;
	bool set_anew = false;
	do{
		i--;
#if DEBUG_COMPOSITION
		printf("\t[I]dx[%d] ", i);
#endif

		set_anew	= false;
		do{
			if(idxs[i] < (idxs_sizes[i] - 1)){//increment current digit if possible
					idxs[i]++;
#if DEBUG_COMPOSITION
					printf(">%d",idxs[i]);
#endif
					if(!idxs_skip[i][idxs[i]-1])//if transition will not be skipped exit
						return false;
			}else{
				idxs[i] = 0;
				set_anew	= true;
#if DEBUG_COMPOSITION
					printf("|0");
#endif
			}
		}while(!set_anew && idxs[i] > 0 && idxs_skip[i][idxs[i]-1]);//keep incrmenting if current transition should be skipped
#if DEBUG_COMPOSITION
		printf("\n");
#endif
	}while(i >0);
	return true;
}

/**
 * Checks if the given indexes array has reached maximum value
 * @param idxs the array of indexes values
 * @param automata_count the number of automata to be composed
 * @return true if indexes have reach maximum value, false otherwise
 */
bool automaton_automata_idxs_is_max(uint32_t *idxs, uint32_t automata_count){
	int32_t i;
	for(i = 0; i < automata_count; i++){
		if(idxs[i] != 0)return false;
	}
	return true;
}

/**
 * Evaluates overlapping of the accumulated label with a give transition
 * @param accum_label a boolean array describing the members of the accumulated label
 * @param current_label a boolean array describing the current transition label
 * @param alphabet_overlap a boolean array describing which elements should be considered in the check
 * @return 0 if overlapping is total, 1 if overlapping is partial, -1 if overlapping is empty
 */
int32_t automaton_automata_check_overlap(bool *accum_label, bool *current_label, bool *alphabet_overlap
		, uint32_t alphabet_count){
	uint32_t i;
	bool at_least_one_overlaps	= false;
	bool at_least_one_does_not	= false;
	bool no_overlapping			= true;
	for(i = 0; i < alphabet_count; i++){
		if(alphabet_overlap[i]){
			if(accum_label[i] || current_label[i])no_overlapping = false;
			if(accum_label[i] == current_label[i] && accum_label[i]){
				at_least_one_overlaps = true;
			}
			if(accum_label[i] != current_label[i]){
				at_least_one_does_not = true;
			}
			if(!no_overlapping && at_least_one_does_not)return 1;
		}
	}
	if(no_overlapping)return -1;
	if(at_least_one_does_not)return 1;
	if(at_least_one_overlaps)return 0;
	return -1;
}
/*
void automaton_automata_merge_transition_alphabet_into_bool
(uint32_t *tmp_alphabet, automaton_transition *t, uint32_t alphabet_count)
void automaton_automata_transition_alphabet_to_bool
(uint32_t *tmp_alphabet, automaton_transition *t, uint32_t alphabet_count)
void automaton_automata_bool_to_transition_alphabet
(uint32_t *tmp_alphabet, automaton_transition *t, uint32_t alphabet_count)
bool automaton_automata_compose_increment_idxs(uint32_t *idxs, uint32_t *idxs_sizes, uint32_t automata_count)

*/

/**
 * Composes the automata according to their synchronization type.
 * @param automata the array of pointers to automata to be composed
 * @param synch_type the array of synchronization types to be applied sequentially and pairwise for the provided automata
 * @param automata_count the number of automata to be composed
 * @param is_game true if the resulting automaton will hold valuations on its states, false otherwise
 * @param composition_name the name of the composition as specified in the syntax
 * @return the CLTS resulting from applying the composition type according to synch_type definitions over automata
 */
automaton_automaton* automaton_automata_compose(automaton_automaton** automata, automaton_synchronization_type* synch_type, uint32_t automata_count
		, bool is_game, char* composition_name){
	clock_t begin = clock();
	uint32_t transitions_added_count	= 0;
	uint32_t i, j, k, l, m, n, o, p;
	uint32_t alphabet_count, fluents_count;	uint32_t* alphabet; uint32_t **complementary_alphabet; uint32_t *complementary_alphabet_count;
	alphabet_count	= 0;
	// get union of alphabets check ctx and compute alphabet size
	automaton_automata_context* ctx	= NULL;

	alphabet = automaton_automata_get_union_alphabet(&ctx, automata, automata_count, &alphabet_count);
	// create automaton
	automaton_automaton* composition;
	char buff[255];
	snprintf(buff, sizeof(buff), "%s_Game", composition_name);
	if(is_game){
		composition = automaton_automaton_create(buff, ctx, alphabet_count, alphabet, true, false);
	}else{
		composition = automaton_automaton_create(composition_name, ctx, alphabet_count, alphabet, false, false);
	}

	alphabet_count = ctx->global_alphabet->count;

	// create boolean alphabets sigma_i,accumulated alphabet sigma_1..i-1 and the overlapping between the two
	bool **boolean_alphabet = calloc(automata_count, sizeof(bool*));
	bool **accumulated_boolean_alphabet = calloc(automata_count, sizeof(bool*));
	bool **accumulated_alphabet_overlap = calloc(automata_count, sizeof(bool*));
	bool *current_label	= calloc(alphabet_count, sizeof(bool));
	for(i = 0; i < automata_count; i++){
		boolean_alphabet[i]	= calloc(alphabet_count, sizeof(bool));
		accumulated_boolean_alphabet[i] = calloc(alphabet_count, sizeof(bool));
		accumulated_alphabet_overlap[i] = calloc(alphabet_count, sizeof(bool));
		for(j = 0; j < automata[i]->local_alphabet_count; j++){
			boolean_alphabet[i][automata[i]->local_alphabet[j]]	= true;
			accumulated_boolean_alphabet[i][automata[i]->local_alphabet[j]]	= true;
		}
		if(i > 0){
			for(j = 0; j < alphabet_count; j++){
				if(accumulated_boolean_alphabet[i -1][j])accumulated_boolean_alphabet[i][j]	= true;
				accumulated_alphabet_overlap[i][j] = boolean_alphabet[i][j] && accumulated_boolean_alphabet[i - 1][j];
			}
		}
	}


#if DEBUG_COMPOSITION
	printf("[K] Composing %s, |ctx.alpha.|:%d alphabet_count: %d\n", composition->name, ctx->global_alphabet->count
			, alphabet_count);
	for(i = 0; i < automata_count; i++){
		printf("[A] Alphabets for i:%d, |local.alpha.|:%d\n", i, automata[i]->local_alphabet_count);
		printf("\t[B] Boolean alphabet:");
		for(j = 0; j < alphabet_count; j++){
			if(boolean_alphabet[i][j])printf("%s ", ctx->global_alphabet->list[j].name);
		}
		printf("\n\t[C] ACcumulated boolean alphabet:");
		for(j = 0; j < alphabet_count; j++){
			if(accumulated_boolean_alphabet[i][j])printf("%s ", ctx->global_alphabet->list[j].name);
		}
		printf("\n\t[O] Accumulated boolean alphabet Overlap 1..i:");
		for(j = 0; j < alphabet_count; j++){
			if(accumulated_alphabet_overlap[i][j])printf("%s ", ctx->global_alphabet->list[j].name);
		}
		printf("\n");
	}
#endif

	// create key tree,frontier bucket list, get current state transitions list harness

	automaton_composite_tree* tree	= automaton_composite_tree_create(is_game?
			automata_count + ctx->global_fluents_count :automata_count);
	automaton_bucket_list* bucket_list	= automaton_bucket_list_create(BUCKET_SIZE);
	uint32_t max_degree_sum		= 0;	uint32_t max_signals_count	= 0;
	uint32_t max_out_degree		= 0;
	// compute max_signals_count
	for(i = 0; i < automata_count; i++){
		max_degree_sum	+= (automata[i]->max_out_degree) * (automata[i]->max_concurrent_degree);
		if(max_out_degree < automata[i]->max_out_degree)
			max_out_degree	= automata[i]->max_out_degree;
		if(automata[i]->local_alphabet_count > max_signals_count)
			max_signals_count	= automata[i]->local_alphabet_count;
	}
	max_degree_sum							*= max_degree_sum; //check this boundary
	/** compose transition relation **/
	uint32_t frontier_size			= LIST_INITIAL_SIZE;//TODO: check this number is fixed
	uint32_t frontier_count			= 1;
	uint32_t key_size			= is_game ? automata_count + ctx->global_fluents_count : automata_count;
	uint32_t* frontier			= malloc(sizeof(uint32_t) * key_size * frontier_size);
	uint32_t* composite_frontier		= malloc(sizeof(uint32_t) * frontier_size);
	uint32_t* current_state			= malloc(sizeof(uint32_t) * key_size);
	uint32_t* current_to_state			= malloc(sizeof(uint32_t) * key_size);
	uint32_t from_state;	uint32_t to_state;	bool overlaps;	bool shared_equals;
	//uint32_t* signals_union			= malloc(sizeof(uint32_t) * max_degree_sum);
	//uint32_t signals_union_count	= 0;


	//indexes structures
	uint32_t* idxs	= calloc(automata_count, sizeof(uint32_t));
	uint32_t* idxs_size	= calloc(automata_count, sizeof(uint32_t));
	bool** idxs_skip	= calloc(automata_count, sizeof(bool*));
	bool** automata_accum	= calloc(automata_count, sizeof(bool*));
	for(i = 0; i < automata_count; i++){
		idxs_skip[i] 		= calloc(automata[i]->max_out_degree, sizeof(bool));
		automata_accum[i]	= calloc(alphabet_count, sizeof(bool));
	}
	bool* label_accum	= calloc(alphabet_count, sizeof(bool));


	int32_t last_char	= -1;	signal_t current_signal, current_other_signal;
	long int found_hits	= 0;	long int found_misses	= 0;	//set initial state
	for(i = 0; i < automata_count; i++){
		frontier[i]	= automata[i]->initial_states[0];
	}
	if(is_game){
		for(i = 0; i < ctx->global_fluents_count; i++)
			frontier[automata_count + i]	= ctx->global_fluents[i].initial_valuation? 1 : 0;
	}
	uint32_t composite_initial_state	= automaton_composite_tree_get_key(tree, frontier);
	composite_frontier[0]			= composite_initial_state;
	automaton_bucket_add_entry(bucket_list, composite_initial_state);
	automaton_automaton_add_initial_state(composition, composite_initial_state);
	uint32_t fluent_count			= ctx->global_fluents_count;
	uint32_t liveness_valuations_count	= ctx->liveness_valuations_count;
	// set initial state valuation
	if(is_game){
		uint32_t fluent_index;
		for(i = 0; i < fluent_count; i++){
			fluent_index	= GET_STATE_FLUENT_INDEX(fluent_count, 0, i);
			if(ctx->global_fluents[i].initial_valuation)
				SET_FLUENT_BIT(composition->valuations, fluent_index);
			else
				CLEAR_FLUENT_BIT(composition->valuations, fluent_index);
		}
		for(i = 0; i < liveness_valuations_count; i++){
			fluent_index	= GET_STATE_FLUENT_INDEX(liveness_valuations_count, 0, i);
			//TODO:we are assuming fluents start as false
			CLEAR_FLUENT_BIT(composition->liveness_valuations, fluent_index);
		}
	}
#if DEBUG_COMPOSITION
	for(i = 0; i < automata_count; i++){
		printf("[C] Composition type for %d: %s\n", i
				, synch_type[i] == SYNCHRONOUS ? "SYNCH" :
						(synch_type[i] == CONCURRENT ? "CONCURRENT" : "ASYNCH"));
	}
#endif
	// POP from frontier
	while(frontier_count > 0){
		from_state	= composite_frontier[frontier_count - 1];
#if DEBUG_COMPOSITION
		printf("[p] pop frontier state %d:[", from_state);
#endif
		automaton_bucket_remove_entry(bucket_list, from_state);
		//check if frontier state was already decomposed
		if(composition->out_degree[from_state] > 0){
			frontier_count--;
#if DEBUG_COMPOSITION
			printf("X] already processed \n");
#endif
			continue;
		}
		//pop a state
		automaton_transition *current_transition	= NULL;
		for(i = 0; i < automata_count; i++){
			//initialize current state and indexes structs.
			current_state[i]= frontier[(frontier_count - 1) * key_size + i];
			idxs_size[i]	= automata[i]->out_degree[current_state[i]] + 1;
			idxs[i]			= 0;
			//initialize label skips and automata label accum struct
			for(j = 0; j < alphabet_count; j++)	automata_accum[i][j]	= false;
			for(j = 0; j < automata[i]->out_degree[current_state[i]]; j++)idxs_skip[i][j]	= false;
			for(j = 0; j < automata[i]->out_degree[current_state[i]]; j++){
				current_transition = &(automata[i]->transitions[current_state[i]][j]);
				for(k = 0; k < current_transition->signals_count; k++){
					int32_t sig_idx = GET_TRANSITION_SIGNAL(current_transition,k);
					automata_accum[i][sig_idx] = true;
				}
			}
#if DEBUG_COMPOSITION
			printf("%s%d", i > 0 ? "," : "", current_state[i]);
#endif
		}
		//if automata is game take fluent valuation into consideration
		if(is_game){
			for(i = automata_count; i < key_size; i++){
				current_state[i]= frontier[(frontier_count - 1) * key_size + i];
			}
		}

		frontier_count--;
#if DEBUG_COMPOSITION
		printf("]\n");
#endif

#if DEBUG_COMPOSITION
		for(i = 0; i < automata_count; i++){
			printf("\t[A] Accum alphabet[%d]:",i);
			for(j = 0; j < ctx->global_alphabet->count; j++){
				if(automata_accum[i][j])printf("%s ", ctx->global_alphabet->list[j].name);
			}
			printf("\n");
		}
#endif
		//set idxs_skip marks
		bool transition_skipped = false;
		for(i = 0; i < automata_count; i++){
			for(j = 0; j < automata[i]->out_degree[current_state[i]]; j++){
				current_transition = &(automata[i]->transitions[current_state[i]][j]);
				transition_skipped = false;
				for(k = 0; k < automata_count; k++ && !transition_skipped){
					if(k == i)continue;
					//if l_i_j intersects sigma_k != l_i_j intersects automata_accum_k skip l_i_j
					for(l = 0; l < current_transition->signals_count; l++ && ! transition_skipped){
						int32_t sig_idx	= GET_TRANSITION_SIGNAL(current_transition,l);
						if(boolean_alphabet[k][sig_idx] != automata_accum[k][sig_idx]){
							idxs_skip[i][j]		= true;
							transition_skipped	= true;
						}
					}
				}

			}
		}

		//get first non null combination
		automaton_automata_compose_increment_idxs(idxs, idxs_size, idxs_skip, automata_count);
#if DEBUG_COMPOSITION
		printf("\t[i] Initial Idxs: [");
		for(j = 0; j < automata_count; j++){
			printf("%d%s", idxs[j], j < (automata_count -1)? "," : "");
		}
		printf("]\n");
#endif
		for(i = 0; i < alphabet_count; i++)label_accum[i]	= false;


		bool viable; bool not_considered; bool local_overlap;
		//explore possible combinations of transitions

		while(!automaton_automata_idxs_is_max(idxs, automata_count)){
#if DEBUG_COMPOSITION
			printf("\t[I] Idxs: [");
			for(j = 0; j < automata_count; j++){
				printf("%d%s", idxs[j], j < (automata_count -1)? "," : "");
			}
			printf("]\n");
			char buff[20];
			printf("\t[T] Components transitions:\n");
			for(j = 0; j < automata_count; j++){
				snprintf(buff, sizeof(buff) - strlen(buff) - 1, "\t\t i:[%d]", j);
				if(idxs[j] > 0)
					automaton_transition_print(&(automata[j]->transitions[current_state[j]][idxs[j] - 1]), ctx, buff,"\n");
				else
					printf("%s [not considered]\n", buff);
			}


#endif
			//initialize label accumulator
			bool accum_set = false; bool is_input = automata[0]->transitions[current_state[0]]->is_input;
			bool accum_input	= false;
			if(idxs[0] > 0){
				is_input = is_input || automata[0]->transitions[current_state[0]][idxs[0] - 1].is_input;
				accum_input = accum_input || automata[0]->transitions[current_state[0]][idxs[0] - 1].is_input;
				automaton_automata_transition_alphabet_to_bool(label_accum
						, &(automata[0]->transitions[current_state[0]][idxs[0] - 1]), alphabet_count);
				accum_set = true;
			}else{
				for(j = 0; j < alphabet_count; j++)label_accum[j] = false;
			}
			viable = true;
			//check if current combination is viable
			for(j = 1; j < automata_count; j++){
				if(idxs[j] == 0){//transition not being considered
					is_input = is_input || automata[j]->transitions[current_state[j]]->is_input;
					not_considered = true;
					for(k = 0; k < alphabet_count; k++)
						current_label[k]	= false;
				}else{
					is_input = is_input || automata[j]->transitions[current_state[j]][idxs[j] - 1].is_input;
					not_considered = false;
					automaton_automata_transition_alphabet_to_bool(current_label
							, &(automata[j]->transitions[current_state[j]][idxs[j] - 1]), alphabet_count);
				}
				//check blocking
				int32_t overlapping = automaton_automata_check_overlap(label_accum, current_label
						, accumulated_alphabet_overlap[j], alphabet_count);
#if DEBUG_COMPOSITION
				printf("\t\t[LCA] Accum. label:[");
				for(k = 0; k < alphabet_count; k++){
					if(label_accum[k])printf("%s ", ctx->global_alphabet->list[k].name);
				}
				printf("]");
				printf("Current label:[");
				for(k = 0; k < alphabet_count; k++){
					if(current_label[k])printf("%s ", ctx->global_alphabet->list[k].name);
				}
				printf("]");
				printf("Accum. alphabet overlap:[");
				for(k = 0; k < alphabet_count; k++){
					if(accumulated_alphabet_overlap[j][k])printf("%s ", ctx->global_alphabet->list[k].name);
				}
				printf("]");

				fflush(stdout);
#endif
				if(overlapping == 1){//partial synch not allowed
					viable = false;
#if DEBUG_COMPOSITION
					printf("[BLOCKS]\n");
#endif
					break;
				}
				if(synch_type[j] == ASYNCHRONOUS && overlapping == -1 && idxs[j] > 0 && accum_set){//no overlapping not allowed in asynch
					viable = false;
#if DEBUG_COMPOSITION
					printf("[NOT ASYNCH]\n");
#endif
					break;
				}
				if(synch_type[j] == SYNCHRONOUS && (not_considered || !accum_set)){
					viable = false;
#if DEBUG_COMPOSITION
					printf("[NOT SYNCH]\n");
#endif
					break;
				}
				if(!viable){
					break;
				}
#if DEBUG_COMPOSITION
				printf(">");
#endif
				if(idxs[j] > 0){
					automaton_automata_merge_transition_alphabet_into_bool(label_accum
							, &(automata[j]->transitions[current_state[j]][idxs[j] - 1]));
					accum_set = true;
				}
			}

			//add viable transitions
			if(viable){
#if DEBUG_COMPOSITION
				printf("[OK]\n");
#endif
				for(k = 0; k < automata_count; k++){
					if(idxs[k] == 0)current_to_state[k]= current_state[k];
					else current_to_state[k]	= automata[k]->transitions[current_state[k]][idxs[k] - 1].state_to;
				}
				bool satisfies_one_condition = false;
				bool satisfies_fluent_condition = true;
				//TODO: update fluent valuation on current_to_state
				if(is_game){
					//check starting conditions
					for(k = 0; k < ctx->global_fluents_count; k++){
						current_to_state[automata_count + k]	= current_state[automata_count + k];
						if(current_state[automata_count + k]){//check ending for those that are up
							satisfies_one_condition	= false;
							for(l = 0; l < ctx->global_fluents[k].ending_signals_count; l++){
								satisfies_fluent_condition = true;
								for(m = 0; m < ctx->global_fluents[k].ending_signals_element_count[l]; m++){
									if(!label_accum[ctx->global_fluents[k].ending_signals[l][m]]){
										satisfies_fluent_condition = false;
										break;
									}
								}
								if(satisfies_fluent_condition){
									satisfies_one_condition	= true;
									break;
								}
							}
							if(satisfies_one_condition)
								current_to_state[automata_count + k]	= false;
						}else{//check starting for those that are down
							satisfies_one_condition	= false;
							for(l = 0; l < ctx->global_fluents[k].starting_signals_count; l++){
								satisfies_fluent_condition	= true;
								for(m = 0; m < ctx->global_fluents[k].starting_signals_element_count[l]; m++){
									if(!label_accum[ctx->global_fluents[k].starting_signals[l][m]]){
										satisfies_fluent_condition = false;
										break;
									}
								}
								if(satisfies_fluent_condition){
									satisfies_one_condition	= true;
									break;
								}
							}
							if(satisfies_one_condition)
								current_to_state[automata_count + k]	= true;
						}
					}
				}
				uint32_t composite_to			= automaton_composite_tree_get_key(tree, current_to_state);
				automaton_transition *current_transition	= automaton_transition_create(from_state, composite_to);
				current_transition->is_input	= accum_input;
				automaton_automata_bool_to_transition_alphabet(label_accum, current_transition, alphabet_count);
				//current_transition->is_input	= is_input;
				automaton_automaton_add_transition(composition, current_transition);
#if DEBUG_COMPOSITION
				printf("\t[+] Adding trans: %d {", current_transition->state_from);
				for(k = 0; k < current_transition->signals_count; k++){
					printf("%s%s", k > 0 ? "," : ""
							, automata[0]->context->global_alphabet->list[GET_TRANSITION_SIGNAL(current_transition, k)].name);
				}
				printf("}->[");
				for(l = 0; l < automata_count; l++){
					printf("%s%d", l > 0 ? "," : "", current_to_state[l]);
				}
				printf("]:%d\n", current_transition->state_to);
#endif
				// set state valuation
				if(is_game){
					uint32_t fluent_index;
					uint32_t fluent_automata_index;
					int32_t state_position	= -1;
					bool state_found;
					for(i = 0; i < fluent_count; i++){
						fluent_index	= GET_STATE_FLUENT_INDEX(fluent_count, composite_to, i);
						//set new valuation
						fluent_automata_index	= automata_count + i;
						if(current_to_state[fluent_automata_index] == 1){
							//Check if it should be added to the inverted valuation list
							state_found		= automaton_bucket_has_entry(composition->inverted_valuations[i], composite_to);
							if(!state_found){
								automaton_bucket_add_entry(composition->inverted_valuations[i], composite_to);
							}
							SET_FLUENT_BIT(composition->valuations, fluent_index);
						}else{
							CLEAR_FLUENT_BIT(composition->valuations, fluent_index);
						}
					}
					//set liveness valuations
					for(i = 0; i < liveness_valuations_count; i++){
						bool current_valuation = true;
						for(j = 0; j < automata_count; j++){
							if(automata[j]->built_from_ltl){
								fluent_index	= GET_STATE_FLUENT_INDEX(liveness_valuations_count, current_to_state[j], i);
								current_valuation = current_valuation && TEST_FLUENT_BIT(automata[j]->liveness_valuations, fluent_index);
							}
							if(!current_valuation)break;
						}
						fluent_index	= GET_STATE_FLUENT_INDEX(liveness_valuations_count, composite_to, i);
						if(current_valuation){
							//Check if it should be added to the inverted valuation list
							state_found		= automaton_bucket_has_entry(composition->liveness_inverted_valuations[i], composite_to);
							//
							if(!state_found){
								automaton_bucket_add_entry(composition->liveness_inverted_valuations[i], composite_to);
							}
							SET_FLUENT_BIT(composition->liveness_valuations, fluent_index);
						}else{
							CLEAR_FLUENT_BIT(composition->liveness_valuations, fluent_index);
						}
					}
				}
				transitions_added_count++;
#if VERBOSE
				if(transitions_added_count % 10000 == 0){
					printf("\t[!] Added %d transitions\n", transitions_added_count);
					fflush(stdout);
				}
				if((found_hits + found_misses) % 5000 == 0){
					printf("\t[!] %d hits, %d misses\n", found_hits, found_misses);
					fflush(stdout);
				}
#endif
				automaton_transition_destroy(current_transition, true);
				// expand frontier
				bool found = automaton_bucket_has_entry(bucket_list, composite_to);
				if(found){found_hits++;}else{found_misses++;}
				if(!found){
					composite_frontier[frontier_count]	= composite_to;
					automaton_bucket_add_entry(bucket_list, composite_to);
					for(n = 0; n < key_size; n++){
						frontier[frontier_count * key_size + n]	= current_to_state[n];
					}
					frontier_count++;
					if(frontier_count >= (frontier_size - 1)){
						uint32_t new_size	= frontier_size * LIST_INCREASE_FACTOR;
						uint32_t i, key_size;
						if(is_game){
							key_size	= automata_count + ctx->global_fluents_count;
						}else{
							key_size	= automata_count;
						}
						uint32_t* new_frontier				= malloc(sizeof(uint32_t) * new_size * key_size);
						uint32_t* new_composite_frontier	= malloc(sizeof(uint32_t) * new_size);
						for(i = 0; i < frontier_size * key_size; i++){
							new_frontier[i]					= frontier[i];
						}
						for(i = 0; i < frontier_size; i++){
							new_composite_frontier[i]			= composite_frontier[i];
						}
						free(frontier);
						free(composite_frontier);
						frontier_size						= new_size;
						frontier							= new_frontier;
						composite_frontier					= new_composite_frontier;
					}
				}
			}
			automaton_automata_compose_increment_idxs(idxs, idxs_size, idxs_skip, automata_count);
		}
	}
#if VERBOSE
	printf("TOTAL Composition has [%09d] states and [%09d] transitions run for [%08f] KEY ACCESS.: [Misses:%li,hits:%li]\n", tree->max_value, composition->transitions_composite_count, (double)(clock() - begin) / CLOCKS_PER_SEC, found_misses, found_hits);
#endif
	//CLEANUP
	for(i = 0; i < automata_count; i++){
		free(boolean_alphabet[i]);
		free(accumulated_alphabet_overlap[i]);
		free(accumulated_boolean_alphabet[i]);
		free(idxs_skip[i]);
		free(automata_accum[i]);
	}
	free(boolean_alphabet);free(accumulated_alphabet_overlap); free(accumulated_boolean_alphabet);
	free(current_label);free(frontier);free(composite_frontier);free(current_state);free(current_to_state);
	free(idxs);free(idxs_size);free(label_accum);free(idxs_skip);free(automata_accum);
	free(alphabet);
	automaton_composite_tree_destroy(tree);
	automaton_bucket_destroy(bucket_list);

	return composition;
}

/**
 * Composes the automata according to their synchronization type.
 * @param automata the array of pointers to automata to be composed
 * @param synch_type the array of synchronization types to be applied sequentially and pairwise for the provided automata
 * @param automata_count the number of automata to be composed
 * @param is_game true if the resulting automaton will hold valuations on its states, false otherwise
 * @param composition_name the name of the composition as specified in the syntax
 * @return the CLTS resulting from applying the composition type according to synch_type definitions over automata
 */
/*
automaton_automaton* automaton_automata_compose_2(automaton_automaton** automata, automaton_synchronization_type* synch_type, uint32_t automata_count
		, bool is_game, char* composition_name){
	clock_t begin = clock();
	uint32_t transitions_added_count	= 0;
	uint32_t i, j, k, l, m, n, o, p;
	uint32_t alphabet_count, fluents_count;	uint32_t* alphabet; uint32_t **complementary_alphabet; uint32_t *complementary_alphabet_count;
	alphabet_count	= 0;
	// get union of alphabets check ctx and compute alphabet size
	automaton_automata_context* ctx	= NULL;

	alphabet = automaton_automata_get_union_alphabet(&ctx, automata, automata_count, &alphabet_count);

	// create automaton
	automaton_automaton* composition;
	if(is_game){
		composition = automaton_automaton_create("Game", ctx, alphabet_count, alphabet, true, false);
	}else{
		composition = automaton_automaton_create(composition_name, ctx, alphabet_count, alphabet, false, false);
	}

	alphabet_count = ctx->global_alphabet->count;
	uint32_t 	*alphabet_dependency	= calloc(alphabet_count, sizeof(uint32_t));
	uint32_t 	*alphabet_independency	= calloc(alphabet_count, sizeof(uint32_t));
	//get set of independent signals
	for(j = 0; j < automata_count; j++){
		for(k = 0; k < automata[j]->local_alphabet_count; k++){
			alphabet_independency[automata[j]->local_alphabet[k]]++;
		}
	}

	// create key tree,frontier bucket list, get current state transitions list harness
	automaton_composite_tree* tree	= automaton_composite_tree_create(automata_count);
	automaton_bucket_list* bucket_list	= automaton_bucket_list_create(BUCKET_SIZE);
	uint32_t max_degree_sum		= 0;	uint32_t max_signals_count	= 0;
	uint32_t max_out_degree		= 0;
	// compute max_signals_count
	for(i = 0; i < automata_count; i++){
		max_degree_sum	+= (automata[i]->max_out_degree) * (automata[i]->max_concurrent_degree);
		if(max_out_degree < automata[i]->max_out_degree)
			max_out_degree	= automata[i]->max_out_degree;
		if(automata[i]->local_alphabet_count > max_signals_count)
			max_signals_count	= automata[i]->local_alphabet_count;
	}
	max_degree_sum							*= max_degree_sum; //check this boundary
	bool* pending_added		= calloc(max_degree_sum, sizeof(automaton_transition*));

	bool* delta_i_set		= calloc(max_out_degree, sizeof(bool));
	//delta_union structs
	automaton_transition** delta_union		= malloc(sizeof(automaton_transition*) * max_degree_sum);
	uint32_t delta_union_count				= 0;
	uint32_t* delta_union_to_state			= calloc(automata_count * max_degree_sum, sizeof(int32_t));
	uint32_t* delta_union_alphabet				= calloc(alphabet_count * max_degree_sum, sizeof(uint32_t));
	//delta_union_p structs
	automaton_transition** delta_union_p		= malloc(sizeof(automaton_transition*) * max_degree_sum);
	uint32_t delta_union_p_count				= 0;
	uint32_t* delta_union_p_to_state			= calloc(automata_count * max_degree_sum, sizeof(int32_t));
	uint32_t* delta_union_p_alphabet				= calloc(alphabet_count * max_degree_sum, sizeof(uint32_t));
	//delta_union_tmp structs
	automaton_transition** delta_union_tmp;
	uint32_t delta_union_tmp_count;
	uint32_t* delta_union_tmp_to_state;
	bool* delta_union_tmp_alphabet;

	automaton_transition** processed	= malloc(sizeof(automaton_transition*) * max_degree_sum);
	uint32_t processed_count			= 0;

	uint32_t current_other_out_degree	= 0;
	automaton_transition* current_transition		= NULL;
	uint32_t* current_to_state			= malloc(sizeof(int32_t) * automata_count);


	uint32_t* pending_label				= calloc(alphabet_count, sizeof(uint32_t));
	uint32_t* current_label				= calloc(alphabet_count, sizeof(uint32_t));
	uint32_t* tmp_transition_alphabet	= malloc(sizeof(int32_t) * alphabet_count);
	uint32_t tmp_transition_count		= 0;
	// compose transition relation
	uint32_t frontier_size			= LIST_INITIAL_SIZE;//TODO: check this number is fixed
	uint32_t frontier_count			= 1;
	uint32_t* frontier			= malloc(sizeof(uint32_t) * automata_count * frontier_size);
	uint32_t* composite_frontier		= malloc(sizeof(uint32_t) * frontier_size);
	uint32_t* current_state			= malloc(sizeof(uint32_t) * automata_count);
	bool* current_set_state			= malloc(sizeof(bool) * automata_count);
	uint32_t from_state;	uint32_t to_state;	bool overlaps;	bool shared_equals;
	uint32_t* signals_union			= malloc(sizeof(uint32_t) * max_degree_sum);
	uint32_t signals_union_count	= 0;

	int32_t last_char	= -1;	signal_t current_signal, current_other_signal;
	long int found_hits	= 0;	long int found_misses	= 0;	//set initial state
	for(i = 0; i < automata_count; i++){
		frontier[i]	= automata[i]->initial_states[0];
	}
	uint32_t composite_initial_state	= automaton_composite_tree_get_key(tree, frontier);
	composite_frontier[0]			= composite_initial_state;
	automaton_bucket_add_entry(bucket_list, composite_initial_state);
	automaton_automaton_add_initial_state(composition, composite_initial_state);
	uint32_t fluent_count			= ctx->global_fluents_count;
	uint32_t liveness_valuations_count	= ctx->liveness_valuations_count;
	// set initial state valuation
	if(is_game){
		uint32_t fluent_index;
		for(i = 0; i < fluent_count; i++){
			fluent_index	= GET_STATE_FLUENT_INDEX(fluent_count, 0, i);
			if(ctx->global_fluents[i].initial_valuation)
				SET_FLUENT_BIT(composition->valuations, fluent_index);
			else
				CLEAR_FLUENT_BIT(composition->valuations, fluent_index);
		}
		for(i = 0; i < liveness_valuations_count; i++){
			fluent_index	= GET_STATE_FLUENT_INDEX(liveness_valuations_count, 0, i);
			//TODO:we are assuming fluents start as false
			CLEAR_FLUENT_BIT(composition->liveness_valuations, fluent_index);
		}
	}
#if DEBUG_COMPOSITION
	for(i = 0; i < automata_count; i++){
		printf("[C] Composition type for %d: %s\n", i
				, synch_type[i] == SYNCHRONOUS ? "SYNCH" :
						(synch_type[i] == CONCURRENT ? "CONCURRENT" : "ASYNCH"));
	}
#endif
	// POP from frontier
	while(frontier_count > 0){
		from_state	= composite_frontier[frontier_count - 1];
#if DEBUG_COMPOSITION
		printf("[p] pop frontier state %d:[", from_state);
#endif
		automaton_bucket_remove_entry(bucket_list, from_state);
		//check if frontier state was already decomposed
		if(composition->out_degree[from_state] > 0){
			frontier_count--;
#if DEBUG_COMPOSITION
			printf("X] already processed \n");
#endif
			continue;
		}
		//pop a state
		for(i = 0; i < automata_count; i++){
			current_state[i] 		= frontier[(frontier_count - 1) * automata_count + i];
#if DEBUG_COMPOSITION
		printf("%s%d", i > 0 ? "," : "", current_state[i]);
#endif
		}
#if DEBUG_COMPOSITION
		printf("]\n");
#endif
		frontier_count--;
		uint32_t current_out_degree			= automata[0]->out_degree[current_state[0]];
		//Delta_union = Delta_0
		for(j = 0; j < current_out_degree; j++){
			//add new transition to p_sigs and partial_state
			automaton_transition* starting_transition					= automaton_transition_clone(&(automata[0]->transitions[current_state[0]][j]));
			starting_transition->state_from								= from_state;
#if DEBUG_COMPOSITION
			printf("\t\t[t] Current trans (Delta_I = Delta_i): %d {", from_state);
			for(k = 0; k < starting_transition->signals_count; k++){
				printf("%s%s", k > 0 ? "," : ""
						, automata[0]->context->global_alphabet->list[GET_TRANSITION_SIGNAL(starting_transition, k)].name);
			}
			printf("}->[");
#endif
			for(k = 0; k < automata_count; k++){
				if(k != 0){
					current_to_state[k]	= current_state[k];
				}else{
#if DEBUG_COMPOSITION
					printf("*");
#endif
					current_to_state[k]	= starting_transition->state_to;
				}
#if DEBUG_COMPOSITION
				printf("%s%d", k > 0 ? "," : "", current_to_state[k]);
#endif
				delta_union_to_state[delta_union_count * automata_count + k]	= current_to_state[k];
			}
			starting_transition->state_to		= automaton_composite_tree_get_key(tree, current_to_state);
#if DEBUG_COMPOSITION
			printf("]:%d\n", starting_transition->state_to);
#endif
			automaton_automata_transition_alphabet_to_bool(&(delta_union_alphabet[delta_union_count * alphabet_count]), starting_transition, alphabet_count);
			delta_union[(delta_union_count)++]		= starting_transition;
			for(k = 0; k < automata[0]->local_alphabet_count; k++){
				alphabet_dependency[automata[0]->local_alphabet[k]] = true;
			}
			if(delta_union_count >= max_degree_sum){
				printf("[FATAL ERROR] WRONG BOUNDARY AT PENDING COUNT\n");
				exit(-1);
			}
		}

		for(i = 1; i < automata_count; i++){
#if DEBUG_COMPOSITION
			printf("\t[i] A_%d at %d\n", i, current_state[i]);
#endif
			automaton_synchronization_type current_type = i == (automata_count - 1)? synch_type[i] : synch_type[i+1];
			current_out_degree		= automata[i]->out_degree[current_state[i]];
			//pending_count == 0 is there to make sure that the initial Delta_i(s) is added if Delta_union is empty
			//partial composition
			for(k = 0; k < current_out_degree; k++){
				delta_i_set[k]		= false;
			}
			//if delta_union_count is 0 but label is independent it should be added
			bool pending_overlaps_current, current_overlaps_pending, labels_overlap, empty_intersection;
			if(delta_union_count == 0){
				for(k = 0; k < current_out_degree; k++){
					automaton_transition *independent_transition	= automaton_transition_clone(&(automata[i]->transitions[current_state[i]][k]));
					automaton_automata_transition_alphabet_to_bool(current_label, independent_transition
										, alphabet_count);
#if DEBUG_COMPOSITION
					printf("\t\t[?] Evaluating for indenpendcy trans.: %d {", from_state);
					for(m = 0; m < independent_transition->signals_count; m++){
						printf("%s%s", m > 0 ? "," : ""
								, automata[i]->context->global_alphabet->list[GET_TRANSITION_SIGNAL(independent_transition, m)].name);
					}
					printf("}->[?]");
					fflush(stdout);
#endif
					current_overlaps_pending	= false;
					for(l = 0; l < alphabet_count; l++){
						if(current_label[l] && alphabet_independency[l] > 1){
							current_overlaps_pending = true;
							break;
						}
					}
#if DEBUG_COMPOSITION
						printf("%s\n", current_overlaps_pending? "DEP." : "INDEP.");
#endif
					if(!current_overlaps_pending){
#if DEBUG_COMPOSITION
						printf("\t\t[E] Creating independent trans.: %d {", from_state);
						for(m = 0; m < independent_transition->signals_count; m++){
							printf("%s%s", m > 0 ? "," : ""
									, automata[i]->context->global_alphabet->list[GET_TRANSITION_SIGNAL(independent_transition, m)].name);
						}
						printf("}->[");
#endif
						for(l = 0; l < automata_count; l++){
							if(l != 0){
								current_to_state[l]	= current_state[l];
							}else{
#if DEBUG_COMPOSITION
								printf("*");
#endif
								current_to_state[l]	= independent_transition->state_to;
							}
#if DEBUG_COMPOSITION
							printf("%s%d", l > 0 ? "," : "", current_to_state[l]);
#endif
							delta_union_p_to_state[delta_union_p_count * automata_count + l]	= current_to_state[l];
						}
						independent_transition->state_to		= automaton_composite_tree_get_key(tree, current_to_state);
#if DEBUG_COMPOSITION
						printf("]:%d KEPT\n", independent_transition->state_to);
#endif
						independent_transition->state_from		= from_state;
						automaton_automata_transition_alphabet_to_bool(&(delta_union_p_alphabet[delta_union_p_count * alphabet_count]), independent_transition, alphabet_count);
						delta_union_p[(delta_union_p_count)++]			= independent_transition;
					}else{
						automaton_transition_destroy(independent_transition, true);
					}
				}
			}else{
				for(j = 0; j < delta_union_count; j++){
#if DEBUG_COMPOSITION
					printf("\t\t<j> pending[%d] \n", j);
#endif
					bool pending_added = false;
					for(k = 0; k < current_out_degree; k++){
#if DEBUG_COMPOSITION
						printf("\t\t<k> transitions[%d] \n", k);
#endif
						//add new transition to p_sigs and partial_state
						automaton_transition* starting_transition					= automaton_transition_clone(&(automata[i]->transitions[current_state[i]][k]));
#if DEBUG_COMPOSITION
								printf("\t\t[P] Popping trans.: %d {", starting_transition->state_from);
								for(m = 0; m < starting_transition->signals_count; m++){
									printf("%s%s", m > 0 ? "," : ""
											, automata[i]->context->global_alphabet->list[GET_TRANSITION_SIGNAL(starting_transition, m)].name);
								}
								printf("}->[%d]\n", starting_transition->state_to);
#endif
						for( l = 0; l < alphabet_count; l++){
							pending_label[l]				= delta_union_alphabet[j *  alphabet_count + l];
						}
						bool was_set = false;
						automaton_automata_transition_alphabet_to_bool(current_label, starting_transition, alphabet_count);

						//check if transition doesn't block automaton_automata_tmp_transition_blocks

						if(!automaton_automata_transition_mismatch(pending_label, current_label, alphabet_independency, alphabet_count, automata[i]->local_alphabet
								, automata[i]->local_alphabet_count
								, &labels_overlap, &pending_overlaps_current, &current_overlaps_pending, &empty_intersection
								, i == (automata_count -1))){
							if((current_type != ASYNCHRONOUS) || !empty_intersection){//merge transitions if needed
								automaton_automata_merge_transition_alphabet_into_bool(pending_label, starting_transition, alphabet_count);
								automaton_automata_bool_to_transition_alphabet(pending_label, starting_transition, alphabet_count);
#if DEBUG_COMPOSITION
								printf("\t\t[M]erge pending trans.: %d {", from_state);
								for(m = 0; m < starting_transition->signals_count; m++){
									printf("%s%s", m > 0 ? "," : ""
											, automata[i]->context->global_alphabet->list[GET_TRANSITION_SIGNAL(starting_transition, m)].name);
								}
								printf("}->[");
#endif
								for(l = 0; l < automata_count; l++){
									if(l != i){
										current_to_state[l]	= delta_union_to_state[j * automata_count + l];
									}else{
#if DEBUG_COMPOSITION
										printf("*");
#endif
										current_to_state[l]	= starting_transition->state_to;
									}
#if DEBUG_COMPOSITION
									printf("%d%s", current_to_state[l], l < automata_count -1 ? "," : "");
#endif
									delta_union_p_to_state[delta_union_p_count * automata_count + l]	= current_to_state[l];
								}
								starting_transition->state_from		= from_state;
								starting_transition->state_to		= automaton_composite_tree_get_key(tree, current_to_state);
#if DEBUG_COMPOSITION
								printf("]:%d\n", starting_transition->state_to);
								printf("\t\t[A] Current label ");
								for(m = 0; m < starting_transition->signals_count; m++){
									printf("%s%s", m > 0 ? "," : ""
										, automata[i]->context->global_alphabet->list[GET_TRANSITION_SIGNAL(starting_transition, m)].name);
								}
								printf("\n");
#endif
								for(m = 0; m < alphabet_count; m++){
									delta_union_p_alphabet[delta_union_p_count * alphabet_count + m]	= pending_label[m];
								}
								//automaton_automata_transition_alphabet_to_bool(&(delta_union_p_alphabet[delta_union_p_count * alphabet_count]), starting_transition, alphabet_count);
								delta_union_p[(delta_union_p_count)++]			= starting_transition;
								was_set = true;
							}
							if(current_type != SYNCHRONOUS && empty_intersection){//if can split and type not synch add to delta union both original transitions
								//Delta_i trans
								if(!delta_i_set[k]){
									delta_i_set[k]	= true;
									if(was_set)
										starting_transition					= automaton_transition_clone(&(automata[i]->transitions[current_state[i]][k]));
#if DEBUG_COMPOSITION
									printf("\t\t[t] Delta i non merged trans.: %d {", from_state);
									for(m = 0; m < starting_transition->signals_count; m++){
										printf("%s%s", m > 0 ? "," : ""
												, automata[i]->context->global_alphabet->list[GET_TRANSITION_SIGNAL(starting_transition, m)].name);
									}
									printf("}->[");
#endif
									for(l = 0; l < automata_count; l++){
										if(l != i){
											current_to_state[l]	= current_state[l];
										}else{
#if DEBUG_COMPOSITION
											printf("*");
#endif
											current_to_state[l]	= starting_transition->state_to;
										}
#if DEBUG_COMPOSITION
										printf("%d%s", current_to_state[l], l < automata_count -1 ? "," : "");
#endif
										delta_union_p_to_state[delta_union_p_count * automata_count + l]	= current_to_state[l];
									}
									starting_transition->state_from		= from_state;
									starting_transition->state_to		= automaton_composite_tree_get_key(tree, current_to_state);
#if DEBUG_COMPOSITION
									printf("]:%d\n", starting_transition->state_to);
									printf("\t\t[A] Current label ");
									for(m = 0; m < starting_transition->signals_count; m++){
										printf("%s%s", m > 0 ? "," : ""
											, automata[i]->context->global_alphabet->list[GET_TRANSITION_SIGNAL(starting_transition, m)].name);
									}
									printf("\n");
#endif
									automaton_automata_transition_alphabet_to_bool(&(delta_union_p_alphabet[delta_union_p_count * alphabet_count]), starting_transition, alphabet_count);
									delta_union_p[(delta_union_p_count)++]			= starting_transition;
									was_set = true;
								}
								//Delta_union trans
								if(!pending_added){
									pending_added 						= true;
									if(!was_set)
										automaton_transition_destroy(starting_transition, true);
									starting_transition					= automaton_transition_clone(delta_union[j]);
									//automaton_automata_bool_to_transition_alphabet(pending_label, starting_transition, alphabet_count);
									//automaton_automata_transition_alphabet_to_bool(&(delta_union_p_alphabet[delta_union_p_count * alphabet_count]), starting_transition, alphabet_count);
#if DEBUG_COMPOSITION
									printf("\t\t[t] Pending non merged trans.: %d {", from_state);
									for(m = 0; m < starting_transition->signals_count; m++){
										printf("%s%s", m > 0 ? "," : ""
												, automata[i]->context->global_alphabet->list[GET_TRANSITION_SIGNAL(starting_transition, m)].name);
									}
									printf("}->[");
#endif
									for(l = 0; l < automata_count; l++){
										current_to_state[l]	= delta_union_to_state[j * automata_count + l];
#if DEBUG_COMPOSITION
										printf("%d%s", current_to_state[l], l < automata_count -1 ? "," : "");
#endif
										delta_union_p_to_state[delta_union_p_count * automata_count + l]	= current_to_state[l];
									}
									starting_transition->state_from		= from_state;
									starting_transition->state_to		= automaton_composite_tree_get_key(tree, current_to_state);
#if DEBUG_COMPOSITION
									printf("]:%d\n", starting_transition->state_to);
									printf("\t\t[A] Current label ");
									for(m = 0; m < starting_transition->signals_count; m++){
										printf("%s%s", m > 0 ? "," : ""
											, automata[i]->context->global_alphabet->list[GET_TRANSITION_SIGNAL(starting_transition, m)].name);
									}
									printf("\n");
#endif
									for(m = 0; m < alphabet_count; m++){
										delta_union_p_alphabet[delta_union_p_count * alphabet_count + m]	= pending_label[m];
									}
									//automaton_automata_transition_alphabet_to_bool(&(delta_union_p_alphabet[delta_union_p_count * alphabet_count]), starting_transition, alphabet_count);
									delta_union_p[(delta_union_p_count)++]			= starting_transition;
									was_set = true;
								}
							}
						}else if(!labels_overlap){
#if DEBUG_COMPOSITION

							printf("\t\t[B] Pending trans.: %d {", from_state);
							for(m = 0; m < starting_transition->signals_count; m++){
								printf("%s%s", m > 0 ? "," : ""
										, automata[i]->context->global_alphabet->list[GET_TRANSITION_SIGNAL(starting_transition, m)].name);
							}
							printf("}->[");
							for(l = 0; l < automata_count; l++){
								if(l != i){
									current_to_state[l]	= current_state[l];
								}else{
									printf("*");
									current_to_state[l]	= starting_transition->state_to;
								}
								printf("%d%s", current_to_state[l], l < automata_count -1 ? "," : "");
							}
							printf("]:%d mismatches shared alphabet\n", starting_transition->state_to);

#endif
							if(!delta_i_set[k] && !current_overlaps_pending){
								delta_i_set[k]	= true;
#if DEBUG_COMPOSITION
								printf("\t\t[B] Delta i non merged trans.: %d {", from_state);
								for(m = 0; m < starting_transition->signals_count; m++){
									printf("%s%s", m > 0 ? "," : ""
											, automata[i]->context->global_alphabet->list[GET_TRANSITION_SIGNAL(starting_transition, m)].name);
								}
								printf("}->[");
#endif
								for(l = 0; l < automata_count; l++){
									if(l != i){
										current_to_state[l]	= current_state[l];
									}else{
#if DEBUG_COMPOSITION
										printf("*");
#endif
										current_to_state[l]	= starting_transition->state_to;
									}
#if DEBUG_COMPOSITION
									printf("%d%s", current_to_state[l], l < automata_count -1 ? "," : "");
#endif
									delta_union_p_to_state[delta_union_p_count * automata_count + l]	= current_to_state[l];
								}
								starting_transition->state_from		= from_state;
								starting_transition->state_to		= automaton_composite_tree_get_key(tree, current_to_state);
#if DEBUG_COMPOSITION
								printf("]:%d KEPT \n", starting_transition->state_to);
#endif
								automaton_automata_transition_alphabet_to_bool(&(delta_union_p_alphabet[delta_union_p_count * alphabet_count]), starting_transition, alphabet_count);
								delta_union_p[(delta_union_p_count)++]			= starting_transition;
								was_set = true;
							}
							//add independent labels
							if(!pending_added && !pending_overlaps_current){
								pending_added 						= true;
								if(!was_set)
									automaton_transition_destroy(starting_transition, true);
								starting_transition					= automaton_transition_clone(delta_union[j]);

#if DEBUG_COMPOSITION

								printf("\t\t[B] Pending non merged trans.: %d {", from_state);
								for(m = 0; m < starting_transition->signals_count; m++){
									printf("%s%s", m > 0 ? "," : ""
											, automata[i]->context->global_alphabet->list[GET_TRANSITION_SIGNAL(starting_transition, m)].name);
								}
								printf("}->[");
#endif
								for(l = 0; l < automata_count; l++){
									current_to_state[l]	= delta_union_to_state[j * automata_count + l];
#if DEBUG_COMPOSITION
									printf("%d%s", current_to_state[l], l < automata_count -1 ? "," : "");
#endif
									delta_union_p_to_state[delta_union_p_count * automata_count + l]	= current_to_state[l];
								}
#if DEBUG_COMPOSITION
								printf("]:%d KEPT\n", starting_transition->state_to);
#endif
								starting_transition->state_from		= from_state;
								automaton_automata_transition_alphabet_to_bool(&(delta_union_p_alphabet[delta_union_p_count * alphabet_count]), starting_transition, alphabet_count);
								delta_union_p[(delta_union_p_count)++]			= starting_transition;
								was_set = true;
							}
						}
						if(!was_set)
							automaton_transition_destroy(starting_transition, true);
					}
				}
			}
#if DEBUG_COMPOSITION
			printf("\t[S] Pending swap\n");
#endif
			//swap delta union p with delta union
			for(m = 0; m < delta_union_count; m++){
				automaton_transition_destroy(delta_union[m], true);
			}
			delta_union_count = 0;
			delta_union_tmp = delta_union; delta_union_tmp_count = delta_union_count;
			delta_union_tmp_alphabet = delta_union_alphabet; delta_union_tmp_to_state = delta_union_to_state;
			delta_union = delta_union_p; delta_union_count = delta_union_p_count;
			delta_union_alphabet = delta_union_p_alphabet; delta_union_to_state = delta_union_p_to_state;
			delta_union_p = delta_union_tmp; delta_union_p_count = delta_union_tmp_count;
			delta_union_p_alphabet = delta_union_tmp_alphabet; delta_union_p_to_state = delta_union_tmp_to_state;

			for(k = 0; k < automata[i]->local_alphabet_count; k++){
				alphabet_dependency[automata[i]->local_alphabet[k]] = true;
			}
		}
		// add processed transitions from delta_union list
		while(delta_union_count > 0){
			delta_union_count--;
			for(n = 0; n < automata_count; n++){
				current_to_state[n]	= delta_union_to_state[delta_union_count * automata_count + n];
			}

			current_transition				= delta_union[delta_union_count];
			uint32_t composite_to			= automaton_composite_tree_get_key(tree, current_to_state);
			current_transition->state_to	= composite_to;
			automaton_automaton_add_transition(composition, current_transition);
#if DEBUG_COMPOSITION
			printf("\t[+] Adding trans: %d {", current_transition->state_from);
			for(k = 0; k < current_transition->signals_count; k++){
				printf("%s%s", k > 0 ? "," : ""
						, automata[0]->context->global_alphabet->list[GET_TRANSITION_SIGNAL(current_transition, k)].name);
			}
			printf("}->[");
			for(l = 0; l < automata_count; l++){
				printf("%s%d", l > 0 ? "," : "", current_to_state[l]);
			}
			printf("]:%d\n", current_transition->state_to);
#endif
			// set state valuation
			if(is_game){
				uint32_t fluent_index;
				uint32_t fluent_automata_index;
				int32_t state_position	= -1;
				bool state_found;
				for(i = 0; i < fluent_count; i++){
					fluent_index	= GET_STATE_FLUENT_INDEX(fluent_count, composite_to, i);
					//set new valuation
					fluent_automata_index	= automata_count - fluent_count + i;
					if(current_to_state[fluent_automata_index] == 1){
						//Check if it should be added to the inverted valuation list
						state_found		= automaton_bucket_has_entry(composition->inverted_valuations[i], composite_to);
						if(!state_found){
							automaton_bucket_add_entry(composition->inverted_valuations[i], composite_to);
						}
						SET_FLUENT_BIT(composition->valuations, fluent_index);
					}else{
						CLEAR_FLUENT_BIT(composition->valuations, fluent_index);
					}
				}
				//set liveness valuations
				for(i = 0; i < liveness_valuations_count; i++){
					bool current_valuation = true;
					for(j = 0; j < automata_count - fluent_count; j++){
						if(automata[j]->built_from_ltl){
							fluent_index	= GET_STATE_FLUENT_INDEX(liveness_valuations_count, current_to_state[j], i);
							current_valuation = current_valuation && TEST_FLUENT_BIT(automata[j]->liveness_valuations, fluent_index);
						}
						if(!current_valuation)break;
					}
					fluent_index	= GET_STATE_FLUENT_INDEX(liveness_valuations_count, composite_to, i);
					if(current_valuation){
						//Check if it should be added to the inverted valuation list
						state_found		= automaton_bucket_has_entry(composition->liveness_inverted_valuations[i], composite_to);
						//
						if(!state_found){
							automaton_bucket_add_entry(composition->liveness_inverted_valuations[i], composite_to);
						}
						SET_FLUENT_BIT(composition->liveness_valuations, fluent_index);
					}else{
						CLEAR_FLUENT_BIT(composition->liveness_valuations, fluent_index);
					}
				}
			}
			transitions_added_count++;
			automaton_transition_destroy(current_transition, true);
			// expand frontier
			bool found = automaton_bucket_has_entry(bucket_list, composite_to);
			if(found){found_hits++;}else{found_misses++;}
			if(!found){
				composite_frontier[frontier_count]	= composite_to;
				automaton_bucket_add_entry(bucket_list, composite_to);
				for(n = 0; n < automata_count; n++){
					frontier[frontier_count * automata_count + n]	= current_to_state[n];
				}
				frontier_count++;
				if(frontier_count >= (frontier_size - 1)){
					uint32_t new_size	= frontier_size * LIST_INCREASE_FACTOR;
					uint32_t i;
					uint32_t* new_frontier				= malloc(sizeof(uint32_t) * new_size * automata_count);
					uint32_t* new_composite_frontier	= malloc(sizeof(uint32_t) * new_size);
					for(i = 0; i < frontier_size * automata_count; i++){
						new_frontier[i]					= frontier[i];
					}
					for(i = 0; i < frontier_size; i++){
						new_composite_frontier[i]			= composite_frontier[i];
					}
					free(frontier);
					free(composite_frontier);
					frontier_size						= new_size;
					frontier							= new_frontier;
					composite_frontier					= new_composite_frontier;
				}
			}
		}
	}
#if VERBOSE
	printf("TOTAL Composition has [%09d] states and [%09d] transitions run for [%08f] KEY ACCESS.: [Misses:%li,hits:%li]\n", tree->max_value, composition->transitions_composite_count, (double)(clock() - begin) / CLOCKS_PER_SEC, found_misses, found_hits);
#endif
	//CLEANUP
	automaton_bucket_destroy(bucket_list); bucket_list	= NULL;
	free(composite_frontier);	composite_frontier	= NULL;
	free(signals_union);  signals_union	= NULL;
	free(current_to_state); current_to_state	= NULL;
	free(delta_union_to_state); delta_union_to_state	= NULL;
	free(delta_union_alphabet); delta_union_alphabet	= NULL;
	for(i = 0; i < delta_union_count; i++) automaton_transition_destroy(delta_union[i], true);
	free(delta_union); delta_union	= NULL;
	free(delta_union_p_to_state); delta_union_p_to_state	= NULL;
	free(delta_union_p_alphabet); delta_union_p_alphabet	= NULL;
	for(i = 0; i < delta_union_p_count; i++) automaton_transition_destroy(delta_union_p[i], true);
	free(delta_union_p); delta_union_p	= NULL;
	free(pending_label); pending_label	= NULL;
	free(current_label); current_label	= NULL;
	free(tmp_transition_alphabet); tmp_transition_alphabet = NULL;
	free(delta_i_set);
	free(pending_added);
	for(i = 0; i < processed_count; i++) automaton_transition_destroy(processed[i], true);
	free(processed); processed	= NULL;
	automaton_composite_tree_destroy(tree);tree = NULL;
	free(frontier); frontier = NULL;
	free(current_state); current_state = NULL;
	free(current_set_state); current_set_state = NULL;
	free(alphabet);	alphabet	= NULL;
	free(alphabet_independency);
	free(alphabet_dependency);
	return composition;
}
*/
bool automaton_automaton_check_invariant(automaton_automaton* current_automaton){ return false; }
bool automaton_automaton_update_valuation(automaton_automaton* current_automaton){ return false; }

automaton_automaton* automaton_automaton_determinize(automaton_automaton* left_automaton){return NULL; }
automaton_automaton* automaton_automaton_obs_minimize(automaton_automaton* left_automaton){ return NULL; }
automaton_automaton* automaton_automaton_minimize(automaton_automaton* current_automaton){ return NULL; }

/**
 * Checks if two deterministic and minimized automata are equivalent.
 * @param left_automaton the first deterministic minimal automaton to be check against equivalence
 * @param right_automaton the second deterministic minimal automaton to be check against equivalence
 * @return true if both automata are equivalent down to state renaming, false otherwise
 */
bool automaton_automata_are_equivalent(automaton_automaton* left_automaton, automaton_automaton* right_automaton){
	if(left_automaton->out_degree[left_automaton->initial_states[0]]
								  != right_automaton->out_degree[right_automaton->initial_states[0]])
		return false;
	bool* processed	= calloc(left_automaton->transitions_count * 2, sizeof(bool));
	uint32_t* left_frontier		=  calloc(left_automaton->transitions_composite_count * 2, sizeof(uint32_t));
	uint32_t* right_frontier	=  calloc(right_automaton->transitions_composite_count * 2, sizeof(uint32_t));
	uint32_t frontier_count	= 0;
	left_frontier[0]	= left_automaton->initial_states[0];
	right_frontier[0]	= right_automaton->initial_states[0];
	frontier_count++;

	uint32_t left_state, right_state, i, j, k;
	bool match_found = false, label_mismatch = false, transition_mismatch = false;

	while(frontier_count > 0 && !transition_mismatch){
		frontier_count--;
		left_state	= left_frontier[frontier_count];
		right_state	= right_frontier[frontier_count];

		if(processed[left_state])
			continue;
		processed[left_state]	= true;

		for(i = 0; i < left_automaton->out_degree[left_state]; i++){
			match_found = false;
			for(j = 0; j < right_automaton->out_degree[right_state]; j++){
				if(left_automaton->transitions[left_state][i].signals_count
						!= right_automaton->transitions[right_state][j].signals_count)
					continue;
				label_mismatch = false;
				for(k = 0; k < left_automaton->transitions[left_state][i].signals_count; k++){
					if(GET_TRANSITION_SIGNAL(&(left_automaton->transitions[left_state][i]), k)
							!= GET_TRANSITION_SIGNAL(&(right_automaton->transitions[right_state][j]), k)){
						label_mismatch = true;
						break;
					}
				}
				if(!label_mismatch){
					match_found	= true;
					if(!processed[left_automaton->transitions[left_state][i].state_to]){
						if(left_automaton->out_degree[left_automaton->transitions[left_state][i].state_to]
													  != right_automaton->out_degree[right_automaton->transitions[right_state][j].state_to]){
							match_found = false;
						}else{
							left_frontier[frontier_count]	= left_automaton->transitions[left_state][i].state_to;
							right_frontier[frontier_count]	= right_automaton->transitions[right_state][j].state_to;
							frontier_count++;
						}
					}
					break;
				}
			}
			if(!match_found){
				transition_mismatch = true;
				break;
			}
		}
	}

	free(processed);
	free(left_frontier);
	free(right_frontier);
	return !transition_mismatch;
}

bool automaton_automaton_check_reachability(automaton_automaton* current_automaton, automaton_valuation target){ return false; }
bool automaton_automaton_check_liveness(automaton_automaton* current_automaton, automaton_valuation target){ return false; }
/** COMPOSITE TREE **/
void automaton_composite_tree_entry_print(automaton_composite_tree_entry* entry, bool comes_from_next, uint32_t* tabs){
	uint32_t i;
	if(comes_from_next){
		for(i = 0; i < *tabs; i++)
			printf("\t");
	}
	if(entry->next == NULL && entry->succ == NULL){
		printf("->%d", entry->value);
	}else{
		printf("[%d]\t", entry->value);
	}
	if(entry->succ != NULL){
		(*tabs)++;
		automaton_composite_tree_entry_print(entry->succ, false, tabs);
		(*tabs)--;
	}
	if(entry->next != NULL){
		printf("\n");
		automaton_composite_tree_entry_print(entry->next, true, tabs);
	}
}

void automaton_composite_tree_print(automaton_composite_tree* tree){
	printf("Composite Tree.\n");
	if(tree->first_entry != NULL){
		uint32_t tabs	= 0;
		automaton_composite_tree_entry_print(tree->first_entry, false, &tabs);
	}
	printf("\n");
}

automaton_composite_tree* automaton_composite_tree_create(uint32_t key_length){
	automaton_composite_tree* tree	= malloc(sizeof(automaton_composite_tree));
	tree->key_length	= key_length;
	tree->max_value		= 0;
	tree->first_entry	= NULL;
	tree->entries_size_count		= 1;
	tree->entries_composite_count	= 0;
	tree->entries_size				= malloc(sizeof(uint32_t) * tree->entries_size_count);
	tree->entries_count				= malloc(sizeof(uint32_t) * tree->entries_size_count);
	tree->entries_size[0]			= LIST_INITIAL_SIZE;
	tree->entries_count[0]			= 0;
	tree->entries_composite_size	= tree->entries_size[0];
	tree->entries_pool				= malloc(sizeof(automaton_composite_tree_entry*) * tree->entries_size_count);
	tree->entries_pool[0]			= malloc(sizeof(automaton_composite_tree_entry) * tree->entries_size[0]);
	return tree;
}

automaton_composite_tree_entry* automaton_composite_tree_entry_get_from_pool(automaton_composite_tree* tree){
	uint32_t current_pool					= tree->entries_size_count - 1;
	uint32_t i;
	if(tree->entries_composite_count >= tree->entries_composite_size){
		uint32_t new_size					= tree->entries_size[current_pool] * LIST_INCREASE_FACTOR;
		tree->entries_composite_size		+= new_size;
		tree->entries_size_count++;
		current_pool						= tree->entries_size_count - 1;
		uint32_t* new_entries_size		= malloc(sizeof(uint32_t) * tree->entries_size_count);
		uint32_t* new_entries_count		= malloc(sizeof(uint32_t) * tree->entries_size_count);
		automaton_composite_tree_entry** new_entries_pool	= malloc(sizeof(automaton_composite_tree_entry*) * tree->entries_size_count);
		for(i = 0; i < (tree->entries_size_count - 1); i++){
			new_entries_size[i]			= tree->entries_size[i];
			new_entries_count[i]		= tree->entries_count[i];
			new_entries_pool[i]					= tree->entries_pool[i];
		}
		new_entries_size[current_pool]	= new_size;
		new_entries_count[current_pool]	= 0;
		new_entries_pool[current_pool]			= malloc(sizeof(automaton_composite_tree_entry) * new_size);
		free(tree->entries_size);
		free(tree->entries_count);
		free(tree->entries_pool);
		tree->entries_size				= new_entries_size;
		tree->entries_count				= new_entries_count;
		tree->entries_pool				= new_entries_pool;
	}

	automaton_composite_tree_entry* entry	= &(tree->entries_pool[current_pool][tree->entries_count[current_pool]++]);
	tree->entries_composite_count++;
	return entry;
}

uint32_t automaton_composite_tree_get_key(automaton_composite_tree* tree, uint32_t* composite_key){
	uint32_t i,j;
	automaton_composite_tree_entry* current_entry 	= NULL;
	automaton_composite_tree_entry* last_entry		= NULL;
	automaton_composite_tree_entry* terminal_entry	= NULL;
	//if first entry then add whole strip and return first value
#if DEBUG_COMPOSITE_TREE
	printf("(");
	for(i = 0; i < tree->key_length; i++){
		printf("%d%s", composite_key[i], (i == (tree->key_length - 1)) ? "": ",");
	}
	printf("):");
	fflush(stdout);
#endif
	if(tree->first_entry == NULL){
		for(i = 0; i < tree->key_length; i++){
			current_entry			= automaton_composite_tree_entry_get_from_pool(tree);
			current_entry->value	= composite_key[i];
			current_entry->next		= NULL;
			current_entry->succ 	= NULL;
			if(i == 0){
				tree->first_entry	= current_entry;
			}
			if(last_entry != NULL){
				last_entry->succ = current_entry;
			}
			last_entry = current_entry;
			if(i == (tree->key_length -1)){
				terminal_entry			= automaton_composite_tree_entry_get_from_pool(tree);
				terminal_entry->value	= tree->max_value++;
				terminal_entry->succ	= NULL;
				terminal_entry->next	= NULL;
				last_entry->succ		= terminal_entry;
#if DEBUG_COMPOSITE_TREE
				printf("FIRST\n");
#endif
				return terminal_entry->value;
			}
		}
	}else{
		current_entry	= tree->first_entry;
		bool found;
		for(i = 0; i < tree->key_length; i++){
			found	= false;
			while(!found && current_entry != NULL){
				if(current_entry->value == composite_key[i]){
#if DEBUG_COMPOSITE_TREE
					printf("[%d]", composite_key[i]);
#endif
					if(i == (tree->key_length -1)){
#if DEBUG_COMPOSITE_TREE
						printf("<%d>\n",current_entry->succ->value);
#endif
						return current_entry->succ->value;
					}
					current_entry	= current_entry->succ;
					found	= true;
				}else{
					last_entry		= current_entry;
					current_entry	= current_entry->next;
				}
#if DEBUG_COMPOSITE_TREE
				fflush(stdout);
#endif
			}
			//if not found add whole strip from this point on
			if(!found){
#if DEBUG_COMPOSITE_TREE
				printf("X.");
#endif
				break;
			}
		}
		//last_entry	= NULL;
		for(j = i; j < tree->key_length; j++){
			current_entry			= automaton_composite_tree_entry_get_from_pool(tree);
			current_entry->value	= composite_key[j];
#if DEBUG_COMPOSITE_TREE
			printf("%d.", composite_key[j]);
#endif
			current_entry->next	= NULL;
			current_entry->succ = NULL;
			if(j == i && last_entry != NULL){
				last_entry->next = current_entry;
			}else if(last_entry != NULL){
				last_entry->succ = current_entry;
			}
			last_entry = current_entry;
			if(j == (tree->key_length -1)){
				terminal_entry			= automaton_composite_tree_entry_get_from_pool(tree);
				terminal_entry->value	= tree->max_value++;
				terminal_entry->succ	= NULL;
				terminal_entry->next	= NULL;
				last_entry->succ		= terminal_entry;
#if DEBUG_COMPOSITE_TREE
				printf("<%d>\n",terminal_entry->value);
#endif
				return terminal_entry->value;
			}
		}
	}
	return 0;
}
void automaton_composite_tree_destroy_entry(automaton_composite_tree* tree, automaton_composite_tree_entry* tree_entry){
	if(tree_entry->succ != NULL){
		automaton_composite_tree_destroy_entry(tree, tree_entry->succ);
		tree_entry->succ = NULL;
	}
	if(tree_entry->next != NULL){
		automaton_composite_tree_destroy_entry(tree, tree_entry->next);
		tree_entry->next = NULL;
	}
	tree_entry->value = 0;
	//free(tree_entry);
}

void automaton_composite_tree_destroy(automaton_composite_tree* tree){
	if(tree->first_entry != NULL){
		automaton_composite_tree_destroy_entry(tree, tree->first_entry);
		tree->first_entry = NULL;
	}
	tree->max_value 	= 0;
	tree->key_length	= 0;
	uint32_t i;
	for(i = 0; i < tree->entries_size_count; i++)free(tree->entries_pool[i]);free(tree->entries_pool);
	free(tree->entries_size);free(tree->entries_count);
	tree->entries_size_count		= 0;
	tree->entries_composite_count	= 0;
	tree->entries_composite_size	= 0;
	tree->entries_pool				= NULL;
	tree->entries_size				= NULL;
	tree->entries_count				= NULL;
	free(tree);
}
