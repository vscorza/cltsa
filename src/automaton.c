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
			printf("%s%s", ctx->global_alphabet->list[current_automaton->local_alphabet[i]].name, (i < (current_automaton->local_alphabet_count -1)? "," : ""));
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
automaton_automata_context* automaton_automata_context_create(char* name, automaton_alphabet* alphabet, uint32_t fluents_count, automaton_fluent* fluents){
	automaton_automata_context* ctx		= malloc(sizeof(automaton_automata_context));
	automaton_automata_context_initialize(ctx, name, alphabet, fluents_count, fluents);
	return ctx;
}
automaton_automaton* automaton_automaton_create(char* name, automaton_automata_context* ctx, uint32_t local_alphabet_count, uint32_t* local_alphabet){
	automaton_automaton* automaton		= malloc(sizeof(automaton_automaton));
	automaton_automaton_initialize(automaton, name, ctx, local_alphabet_count, local_alphabet);
	return automaton;
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
void automaton_automata_context_initialize(automaton_automata_context* ctx, char* name, automaton_alphabet* alphabet, uint32_t fluents_count, automaton_fluent* fluents){
	ctx->name					= malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(ctx->name, name);
	ctx->global_alphabet		= automaton_alphabet_clone(alphabet);
	ctx->global_fluents_count	= fluents_count;
	ctx->global_fluents			= malloc(sizeof(automaton_fluent) * ctx->global_fluents_count);
	uint32_t i;
	for(i = 0; i < ctx->global_fluents_count; i++){
		automaton_fluent_copy(&(fluents[i]), &(ctx->global_fluents[i]));	
	}
}
void automaton_automaton_initialize(automaton_automaton* automaton, char* name, automaton_automata_context* ctx, uint32_t local_alphabet_count, uint32_t* local_alphabet){
	automaton->name						= malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(automaton->name, name);
	automaton->context					= ctx;
	automaton->local_alphabet_count		= local_alphabet_count;
	automaton->local_alphabet			= malloc(sizeof(uint32_t) * local_alphabet_count);
	uint32_t i;
	for(i = 0; i < local_alphabet_count; i++){ automaton->local_alphabet[i]	= local_alphabet[i];}
	automaton->states_count				= 0;
	automaton->transitions_size			= LIST_INITIAL_SIZE;
	automaton->transitions_count		= 0;
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
/** DESTROY FUNCTIONS **/
void automaton_signal_event_destroy(automaton_signal_event* signal_event){
	free(signal_event->name);
	signal_event->name	= NULL;
}
void automaton_alphabet_destroy(automaton_alphabet* alphabet){
	uint32_t i;
	for(i = 0; i < alphabet->count; i++){
		automaton_signal_event_destroy(&(alphabet->list[i]));
	}
	free(alphabet->list);
	alphabet->list	= NULL;
	alphabet->count	= 0;
	alphabet->size	= 0;
}
void automaton_transition_destroy(automaton_transition* transition){
	free(transition->signals);
	transition->signals			= NULL;
	transition->state_from		= 0;
	transition->state_to		= 0;
	transition->signals_count	= 0;
}
void automaton_fluent_destroy(automaton_fluent* fluent){
	free(fluent->name);
	free(fluent->starting_signals);
	free(fluent->ending_signals);
	fluent->name					= NULL;
	fluent->starting_signals_count	= 0;
	fluent->starting_signals		= NULL;
	fluent->ending_signals_count	= 0;
	fluent->ending_signals			= NULL;
	fluent->initial_valuation		= false;
}
void automaton_valuation_destroy(automaton_valuation* valuation){
	free(valuation->active_fluents);
	valuation->active_fluents_count	= 0;
	valuation->state				= 0;
	valuation->active_fluents		= NULL;
}
void automaton_automata_context_destroy(automaton_automata_context* ctx){
	free(ctx->name);
	automaton_alphabet_destroy(ctx->global_alphabet);
	free(ctx->global_alphabet);
	uint32_t i;
	for(i = 0; i < ctx->global_fluents_count; i++){
		automaton_fluent_destroy(&(ctx->global_fluents[i]));
	}
	free(ctx->global_fluents);
	ctx->name					= NULL;
	ctx->global_alphabet		= NULL;
	ctx->global_fluents_count	= 0;
	ctx->global_fluents			= NULL;
}
void automaton_automaton_destroy(automaton_automaton* automaton){
	free(automaton->name);
	free(automaton->local_alphabet);
	uint32_t i,j;
	for(i = 0; i < automaton->transitions_count; i++){
		for(j = 0; j < automaton->out_degree[i]; j++){
			automaton_transition_destroy(&(automaton->transitions[i][j]));
		}
		free(automaton->transitions[i]);
		for(j = 0; j < automaton->in_degree[i]; j++){
			automaton_transition_destroy(&(automaton->inverted_transitions[i][j]));
		}
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
	automaton->out_degree			= NULL;
	automaton->transitions			= NULL;
	automaton->in_degree			= NULL;
	automaton->inverted_transitions	= NULL;
	automaton->initial_states_count	= 0;
	automaton->initial_states		= NULL;
	automaton->valuations_size		= 0;
	automaton->valuations_count		= 0;
	automaton->valuations			= NULL;
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
			automaton_signal_event_destroy(&(alphabet->list[i]));
		}
		free(alphabet->list);
		alphabet->list	= new_list;
		alphabet->size	= new_size;
	}else if(signal_ordered_index > -1){
		for(i = (alphabet->count) - 1; i > signal_ordered_index; i--){
			automaton_signal_event_copy(&(alphabet->list[i]), &(alphabet->list[i + 1]));
			automaton_signal_event_destroy(&(alphabet->list[i]));
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
	for(i = 0; i < out_degree; i++){
		if(current_transitions[i].signals_count != transition->signals_count)
			continue;
		if(current_transitions[i].state_to == to_state){
			bool found	= false;
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
	return true;
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
			automaton_transition_destroy(&(current_automaton->transitions[i][j]));
		}
		next_inv_trans[i]				= malloc(sizeof(automaton_transition) * next_in_degree[i]);
		for(j = 0; j < next_in_degree[i]; j++){ 
			automaton_transition_copy(&(current_automaton->inverted_transitions[i][j]), &(next_inv_trans[i][j]));
			automaton_transition_destroy(&(current_automaton->inverted_transitions[i][j]));
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
		automaton_transition_destroy(&(current_automaton->transitions[from_state][i]));
	}
	for(i = 0; i < old_in_degree; i++){
		automaton_transition_copy(&(current_automaton->inverted_transitions[to_state][i]), &(new_in[i]));
		automaton_transition_destroy(&(current_automaton->inverted_transitions[to_state][i]));
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
	uint32_t i, j, k, alphabet_count, fluents_count, alphabet_size;
	uint32_t* alphabet;
	alphabet_count	= 0;
	alphabet_size	= 0;
	//get union of alphabets
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
			if(new_index > -1){
				for(k = (alphabet_count -1); k > (uint32_t)new_index; k--){
					alphabet[k]	= alphabet[k - 1];
				}
			}else{
				new_index	= alphabet_count;
			}
			if(!found){
				alphabet[new_index]	= current_entry;
				alphabet_count++;
			}
		}
	}
	automaton_automaton* composition= automaton_automaton_create("Composition", ctx, alphabet_count, alphabet);
	//get signal to automata list structure
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
	//compose transition relation
	automaton_composite_tree* tree	= automaton_composite_tree_create(automata_count);
	uint32_t frontier_size			= LIST_INITIAL_SIZE;
	uint32_t frontier_count			= 1;
	uint32_t* frontier				= malloc(sizeof(uint32_t) * automata_count * frontier_size);
	uint32_t* current_state			= malloc(sizeof(uint32_t) * automata_count);
	uint32_t from_state;
	uint32_t to_state;
	//set initial state
	for(i = 0; i < automata_count; i++){
		frontier[i]	= automata[i]->initial_states[0];
	}
	automaton_automaton_add_initial_state(composition, automaton_composite_tree_get_key(tree, frontier));
	while(frontier_count > 0){
		//pop a state
		for(i = 0; i < automata_count; i++){
			current_state[i] 		= frontier[(frontier_count - 1) * automata_count + i];
		}
		frontier_count--;
		from_state	= automaton_composite_tree_get_key(tree, current_state);
		//check if frontier state was already decomposed
		if(automaton_automaton_has_state(composition, from_state)){
			continue;
		}
		//TODO: tengo que poder armar conjuntos de transiciones e ir sacando las que tienen que ir juntas
		//o bien porque sincronizan o porque se bloquean
		for(i = 0; i < automata_count; i++){
			uint32_t out_degree	= automata[i]->out_degree[current_state[i]];
			for(j = 0; j < out_degree; j++){

			}
		}
	}
	//free structures
	automaton_composite_tree_destroy(tree);
	for(i = 0; i < alphabet_count; i++){ 
		if(signal_to_automata != NULL){
			free(signal_to_automata[i]);
			signal_to_automata[i]	= NULL;
		}
	}
	free(signal_to_automata);	signal_to_automata	= NULL;
	free(tree); tree = NULL;
	free(frontier); frontier = NULL;
	free(current_state); current_state = NULL;
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
					if(i == (tree->key_length -1)){
						return current_entry->succ->value;
					}
					current_entry	= current_entry->succ;
					found	= true;
				}else{
					last_entry		= current_entry;
					current_entry	= current_entry->next;
				}
			}
			//if not found add whole strip from this point on
			if(!found){
				break;
			}
		}
		
		for(j = i; j < tree->key_length; j++){
			current_entry			= malloc(sizeof(automaton_composite_tree_entry));
			current_entry->value	= composite_key[j];
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
				return terminal_entry->value;
			}
		}
	}
	return 0;
}
void automaton_composite_tree_destroy_entry(automaton_composite_tree* tree, automaton_composite_tree_entry* tree_entry){
	if(tree_entry->succ != NULL){
		automaton_composite_tree_destroy_entry(tree, tree_entry->succ);
		free(tree_entry->succ); tree_entry->succ = NULL;
	}
	if(tree_entry->next != NULL){
		automaton_composite_tree_destroy_entry(tree, tree_entry->next);
		free(tree_entry->next); tree_entry->next = NULL;
	}
	tree_entry->value = 0;
}

void automaton_composite_tree_destroy(automaton_composite_tree* tree){
	if(tree->first_entry != NULL){
		automaton_composite_tree_destroy_entry(tree, tree->first_entry);
		free(tree->first_entry); tree->first_entry = NULL;
	}
	tree->max_value 	= 0;
	tree->key_length	= 0;
}
/** AUX FUNCTIONS **/
uint32_t str_len(char* a) {
	uint32_t i=0;
	while(a[i]!=0) i++; i++;
	return i;
}

