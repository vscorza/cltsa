/*
 * automaton_context.h
 *
 *  Created on: Apr 19, 2018
 *      Author: mariano
 */

#ifndef AUTOMATON_CONTEXT_H_
#define AUTOMATON_CONTEXT_H_

#include "automaton.h"
#include "parser_utils.h"

/****************
==== STRUCTS ====
*/
typedef struct automaton_context_str{
	char* name;
}automaton_context;

automaton_context* automaton_context_create_from_syntax(automaton_program_syntax* program, char* ctx_name);

#endif /* AUTOMATON_CONTEXT_H_ */
