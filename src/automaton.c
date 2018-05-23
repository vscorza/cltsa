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
	target->signals					= malloc(sizeof(uint32_t) * target->signals_count);
	uint32_t i;
	for(i = 0; i < target->signals_count; i++){
		target->signals[i]			= source->signals[i];	
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
}
automaton_automaton* automaton_automaton_clone(automaton_automaton* source){
	automaton_automaton* copy		= malloc(sizeof(automaton_automaton));
	automaton_automaton_copy(source, copy);
	return copy;
}
void automaton_automaton_copy(automaton_automaton* source, automaton_automaton* target){
	uint32_t i, j, inDegree, outDegree;
	target->name					= malloc(sizeof(char) * (strlen(source->name) + 1));
	strcpy(target->name, source->name);
	target->context					= source->context;
	target->local_alphabet_count	= source->local_alphabet_count;
	target->local_alphabet			= malloc(sizeof(uint32_t) * target->local_alphabet_count);
	for(i = 0; i < target->local_alphabet_count; i++){ target->local_alphabet[i]	= source->local_alphabet[i]; }
	target->states_count			= source->states_count;
	target->transitions_count		= source->transitions_count;
	target->transitions_size		= source->transitions_size;
	target->in_degree				= malloc(sizeof(uint32_t) * target->transitions_size);
	target->max_out_degree			= source->max_out_degree;
	target->max_concurrent_degree	= source->max_concurrent_degree;
	target->out_degree				= malloc(sizeof(uint32_t) * target->transitions_size);
	target->transitions				= malloc(sizeof(automaton_transition*) * target->transitions_size);
	target->inverted_transitions	= malloc(sizeof(automaton_transition*) * target->transitions_size);
	
	for(i = 0; i < target->transitions_count; i++){
		target->in_degree[i]	= source->in_degree[i];
		inDegree				= target->in_degree[i];
		if(inDegree > 0){
			target->inverted_transitions	= malloc(sizeof(automaton_transition) * inDegree);
			for(j = 0; j < inDegree; j++){
				automaton_transition_copy(&(source->inverted_transitions[i][j]), &(target->inverted_transitions[i][j]));
			}
		}
		target->out_degree[i]	= source->out_degree[i];
		outDegree				= target->out_degree[i];
		if(outDegree > 0){
			target->transitions	= malloc(sizeof(automaton_transition) * outDegree);
			for(j = 0; j < outDegree; j++){
				automaton_transition_copy(&(source->transitions[i][j]), &(target->transitions[i][j]));
			}
		}
	}
	target->initial_states_count	= source->initial_states_count;
	target->initial_states			= malloc(sizeof(uint32_t) * target->initial_states_count);
	for(i = 0; i < target->initial_states_count; i++){
		target->initial_states[i]	= source->initial_states[i];
	}
	target->valuations_count		= source->valuations_count;
	target->valuations_size			= source->valuations_size;
	target->valuations				= malloc(sizeof(automaton_valuation) * target->valuations_size);
	for(i = 0; i < target->valuations_count; i++){
		automaton_valuation_copy(&(source->valuations[i]), &(target->valuations[i]));
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
		printf("%s%s", ctx->global_alphabet->list[transition->signals[i]].name, (i < (transition->signals_count -1)? "," : ""));
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
	if(print_valuations){
		printf("%sValuations:\n", prefix2);
		for(i = 0; i < current_automaton->valuations_count; i ++){
			automaton_valuation_print(&(current_automaton->valuations[i]), ctx, prefix2, "\n");
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
		for(j = 0; j < current_automaton->out_degree[i]; j++){
			current_transition	= &(current_automaton->transitions[i][j]);
			if(j == 0){fprintf(f, "S_%d = (", current_transition->state_from);}
			if(current_transition->signals_count > 1)fprintf(f, "<");
			for(k = 0; k < current_transition->signals_count; k++){
				fprintf(f,"%s%s", ctx->global_alphabet->list[current_transition->signals[k]].name, (k < (current_transition->signals_count -1)? "," : ""));
			}
			if(current_transition->signals_count > 1)fprintf(f, ">");
			fprintf(f,"-> S_%d", current_transition->state_to);
			if(j < (current_automaton->out_degree[i] - 1))fprintf(f, "|");
			if(j == (current_automaton->out_degree[i] - 1))fprintf(f, ")");
		}
		if(i == (current_automaton->transitions_count - 1)){fprintf(f, ".");}else{fprintf(f, ",");}
	}
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
automaton_automata_context* automaton_automata_context_create(char* name, automaton_alphabet* alphabet, uint32_t fluents_count, automaton_fluent** fluents){
	automaton_automata_context* ctx		= malloc(sizeof(automaton_automata_context));
	automaton_automata_context_initialize(ctx, name, alphabet, fluents_count, fluents);
	return ctx;
}
automaton_automaton* automaton_automaton_create(char* name, automaton_automata_context* ctx, uint32_t local_alphabet_count, uint32_t* local_alphabet){
	automaton_automaton* automaton		= malloc(sizeof(automaton_automaton));
	automaton_automaton_initialize(automaton, name, ctx, local_alphabet_count, local_alphabet);
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
	transition->signals		= NULL;
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
void automaton_automata_context_initialize(automaton_automata_context* ctx, char* name, automaton_alphabet* alphabet, uint32_t fluents_count, automaton_fluent** fluents){
	ctx->name					= malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(ctx->name, name);
	ctx->global_alphabet		= automaton_alphabet_clone(alphabet);
	ctx->global_fluents_count	= fluents_count;
	ctx->global_fluents			= malloc(sizeof(automaton_fluent) * ctx->global_fluents_count);
	uint32_t i;
	for(i = 0; i < ctx->global_fluents_count; i++){
		automaton_fluent_copy(fluents[i], &(ctx->global_fluents[i]));
	}
}
void automaton_automaton_initialize(automaton_automaton* automaton, char* name, automaton_automata_context* ctx, uint32_t local_alphabet_count, uint32_t* local_alphabet){
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
	automaton->states_count				= 0;
	automaton->transitions_size			= LIST_INITIAL_SIZE;
	automaton->transitions_count		= 0;
	automaton->max_out_degree			= 0;
	automaton->max_concurrent_degree	= 0;
	automaton->out_degree				= malloc(sizeof(uint32_t) * automaton->transitions_size);
	automaton->in_degree				= malloc(sizeof(uint32_t) * automaton->transitions_size);
	automaton->transitions				= malloc(sizeof(automaton_transition*) * automaton->transitions_size);
	automaton->inverted_transitions		= malloc(sizeof(automaton_transition*) * automaton->transitions_size);
	for(i = 0; i < automaton->transitions_size; i++){
		automaton->out_degree[i]			= 0;
		automaton->in_degree[i]				= 0;
		automaton->transitions[i]			= NULL;
		automaton->inverted_transitions[i]	= NULL;
	}
	automaton->initial_states_count		= 0;
	automaton->valuations_size			= LIST_INITIAL_SIZE;
	automaton->valuations_count			= 0;
	automaton->valuations 				= malloc(sizeof(automaton_valuation) * automaton->valuations_count);
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
	free(transition->signals);
	transition->signals			= NULL;
	transition->state_from		= 0;
	transition->state_to		= 0;
	transition->signals_count	= 0;
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
	for(i = 0; i < automaton->transitions_count; i++){
		for(j = 0; j < automaton->out_degree[i]; j++){
			automaton_transition_destroy(&(automaton->transitions[i][j]), false);
		}
		for(j = 0; j < automaton->in_degree[i]; j++){
			automaton_transition_destroy(&(automaton->inverted_transitions[i][j]), false);
		}
		free(automaton->transitions[i]);
		free(automaton->inverted_transitions[i]);
	}
	free(automaton->out_degree);
	free(automaton->transitions);
	free(automaton->in_degree);
	free(automaton->inverted_transitions);
	free(automaton->initial_states);
	for(i = 0; i < automaton->valuations_count; i++){
		automaton_valuation_destroy(&(automaton->valuations[i]));
	}
	free(automaton->valuations);
	automaton->name			= NULL;
	automaton->context		= NULL;
	automaton->local_alphabet_count	= 0;
	automaton->local_alphabet		= NULL;
	automaton->states_count			= 0;
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
	automaton->valuations_count		= 0;
	automaton->valuations			= NULL;
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
			if(signal_ordered_index >= i){
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
		for(i = (alphabet->count) - 1; i > signal_ordered_index; i--){
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
uint32_t automaton_alphabet_get_signal_index(automaton_alphabet* alphabet, automaton_signal_event* signal_event){
	uint32_t i;
	for(i = 0; i < alphabet->size; i++){
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
		if(transition->signals[i] == signal_index)
			return true;
	}
	return false;
}

bool automaton_transition_add_signal_event(automaton_transition* transition, automaton_automata_context* ctx, automaton_signal_event* signal_event){ 
	if(automaton_transition_has_signal_event(transition, ctx, signal_event))
		return true;
	//signals should preserve order within the transition
	int32_t i, signal_ordered_index	= -1;
	int32_t old_count	= (int32_t)transition->signals_count;
	uint32_t signal_index	= automaton_alphabet_get_signal_index(ctx->global_alphabet, signal_event);
	for(i = 0; i < old_count; i++){
		if(signal_index < transition->signals[i]){
			signal_ordered_index	= i;
			break;
		}
	}
	uint32_t* new_signals	= malloc(sizeof(uint32_t) * (old_count + 1));
	for(i = 0; i < old_count; i++){
		if(signal_ordered_index >= i){
			new_signals[i+1]	= transition->signals[i];
		}else{
			new_signals[i]	= transition->signals[i];
		}
	}
	free(transition->signals);
	if(signal_ordered_index == -1){
		signal_ordered_index	= transition->signals_count;
	}
	new_signals[signal_ordered_index]	= signal_index;
	transition->signals	= new_signals;
	transition->signals_count++;
	return false;
}
/** FLUENT **/
bool automaton_fluent_has_starting_signal(automaton_fluent* fluent, automaton_alphabet* alphabet, automaton_signal_event* signal_event){
	uint32_t i;
	uint32_t signal_index	= automaton_alphabet_get_signal_index(alphabet, signal_event);
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
	uint32_t signal_index	= automaton_alphabet_get_signal_index(alphabet, signal_event);
	uint32_t* new_signals	= malloc(sizeof(uint32_t) * (fluent->starting_signals_count + 1));
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
	uint32_t signal_index	= automaton_alphabet_get_signal_index(alphabet, signal_event);
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
	uint32_t signal_index	= automaton_alphabet_get_signal_index(alphabet, signal_event);
	uint32_t* new_signals	= malloc(sizeof(uint32_t) * (fluent->ending_signals_count + 1));
	for(i = 0; i < fluent->ending_signals_count; i++)
		new_signals[i]	= fluent->ending_signals[i];
	free(fluent->ending_signals);
	new_signals[fluent->ending_signals_count]	= signal_index;
	fluent->ending_signals	= new_signals;
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
	bool found	= false;
	for(i = 0; i < out_degree; i++){
		if(current_transitions[i].signals_count != transition->signals_count)
			continue;
		if(current_transitions[i].state_to == to_state){
			for(j = 0; j < transition->signals_count; j++){
				for(k = 0; k < current_transitions[i].signals_count; k++){
					if(transition->signals[j] == current_transitions[i].signals[k]){
						found	= true;
						break;
					}
				}
				if(found)break;
			}
			if(!found)
				return false;
		}
	}
	return found;
}
void automaton_automaton_resize_to_state(automaton_automaton* current_automaton, uint32_t state){
	if(state < current_automaton->transitions_size) return;
	uint32_t i,j;
	uint32_t old_size	= current_automaton->transitions_size;
	uint32_t next_size	= old_size;
	while(state >= next_size) next_size *= LIST_INCREASE_FACTOR;
	uint32_t* next_out_degree			= malloc(sizeof(uint32_t) * next_size);
	uint32_t* next_in_degree			= malloc(sizeof(uint32_t) * next_size);
	automaton_transition** next_trans	= malloc(sizeof(automaton_transition*) * next_size);
	automaton_transition** next_inv_trans= malloc(sizeof(automaton_transition*) * next_size);
	for(i = 0; i < current_automaton->transitions_count; i++){
		next_out_degree[i]				= current_automaton->out_degree[i];
		next_in_degree[i]				= current_automaton->in_degree[i];
		next_trans[i]					= malloc(sizeof(automaton_transition) * next_out_degree[i]);
		for(j = 0; j < next_out_degree[i]; j++){ 
			automaton_transition_copy(&(current_automaton->transitions[i][j]), &(next_trans[i][j]));
			automaton_transition_destroy(&(current_automaton->transitions[i][j]), false);
		}
		next_inv_trans[i]				= malloc(sizeof(automaton_transition) * next_in_degree[i]);
		for(j = 0; j < next_in_degree[i]; j++){ 
			automaton_transition_copy(&(current_automaton->inverted_transitions[i][j]), &(next_inv_trans[i][j]));
			automaton_transition_destroy(&(current_automaton->inverted_transitions[i][j]), false);
		}
	}
	free(current_automaton->out_degree);
	free(current_automaton->in_degree);
	free(current_automaton->transitions);
	free(current_automaton->inverted_transitions);

	current_automaton->out_degree	= next_out_degree;
	current_automaton->in_degree	= next_in_degree;
	current_automaton->transitions	= next_trans;
	current_automaton->inverted_transitions	= next_inv_trans;
	current_automaton->transitions_size		= next_size;
	for(i = old_size; i < current_automaton->transitions_size; i++){
		current_automaton->out_degree[i]			= 0;
		current_automaton->in_degree[i]				= 0;
		current_automaton->transitions[i]			= NULL;
		current_automaton->inverted_transitions[i]	= NULL;
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
	automaton_transition* new_out	= malloc(sizeof(automaton_transition) * (old_out_degree + 1));
	automaton_transition* new_in	= malloc(sizeof(automaton_transition) * (old_in_degree + 1));
	for(i = 0; i < old_out_degree; i++){
		automaton_transition_copy(&(current_automaton->transitions[from_state][i]), &(new_out[i]));
		automaton_transition_destroy(&(current_automaton->transitions[from_state][i]), false);
	}
	for(i = 0; i < old_in_degree; i++){
		automaton_transition_copy(&(current_automaton->inverted_transitions[to_state][i]), &(new_in[i]));
		automaton_transition_destroy(&(current_automaton->inverted_transitions[to_state][i]), false);
	}
	automaton_transition_copy(transition, &(new_in[old_in_degree]));
	automaton_transition_copy(transition, &(new_out[old_out_degree]));
	if(current_automaton->transitions[from_state] != NULL)
		free(current_automaton->transitions[from_state]);
	if(current_automaton->inverted_transitions[to_state] != NULL)
		free(current_automaton->inverted_transitions[to_state]);
	current_automaton->transitions[from_state]			= new_out;
	current_automaton->inverted_transitions[to_state]	= new_in;
	current_automaton->out_degree[from_state]++;
	if(current_automaton->max_out_degree < current_automaton->out_degree[from_state])
		current_automaton->max_out_degree	= current_automaton->out_degree[from_state];
	if(current_automaton->max_concurrent_degree < transition->signals_count)
		current_automaton->max_concurrent_degree	= transition->signals_count;
	current_automaton->in_degree[to_state]++;
	return true;
}
bool automaton_automaton_remove_transition(automaton_automaton* current_automaton, automaton_transition* transition){ return false; }

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
uint32_t automaton_automata_get_composite_state(uint32_t states_count, uint32_t* states){
	return 0;
}
automaton_automaton* automaton_automata_compose(automaton_automaton** automata, uint32_t automata_count, automaton_synchronization_type type){
	clock_t begin = clock();
	uint32_t max_frontier = 0;
	uint32_t transitions_added_count	= 0;
	uint32_t i, j, k, l, m, n, o;
	uint32_t alphabet_count, fluents_count, alphabet_size;
	uint32_t* alphabet;
	alphabet_count	= 0;
	alphabet_size	= 0;
	/** get union of alphabets **/
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
	automaton_automaton* composition= automaton_automaton_create("Composition", ctx, alphabet_count, alphabet);
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
	uint32_t current_signal, current_other_signal;
	long int found_hits	= 0;
	long int found_misses	= 0;
	//set initial state
	for(i = 0; i < automata_count; i++){
		frontier[i]	= automata[i]->initial_states[0];
	}
	uint32_t composite_initial_state	= automaton_composite_tree_get_key(tree, frontier);
	composite_frontier[0]				= composite_initial_state;
	automaton_bucket_add_entry(bucket_list, composite_initial_state);
	max_frontier						= composite_initial_state;
	automaton_automaton_add_initial_state(composition, composite_initial_state);
	//consume frontier
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
		/*if(automaton_automaton_has_state(composition, from_state)){
			continue;
		}*/
		//see if the transition needs to be added

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
				if(pending_count >= max_degree_sum){
					printf("[FATAL ERROR] WRONG BOUNDARY AT PENDING COUNT\n");
					exit(-1);
				}
			}
		}
		for(k = 0; k < automata_count; k++){
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
					printf("%d,", partial_states[(pending_count) * automata_count + m]);
				}
				printf("]\n");
#endif

				current_local_alphabet			= automata[k]->local_alphabet;
				current_local_alphabet_count	= automata[k]->local_alphabet_count;
				overlaps						= false;
				//check if current transition synchronizes with other automata
				for(m = 0; m < current_transition->signals_count; m++){
					for(n = 0; n < current_local_alphabet_count; n++){
						if(current_transition->signals[m] == current_local_alphabet[n]){
							overlaps		= true;
							break;
						}
					}
					if(overlaps) break;
				}
				//if does not overlap pass forward
				if(!overlaps){
					//for every other asynch transition add posible combination if applicable
					if(type == SYNCHRONOUS){
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
								//merge to state and signals
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
								signals_union_count	= 0;
								n = o = 0;
								while(n < current_transition->signals_count && o < merged_other_transition->signals_count){
									current_signal		= current_transition->signals[n];
									current_other_signal= merged_other_transition->signals[o];
									if(current_signal == current_other_signal){
										signals_union[signals_union_count++]	= current_signal;
										n++; o++;
									}else if(current_signal < current_other_signal){
										signals_union[signals_union_count++]	= current_signal;
										n++;
									}else{
										signals_union[signals_union_count++]	= current_other_signal;
										o++;
									}
								}
								while(n < (current_transition->signals_count)){
									signals_union[signals_union_count++]	= current_transition->signals[n++];
								}
								while(o < (merged_other_transition->signals_count)){
									signals_union[signals_union_count++]	= merged_other_transition->signals[o++];
								}
								for(n = 0; n < signals_union_count; n++){
									automaton_transition_add_signal_event(merged_transition, ctx, &(ctx->global_alphabet->list[signals_union[n]]));
								}
#if DEBUG_COMPOSITION
								for(n = 0; n < signals_union_count; n++){
										printf("%s,", ctx->global_alphabet->list[signals_union[n]].name);
								}
								printf("}->[");
#endif
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
					continue;
				}
				//if it synchronizes
				current_other_out_degree	= automata[k]->out_degree[current_state[k]];
				for(m = 0; m < current_other_out_degree; m++){
					current_other_transition	= &(automata[k]->transitions[current_state[k]][m]);
					current_overlapping_signals_count		= 0;
					current_other_overlapping_signals_count	= 0;
					//check if shared signals are equal
					shared_equals			= false;
					for(n = 0; n < current_transition->signals_count; n++){
						for(o = 0; o < automata[k]->local_alphabet_count; o++){
							if(current_transition->signals[n] == automata[k]->local_alphabet[o]){
								current_overlapping_signals[current_overlapping_signals_count++]
															= current_transition->signals[n];
							}
						}
					}
					for(n = 0; n < current_other_transition->signals_count; n++){
						for(o = 0; o < current_local_alphabet_count; o++){
							if(current_other_transition->signals[n] == current_local_alphabet[o]){
								current_other_overlapping_signals[current_other_overlapping_signals_count++]
																  = current_other_transition->signals[n];
							}
						}
					}
					shared_equals	= current_overlapping_signals_count == current_other_overlapping_signals_count;
					//alphabets are ordered
					if(shared_equals){
						for(n = 0; n < current_other_overlapping_signals_count; n++){
							if(current_overlapping_signals[n] != current_other_overlapping_signals[n]){
								shared_equals	= false;
								break;
							}
						}
					}
					if(shared_equals){
						//create ordered union of signals
						signals_union_count	= 0;
						n = o = 0;
						while(n < current_transition->signals_count && o < current_other_transition->signals_count){
							current_signal		= current_transition->signals[n];
							current_other_signal= current_other_transition->signals[o];
							if(current_signal == current_other_signal){
								signals_union[signals_union_count++]	= current_signal;
								n++; o++;
							}else if(current_signal < current_other_signal){
								signals_union[signals_union_count++]	= current_signal;
								n++;
							}else{
								signals_union[signals_union_count++]	= current_other_signal;
								o++;
							}
						}
						while(n < (current_transition->signals_count)){
							signals_union[signals_union_count++]	= current_transition->signals[n++];
						}
						while(o < (current_other_transition->signals_count)){
							signals_union[signals_union_count++]	= current_other_transition->signals[o++];
						}
						//should add
						automaton_transition* new_transition	= automaton_transition_create(current_transition->state_from
								, current_transition->state_to);
						for(n = 0; n < signals_union_count; n++){
							automaton_transition_add_signal_event(new_transition, ctx, &(ctx->global_alphabet->list[signals_union[n]]));
						}
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
								printf("%d,", processed_partial_states[processed_count * automata_count + n]);
#endif
							}

						}
#if DEBUG_COMPOSITION
						printf("]\n");
#endif
						processed_count++;
					}
				}
				automaton_transition_destroy(current_transition, true);
			}
			//move processed to pending to handle next automaton transitions
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
		}
		//move asynch to pending to handle next automaton transitions
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


		//add processed transitions
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
			transitions_added_count++;
#if PRINT_PARTIAL_COMPOSITION
			if((transitions_added_count % 300000) == 0){
				/* here, do your time-consuming job */
				printf("Partial Composition has [%09d] states and [%09d] transitions, frontier is of size [%09d] running for [%08f]s\n", tree->max_value, composition->transitions_count, frontier_count, (double)(clock() - begin) / CLOCKS_PER_SEC);

			}
#endif
			automaton_transition_destroy(current_transition, true);
			//expand frontier
			bool found = false;
			if(composition->out_degree[composite_to] > 0){
				found = true;
			}else if((max_frontier < composite_to) || automaton_bucket_has_entry(bucket_list, composite_to)){
				found	= false;
			}else{
				int32_t n2;
				uint32_t max_frontier2	= composite_to;
				for(n2 = frontier_count - 1; n2 >= 0 ; n2--){
					if(composite_frontier[n2] > max_frontier2)
						max_frontier2	= composite_frontier[n2];
					if(composite_frontier[n2] == composite_to){
						found	= true;
						break;
					}
				}
				if(!found)
					max_frontier	= max_frontier2;
			}
			if(found){found_hits++;}else{found_misses++;}
			if(!found){
				composite_frontier[frontier_count]	= composite_to;
				automaton_bucket_add_entry(bucket_list, composite_to);
				if(max_frontier < composite_to)max_frontier	= composite_to;
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
	printf("TOTAL Composition has [%09d] states and [%09d] transitions, frontier is of size [%09d] running for [%08f]s\n", tree->max_value, composition->transitions_count, frontier_count, (double)(clock() - begin) / CLOCKS_PER_SEC);
	printf("FOUND: [Misses:\t%li, \t hits:\t%li]\n", found_misses, found_hits);
	//free structures
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
	return tree;
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
			current_entry			= malloc(sizeof(automaton_composite_tree_entry));
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
				terminal_entry			= malloc(sizeof(automaton_composite_tree_entry));
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
				fflush(stdout);
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
			current_entry			= malloc(sizeof(automaton_composite_tree_entry));
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
				terminal_entry			= malloc(sizeof(automaton_composite_tree_entry));
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
	free(tree_entry);
}

void automaton_composite_tree_destroy(automaton_composite_tree* tree){
	if(tree->first_entry != NULL){
		automaton_composite_tree_destroy_entry(tree, tree->first_entry);
		tree->first_entry = NULL;
	}
	tree->max_value 	= 0;
	tree->key_length	= 0;
	free(tree);
}
