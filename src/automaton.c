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
	target->list						= malloc(sizeof(automaton_signal_event) * target->size);
	uint32_t i;
	for(i = 0; i < target->count; i++){
		automaton_signal_event_copy(&(source->list[i]), &(target->list[i]));
	}
}
automaton_transition* automaton_transition_clone(automaton_transition* source){
	automaton_transition* copy		= malloc(sizeof(automaton_transition));
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
		target->other_signals					= malloc(sizeof(signal_t) * (target->signals_size - FIXED_SIGNALS_COUNT));
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
	target->starting_signals_count	= source->starting_signals_count;
	target->starting_signals		= malloc(sizeof(uint32_t) * target->starting_signals_count);
	uint32_t i;
	for(i = 0; i < target->starting_signals_count; i++){
		target->starting_signals[i]	= source->starting_signals[i];
	}
	target->ending_signals_count	= source->ending_signals_count;
	target->ending_signals			= malloc(sizeof(uint32_t) * target->ending_signals_count);
	for(i = 0; i < target->ending_signals_count; i++){
		target->ending_signals[i]	= source->ending_signals[i];
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
	target->is_controllable			= malloc(sizeof(bool) * target->transitions_count);
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
			target->inverted_transitions	= malloc(sizeof(automaton_transition) * in_size);
			for(j = 0; j < in_degree; j++){
				automaton_transition_copy(&(source->inverted_transitions[i][j]), &(target->inverted_transitions[i][j]));
			}
		}
		target->out_degree[i]	= source->out_degree[i];
		target->out_size[i]		= source->out_size[i];
		out_degree				= target->out_degree[i];
		out_size				= target->out_size[i];
		if(out_size > 0){
			target->transitions[i]	= malloc(sizeof(automaton_transition) * out_size);
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
		target->liveness_valuations			= malloc(sizeof(uint32_t) * target->liveness_valuations_size);
		for(i = 0; i < target->liveness_valuations_size; i++){
				target->liveness_valuations[i]		= source->liveness_valuations[i];
			}
		target->inverted_valuations						= malloc(sizeof(automaton_bucket_list*) * target->context->global_fluents_count);
		for(i = 0; i < target->context->global_fluents_count; i++){
			target->inverted_valuations[i]				= automaton_bucket_list_create(FLUENT_BUCKET_SIZE);
		}
		target->liveness_inverted_valuations			= malloc(sizeof(automaton_bucket_list*) * target->context->liveness_valuations_count);
		for(i = 0; i < target->context->liveness_valuations_count; i++){
			target->liveness_inverted_valuations[i]		= automaton_bucket_list_create(FLUENT_BUCKET_SIZE);
		}
		//TODO: copy buckets contents
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
		target->ranges[i]			= source->ranges[i];
	}
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
	uint32_t i;

	automaton_alphabet*	alphabet	= ctx->global_alphabet;

	printf("Fluent %s:<{", fluent->name);
	for(i = 0; i < fluent->starting_signals_count; i++){
		printf("%s%s", alphabet->list[fluent->starting_signals[i]].name, (i < (fluent->starting_signals_count -1)? "," : ""));
	}
	printf("},{");
	for(i = 0; i < fluent->ending_signals_count; i++){
		printf("%s%s", alphabet->list[fluent->ending_signals[i]].name, (i < (fluent->ending_signals_count -1)? "," : ""));
	}
	printf("}> initially %s", (fluent->initial_valuation? "true":"false"));
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
	for(i = 0; i < ctx->liveness_valuations_count; i++){
		printf("%s:\n", ctx->liveness_valuations_names[i]);
		obdd_print(ctx->liveness_valuations[i]);
	}
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
	automaton_transition* current_transition;
	for(i = 0; i < current_automaton->transitions_count; i++){
		if(current_automaton->out_degree[i] <= 0){
			fprintf(f, "S_%d = STOP", i);
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
		if(i == (current_automaton->transitions_count - 1)){fprintf(f, ".");}else{fprintf(f, ",\n");}
	}
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
	uint32_t i;
	for(i = 0; i < fluent->starting_signals_count; i++){
		fprintf(f, "%d%s", fluent->starting_signals[i], i == (fluent->starting_signals_count - 1)? "" :  AUT_SER_SEP);
	}
	fprintf(f, "%s%s%d%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, fluent->ending_signals_count, AUT_SER_SEP, AUT_SER_ARRAY_START);
	for(i = 0; i < fluent->ending_signals_count; i++){
		fprintf(f, "%d%s", fluent->ending_signals[i], i == (fluent->ending_signals_count - 1)? "" :  AUT_SER_SEP);
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
	for(i = 0; i < automaton->transitions_count; i++){
		for(j = 0; j < automaton->out_degree[i]; j++){
			if(&(automaton->transitions[i][j]) != NULL){
				automaton_transition_serialize_report(f, &(automaton->transitions[i][j]));
				fprintf(f, "%s",current_count == (automaton->transitions_composite_count - 1)? "" :  AUT_SER_SEP);
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
	automaton_transition* transition	= malloc(sizeof(automaton_transition));
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
	fluent->starting_signals		= NULL;
	fluent->ending_signals_count	= 0;
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
	ctx->global_fluents			= malloc(sizeof(automaton_fluent) * ctx->global_fluents_count);
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
		automaton->transitions[i]			= malloc(sizeof(automaton_transition) * automaton->out_size[i]);
		for(j = 0; j < automaton->out_size[i]; j++)
			automaton_transition_initialize(&(automaton->transitions[i][j]), 0, 0);
		automaton->inverted_transitions[i]	= malloc(sizeof(automaton_transition) * automaton->in_size[i]);
		for(j = 0; j < automaton->in_size[i]; j++)
			automaton_transition_initialize(&(automaton->inverted_transitions[i][j]), 0, 0);
	}
	automaton->initial_states_count		= 0;
	automaton->initial_states			= NULL;
	automaton->is_game					= is_game;
	automaton->liveness_valuations_size = 0;
	if(is_game){
		uint32_t new_size					= GET_FLUENTS_ARR_SIZE(automaton->context->global_fluents_count, automaton->transitions_size);
		automaton->valuations 				= malloc(sizeof(uint32_t) * new_size);
		automaton->inverted_valuations		= malloc(sizeof(automaton_bucket_list*) * automaton->context->global_fluents_count);
		for(i = 0; i < automaton->context->global_fluents_count; i++){
			automaton->inverted_valuations[i]	= automaton_bucket_list_create(FLUENT_BUCKET_SIZE);
		}
		new_size					= GET_FLUENTS_ARR_SIZE(automaton->context->liveness_valuations_count, automaton->transitions_size);
		//automaton->liveness_valuations_size			= new_size;
		automaton->liveness_valuations 				= malloc(sizeof(uint32_t) * new_size);
		automaton->liveness_inverted_valuations		= malloc(sizeof(automaton_bucket_list*) * automaton->context->liveness_valuations_count);
		for(i = 0; i < automaton->context->liveness_valuations_count; i++){
			automaton->liveness_inverted_valuations[i]	= automaton_bucket_list_create(FLUENT_BUCKET_SIZE);
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
	free(fluent->starting_signals);
	free(fluent->ending_signals);
	fluent->name					= NULL;
	fluent->starting_signals_count	= 0;
	fluent->starting_signals		= NULL;
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
		for(j = 0; j < automaton->out_size[i]; j++){
			automaton_transition_destroy(&(automaton->transitions[i][j]), false);
		}
		for(j = 0; j < automaton->in_size[i]; j++){
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
		free(automaton->inverted_valuations);
		free(automaton->valuations);
		for(i = 0; i < automaton->context->liveness_valuations_count; i++)
			automaton_bucket_destroy(automaton->liveness_inverted_valuations[i]);
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
		automaton_range_destroy(valuation->ranges[i]);
	}
	free(valuation->ranges);
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
int32_t automaton_indexes_valuation_get_value(automaton_indexes_valuation* valuation, char* range_name){
	uint32_t i;
	for(i = 0; i < valuation->count; i++){
		if(strcmp(valuation->ranges[i]->name, range_name))
			return valuation->current_values[i];
	}
	return 0;
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
			if(signal_ordered_index <= i){
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
	uint32_t i;
	uint32_t signal_index	= automaton_alphabet_get_signal_index(ctx->global_alphabet, signal_event);
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
	if(automaton_transition_has_signal_event(transition, ctx, signal_event))
		return true;
	if(signal_event->type == INPUT_SIG)
		transition->is_input	= true;
	//signals should preserve order within the transition
	int32_t i, signal_ordered_index	= -1;
	int32_t old_count	= (int32_t)transition->signals_count;
	uint32_t signal_index	= automaton_alphabet_get_signal_index(ctx->global_alphabet, signal_event);
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
			transition->other_signals	= malloc(sizeof(signal_t) * FIXED_SIGNALS_COUNT);
			transition->signals_size	+= FIXED_SIGNALS_COUNT;
		}else{//should update list
			uint32_t old_size			= transition->signals_size - FIXED_SIGNALS_COUNT;
			uint32_t new_size			= old_size * SIGNALS_INCREASE_FACTOR;
			signal_t* new_signals		= malloc(sizeof(signal_t) * new_size);
			for(i = 0; i < (int32_t)old_size; i++)
				new_signals[i]			= transition->other_signals[i];
			free(transition->other_signals);
			transition->other_signals	= new_signals;
			transition->signals_size	= FIXED_SIGNALS_COUNT + new_size;
		}
	}

	for(i = old_count -1; i >= 0; i--){
		if(signal_ordered_index >= i){
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
bool automaton_fluent_has_starting_signal(automaton_fluent* fluent, automaton_alphabet* alphabet, automaton_signal_event* signal_event){
	uint32_t i;
	signal_t signal_index	= automaton_alphabet_get_signal_index(alphabet, signal_event);
	for(i = 0; i < fluent->starting_signals_count; i++){
		if(fluent->starting_signals[i] == signal_index)
			return true;
	}
	return false;
}
bool automaton_fluent_add_starting_signal(automaton_fluent* fluent, automaton_alphabet* alphabet, automaton_signal_event* signal_event){
	if(automaton_fluent_has_starting_signal(fluent, alphabet, signal_event))
		return true;
	uint32_t i;
	signal_t signal_index	= automaton_alphabet_get_signal_index(alphabet, signal_event);
	signal_t* new_signals	= malloc(sizeof(signal_t) * (fluent->starting_signals_count + 1));
	for(i = 0; i < fluent->starting_signals_count; i++)
		new_signals[i]	= fluent->starting_signals[i];
	free(fluent->starting_signals);
	new_signals[fluent->starting_signals_count]	= signal_index;
	fluent->starting_signals	= new_signals;
	fluent->starting_signals_count++;
	return false;
}
bool automaton_fluent_has_ending_signal(automaton_fluent* fluent, automaton_alphabet* alphabet, automaton_signal_event* signal_event){
	uint32_t i;
	signal_t signal_index	= automaton_alphabet_get_signal_index(alphabet, signal_event);
	for(i = 0; i < fluent->ending_signals_count; i++){
		if(fluent->ending_signals[i] == signal_index)
			return true;
	}
	return false;
}
bool automaton_fluent_add_ending_signal(automaton_fluent* fluent, automaton_alphabet* alphabet, automaton_signal_event* signal_event){
	if(automaton_fluent_has_ending_signal(fluent, alphabet, signal_event))
		return true;
	uint32_t i;
	signal_t signal_index	= automaton_alphabet_get_signal_index(alphabet, signal_event);
	signal_t* new_signals	= malloc(sizeof(signal_t) * (fluent->ending_signals_count + 1));
	for(i = 0; i < fluent->ending_signals_count; i++)
		new_signals[i]	= fluent->ending_signals[i];
	free(fluent->ending_signals);
	new_signals[fluent->ending_signals_count]	= signal_index;
	fluent->ending_signals	= new_signals;
	fluent->ending_signals_count++;
	return false;
}
automaton_automaton* automaton_fluent_build_automaton(automaton_automata_context* ctx, uint32_t fluent_index){
	automaton_fluent* current_fluent	= &(ctx->global_fluents[fluent_index]);
	uint32_t i, j;
	//uint32_t* local_alphabet			= malloc(sizeof(uint32_t) * (current_fluent->ending_signals_count + current_fluent->starting_signals_count));
	uint32_t* local_alphabet			= malloc(sizeof(uint32_t) * (ctx->global_alphabet->count));
	uint32_t alphabet_count				= 0;
	bool found							= false;
	/*for(i = 0; i < current_fluent->starting_signals_count; i++){
		found	= false;
		for(j = 0; j < alphabet_count; j++)
			if(local_alphabet[j] == current_fluent->starting_signals[i]){
				found	= true;
				break;
			}
		if(!found)
			local_alphabet[alphabet_count++]	= current_fluent->starting_signals[i];
	}
	for(i = 0; i < current_fluent->ending_signals_count; i++){
		found	= false;
		for(j = 0; j < alphabet_count; j++)
			if(local_alphabet[j] == current_fluent->ending_signals[i]){
				found	= true;
				break;
			}
		if(!found)
			local_alphabet[alphabet_count++]	= current_fluent->ending_signals[i];
	}*/
	for(j = 0; j < ctx->global_alphabet->count; j++)
		local_alphabet[alphabet_count++]	= j;

	automaton_transition *starting_transition	= automaton_transition_create(0, 1);
	automaton_transition *starting_loop			= automaton_transition_create(0, 0);
	automaton_transition *ending_transition		= automaton_transition_create(1, 0);
	automaton_transition *ending_loop			= automaton_transition_create(1, 1);

	//TODO:should be superset both on transition and loop
	automaton_automaton* current_automaton	= automaton_automaton_create(current_fluent->name, ctx, alphabet_count, local_alphabet, false, false);
	for(i = 0; i < ctx->global_alphabet->count; i++){
		for(j = 0; j < current_fluent->starting_signals_count; j++){
			if(i != current_fluent->starting_signals[j]){
				automaton_transition_initialize(starting_loop, 0, 0);
				automaton_transition_add_signal_event(starting_loop, ctx, &(ctx->global_alphabet->list[i]));
				automaton_automaton_add_transition(current_automaton, starting_loop);
				automaton_transition_destroy(starting_loop, false);
			}
		}
		for(j = 0; j < current_fluent->ending_signals_count; j++){
			if(i != current_fluent->ending_signals[j]){
				automaton_transition_initialize(ending_loop, 1, 1);
				automaton_transition_add_signal_event(ending_loop, ctx, &(ctx->global_alphabet->list[i]));
				automaton_automaton_add_transition(current_automaton, ending_loop);
				automaton_transition_destroy(ending_loop, false);
			}
		}
	}
	automaton_transition_destroy(starting_loop, true);
	automaton_transition_destroy(ending_loop, true);

	for(i = 0; i < current_fluent->starting_signals_count; i++){
		automaton_transition_initialize(starting_transition, 0, 1);
		automaton_transition_add_signal_event(starting_transition, ctx, &(ctx->global_alphabet->list[current_fluent->starting_signals[i]]));
		automaton_automaton_add_transition(current_automaton, starting_transition);
		automaton_transition_destroy(starting_transition, false);

	}
	automaton_transition_destroy(starting_transition, true);

	for(i = 0; i < current_fluent->ending_signals_count; i++){
		automaton_transition_initialize(ending_transition, 1, 0);
		automaton_transition_add_signal_event(ending_transition, ctx, &(ctx->global_alphabet->list[current_fluent->ending_signals[i]]));
		automaton_automaton_add_transition(current_automaton, ending_transition);
		automaton_transition_destroy(ending_transition, false);
	}
	free(local_alphabet);
	automaton_transition_destroy(ending_transition, true);
	automaton_automaton_add_initial_state(current_automaton, current_fluent->initial_valuation? 1 : 0);
	return current_automaton;
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
	bool found_signal = false;
	for(i = 0; i < out_degree; i++){
		if(current_transitions[i].signals_count != transition->signals_count)
			continue;
		if(current_transitions[i].state_to == to_state){
			if(transition->signals_count == 0 && current_transitions[i].signals_count == 0){
				return true;
			}else{
				for(j = 0; j < transition->signals_count; j++){
					found_signal = false;
					for(k = 0; k < current_transitions[i].signals_count; k++){
						signal_t sig_j	= j < FIXED_SIGNALS_COUNT? transition->signals[j] : transition->other_signals[j-FIXED_SIGNALS_COUNT];
						signal_t sig_k	= k < FIXED_SIGNALS_COUNT? current_transitions[i].signals[k] : current_transitions[i].other_signals[k - FIXED_SIGNALS_COUNT];
						if(sig_j == sig_k){
							found_signal = true;
							break;
						}
					}
					if(!found_signal)return false;
				}
				return true;
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
	bool* next_is_controllable			= malloc(sizeof(bool) * next_size);
	uint32_t* next_out_degree			= malloc(sizeof(uint32_t) * next_size);
	uint32_t* next_out_size				= malloc(sizeof(uint32_t) * next_size);
	uint32_t* next_in_degree			= malloc(sizeof(uint32_t) * next_size);
	uint32_t* next_in_size				= malloc(sizeof(uint32_t) * next_size);
	automaton_transition** next_trans	= malloc(sizeof(automaton_transition*) * next_size);
	automaton_transition** next_inv_trans= malloc(sizeof(automaton_transition*) * next_size);
	for(i = 0; i < current_automaton->transitions_count; i++){
		next_is_controllable[i]			= current_automaton->is_controllable[i];
		next_out_degree[i]				= current_automaton->out_degree[i];
		next_in_degree[i]				= current_automaton->in_degree[i];
		next_in_size[i]					= current_automaton->in_size[i];
		next_out_size[i]				= current_automaton->out_size[i];
		next_trans[i]					= current_automaton->transitions[i];
		next_inv_trans[i]				= current_automaton->inverted_transitions[i];
	}
	for(i = current_automaton->transitions_count; i < next_size; i++){
		next_is_controllable[i]			= true;
		next_out_degree[i]				= 0;
		next_in_degree[i]				= 0;
		next_in_size[i]					= TRANSITIONS_INITIAL_SIZE;
		next_out_size[i]				= TRANSITIONS_INITIAL_SIZE;
		next_trans[i]					= malloc(sizeof(automaton_transition) * next_out_size[i]);
		for(j = 0; j < next_out_size[i]; j++){
			automaton_transition_initialize(&(next_trans[i][j]), 0, 0);
		}
		next_inv_trans[i]				= malloc(sizeof(automaton_transition) * next_in_size[i]);
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
		old_valuations_size						= GET_FLUENTS_ARR_SIZE(current_automaton->context->global_fluents_count, old_size);
		new_valuations_size						= GET_FLUENTS_ARR_SIZE(current_automaton->context->global_fluents_count, current_automaton->transitions_size);
		uint32_t* new_valuations 				= malloc(sizeof(uint32_t) * new_valuations_size);
		for(i = 0; i < old_valuations_size; i++){
			new_valuations[i]					= current_automaton->valuations[i];
		}
		free(current_automaton->valuations);
		current_automaton->valuations			= new_valuations;
		old_valuations_size						= GET_FLUENTS_ARR_SIZE(current_automaton->context->liveness_valuations_count, old_size);
		new_valuations_size						= GET_FLUENTS_ARR_SIZE(current_automaton->context->liveness_valuations_count, current_automaton->transitions_size);
		new_valuations 							= malloc(sizeof(uint32_t) * new_valuations_size);
		for(i = 0; i < old_valuations_size; i++){
			new_valuations[i]					= current_automaton->liveness_valuations[i];
		}
		free(current_automaton->liveness_valuations);
		current_automaton->liveness_valuations	= new_valuations;
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
		automaton_transition* new_out	= malloc(sizeof(automaton_transition) * current_automaton->out_size[from_state]);
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
		automaton_transition* new_in	= malloc(sizeof(automaton_transition) * current_automaton->in_size[to_state]);
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
	exit(-1);
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
automaton_pending_state* automaton_pending_state_create(uint32_t state, int32_t goal_to_satisfy, int32_t value, uint32_t timestamp){
	automaton_pending_state* pending	= malloc(sizeof(automaton_pending_state));
	pending->state			= state;
	pending->goal_to_satisfy= goal_to_satisfy;
	pending->value			= value;
	pending->timestamp		= timestamp;
	return pending;
}
int32_t automaton_pending_state_compare(void* left_pending_state, void* right_pending_state){
	automaton_pending_state* left	= (automaton_pending_state*)left_pending_state;
	automaton_pending_state* right= (automaton_pending_state*)right_pending_state;

	if(left->value > right->value){
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
		if(min_ranking == NULL){
			min_value = max_value = current_value->value;
			min_ranking = max_ranking = current_value;
		}
		if(is_controllable){
			if((min_value >= current_value->value) || (current_value->value != RANKING_INFINITY && min_value == RANKING_INFINITY)){
				min_value 	= current_value->value;
				min_ranking	= current_value;
			}
		}else{
			if((max_value <= current_value->value) || (current_value->value == RANKING_INFINITY && min_value != RANKING_INFINITY)){
				max_value 	= current_value->value;
				max_ranking	= current_value;
			}
		}
	}
	automaton_ranking* return_value = is_controllable ? min_ranking : max_ranking;
#if DEBUG_SYNTHESIS
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
	return (left->value > right->value) || (left->value == right->value && left->assumption_to_satisfy > right->assumption_to_satisfy);
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
	/**
	 * r_j(v)
	 */
	automaton_ranking*	current_ranking	= ((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking[current_guarantee], state));
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
	fluent_index				= GET_STATE_FLUENT_INDEX(fluent_count, state, assumptions_indexes[current_ranking->assumption_to_satisfy]);
	bool satisfies_assumption	= TEST_FLUENT_BIT(game_automaton->valuations, fluent_index);
	j = satisfies_guarantee? (current_guarantee + 1) % guarantee_count : current_guarantee;
	/**
	 * r_j(w)
	 */
	automaton_ranking* sr		= automaton_state_best_successor_ranking(game_automaton, state, ranking, j
			, guarantee_count, guarantees_indexes);
	if(sr == NULL){
#if DEBUG_SYNTHESIS
		printf("No best successor for %d\n", state);
#endif
		return true;
	}
#if DEBUG_SYNTHESIS
		printf("best successor for %d is %d:(%d,%d)", state, sr->state,sr->value, sr->assumption_to_satisfy);
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
 	printf("%s\n", is_stable? "stable" : "unstable");
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
	for(i = 0; i < game_automaton->in_degree[state]; i++){
		current_transition	= &(game_automaton->inverted_transitions[state][i]);
		//TODO:check if we should add another structure to answer inclusion queries (heap of state_from synched with pending_state)
		if(!automaton_state_is_stable(game_automaton, current_transition->state_from, ranking, current_guarantee, guarantee_count
				, assumptions_count, guarantees_indexes, assumptions_indexes, first_assumption_index) || current_transition->is_input){
#if DEBUG_SYNTHESIS
	printf("[Unstable] Pushing into pending (%d, %d) from state %d\n", current_transition->state_from, current_guarantee, state);
#endif
			//check if entry exists in the pending structure, if so update previous value
			if(automaton_concrete_bucket_has_key(key_list, current_transition->state_from)){
				existing_pending_state					= automaton_concrete_bucket_get_entry(key_list, current_transition->state_from);
				existing_pending_state->value			= ((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking[current_guarantee], current_transition->state_from))->value;
				existing_pending_state->timestamp		= timestamp;
			}else{
				current_pending_state.goal_to_satisfy	= current_guarantee; current_pending_state.state = current_transition->state_from;
				current_pending_state.value				= ((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking[current_guarantee], current_transition->state_from))->value;
				current_pending_state.timestamp			= timestamp;
				automaton_max_heap_add_entry(pending_list, &current_pending_state);
				automaton_concrete_bucket_add_entry(key_list, &(current_transition->state_from));
			}
		}
	}
}
void automaton_ranking_increment(automaton_automaton* game_automaton, automaton_concrete_bucket_list** ranking, automaton_ranking* current_ranking, uint32_t ref_state, uint32_t* max_delta
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t assumptions_count
		, uint32_t* guarantees_indexes, uint32_t* assumptions_indexes, uint32_t first_assumption_index, automaton_ranking* target_ranking){
	automaton_automata_context* ctx		= game_automaton->context;
	uint32_t i;
	bool satisfies_goal, satisfies_assumption;
	//set default values
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
			target_ranking->assumption_to_satisfy = current_ranking->assumption_to_satisfy + 1;
		}else if(current_ranking->value < (int32_t)(max_delta[current_guarantee] - 1)){
			target_ranking->value++;
		}else{
			target_ranking->value 					= RANKING_INFINITY;
		}
	}
}

void automaton_ranking_update(automaton_automaton* game_automaton, automaton_concrete_bucket_list** ranking, automaton_ranking* current_ranking, uint32_t* max_delta
		, uint32_t current_guarantee, uint32_t guarantee_count, uint32_t assumptions_count
		, uint32_t* guarantees_indexes, uint32_t* assumptions_indexes, uint32_t first_assumption_index){
	automaton_ranking incr_ranking;
	incr_ranking.assumption_to_satisfy 	= current_ranking->assumption_to_satisfy;
	incr_ranking.state					= current_ranking->state;
	incr_ranking.value					= current_ranking->value;
	/**
	 * get best_j(v)
	 */
	automaton_ranking* best_ranking		= automaton_state_best_successor_ranking(game_automaton, current_ranking->state, ranking, current_guarantee, guarantee_count, guarantees_indexes);
	/**
	 * get incr_j_v(best_j(v))
	 */
	automaton_ranking_increment(game_automaton, ranking, best_ranking, current_ranking->state, max_delta, current_guarantee, guarantee_count, assumptions_count
			, guarantees_indexes, assumptions_indexes, first_assumption_index, &incr_ranking);
	/**
	 * update to max between current ranking and the next
	 */
	if(automaton_ranking_lt(current_ranking, &incr_ranking)){
		current_ranking->value			= incr_ranking.value;
		current_ranking->assumption_to_satisfy	= incr_ranking.assumption_to_satisfy;
	}
}

automaton_automaton* automaton_get_gr1_strategy(automaton_automaton* game_automaton, char** assumptions, uint32_t assumptions_count
		, char** guarantees, uint32_t guarantees_count){
	clock_t begin = clock();
	uint32_t pending_processed	= 0;
	//TODO: preallocate pending states and rankings
	automaton_automata_context* ctx				= game_automaton->context;
	uint32_t i, j ,k, l, m;
	automaton_concrete_bucket_list** ranking_list	= malloc(sizeof(automaton_concrete_bucket_list*) * guarantees_count);
	automaton_max_heap* pending_list	= automaton_max_heap_create(sizeof(automaton_pending_state)
			, automaton_pending_state_compare);
	automaton_concrete_bucket_list* key_list		= automaton_concrete_bucket_list_create(RANKING_BUCKET_SIZE, automaton_int_extractor, sizeof(uint32_t));
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
	printf("Infinity_%d:%d", l, max_delta[l]);
#endif
		}
	}
#if DEBUG_SYNTHESIS
	printf("\n");
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
#if DEBUG_SYNTHESIS
	printf("[Deadlock] Adding unstable pred for %d\n", i);
#endif
				automaton_add_unstable_predecessors(game_automaton, pending_list, key_list, i, ranking_list, /*guarantees_indexes[j] <- WAS*/ j, guarantees_count
						, assumptions_count, guarantees_indexes, assumptions_indexes, assumptions_indexes[first_assumption_index], pending_processed);
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
	printf("[Init] Pushing into pending (%d, %d)\n", i, guarantees_indexes[j]);
#endif
							concrete_pending_state.state 	= i; concrete_pending_state.goal_to_satisfy	= j;//TODO: check this, was ...goal_to_satisfy = guarantees_indexes[j];
							concrete_pending_state.value	= 0; concrete_pending_state.timestamp		= 0;
							automaton_max_heap_add_entry(pending_list, &concrete_pending_state);
							automaton_concrete_bucket_add_entry(key_list, &(i));
							break;
						}
					}
				}
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
	printf("!!STABILIZING GR1 GAME FOR %s!!\n", strategy_name);
	automaton_automaton_print(game_automaton, false, false, true, "", "\n");
#endif
	uint32_t* state_location;
	while(pending_list->count > 0){
		//current_pending_state	= (automaton_pending_state*)automaton_ptr_bucket_pop_entry(pending_list);
		automaton_max_heap_pop_entry(pending_list, &current_pending_state);

		state_location	= (uint32_t*)automaton_concrete_bucket_get_entry(key_list, current_pending_state.state);
		//TODO:check, why are we getting a NULL at state location
		if(state_location != NULL)
			automaton_concrete_bucket_remove_entry(key_list, state_location);
		current_ranking			= (automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[current_pending_state.goal_to_satisfy], current_pending_state.state);
#if DEBUG_SYNTHESIS
			printf("Popping from pending (%d, %d) with ranking (%d, %d)\n", current_pending_state.state, current_pending_state.goal_to_satisfy, current_ranking->value, current_ranking->assumption_to_satisfy);
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
				(automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[current_pending_state.goal_to_satisfy], current_pending_state.state)
				, max_delta, current_pending_state.goal_to_satisfy, guarantees_count, assumptions_count
				, guarantees_indexes, assumptions_indexes, first_assumption_index);
#if DEBUG_SYNTHESIS
	printf("[PENDING PROC.] Adding unstable pred for %d\n", current_pending_state.state);
#endif
		automaton_add_unstable_predecessors(game_automaton, pending_list, key_list, current_pending_state.state
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
#if DEBUG_SYNTHESIS
	printf("!!RANKING STABILIZATION ACHIEVED FOR %s!!\n", strategy_name);
	for(i = 0; i < game_automaton->transitions_count; i++){
		if(game_automaton->out_degree[i] <= 0) continue;
		//printf("S %d:\t", i);
		printf("[S %d:", i);
		for(j = 0; j < guarantees_count; j++){
			current_ranking	= ((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[j], i));
			//printf("R_%d=<%d,%d>\t", j, current_ranking->value, current_ranking->assumption_to_satisfy);
			printf("%d,<%d,%d>", j, current_ranking->value, current_ranking->assumption_to_satisfy);
		}
		printf("]");
	}
	printf("\tpending:%d\n", pending_list->count);
#endif

	printf("Synthesis processed [%09d] states, run for [%08f]s\n", pending_processed, (double)(clock() - begin) / CLOCKS_PER_SEC);
	//build strategy

	strategy	= automaton_automaton_create(strategy_name, game_automaton->context, game_automaton->local_alphabet_count, game_automaton->local_alphabet, false, false);



	bool is_winning = true;
	bool one_option = false;
	//check if all rankings have the initial state
	for(i = 0; i < guarantees_count; i++){
		one_option	= false;
		for(j = 0; j < game_automaton->out_degree[game_automaton->initial_states[0]]; j++){
			if(((automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[i]
																					 , game_automaton->transitions[game_automaton->initial_states[0]][j].state_to))->value != RANKING_INFINITY){
				one_option = true;
			}
		}
		if(!one_option){
			is_winning = false;
			break;
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
					}
					may_increase	= automaton_bucket_has_entry(game_automaton->inverted_valuations[guarantees_indexes[i]], current_ranking->state);
					succ_guarantee	= may_increase ? (i % guarantees_count) : i;

					is_controllable	= game_automaton->is_controllable[current_ranking->state];
					for(l = 0; l < game_automaton->out_degree[current_ranking->state]; l++){
						current_transition	= &(game_automaton->transitions[current_ranking->state][l]);
						succ_ranking		=  (automaton_ranking*)automaton_concrete_bucket_get_entry(ranking_list[succ_guarantee], current_transition->state_to);
						//TODO: check this condition
						if(succ_ranking == NULL) continue;
						//check if succ ranking is better than current

						if(!is_controllable || ((may_increase && succ_ranking->value != RANKING_INFINITY)
							||(!may_increase &&
								((current_ranking->value > succ_ranking->value || (current_ranking->value == RANKING_INFINITY && succ_ranking->value != RANKING_INFINITY))||
										(current_ranking->value == succ_ranking->value && current_ranking->assumption_to_satisfy > succ_ranking->assumption_to_satisfy)
								)||(current_ranking->assumption_to_satisfy == succ_ranking->assumption_to_satisfy
								&& current_ranking->value == succ_ranking->value
								&& !automaton_bucket_has_entry(game_automaton->inverted_valuations[assumptions_indexes[current_ranking->assumption_to_satisfy]], current_ranking->state))))){
							if(!strategy_maps_is_set[i][current_ranking->state]){
								strategy_maps[i][current_ranking->state]	= last_strategy_state++;
								strategy_maps_is_set[i][current_ranking->state]	= true;
							}
							if(!strategy_maps_is_set[succ_guarantee][succ_ranking->state]){
								strategy_maps[succ_guarantee][succ_ranking->state]	= last_strategy_state++;
								strategy_maps_is_set[succ_guarantee][succ_ranking->state] = true;
							}
							automaton_transition_copy(current_transition, strategy_transition);
							strategy_transition->state_from	= strategy_maps[i][current_ranking->state];
							strategy_transition->state_to	= strategy_maps[succ_guarantee][succ_ranking->state];
							strategy_transition->signals_size	= current_transition->signals_size;
							strategy_transition->signals_count	= current_transition->signals_count;
							for(m = 0; m < FIXED_SIGNALS_COUNT; m++)
								strategy_transition->signals[m]		= current_transition->signals[m];
							strategy_transition->other_signals	= current_transition->other_signals;
							if(succ_ranking->value != RANKING_INFINITY)
								automaton_automaton_add_transition(strategy, strategy_transition);
							//add initial state if not already added
							if(strategy->initial_states_count == 0 && current_ranking->state == game_automaton->initial_states[0]){
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

	//destroy structures
	free(max_delta);
	for(i = 0; i < guarantees_count; i++){
		automaton_concrete_bucket_destroy(ranking_list[i]);
	}
	free(ranking_list);
	automaton_max_heap_destroy(pending_list);
	automaton_concrete_bucket_destroy(key_list);
	free(assumptions_indexes);
	free(guarantees_indexes);
	return strategy;
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

automaton_automaton* automaton_automata_compose(automaton_automaton** automata, uint32_t automata_count, automaton_synchronization_type type, bool is_game){
	clock_t begin = clock();
	uint32_t transitions_added_count	= 0;
	uint32_t i, j, k, l, m, n, o;
	uint32_t alphabet_count, fluents_count, alphabet_size;
	uint32_t* alphabet;
	alphabet_count	= 0;
	alphabet_size	= 0;
	/***********************
	 * get union of alphabets
	 * check ctx and compute alphabet size
	 ***********************/
#if DEBUG_COMPOSITION
	if(is_game){
		printf("!!COMPOSING GAME!!\n");
	}else{
		printf("!!COMPOSING AUTOMATON!!\n");
	}
	printf("Automata:[");
	for(i =0; i < automata_count; i++){
		printf("%s,", automata[i]->name);
	}
	printf("]\n");
#endif
	automaton_automata_context* ctx	= NULL;
	for(i = 0; i < automata_count; i++){
		if(automata[i]->initial_states_count != 1)
			return NULL;
		if(ctx == NULL){
			ctx = automata[i]->context;
		}else if(ctx != automata[i]->context){
			return NULL;
		}
		alphabet_size += automata[i]->local_alphabet_count;
	}
	alphabet	= malloc(sizeof(uint32_t) * alphabet_size);
	/***********************
	 * alphabet <- Union of alphabets
	 ***********************/
	int32_t new_index	= -1;
	uint32_t current_entry;
	for(i = 0; i < automata_count; i++){
		for(j = 0; j < automata[i]->local_alphabet_count; j++){
			new_index	= -1;
			bool found	= false;
			current_entry	= automata[i]->local_alphabet[j];
			for(k = 0; k < alphabet_count; k++){
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
				for(k = (alphabet_count); k > (uint32_t)new_index; k--){
					alphabet[k]	= alphabet[k - 1];
				}
			}else{
				new_index	= alphabet_count;
			}
			if(!found){
				alphabet[new_index]	= current_entry;
				alphabet_count++;
			}
			//test for alphabet ordering
#if DEBUG_COMPOSITION
			for(k = 0; k < alphabet_count; k++){printf("%d.", alphabet[k]);	}printf("\n");
#endif
		}
	}
	/***********************
	 * create automaton
	 ***********************/
	automaton_automaton* composition;
	if(is_game){
		composition = automaton_automaton_create("Game", ctx, alphabet_count, alphabet, true, false);
	}else{
		composition = automaton_automaton_create("Composition", ctx, alphabet_count, alphabet, false, false);
	}
	/** get signal to automata list structure **/
	bool** signal_to_automata	= malloc(sizeof(bool*) * alphabet_count);
	for(i = 0; i < alphabet_count; i++){
		bool* automata_having_signal	= malloc(sizeof(bool) * automata_count);
		for(j = 0; j < automata_count; j++){
			automata_having_signal[j]	= false;
			for(k = 0; k < automata[j]->local_alphabet_count; k++){
				if(automata[j]->local_alphabet[k] == alphabet[i]){
					automata_having_signal[j]		= true;
					break;
				}
			}
		}
		signal_to_automata[i]	= automata_having_signal;
	}
	/** create key tree **/
	automaton_composite_tree* tree	= automaton_composite_tree_create(automata_count);
	/** frontier bucket list **/
	automaton_bucket_list* bucket_list	= automaton_bucket_list_create(BUCKET_SIZE);
	/** get current state transitions list harness **/
	uint32_t max_degree_sum		= 0;
	uint32_t max_signals_count	= 0;
	/***********************
	 * compute max_signals_count
	 ***********************/
	for(i = 0; i < automata_count; i++){
		max_degree_sum	+= (automata[i]->max_out_degree) * (automata[i]->max_concurrent_degree);
		if(automata[i]->local_alphabet_count > max_signals_count)
			max_signals_count	= automata[i]->local_alphabet_count;
	}
	max_degree_sum							*= max_degree_sum; //check this boundary
	uint32_t* asynch_partial_states			= malloc(sizeof(uint32_t) * automata_count * max_degree_sum);
	bool* asynch_partial_set_states			= malloc(sizeof(bool) * automata_count * max_degree_sum);
	automaton_transition** pending_asynch	= malloc(sizeof(automaton_transition*) * max_degree_sum);
	uint32_t pending_asynch_count			= 0;
	uint32_t*	partial_states			= malloc(sizeof(uint32_t) * automata_count * max_degree_sum);
	bool*	partial_set_states		= malloc(sizeof(bool) * automata_count * max_degree_sum);
	automaton_transition** pending		= malloc(sizeof(automaton_transition*) * max_degree_sum);
	uint32_t pending_count				= 0;
	uint32_t*	processed_partial_states= malloc(sizeof(uint32_t) * automata_count * max_degree_sum);
	bool*	processed_partial_set_states= malloc(sizeof(bool) * automata_count * max_degree_sum);
	automaton_transition** processed	= malloc(sizeof(automaton_transition*) * max_degree_sum);
	uint32_t processed_count			= 0;
	uint32_t current_out_degree			= 0;
	uint32_t current_other_out_degree	= 0;
	automaton_transition* current_transition		= NULL;
	automaton_transition* current_other_transition	= NULL;
	uint32_t* current_to_state			= malloc(sizeof(int32_t) * automata_count);
	bool* current_to_set_state				= malloc(sizeof(bool) * automata_count);
	uint32_t* current_merged_to_state		= malloc(sizeof(int32_t) * automata_count);
	bool* current_merged_to_set_state		= malloc(sizeof(bool) * automata_count);
	uint32_t* current_from_state			= malloc(sizeof(int32_t) * automata_count);
	bool* current_from_set_state		= malloc(sizeof(bool) * automata_count);
	/** compose transition relation **/
	uint32_t frontier_size			= LIST_INITIAL_SIZE;//TODO: check this number is fixed
	uint32_t frontier_count			= 1;
	uint32_t* frontier				= malloc(sizeof(uint32_t) * automata_count * frontier_size);
	uint32_t* composite_frontier	= malloc(sizeof(uint32_t) * frontier_size);
	uint32_t* current_state			= malloc(sizeof(uint32_t) * automata_count);
	bool* current_set_state			= malloc(sizeof(bool) * automata_count);
	uint32_t from_state;
	uint32_t to_state;
	bool overlaps;
	bool shared_equals;
	uint32_t* signals_union			= malloc(sizeof(uint32_t) * max_degree_sum);
	uint32_t signals_union_count	= 0;
	uint32_t* current_local_alphabet;
	uint32_t current_local_alphabet_count;
	uint32_t* current_overlapping_signals		= malloc(sizeof(uint32_t) * max_signals_count);
	uint32_t current_overlapping_signals_count	= 0;
	uint32_t* current_other_overlapping_signals		= malloc(sizeof(uint32_t) * max_signals_count);
	uint32_t current_other_overlapping_signals_count	= 0;

	int32_t last_char	= -1;
	signal_t current_signal, current_other_signal;
	long int found_hits	= 0;
	long int found_misses	= 0;
	//set initial state
	for(i = 0; i < automata_count; i++){
		frontier[i]	= automata[i]->initial_states[0];
	}
	uint32_t composite_initial_state	= automaton_composite_tree_get_key(tree, frontier);
	composite_frontier[0]				= composite_initial_state;
	automaton_bucket_add_entry(bucket_list, composite_initial_state);
	automaton_automaton_add_initial_state(composition, composite_initial_state);
	uint32_t fluent_count				= ctx->global_fluents_count;
	uint32_t liveness_valuations_count	= ctx->liveness_valuations_count;
	/***********************
	 * set initial state valuation
	 ***********************/
	if(is_game){
		uint32_t fluent_index;
		for(i = 0; i < fluent_count; i++){
			fluent_index	= GET_STATE_FLUENT_INDEX(fluent_count, 0, i);
			//TODO:we are assuming fluents start as false
			CLEAR_FLUENT_BIT(composition->valuations, fluent_index);
		}
		for(i = 0; i < liveness_valuations_count; i++){
			fluent_index	= GET_STATE_FLUENT_INDEX(liveness_valuations_count, 0, i);
			//TODO:we are assuming fluents start as false
			CLEAR_FLUENT_BIT(composition->liveness_valuations, fluent_index);
		}
	}
	/***********************
	 * POP from frontier
	 ***********************/
	while(frontier_count > 0){
		from_state	= composite_frontier[frontier_count - 1];
		automaton_bucket_remove_entry(bucket_list, from_state);
		//check if frontier state was already decomposed
		if(composition->out_degree[from_state] > 0){
			frontier_count--;
			continue;
		}
		//pop a state
#if DEBUG_COMPOSITION
		printf("<POP frontier>[");
#endif
		for(i = 0; i < automata_count; i++){
			current_state[i] 		= frontier[(frontier_count - 1) * automata_count + i];
			current_out_degree		= automata[i]->out_degree[current_state[i]];
#if DEBUG_COMPOSITION
			printf("%d,", current_state[i]);
#endif
			current_from_state[i]	= current_state[i];
		}
#if DEBUG_COMPOSITION
		printf("]\n");
#endif
		frontier_count--;
		//see if the transition needs to be added
		/***********************
		 * add pending transitions from outgoing transitions in current_state
		 ***********************/
		for(i = 0; i < automata_count; i++){
			current_out_degree		= automata[i]->out_degree[current_state[i]];
			for(j = 0; j < current_out_degree; j++){
				//add new transition to p_sigs and partial_state
				automaton_transition* starting_transition					= automaton_transition_clone(&(automata[i]->transitions[current_state[i]][j]));
				starting_transition->state_from								= from_state;
				for(k = 0; k < automata_count; k++){
					if(k == i){
						partial_states[pending_count * automata_count + k]	= starting_transition->state_to;
						partial_set_states[pending_count * automata_count + k]	= true;
					}else{
						partial_states[pending_count * automata_count + k]	= current_state[k];
						partial_set_states[pending_count * automata_count + k]	= false;
					}
				}
				pending[pending_count++]			= starting_transition;
#if DEBUG_COMPOSITION
				printf("\t<PUSH pending>[");
				for(m = 0; m < automata_count; m++){
						printf("%d,", current_from_state[m]);
				}
				printf("]");
				printf("{");
				for(m = 0; m < pending[pending_count-1]->signals_count; m++){
						printf("%s,", ctx->global_alphabet->list[pending[pending_count-1]->signals[m]].name);
				}
				printf("}->");
				printf("[");
				for(m = 0; m < automata_count; m++){
					if(!partial_set_states[(pending_count-1) * automata_count + m])
						printf("%d,", partial_states[(pending_count-1) * automata_count + m]);
					else
						printf("%d^,", partial_states[(pending_count-1) * automata_count + m]);
				}
				printf("]\n");
#endif
				if(pending_count >= max_degree_sum){
					printf("[FATAL ERROR] WRONG BOUNDARY AT PENDING COUNT\n");
					exit(-1);
				}
			}
		}
		for(k = 0; k < automata_count; k++){
#if DEBUG_COMPOSITION
			printf("WILL EVALUATE AGAINST %dth automaton\n", k);
#endif
			/***********************
			 * POP from pending transitions
			 ***********************/
			while(pending_count > 0){

				for(m = 0; m < automata_count; m++){
					current_to_state[m]			= partial_states[(pending_count-1) * automata_count + m];
					current_to_set_state[m]		= partial_set_states[(pending_count-1) * automata_count + m];
				}
				current_transition				= pending[pending_count-1];
				pending_count--;
#if DEBUG_COMPOSITION
				printf("\t<POP pending>[");
				for(m = 0; m < automata_count; m++){
						printf("%d,", current_from_state[m]);
				}
				printf("]");
				printf("{");
				for(m = 0; m < pending[pending_count]->signals_count; m++){
						printf("%s,", ctx->global_alphabet->list[pending[pending_count]->signals[m]].name);
				}
				printf("}->");
				printf("[");
				for(m = 0; m < automata_count; m++){
					if(!current_to_set_state[m])
						printf("%d,", partial_states[(pending_count) * automata_count + m]);
					else
						printf("%d^,", partial_states[(pending_count) * automata_count + m]);
				}
				printf("]\n");
#endif

				current_local_alphabet			= automata[k]->local_alphabet;
				current_local_alphabet_count	= automata[k]->local_alphabet_count;
				overlaps						= false;
				/***********************
				 * check if current transition synchronizes with other automata
				 ***********************/
				for(m = 0; m < current_transition->signals_count; m++){
					for(n = 0; n < current_local_alphabet_count; n++){
						signal_t sig	= m < FIXED_SIGNALS_COUNT ? current_transition->signals[m] : current_transition->other_signals[m-FIXED_SIGNALS_COUNT];
						if(sig == current_local_alphabet[n]){
							overlaps		= true;
							break;
						}
					}
					if(overlaps) break;
				}
				//if does not overlap pass forward
				if(!overlaps){
					/***********************
					 * for every other asynch transition add posible combination if applicable
					 ***********************/
					if(type == CONCURRENT){
						if(pending_asynch_count > 0){
#if DEBUG_COMPOSITION
							printf("\t<PUSH asynch>(no overlap*) [");
							for(n = 0; n < automata_count; n++){
									printf("%d,", current_from_state[n]);
							}
							printf("]{");
#endif
							int32_t m;
							for(m = (pending_asynch_count - 1); m >= 0; m--){
								/***********************
								 * merge to state and signals
								 ***********************/
								for(n = 0; n < automata_count; n++){
									if(current_to_set_state[n]){
										current_merged_to_set_state[n]	= true;
										current_merged_to_state[n]		= current_to_state[n];
									}else{
										current_merged_to_set_state[n]	= asynch_partial_set_states[m * automata_count + n];
										current_merged_to_state[n]		= asynch_partial_states[m * automata_count + n];
									}
								}
								automaton_transition* merged_other_transition	= pending_asynch[m];
								automaton_transition* merged_transition	= automaton_transition_create(current_transition->state_from, automaton_composite_tree_get_key(tree, current_merged_to_state));
								automaton_automata_add_ordered_union_of_signals(ctx, signals_union, &signals_union_count, current_transition, merged_other_transition, merged_transition);
								pending_asynch[pending_asynch_count]	= merged_transition;
								for(n = 0; n < automata_count; n++){
									asynch_partial_states[pending_asynch_count * automata_count + n]	= current_merged_to_state[n];
									asynch_partial_set_states[pending_asynch_count * automata_count + n]	= current_merged_to_set_state[n];
#if DEBUG_COMPOSITION
									printf("%d,", asynch_partial_states[pending_asynch_count * automata_count + n]);
#endif
								}
#if DEBUG_COMPOSITION
								printf("]\n");
#endif
								pending_asynch_count++;
								if(pending_asynch_count >= max_degree_sum){
									printf("[FATAL ERROR] WRONG BOUNDARY AT PENDING ASYNCH COUNT\n");
									exit(-1);
								}
							}
						}
					}
					pending_asynch[pending_asynch_count]	= current_transition;
#if DEBUG_COMPOSITION
					printf("\t<PUSH asynch>(no overlap) [");
					for(n = 0; n < automata_count; n++){
							printf("%d,", current_from_state[n]);
					}
					printf("]{");
					for(n = 0; n < current_transition->signals_count; n++){
							printf("%s,", ctx->global_alphabet->list[current_transition->signals[n]].name);
					}
					printf("}->[");
#endif
					for(n = 0; n < automata_count; n++){
						asynch_partial_states[pending_asynch_count * automata_count + n]	= current_to_state[n];
						asynch_partial_set_states[pending_asynch_count * automata_count + n]	= current_to_set_state[n];
#if DEBUG_COMPOSITION
						if(!current_to_set_state[n])
							printf("%d,", asynch_partial_states[pending_asynch_count * automata_count + n]);
						else
							printf("%d^,", asynch_partial_states[pending_asynch_count * automata_count + n]);
#endif
					}
#if DEBUG_COMPOSITION
					printf("]\n");
#endif
					pending_asynch_count++;
					if(pending_asynch_count >= max_degree_sum){
						printf("[FATAL ERROR] WRONG BOUNDARY AT PENDING ASYNCH COUNT\n");
						exit(-1);
					}
					continue;
				}//ENDIF !overlaps
				/***********************
				 * if it synchronizes continues
				 ***********************/
				current_other_out_degree	= automata[k]->out_degree[current_state[k]];
				for(m = 0; m < current_other_out_degree; m++){
					current_other_transition	= &(automata[k]->transitions[current_state[k]][m]);
					current_overlapping_signals_count		= 0;
					current_other_overlapping_signals_count	= 0;
					//check if shared signals are equal
					shared_equals			= false;
					signal_t sig;
					for(n = 0; n < current_transition->signals_count; n++){
						for(o = 0; o < automata[k]->local_alphabet_count; o++){
							sig	= n < FIXED_SIGNALS_COUNT ? current_transition->signals[n] : current_transition->other_signals[n-FIXED_SIGNALS_COUNT];
							if(sig == automata[k]->local_alphabet[o]){
								current_overlapping_signals[current_overlapping_signals_count++] = sig;
							}
						}
					}
					for(n = 0; n < current_other_transition->signals_count; n++){
						for(o = 0; o < current_local_alphabet_count; o++){
							sig	= n < FIXED_SIGNALS_COUNT ? current_other_transition->signals[n] : current_other_transition->other_signals[n-FIXED_SIGNALS_COUNT];
							if(sig == current_local_alphabet[o]){
								current_other_overlapping_signals[current_other_overlapping_signals_count++] = sig;
							}
						}
					}
					/***********************
					 * check if shared alphabet is equal
					 * first checks size, then proper signals
					 * (remember that local alphabets are ordered)
					 ***********************/
					shared_equals	= current_overlapping_signals_count == current_other_overlapping_signals_count;
					if(shared_equals){
						for(n = 0; n < current_other_overlapping_signals_count; n++){
							if(current_overlapping_signals[n] != current_other_overlapping_signals[n]){
								shared_equals	= false;
								break;
							}
						}
					}
					if(shared_equals){
						/***********************
						 * should add new transition to processed list
						 * first create ordered union of signals
						 ***********************/
						automaton_transition* new_transition	= automaton_transition_create(current_transition->state_from
														, current_transition->state_to);
						automaton_automata_add_ordered_union_of_signals(ctx, signals_union, &signals_union_count, current_transition, current_other_transition, new_transition);
						processed[processed_count]	= new_transition;
#if DEBUG_COMPOSITION
						printf("\t<PUSH processed> [");
						for(n = 0; n < automata_count; n++){
								printf("%d,", current_from_state[n]);
						}
						printf("]{");
						for(n = 0; n < signals_union_count; n++){
								printf("%s,", ctx->global_alphabet->list[signals_union[n]].name);
						}
						printf("}->[");
#endif
						for(n = 0; n < automata_count; n++){
							if(n == k){
								processed_partial_states[processed_count * automata_count + n]	= current_other_transition->state_to;
								processed_partial_set_states[processed_count * automata_count + n]	= true;
#if DEBUG_COMPOSITION
								printf("%d*,", processed_partial_states[processed_count * automata_count + n]);
#endif
							}else{
								processed_partial_states[processed_count * automata_count + n]	= current_to_state[n];
								processed_partial_set_states[processed_count * automata_count + n]	= current_to_set_state[n];
#if DEBUG_COMPOSITION
								if(!current_to_set_state[n])
									printf("%d,", processed_partial_states[processed_count * automata_count + n]);
								else
									printf("%d^,", processed_partial_states[processed_count * automata_count + n]);
#endif
							}

						}
#if DEBUG_COMPOSITION
						printf("]\n");
#endif
						processed_count++;
					}//ENDIF shared_equals
				}
				automaton_transition_destroy(current_transition, true);
			}//ENDWHILE (pending_count > 0)
			/***********************
			 * move processed to pending to
			 * handle next automaton transitions
			 ***********************/
#if DEBUG_COMPOSITION
			printf("<MOV processed>pending>");
#endif
			for(i = 0; i < processed_count; i++){
				pending[i]	= processed[i];
#if DEBUG_COMPOSITION
				printf("->[");
#endif
				for(n = 0; n < automata_count; n++){
					partial_states[i * automata_count + n]	= processed_partial_states[i * automata_count + n];
					partial_set_states[i * automata_count + n]	= processed_partial_set_states[i * automata_count + n];
#if DEBUG_COMPOSITION
					printf("%d,",processed_partial_states[i * automata_count + n]);
#endif
				}
#if DEBUG_COMPOSITION
				printf("]");
#endif
			}
			pending_count	= processed_count;
			processed_count	= 0;
#if DEBUG_COMPOSITION
			printf("\n");
#endif
			/***********************
			 * move asynch to pending
			 * handle next automaton transitions
			 ***********************/
#if DEBUG_COMPOSITION
			printf("<MOV asynch>pending>");
#endif
			uint32_t asynch_index;
			for(i = 0; i < pending_asynch_count; i++){
				asynch_index	= pending_count;
				pending[asynch_index]	= pending_asynch[i];
#if DEBUG_COMPOSITION
				printf("->[");
#endif
				for(n = 0; n < automata_count; n++){
					partial_states[asynch_index * automata_count + n]	= asynch_partial_states[i * automata_count + n];
					partial_set_states[asynch_index * automata_count + n]	= asynch_partial_set_states[i * automata_count + n];
#if DEBUG_COMPOSITION
					printf("%d,",asynch_partial_states[i * automata_count + n]);
#endif
				}
#if DEBUG_COMPOSITION
				printf("]");
#endif
				pending_count++;
			}
			pending_asynch_count	= 0;
#if DEBUG_COMPOSITION
			printf("\n");
#endif
		}//ENDFOR automata encapsulating pending

		/***********************
		 * add processed transitions
		 * from pending list
		 ***********************/
		while(pending_count > 0){
			pending_count--;
			for(n = 0; n < automata_count; n++){
				current_to_state[n]	= partial_states[pending_count * automata_count + n];
				current_to_set_state[n]	= partial_set_states[pending_count * automata_count + n];
			}

			current_transition				= pending[pending_count];
			uint32_t composite_to			= automaton_composite_tree_get_key(tree, current_to_state);
			current_transition->state_to	= composite_to;
#if DEBUG_COMPOSITION
			printf("<TRANS add>[%d]{", current_transition->state_from);
			for(n = 0; n < current_transition->signals_count; n++){
				printf("%s,", ctx->global_alphabet->list[current_transition->signals[n]].name);
				if(n > 0)
					printf(".");
			}
			printf("}->[");
			for(n = 0; n < automata_count; n++){
				printf("%d,", current_to_state[n]);
			}
			printf("](%d)\n", composite_to);
#endif
			automaton_automaton_add_transition(composition, current_transition);
			/***********************
			 * set state valuation
			 ***********************/
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
						//
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
							current_valuation &= TEST_FLUENT_BIT(automata[j]->liveness_valuations, fluent_index);
						}
						if(!current_valuation)break;
					}
					fluent_index	= GET_STATE_FLUENT_INDEX(fluent_count, composite_to, i);
					if(current_valuation){
						SET_FLUENT_BIT(composition->liveness_valuations, fluent_index);
					}else{
						CLEAR_FLUENT_BIT(composition->liveness_valuations, fluent_index);
					}
				}
			}
			transitions_added_count++;
#if PRINT_PARTIAL_COMPOSITION
			if((transitions_added_count % 300000) == 0){
				/* here, do your time-consuming job */
				printf("Partial Composition has [%09d] states and [%09d] transitions, frontier is of size [%09d] running for [%08f]s\n", tree->max_value, composition->transitions_composite_count, frontier_count, (double)(clock() - begin) / CLOCKS_PER_SEC);

			}
#endif
			automaton_transition_destroy(current_transition, true);
			/***********************
			 * expand frontier
			 ***********************/
			bool found = automaton_bucket_has_entry(bucket_list, composite_to);
			if(found){found_hits++;}else{found_misses++;}
			if(!found){
				composite_frontier[frontier_count]	= composite_to;
				automaton_bucket_add_entry(bucket_list, composite_to);
#if DEBUG_COMPOSITION
				printf("<PUSH frontier> [");
#endif
				for(n = 0; n < automata_count; n++){
#if DEBUG_COMPOSITION
					printf("%d,", current_to_state[n]);
#endif

					frontier[frontier_count * automata_count + n]	= current_to_state[n];
				}
#if DEBUG_COMPOSITION
				printf("]\n");
#endif
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
	printf("TOTAL Composition has [%09d] states and [%09d] transitions run for [%08f] KEY ACCESS.: [Misses:%li,hits:%li]\n", tree->max_value, composition->transitions_composite_count, (double)(clock() - begin) / CLOCKS_PER_SEC, found_misses, found_hits);
	/***********************
	 * CLEANUP
	 ***********************/
	automaton_bucket_destroy(bucket_list); bucket_list	= NULL;
	free(asynch_partial_states); asynch_partial_states = NULL;
	free(asynch_partial_set_states); asynch_partial_set_states = NULL;
	free(pending_asynch);	pending_asynch	= NULL;
	free(composite_frontier);	composite_frontier	= NULL;
	free(signals_union);  signals_union	= NULL;
	free(current_to_state); current_to_state	= NULL;
	free(current_to_set_state); current_to_set_state	= NULL;
	free(current_merged_to_state); current_merged_to_state	= NULL;
	free(current_merged_to_set_state); current_merged_to_set_state	= NULL;
	free(current_from_state); current_from_state	= NULL;
	free(current_from_set_state); current_from_set_state	= NULL;
	free(current_overlapping_signals); current_overlapping_signals	= NULL;
	free(current_other_overlapping_signals); current_other_overlapping_signals	= NULL;
	for(i = 0; i < alphabet_count; i++){ 
		if(signal_to_automata != NULL){
			free(signal_to_automata[i]);
			signal_to_automata[i]	= NULL;
		}
	}
	free(processed_partial_states); processed_partial_states	= NULL;
	free(processed_partial_set_states); processed_partial_set_states	= NULL;
	free(partial_states); partial_states	= NULL;
	free(partial_set_states); partial_set_states	= NULL;
	for(i = 0; i < pending_count; i++) automaton_transition_destroy(pending[i], true);
	free(pending); pending	= NULL;
	for(i = 0; i < processed_count; i++) automaton_transition_destroy(processed[i], true);
	free(processed); processed	= NULL;
	free(signal_to_automata);	signal_to_automata	= NULL;
	automaton_composite_tree_destroy(tree);tree = NULL;
	free(frontier); frontier = NULL;
	free(current_state); current_state = NULL;
	free(current_set_state); current_set_state = NULL;
	free(alphabet);	alphabet	= NULL;
	return composition;
}
bool automaton_automaton_check_invariant(automaton_automaton* current_automaton){ return false; }
bool automaton_automaton_update_valuation(automaton_automaton* current_automaton){ return false; }
void automaton_automaton_minimize(automaton_automaton* current_automaton){ return;}
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
