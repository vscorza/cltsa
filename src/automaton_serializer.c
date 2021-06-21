/*
 * automaton_serializer.c
 *
 * Serializing and printing functions implementations related to the automaton struct
 *
 *  Created on: Apr 16, 2021
 *      Author: mariano
 */
#include "automaton.h"
#include "assert.h"

extern int __automaton_global_print_id;

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

void automaton_transition_print(automaton_transition* transition, automaton_automata_context* ctx, char* prefix, char* suffix, int link_id){
	if(prefix != NULL)
		printf("%s", prefix);
	uint32_t i,j;

	if(TRANSITION_IS_INPUT(transition))printf("!");
	printf("(%d {", transition->state_from);
	bool first_print = true;
	for(i = 0; i < (TRANSITION_ENTRY_SIZE * FIXED_SIGNALS_COUNT) - 1; i++){
		if(TEST_TRANSITION_BIT(transition, i)){
			if(first_print)first_print = false;
			else printf(",");
			printf("%s", ctx->global_alphabet->list[i].name);
		}
	}
	if(link_id >= 0)
#if PRINT_HTML
		printf("}-> <a href='#to_%d_%d'>%d</a>)", link_id, transition->state_to, transition->state_to);
#else
		printf("}-> %d)", transition->state_to);
#endif
	else
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
	__automaton_global_print_id++;
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
		uint32_t fluent_index;
		bool satisfies_valuation;
		printf("%sMonitored state valuations:{", prefix2);
		bool first_state	= true;
		for(i = 0; i < ctx->state_valuations_count; i++){
			fluent_index		= GET_STATE_FLUENT_INDEX(ctx->state_valuations_count, 0, i);
			satisfies_valuation	= TEST_FLUENT_BIT(current_automaton->state_valuations_declared, fluent_index);
			printf("%s%s", first_state? ",": "", ctx->state_valuations_names[i]);
			first_state	= false;
		}
		printf("}\n");
		printf("%sValuations:\n%s", prefix2, prefix2);
		for(j = 0; j < ctx->global_fluents_count; j++){
			printf(ctx->global_fluents[j].name);
			if(j < (ctx->global_fluents_count - 1))printf(",");
		}
		printf("\n%s", prefix2);
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
			printf(" ");
			for(j = 0; j < ctx->state_valuations_count; j++){
				fluent_index		= GET_STATE_FLUENT_INDEX(ctx->state_valuations_count, i, j);
				satisfies_valuation	= TEST_FLUENT_BIT(current_automaton->state_valuations, fluent_index);
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
		if(!current_automaton->is_controllable[i])printf("(!)");
#if PRINT_HTML
		printf("<span id='to_%d_%d'></span>", __automaton_global_print_id, i);
#endif

		for(j = 0; j < current_automaton->out_degree[i]; j++)
		automaton_transition_print(&(current_automaton->transitions[i][j]), ctx, prefix2, "\n", __automaton_global_print_id);
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
	    printf("Error opening file![serial:print_fsp](%s)\n", filename);
	    return false;
	}
	uint32_t i,j,k, controllable_transitions = 0;

	for(i = 0; i < current_automaton->transitions_count; i++){
		for(j = 0; j < current_automaton->out_degree[i]; j++){
			if(!(TRANSITION_IS_INPUT(&(current_automaton->transitions[i][j]))))controllable_transitions++;
		}
	}

	fprintf(f, "/* AUTO-GENERATED_FILE\n\tAutomaton %s(%d states, %d transitions, %d controllable transitions) */\n", current_automaton->name
			, current_automaton->transitions_count, current_automaton->transitions_composite_count, controllable_transitions);
	automaton_automata_context* ctx		= current_automaton->context;

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
			GET_TRANSITION_SIGNAL_COUNT(current_transition);
			if(j == 0){fprintf(f, "S_%d = (", current_transition->state_from);}
			if(signal_count == 0)fprintf(f, AUT_TAU_CONSTANT);
			if(signal_count > 1)fprintf(f, "<");
			bool first_print = true;
			for(k = 0; k < (TRANSITION_ENTRY_SIZE * FIXED_SIGNALS_COUNT) - 1; k++){
				signal_bit_array_t check_value = (TEST_TRANSITION_BIT(current_transition, k));
				if(check_value != (signal_bit_array_t)0){
					if(first_print)first_print = false;
					else fprintf(f,",");
					fprintf(f,"%s", ctx->global_alphabet->list[k].name);
				}
			}
			if(signal_count > 1)fprintf(f, ">");
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
		printf("Error opening file![serial:print_dot](%s)\n", filename);
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
			GET_TRANSITION_SIGNAL_COUNT(current_transition);
			if(signal_count == 0)fprintf(f, "%s", AUT_TAU_CONSTANT);
			bool first_print = true;
			for(k = 0; k < (TRANSITION_ENTRY_SIZE * FIXED_SIGNALS_COUNT) - 1; k++){
				if(TEST_TRANSITION_BIT(current_transition, k)){
					if(first_print)first_print = false;
					else printf(",");
					printf("%s", ctx->global_alphabet->list[k].name);
				}
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
//<name,alphabet,fluents_count,[f_1.name,..,f_n.name],liveness_count,[l_1.name,..,l_m.name],vstates_count,[v_1.name,..,v_k.name]>
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
	fprintf(f, "%s%s%d%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, ctx->state_valuations_count, AUT_SER_SEP, AUT_SER_ARRAY_START);
	for(i = 0; i < ctx->state_valuations_count; i++){
		fprintf(f, "%s%s", ctx->state_valuations_names[i], i == (ctx->state_valuations_count - 1)? "" :  AUT_SER_SEP);
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
	GET_TRANSITION_SIGNAL_COUNT(transition);
	fprintf(f, "%s%d%s%d%s%d%s%s", AUT_SER_OBJ_START, transition->state_from, AUT_SER_SEP, transition->state_to, AUT_SER_SEP, signal_count, AUT_SER_SEP, AUT_SER_ARRAY_START);
	uint32_t i, k;
	bool first_print = true;
	for(k = 0; k < (TRANSITION_ENTRY_SIZE * FIXED_SIGNALS_COUNT) - 1; k++){
		if(TEST_TRANSITION_BIT(transition, k)){
			if(first_print)first_print = false;
			else fprintf(f, "%s", AUT_SER_SEP);
			fprintf(f,"%d", k);
		}
	}
	fprintf(f, "%s%s%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, TRANSITION_IS_INPUT(transition)? "1" :"0",AUT_SER_OBJ_END);
}
//name\t |states|\t |delta|\t mean(delta(s))\t var(delta(s)\t |events total|\t mean(events(trans))\t var(events(trans))\t |controllable options|\t mean(controllable options(s))\t var(controllable options(s))\n
//|alphabet|\t x:occ_1:alphabet_1\t ... x:occ_N:alphabet_N\n (where x is C if controllable U otherwise, occ_i is the total number of occurrences per event)
void automaton_automaton_serialize_metrics(FILE *f, char* filename, automaton_automaton *automaton, bool is_html){
	automaton_automaton_monitored_order_transitions(automaton);
	uint32_t i, j, k;
	uint64_t effective_state_count = 0;
	for(i = 0; i < automaton->transitions_count; i++)if(automaton->out_degree[i] > 0 || automaton->in_degree[i] > 0)effective_state_count++;
	double mean_delta_s	= (automaton->transitions_composite_count * 1.0f) / effective_state_count;

	//compute variance on delta(s)
	double variance_delta_s = 0;
	for(i = 0; i < automaton->transitions_count; i++)variance_delta_s	+= (automaton->out_degree[i] - mean_delta_s) *  (automaton->out_degree[i] - mean_delta_s);
	variance_delta_s	/= ((automaton->transitions_count - 1) * 1.0f);
	uint32_t signal_count = 0,_p_ = 0;
	uint64_t total_signals	= 0;
	uint32_t fixed_max_count	= (uint32_t)ceil(automaton->context->global_alphabet->count*1.0f / (sizeof(signal_bit_array_t) * 8));
	uint32_t signals_bytecount	= (sizeof(signal_bit_array_t) * fixed_max_count);
	//compute total signals
	int current_value;
	for(i = 0; i < automaton->transitions_count; i++)
		for(j=0; j < automaton->out_degree[i]; j++){
			for(k = 0; k < signals_bytecount; k++){
				current_value = k == 0? (((int*)(&(automaton->transitions[i][j].signals)))[k]) & ((int)~0x1) :
						(((int*)(&(automaton->transitions[i][j].signals)))[k]);
				total_signals	+= __builtin_popcount(current_value);
			}
		}
	double mean_signals_t	= (total_signals * 1.0f) / automaton->transitions_composite_count;
	//compute variance on events(t)
	double variance_signals_t = 0;
	for(i = 0; i < automaton->transitions_count; i++)
		for(j=0; j < automaton->out_degree[i]; j++){
			signal_count = 0;
			for(k = 0; k < signals_bytecount; k++){
				current_value = k == 0? (((int*)(&(automaton->transitions[i][j].signals)))[k]) & ((int)~0x1) :
						(((int*)(&(automaton->transitions[i][j].signals)))[k]);
				signal_count	+= __builtin_popcount(current_value);
			}

			variance_signals_t	+= (signal_count - mean_signals_t) * (signal_count - mean_signals_t);
		}
	variance_signals_t	/= ((automaton->transitions_composite_count - 1) * 1.0f);
	//compute signal occurrences
	uint32_t alphabet_count	= automaton->context->global_alphabet->count;
	uint64_t *signal_occurrence	= calloc(alphabet_count, sizeof(uint64_t));
	for(i = 0; i < automaton->transitions_count; i++)
		for(j=0; j < automaton->out_degree[i]; j++){
			for(k = 0; k < alphabet_count; k++){
				if(TEST_TRANSITION_BIT((&(automaton->transitions[i][j])), k))
					signal_occurrence[k]++;
			}
		}
	//cmpute controllable options
	bool new_monitored = false;
	uint64_t controllable_options	= 0, current_controllable	= 0;
	//add all options, if only one monitored option is available per state do not add
	for(i = 0; i < automaton->transitions_count; i++){
		new_monitored	= false;
		for(j = 0; j < automaton->out_degree[i]; j++){
			if(j > 0){
				new_monitored = !(automaton_automaton_transition_monitored_eq(automaton,
						&(automaton->transitions[i][j - 1]),
						&(automaton->transitions[i][j])));
			}else controllable_options++;
			if(new_monitored)controllable_options++;
		}
	}
	double mean_controllable_options	= (controllable_options * 1.0f) / automaton->transitions_composite_count;
	//compute variance on events(t)
	double variance_controllable_options = 0;
	for(i = 0; i < automaton->transitions_count; i++){
		current_controllable	= 0;
		for(j=0; j < automaton->out_degree[i]; j++){
			if(j > 0){
				new_monitored = !(automaton_automaton_transition_monitored_eq(automaton,
						&(automaton->transitions[i][j - 1]),
						&(automaton->transitions[i][j])));
			}else current_controllable++;
			if(new_monitored)current_controllable++;
		}
		variance_controllable_options	+= (current_controllable - mean_controllable_options) * (current_controllable - mean_controllable_options);
	}
	variance_controllable_options	/= ((effective_state_count - 1) * 1.0f);


	if(!is_html){
		fprintf(f, "%s,%s,%" PRIu64 ",%" PRIu64 ",%f,%f,%" PRIu64 ",%f,%f,%" PRIu64 ",%f,%f\n%d,", filename, automaton->name, effective_state_count, automaton->transitions_composite_count,
				mean_delta_s, variance_delta_s, total_signals, mean_signals_t, variance_signals_t, controllable_options, mean_controllable_options, variance_controllable_options, alphabet_count);
		for(i = 0; i < alphabet_count; i++){
			fprintf(f, "%s:%" PRIu64 ":%s%s", automaton->context->global_alphabet->list[i].type == INPUT_SIG ? "U" : "C", signal_occurrence[i], automaton->context->global_alphabet->list[i].name
					, i < (alphabet_count -1)? ",": "\n");
		}
	}else{
		fprintf(f, "<html>\n<head><title>%s metrics</title><style>table,th,td{border: 1px solid black;border-collapse:collapse;}</style></head>\n<body>", automaton->name);
		fprintf(f, "<p><b>Automaton:</b> %s</p>", automaton->name);
		fprintf(f, "<table>\n<tr><td><b>|S|</b></td><td><b>|Delta|</b></td><td><b>mean(out(s))</b></td><td><b>var(out(s))</b></td><td><b>|events|</b></td><td><b>mean(events(t))</b></td><td><b><b>var(events(t))</b></td>"\
				"<td><b>|Delta.ctrl|</b></td><td><b>mean(out.ctrl(s))</b></td><td><b>var(out.ctrl(s))</b></td></tr>\n");

		fprintf(f, "<tr><td>%" PRIu64 "</td><td>%" PRIu64 "</td><td>%f</td><td>%f</td><td>%" PRIu64 "</td><td>%f</td><td>%f</td><td>%" PRIu64 "</td><td>%f</td><td>%f</td></tr>\n</table>\n", effective_state_count, automaton->transitions_composite_count,
						mean_delta_s, variance_delta_s, total_signals, mean_signals_t, variance_signals_t,
						controllable_options, mean_controllable_options, variance_controllable_options);
		fprintf(f,"<p><b>Events occurrences count</b></p>\n<table>\n<tr><td><b>Signal Name</b></td><td><b>Controllable?</b></td><td><b>Occurrences</b></td></tr>\n");
				for(i = 0; i < alphabet_count; i++){
					fprintf(f, "<tr><td>%s</td><td>%s</td><td>%" PRIu64 "</td></tr>\n",
							automaton->context->global_alphabet->list[i].name, automaton->context->global_alphabet->list[i].type == INPUT_SIG ? "" : "[*]", signal_occurrence[i]);
				}
		fprintf(f,"</table>\n</body>\n</html>");
	}
	free(signal_occurrence);
}

/*
 * <name,ctx,local_alphabet_count,[sig_1.idx,..,sig_N.idx],vstates_monitored_count,[vstates_mon1,..,vstates_monN]
,trans_count,[trans_1,..,trans_M],init_count,[init_i,..,init_K],[[val_s_0_f_0,..,val_s_0_f_L],..,[val_s_T_f_0,..,val_s_T_f_L]]
,[[val_s_0_l_0,..,val_s_0_l_R],..,[val_s_T_l_0,..,val_s_T_l_R]]
,[[vstate_s_0_f_0,..,vstate_s_0_f_V],..,[vstate_s_T_f_0,..,vstate_s_T_f_R]]>
 */
void automaton_automaton_serialize_report(FILE *f, automaton_automaton *automaton){
	uint32_t i, j;
	uint32_t fluent_index;
	uint32_t fluent_count	= automaton->context->global_fluents_count;
	uint32_t liveness_count	= automaton->context->liveness_valuations_count;
	uint32_t vstates_count	= automaton->context->state_valuations_count;
	fprintf(f, "%s%s%s", AUT_SER_OBJ_START, automaton->name, AUT_SER_SEP);
	automaton_automata_context_serialize_report(f, automaton->context);
	fprintf(f, "%s%d%s%s", AUT_SER_SEP, automaton->local_alphabet_count, AUT_SER_SEP, AUT_SER_ARRAY_START);
	for(i = 0; i < automaton->local_alphabet_count; i++){
		fprintf(f, "%d%s", automaton->local_alphabet[i], i == (automaton->local_alphabet_count - 1)? "" :  AUT_SER_SEP);
	}
	fprintf(f, "%s%s%d%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, automaton->context->state_valuations_count, AUT_SER_SEP, AUT_SER_ARRAY_START);
	for(i = 0; i < automaton->context->state_valuations_count; i++){
		if(automaton->state_valuations_declared_size == 0){
			fprintf(f, "0%s", i == (automaton->context->state_valuations_count - 1)? "" :  AUT_SER_SEP);
		}else{
		fluent_index	= GET_STATE_FLUENT_INDEX(automaton->context->state_valuations_count, 0, i);
		fprintf(f, "%s%s",
				TEST_FLUENT_BIT(automaton->state_valuations_declared, fluent_index)
				? "1" : "0", i == (automaton->context->state_valuations_count - 1)? "" :  AUT_SER_SEP);
		}
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
	fprintf(f, "%s%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, AUT_SER_ARRAY_START);
	for(i = 0; i < automaton->transitions_count; i++){
		fprintf(f, "%s", AUT_SER_ARRAY_START);
		for(j = 0; j < automaton->context->state_valuations_count; j++){
			if(automaton->is_game && automaton->state_valuations_declared_size > 0){
				fluent_index	= GET_STATE_FLUENT_INDEX(automaton->context->state_valuations_count, i, j);
				fprintf(f, "%s%s", TEST_FLUENT_BIT(automaton->state_valuations, fluent_index) ? "1" : "0", j == (automaton->context->state_valuations_count - 1)? "" :  AUT_SER_SEP);
			}else{
				fprintf(f, "%s%s", "0",  j == (automaton->context->state_valuations_count - 1)? "" :  AUT_SER_SEP);
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
	fprintf(f, "%s%s0%s%s%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, AUT_SER_SEP, AUT_SER_ARRAY_START, AUT_SER_ARRAY_END, AUT_SER_OBJ_END);
}
void automaton_ranking_transition_serialize_report(FILE *f, automaton_transition *transition,uint32_t ranking_value){
	fprintf(f, "%s%d%s%d%s%d%s%s", AUT_SER_OBJ_START, transition->state_from, AUT_SER_SEP, transition->state_to, AUT_SER_SEP, 1 /*transition->signals_count + 1*/, AUT_SER_SEP, AUT_SER_ARRAY_START);
	uint32_t i, k;
	bool first_print = true;
	for(k = 0; k < (TRANSITION_ENTRY_SIZE * FIXED_SIGNALS_COUNT) - 1; k++){
		if(TEST_TRANSITION_BIT(transition, k)){
			if(first_print)first_print = false;
			else fprintf(f, "%s", AUT_SER_SEP);
			fprintf(f,"%d", k);
		}
	}
	if(first_print)first_print = false;
	else fprintf(f, "%s", AUT_SER_SEP);
	fprintf(f,"%d", ranking_value);
	fprintf(f, "%s%s%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, TRANSITION_IS_INPUT(transition)? "1" :"0",AUT_SER_OBJ_END);
}
bool automaton_ranking_print_report(automaton_automaton *automaton,
		automaton_concrete_bucket_list** ranking_list, uint32_t* max_delta, uint32_t guarantee_count,
		char** guarantees){
	uint32_t name_len = strlen(automaton->name) + 60;
	char *filename = calloc(name_len, sizeof(char));
	FILE *f = NULL;
	uint32_t r;
	for(r = 0; r < guarantee_count; r++){
		snprintf(filename, name_len, "/tmp/%s_ranking_%s.rep", automaton->name, guarantees[r]);
		f = fopen(filename, "w");

		if (f == NULL)
		{
			printf("Error opening file![serial:print_ranking_report](%s)\n", filename);
			return false;
		}

		fprintf(f, "%s%s_ranking_%s%s", AUT_SER_OBJ_START, automaton->name, guarantees[r], AUT_SER_SEP);
		automaton_ranking_automata_context_serialize_report(f, automaton->context, max_delta[r]);
		fprintf(f, "%s%d%s%s", AUT_SER_SEP, automaton->local_alphabet_count + max_delta[r], AUT_SER_SEP, AUT_SER_ARRAY_START);
		uint32_t i, j;
		uint32_t fluent_index;
		uint32_t fluent_count	= automaton->context->global_fluents_count;
		uint32_t liveness_count	= automaton->context->liveness_valuations_count;
		for(i = 0; i < automaton->local_alphabet_count; i++){
			fprintf(f, "%d%s", automaton->local_alphabet[i],AUT_SER_SEP);
		}
		fprintf(f, "%d%s", automaton->context->global_alphabet->count + 1, AUT_SER_SEP);
		for(i = 0; i < max_delta[r]; i++){
			fprintf(f, "%d%s", automaton->context->global_alphabet->count + 1 + i, i == (max_delta[r] - 1)? "" :  AUT_SER_SEP);
		}
		fprintf(f, "%s%s%d%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, automaton->context->state_valuations_count, AUT_SER_SEP, AUT_SER_ARRAY_START);
		for(i = 0; i < automaton->context->state_valuations_count; i++){
			if(automaton->state_valuations_declared_size == 0){
				fprintf(f, "0%s", i == (automaton->context->state_valuations_count - 1)? "" :  AUT_SER_SEP);
			}else{
			fluent_index	= GET_STATE_FLUENT_INDEX(automaton->context->state_valuations_count, 0, i);
			fprintf(f, "%s%s",
					TEST_FLUENT_BIT(automaton->state_valuations_declared, fluent_index)
					? "1" : "0", i == (automaton->context->state_valuations_count - 1)? "" :  AUT_SER_SEP);
			}
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
							ranking_value == RANKING_INFINITY? automaton->context->global_alphabet->count :automaton->context->global_alphabet->count + ranking_value + 1);
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
					fprintf(f, "%s%s", "0", j == (liveness_count - 1)? "" :  AUT_SER_SEP);
				}

			}
			fprintf(f, "%s%s", AUT_SER_ARRAY_END, i == (automaton->transitions_count - 1) ? "" : AUT_SER_SEP);
		}
		fprintf(f, "%s%s%s", AUT_SER_ARRAY_END, AUT_SER_SEP, AUT_SER_ARRAY_START);
		for(i = 0; i < automaton->transitions_count; i++){
			fprintf(f, "%s", AUT_SER_ARRAY_START);
			for(j = 0; j < automaton->context->state_valuations_count; j++){
				if(automaton->is_game && automaton->state_valuations_declared_size > 0){
					fluent_index	= GET_STATE_FLUENT_INDEX(automaton->context->state_valuations_count, i, j);
					fprintf(f, "%s%s", TEST_FLUENT_BIT(automaton->state_valuations, fluent_index) ? "1" : "0", j == (automaton->context->state_valuations_count - 1)? "" :  AUT_SER_SEP);
				}else{
					fprintf(f, "%s%s", "0",  j == (automaton->context->state_valuations_count - 1)? "" :  AUT_SER_SEP);
				}

			}
			fprintf(f, "%s%s", AUT_SER_ARRAY_END, i == (automaton->transitions_count - 1) ? "" : AUT_SER_SEP);
		}
		fprintf(f, "%s%s", AUT_SER_ARRAY_END, AUT_SER_OBJ_END);
		fclose(f);
	}
	free(filename);
	return true;
}
bool automaton_automaton_print_report(automaton_automaton *automaton, char *filename){
	FILE *f = fopen(filename, "w");
	if (f == NULL)
	{
		printf("Error opening file![serial:print_automaton_report](%s)\n", filename);
	    return false;
	}
	automaton_automaton_serialize_report(f, automaton);
	fclose(f);
	return true;
}

bool automaton_automaton_print_metrics(automaton_automaton *automaton, char *filename, bool is_html){
	FILE *f = fopen(filename, "w");
	if (f == NULL)
	{
		printf("Error opening file![serial:print_metrics](%s)\n", filename);
	    return false;
	}
	automaton_automaton_serialize_metrics(f, filename, automaton, is_html);
	fclose(f);
	return true;
}
