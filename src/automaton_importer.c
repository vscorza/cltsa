/*
 * automaton_importer.c
 *
 *  Created on: May 18, 2020
 *      Author: mariano
 */
#include "automaton.h"

char *automaton_automaton_copy_string(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer) {
	automaton_automaton_load_string(f, finalizers, finalizers_count, buf, buf_size, last_finalizer);
	char *dst	= NULL;
	aut_dupstr(&dst, *buf);
	return dst;
}

char *automaton_automaton_load_string(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer) {
	*last_finalizer	= EOF;
	char c;
	uint32_t i, pos = 0;
	bool is_finalizer = false;
	while((c = fgetc(f)) != EOF){
		for(i = 0; i < finalizers_count; i++){
			if(finalizers[i] == c){
				is_finalizer = true;
				*last_finalizer = c;
				break;
			}
		}
		if(!is_finalizer){
			*buf[pos++]	= c;
		}
		if(pos >= *buf_size){
			*buf_size *= 2;
			char *str_ptr	= realloc(*buf, *buf_size * sizeof(char));
			if(str_ptr == NULL){printf("Could not allocate enough memory\n");exit(-1);}
			*buf	= str_ptr;
		}
	}
	*buf[pos]	= '\0';
	return *buf;
}

uint32_t automaton_automaton_load_int(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer) {
	return atoi(automaton_automaton_load_string(f, finalizers, finalizers_count, buf, buf_size, last_finalizer));
}

bool automaton_automaton_load_bool(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer) {
	return atoi(automaton_automaton_load_string(f, finalizers, finalizers_count, buf, buf_size, last_finalizer)) == 1;
}

void *automaton_automaton_load_array(FILE *f, void* (*load_function)(FILE*, char*, uint32_t, char**, uint32_t*, char*),
		char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count, automaton_import_type type) {
	uint32_t arr_size	= 32;
	uint32_t arr_count	= 0;
	uint32_t i, sizeof_element;
	switch(type){
		case IMPORT_BOOL:sizeof_element = sizeof(bool);	break;
		case IMPORT_INT:sizeof_element = sizeof(uint32_t);	break;
		case IMPORT_STRING:sizeof_element = sizeof(char*);	break;
		default:
			printf("Import type not defined\n");
			exit(-1);
			break;
	}
	void *arr	= calloc(arr_size, sizeof_element);
	char current 	= 	fgetc(f);
	if(current != AUT_SER_ARRAY_START || current == EOF) {
		printf("Corrupted array\n");
		exit(-1);
	}
	do {
		current		= fgetc(f);
		if(current == AUT_SER_ARRAY_END) {
			break;
		}
		ungetc(current,f);
		void *v	= load_function(f, finalizers, finalizers_count, buf, buf_size, last_finalizer);
		switch(type){
			case IMPORT_BOOL:*(bool *)(&arr + sizeof_element * (arr_count++))	= (bool)v;break;
			case IMPORT_INT:*(uint32_t *)(&arr + sizeof_element * (arr_count++))	= (uint32_t)v;break;
			case IMPORT_STRING:*(char**)(&arr + sizeof_element * (arr_count++))	= (char*)v;break;
			default:
				printf("Import type not defined\n");
				exit(-1);
				break;
		}

		if(arr_count >= arr_size){
			arr_size *= 2;
			void *void_ptr	= realloc(arr, arr_size * sizeof_element);
			if(void_ptr == NULL){printf("Could not allocate enough memory\n");exit(-1);}
			arr	= void_ptr;
		}
	}while(*last_finalizer != EOF && *last_finalizer != AUT_SER_ARRAY_END);

	current 	= 	fgetc(f);
	if(current != EOF) {
		bool is_finalizer	= false;
		for(i = 0; i < finalizers_count; i++){
			if(finalizers[i] == current){
				is_finalizer = true;
				*last_finalizer = current;
				break;
			}
		}
		if(!is_finalizer){
			printf("Corrupted array\n");
			exit(-1);
		}
	}
	void *void_ptr	= realloc(arr, arr_count * sizeof_element);
	if(void_ptr == NULL){printf("Could not allocate enough memory\n");exit(-1);}
	arr	= void_ptr;
	*count	= arr_count;
	return arr;
}

void **automaton_automaton_load_array_array(FILE *f, void* (*load_function)(FILE*, char*, uint32_t, char**, uint32_t*, char*),
		char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count, uint32_t **inner_count, automaton_import_type type) {
	uint32_t arr_size	= 32;
	uint32_t arr_count	= 0;
	uint32_t i, sizeof_element;
	switch(type){
		case IMPORT_BOOL:sizeof_element = sizeof(bool*);	break;
		case IMPORT_INT:sizeof_element = sizeof(uint32_t*);	break;
		case IMPORT_STRING:sizeof_element = sizeof(char**);	break;
		default:
			printf("Import type not defined\n");
			exit(-1);
			break;
	}
	void *arr	= calloc(arr_size, sizeof_element);
	if(*inner_count != NULL)free(*inner_count);
	*inner_count	= calloc(arr_size, sizeof(uint32_t));
	char current 	= 	fgetc(f);
	if(current != AUT_SER_ARRAY_START || current == EOF) {
		printf("Corrupted array\n");
		exit(-1);
	}
	do {
		current		= fgetc(f);
		if(current == AUT_SER_ARRAY_END) {
			break;
		}
		ungetc(current,f);

		void *v	= automaton_automaton_load_array(f, load_function, finalizers,
				finalizers_count, buf, buf_size, last_finalizer, &(*inner_count[arr_count]),type);
		switch(type){
			case IMPORT_BOOL:*(bool **)(&arr + sizeof_element * (arr_count++))	= (bool*)v;break;
			case IMPORT_INT:*(uint32_t **)(&arr + sizeof_element * (arr_count++))	= (uint32_t*)v;break;
			case IMPORT_STRING:*(char***)(&arr + sizeof_element * (arr_count++))	= (char**)v;break;
			default:
				printf("Import type not defined\n");
				exit(-1);
				break;
		}
		if(arr_count >= arr_size){
			arr_size *= 2;
			void *void_ptr	= realloc(arr, arr_size * sizeof_element);
			if(void_ptr == NULL){printf("Could not allocate enough memory\n");exit(-1);}
			arr	= void_ptr;
			uint32_t *int_ptr	= realloc(arr, arr_size * sizeof(uint32_t));
			if(int_ptr == NULL){printf("Could not allocate enough memory\n");exit(-1);}
			*inner_count	= int_ptr;
		}
	}while(*last_finalizer != EOF && *last_finalizer != AUT_SER_ARRAY_END);

	current 	= 	fgetc(f);
	if(current != EOF) {
		bool is_finalizer	= false;
		for(i = 0; i < finalizers_count; i++){
			if(finalizers[i] == current){
				is_finalizer = true;
				*last_finalizer = current;
				break;
			}
		}
		if(!is_finalizer){
			printf("Corrupted array\n");
			exit(-1);
		}
	}
	void *void_ptr	= realloc(arr, arr_count * sizeof_element);
	if(void_ptr == NULL){printf("Could not allocate enough memory\n");exit(-1);}
	arr	= void_ptr;
	uint32_t *int_ptr	= realloc(arr, arr_count * sizeof(uint32_t));
	if(int_ptr == NULL){printf("Could not allocate enough memory\n");exit(-1);}
	*inner_count	= int_ptr;
	*count	= arr_count;
	return arr;
}
bool *automaton_automaton_load_bool_array(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count) {
	return (bool*)automaton_automaton_load_array(f, &automaton_automaton_load_bool, finalizers, finalizers_count,
			buf, buf_size, last_finalizer, count, IMPORT_BOOL);
}
bool **automaton_automaton_load_bool_array_array(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count, uint32_t *inner_count) {
	return (bool**)automaton_automaton_load_array_array(f, &automaton_automaton_load_bool, finalizers, finalizers_count,
			buf, buf_size, last_finalizer, count, inner_count, IMPORT_BOOL);
}
uint32_t *automaton_automaton_load_int_array(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count) {
	return (uint32_t*)automaton_automaton_load_array(f, &automaton_automaton_load_int, finalizers, finalizers_count,
			buf, buf_size, last_finalizer, count, IMPORT_INT);
}
uint32_t *automaton_automaton_load_int_array_array(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count, uint32_t *inner_count) {
	return (uint32_t**)automaton_automaton_load_array_array(f, &automaton_automaton_load_int, finalizers, finalizers_count,
			buf, buf_size, last_finalizer, count, inner_count, IMPORT_INT);
}
char **automaton_automaton_load_string_array(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count) {
	return (char**)automaton_automaton_load_array(f, &automaton_automaton_copy_string, finalizers, finalizers_count,
			buf, buf_size, last_finalizer, count, IMPORT_STRING);
}
char ***automaton_automaton_load_string_array_array(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count, uint32_t *inner_count) {
	return (char***)automaton_automaton_load_array_array(f, &automaton_automaton_copy_string, finalizers, finalizers_count,
			buf, buf_size, last_finalizer, count, inner_count, IMPORT_STRING);
}

automaton_signal_event *automaton_automaton_load_signal_event(FILE *f, char **buf, uint32_t *buf_size,
		char *last_finalizer){
	char current 	= 	fgetc(f);
	if(current != AUT_SER_OBJ_START || current == EOF) {
		printf("Corrupted object\n");
		exit(-1);
	}
	char sep_char	= AUT_SER_SEP;
	char obj_end_char	= AUT_SER_OBJ_END;
	*last_finalizer	= '\0';
	char *name			= automaton_automaton_load_string(f, &sep_char, 1, buf, buf_size, last_finalizer);
	uint32_t type_value	= automaton_automaton_load_int(f, &obj_end_char, 1, buf, buf_size, last_finalizer);
	automaton_signal_type type;
	switch(type_value) {
		case 0: type = INPUT_SIG; break;
		case 1: type = OUTPUT_SIG; break;
		case 2: type = INTERNAL_SIG; break;
		default: printf("Corrupt signal event type"); exit(-1);break;
	}
	return automaton_signal_event_create(name, type);
}

void automaton_automaton_check_alphabet(FILE *f, automaton_alphabet *alphabet, char **buf, uint32_t *buf_size,
		char *last_finalizer){
	char current 	= 	fgetc(f);
	if(current != AUT_SER_OBJ_START || current == EOF) {
		printf("Corrupted object\n");
		exit(-1);
	}
	char separator = AUT_SER_SEP;
	*last_finalizer = '\0';
	uint32_t signals_count	= automaton_automaton_load_int(f, &separator, 1, buf, buf_size, last_finalizer);
	if(signals_count == 0){
		printf("Corrupt signals count\n"); exit(-1);
	}
	current 	= 	fgetc(f);
	if(current != AUT_SER_ARRAY_START || current == EOF) {
		printf("Corrupted array\n");
		exit(-1);
	}
	automaton_alphabet	*current_alphabet	= automaton_alphabet_create();
	do {
		automaton_alphabet_add_signal_event(current_alphabet, automaton_automaton_load_signal_event(f, buf, buf_size, last_finalizer));
		current = fgetc(f);
		if(current != AUT_SER_ARRAY_END && current != AUT_SER_SEP) {
			printf("Corrupted array\n");
			exit(-1);
		}

	}while(current != AUT_SER_ARRAY_END);
	current = fgetc(f);
	if(current != AUT_SER_OBJ_END) {
		printf("Corrupted object\n");
		exit(-1);
	}
	//check current alphabet against parameter
	if(alphabet->count != current_alphabet->count){
		printf("Alphabet count differs from host\n");
		exit(-1);
	}
	uint32_t i;
	for(i = 0; i < alphabet->count; i++){
		if(strcmp(alphabet->list[i].name, current_alphabet->list[i].name) != 0){
			printf("Alphabet elements mismatch from host\n");
			exit(-1);
		}
	}
	automaton_alphabet_destroy(current_alphabet);
}
void automaton_automaton_check_load_context(FILE *f, automaton_automata_context *ctx, char **buf, uint32_t *buf_size,
		char *last_finalizer){
	char current 	= 	fgetc(f);
	if(current != AUT_SER_OBJ_START || current == EOF) {
		printf("Corrupted object\n");
		exit(-1);
	}
	char sep_char = AUT_SER_SEP;
	char *name			= automaton_automaton_load_string(f, &sep_char, 1, buf, buf_size, last_finalizer);
	uint32_t fluents_count = 0;
	char **fluents		= NULL;
	uint32_t liveness_count	= 0;
	char **liveness_names		= NULL;
	automaton_automaton_check_alphabet(f, ctx->global_alphabet, buf, buf_size, last_finalizer);
	if(current != AUT_SER_SEP || current == EOF) {
		printf("Corrupted object\n");
		exit(-1);
	}
	uint32_t count		= automaton_automaton_load_int(f, &sep_char, 1, buf, buf_size, last_finalizer);
	if(count == 0){
		current 	= 	fgetc(f);
		if(current != AUT_SER_ARRAY_START || current == EOF) {
			printf("Corrupted array\n");
			exit(-1);
		}
		current 	= 	fgetc(f);
		if(current != AUT_SER_ARRAY_END || current == EOF) {
			printf("Corrupted array\n");
			exit(-1);
		}
		current 	= 	fgetc(f);
		if(current != AUT_SER_SEP || current == EOF) {
			printf("Corrupted object\n");
			exit(-1);
		}
	}else{
		fluents	= automaton_automaton_load_string_array(f, &sep_char, 1, buf, buf_size, last_finalizer, &fluents_count);
	}
	count		= automaton_automaton_load_int(f, &sep_char, 1, buf, buf_size, last_finalizer);
	if(count == 0){
		current 	= 	fgetc(f);
		if(current != AUT_SER_ARRAY_START || current == EOF) {
			printf("Corrupted array\n");
			exit(-1);
		}
		current 	= 	fgetc(f);
		if(current != AUT_SER_ARRAY_END || current == EOF) {
			printf("Corrupted array\n");
			exit(-1);
		}
		current 	= 	fgetc(f);
		if(current != AUT_SER_SEP || current == EOF) {
			printf("Corrupted object\n");
			exit(-1);
		}
	}else{
		liveness_names	= automaton_automaton_load_string_array(f, &sep_char, 1, buf, buf_size, last_finalizer,
				&liveness_count);
	}
	if(ctx->liveness_valuations_count != liveness_count){
		printf("Liveness count mismatch\n");
		exit(-1);
	}
	if(ctx->global_fluents_count != fluents_count){
		printf("Fluents count mismatch\n");
		exit(-1);
	}
	uint32_t i;
	for(i = 0; i < liveness_count; i++){
		if(strcmp(ctx->liveness_valuations_names[i], liveness_names[i]) != 0){
			printf("Liveness instance mismatch\n");
			exit(-1);
		}
		free(liveness_names[i]);
	}
	for(i = 0; i < fluents_count; i++){
		if(strcmp(ctx->global_fluents[i].name, fluents[i]) != 0){
			printf("Fluents instance mismatch\n");
			exit(-1);
		}
		free(fluents[i]);
	}
	if(liveness_names != NULL)free(liveness_names);
	if(fluents != NULL)free(fluents);
}

automaton_transition *automaton_automaton_load_transition(FILE *f, automaton_automata_context *ctx,
		uint32_t *local_alphabet, bool *input_alphabet,
		char **buf, uint32_t *buf_size, char *last_finalizer){
	char current 	= 	fgetc(f);
	if(current != AUT_SER_OBJ_START || current == EOF) {
		printf("Corrupted object\n");
		exit(-1);
	}
	char sep_char = AUT_SER_SEP;
	char obj_end_char	= AUT_SER_OBJ_END;
	uint32_t from_state		= automaton_automaton_load_int(f, &sep_char, 1, buf, buf_size, last_finalizer);
	uint32_t to_state		= automaton_automaton_load_int(f, &sep_char, 1, buf, buf_size, last_finalizer);
	uint32_t signals_count	= automaton_automaton_load_int(f, &sep_char, 1, buf, buf_size, last_finalizer);
	uint32_t int_count		= 0;
	uint32_t *signals		= automaton_automaton_load_int_array(f, &sep_char, 1, buf, buf_size, last_finalizer,
			&int_count);
	bool is_input			= automaton_automaton_load_bool(f, &obj_end_char, 1, buf, buf_size, last_finalizer);

	automaton_transition	*transition	= automaton_transition_create(from_state, to_state);
	uint32_t i;
	for(i = 0; i < signals_count; i++)
		automaton_transition_add_signal_event_ID(transition, ctx, local_alphabet[signals[i]],
				input_alphabet[i]?INPUT_SIG:OUTPUT_SIG);

	free(signals);

	return transition;
}

automaton_automaton *automaton_automaton_load_report(automaton_automata_context *ctx, char *filename){
	FILE *f = fopen(filename, "w");
	char current, dst;
	if (f == NULL){
		printf("Error opening file!\n");
		return false;
	}
	current = fgetc(f);
	if(current != AUT_SER_OBJ_START || current == EOF){
		printf("Corrupt file at %s\n", filename);
		exit(-1);
	}

	uint32_t buf_size = 32;
	uint32_t i;
	char *buf	= calloc(buf_size, sizeof(char));
	char last_finalizer = EOF;
	char sep_char = AUT_SER_SEP;
	char obj_end_char	= AUT_SER_OBJ_END;
	char* name = automaton_automaton_copy_string(f, &sep_char, 1, &buf, &buf_size, &last_finalizer);
	automaton_automaton_check_load_context(f, ctx, &buf, &buf_size, &last_finalizer);
	uint32_t local_alphabet_count	= 0;
	uint32_t *local_alphabet	= NULL;
	bool *input_alphabet	= NULL;
	uint32_t count		= automaton_automaton_load_int(f, &sep_char, 1, &buf, &buf_size, &last_finalizer);

	if(count == 0){
		current 	= 	fgetc(f);
		if(current != AUT_SER_ARRAY_START || current == EOF) {
			printf("Corrupted array\n");
			exit(-1);
		}
		current 	= 	fgetc(f);
		if(current != AUT_SER_ARRAY_END || current == EOF) {
			printf("Corrupted array\n");
			exit(-1);
		}
		current 	= 	fgetc(f);
		if(current != AUT_SER_SEP || current == EOF) {
			printf("Corrupted object\n");
			exit(-1);
		}
	}else{
		local_alphabet	= automaton_automaton_load_int_array(f, &sep_char, 1, &buf, &buf_size, &last_finalizer, &local_alphabet_count);
		input_alphabet	= calloc(local_alphabet_count, sizeof(bool));
		for(i = 0; i < local_alphabet_count; i++){
			input_alphabet[i]	= ctx->global_alphabet->list[local_alphabet[i]].type == INPUT_SIG;
		}
	}
	automaton_automaton *result = automaton_automaton_create(name, ctx, local_alphabet_count, local_alphabet, false, false);
	free(name);
	count		= automaton_automaton_load_int(f, &sep_char, 1, &buf, &buf_size, &last_finalizer);
	if(count == 0){
		current 	= 	fgetc(f);
		if(current != AUT_SER_ARRAY_START || current == EOF) {
			printf("Corrupted array\n");
			exit(-1);
		}
		current 	= 	fgetc(f);
		if(current != AUT_SER_ARRAY_END || current == EOF) {
			printf("Corrupted array\n");
			exit(-1);
		}
		current 	= 	fgetc(f);
		if(current != AUT_SER_SEP || current == EOF) {
			printf("Corrupted object\n");
			exit(-1);
		}
	}else{
		current 	= fgetc(f);
		if(current != AUT_SER_ARRAY_START) {
			printf("Corrupted array\n");
			exit(-1);
		}
		automaton_transition *current_transition = NULL;
		do {
			current_transition	= automaton_automaton_load_transition(f, ctx, local_alphabet,
					input_alphabet, &buf, &buf_size, &last_finalizer);
			automaton_automaton_add_transition(result, current_transition);
			automaton_transition_destroy(current_transition, true);
			current = fgetc(f);
			if(current != AUT_SER_ARRAY_END && current != AUT_SER_SEP) {
				printf("Corrupted array\n");
				exit(-1);
			}

		}while(current != AUT_SER_ARRAY_END);
	}
	if(local_alphabet != NULL){
		free(local_alphabet);
		free(input_alphabet);
	}
	current = fgetc(f);
	if(current != AUT_SER_SEP) {
		printf("Corrupted object\n");
		exit(-1);
	}
	uint32_t initial_states_count = 0;
	uint32_t *initial_states = NULL;

	count		= automaton_automaton_load_int(f, &sep_char, 1, &buf, &buf_size, &last_finalizer);
	if(count == 0){
		current 	= 	fgetc(f);
		if(current != AUT_SER_ARRAY_START || current == EOF) {
			printf("Corrupted array\n");
			exit(-1);
		}
		current 	= 	fgetc(f);
		if(current != AUT_SER_ARRAY_END || current == EOF) {
			printf("Corrupted array\n");
			exit(-1);
		}
		current 	= 	fgetc(f);
		if(current != AUT_SER_SEP || current == EOF) {
			printf("Corrupted object\n");
			exit(-1);
		}
	}else{
		initial_states	= automaton_automaton_load_int_array(f, &sep_char, 1, &buf, &buf_size, &last_finalizer, &initial_states_count);
		for(i = 0; i < initial_states_count; i++){
			automaton_automaton_add_initial_state(result, initial_states[i]);
		}

	}
	automaton_automaton_update_valuation(result);
	return result;
}