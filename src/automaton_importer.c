/*
 * automaton_importer.c
 *
 *  Created on: May 18, 2020
 *      Author: mariano
 */
#include "automaton.h"

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
		uint32_t sizeof_element, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count) {
	uint32_t arr_size	= 32;
	uint32_t arr_count	= 0;
	uint32_t i;
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
		fungetc(current,f);
		void *v	= load_function(f, finalizers, finalizers_count, buf, buf_size, last_finalizer);
		arr[arr_count++]	= (void*)v;
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

void **automaton_automaton_load_array_array(FILE *f, void* (*load_function)(char*, uint32_t, char**, uint32_t*, char*),
		uint32_t sizeof_element, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count, uint32_t **inner_count) {
	uint32_t arr_size	= 32;
	uint32_t arr_count	= 0;
	uint32_t i;
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
		fungetc(current,f);
		void *v	= automaton_automaton_load_array(f, load_function, sizeof_element, finalizers,
				finalizers_count, buf, buf_size, last_finalizer, &inner_count[arr_count]);
		arr[arr_count++]	= (void*)v;
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
	return (bool*)automaton_automaton_load_array(f, &automaton_automaton_load_bool, sizeof(bool), finalizers, finalizers_count,
			buf, buf_size, last_finalizer, count);
}
bool **automaton_automaton_load_bool_array_array(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count, uint32_t *inner_count) {
	return (bool**)automaton_automaton_load_array_array(f, &automaton_automaton_load_bool, sizeof(bool), finalizers, finalizers_count,
			buf, buf_size, last_finalizer, count, inner_count);
}
uint32_t *automaton_automaton_load_int_array(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count) {
	return (uint32_t*)automaton_automaton_load_array(f, &automaton_automaton_load_int, sizeof(uint32_t), finalizers, finalizers_count,
			buf, buf_size, last_finalizer, count);
}
uint32_t *automaton_automaton_load_int_array_array(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count, uint32_t *inner_count) {
	return (uint32_t**)automaton_automaton_load_array_array(f, &automaton_automaton_load_int, sizeof(uint32_t), finalizers, finalizers_count,
			buf, buf_size, last_finalizer, count, inner_count);
}
char **automaton_automaton_load_string_array(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count) {
	return (char**)automaton_automaton_load_array(f, &automaton_automaton_load_string, sizeof(char*), finalizers, finalizers_count,
			buf, buf_size, last_finalizer, count);
}
char ***automaton_automaton_load_string_array_array(FILE *f, char *finalizers, uint32_t finalizers_count, char **buf, uint32_t *buf_size,
		char *last_finalizer, uint32_t *count, uint32_t *inner_count) {
	return (char***)automaton_automaton_load_array_array(f, &automaton_automaton_load_string, sizeof(char*), finalizers, finalizers_count,
			buf, buf_size, last_finalizer, count, inner_count);
}

void automaton_automaton_check_load_context(FILE *f, automaton_automata_context *ctx){

}

automaton_automaton *automaton_automaton_load_report(automaton_automata_context *ctx, char *filename){
	FILE *f = fopen(filename, "w");
	char c, dst;

	if (f == NULL)
	{
		printf("Error opening file!\n");
		return false;
	}

	c = fgetc(f);
	if(c != AUT_SER_OBJ_START || c == EOF){
		printf("Corrupt file at %s\n", filename);
		exit(-1);
	}
	char* name = automaton_automaton_load_name(f, &AUT_SER_SEP, 1);
	automaton_automaton_check_load_context(f);

	automaton_automaton *result	= automaton_automaton_create();
	result->name	= name;
	return result;
}
	/*
		localAlphabet	= new ArrayList<Integer>();
		transitions		= new ArrayList<ReportTransition>();
		initialStates	= new ArrayList<Integer>();
		fluentValuations	= new ArrayList<List<Boolean>>();
		livenessValuations	= new ArrayList<List<Boolean>>();
	    try {
			char current;
			current = (char) fis.read();
			if(current != ReportConstants.AUT_SER_OBJ_START) {
				isOK = false;
				return;
			}
			name			= readString(fis, ReportConstants.AUT_SER_SEP);
			context			= new ReportContext(fis);
			current = (char) fis.read();
			if(current != ReportConstants.AUT_SER_SEP) {
				isOK = false;
				return;
			}
			int count	= readInt(fis, ReportConstants.AUT_SER_SEP);
			if(count == 0) {
				current 	= (char) fis.read();
				if(current != ReportConstants.AUT_SER_ARRAY_START) {
					isOK	= false;
					return;
				}
				current = (char) fis.read();
				if(current != ReportConstants.AUT_SER_ARRAY_END) {
					isOK	= false;
					return;
				}
			}else {
				localAlphabet	= readIntArray(fis, ReportConstants.AUT_SER_ARRAY_END);
			}
			count		= readInt(fis, ReportConstants.AUT_SER_SEP);
			if(count == 0) {
				current 	= (char) fis.read();
				if(current != ReportConstants.AUT_SER_ARRAY_START) {
					isOK	= false;
					return;
				}
				current = (char) fis.read();
				if(current != ReportConstants.AUT_SER_ARRAY_END) {
					isOK	= false;
					return;
				}
			}else {
				current 	= (char) fis.read();
				if(current != ReportConstants.AUT_SER_ARRAY_START) {
					isOK	= false;
					return;
				}
				do {
					transitions.add(new ReportTransition(fis));
					current = (char) fis.read();
					if(current != ReportConstants.AUT_SER_ARRAY_END && current
							!= ReportConstants.AUT_SER_SEP) {
						isOK	= false;
						return;
					}

				}while(current != ReportConstants.AUT_SER_ARRAY_END);
			}
			current = (char) fis.read();
			if(current != ReportConstants.AUT_SER_SEP) {
				isOK = false;
				return;
			}
			count	= readInt(fis, ReportConstants.AUT_SER_SEP);
			if(count == 0) {
				current 	= (char) fis.read();
				if(current != ReportConstants.AUT_SER_ARRAY_START) {
					isOK	= false;
					return;
				}
				current = (char) fis.read();
				if(current != ReportConstants.AUT_SER_ARRAY_END) {
					isOK	= false;
					return;
				}
			}else {
				initialStates	= readIntArray(fis, ReportConstants.AUT_SER_ARRAY_END);
			}
			fluentValuations	= readBooleanArrayArray(fis, ReportConstants.AUT_SER_SEP);
			livenessValuations	= readBooleanArrayArray(fis, ReportConstants.AUT_SER_OBJ_END);
	      } catch (IOException e) {
	    	  isOK = false;
	    	  e.printStackTrace();
	      }
	}

	public String getLabel(int labelLocalIndex) {
		if(getContext().getAlphabet().getSignals().size() <= labelLocalIndex)
			return "__tau__";
		return getContext().getAlphabet().getSignals().get(labelLocalIndex).getName();
	}


	public String getAutomatonInfo() {
		String retValue		= "<html>Name:<b>[" + name + "]</b></br>Local Alphabet:[";
		boolean firstValue	= true;
		for(int i : getLocalAlphabet()) {
			if(firstValue)firstValue = false;
			else	retValue 	+= ",";
			retValue		+= getLabel(i);
		}
		retValue			+= "]</br>Fluents:[";
		firstValue	= true;
		for(String s : getContext().getfluents()) {
			if(firstValue)firstValue = false;
			else	retValue 	+= ",";
			retValue		+= s;
		}
		for(String s : getContext().getLivenessNames()) {
			if(firstValue)firstValue = false;
			else	retValue 	+= ",";
			retValue		+= s;
		}
		retValue			+= "]</html>";
		return retValue;
	}
*/
