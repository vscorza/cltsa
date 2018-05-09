%{
#include <stdio.h>
#include "parser_utils.h"
int yylex(void);
void yyerror(char *);
int sym[26];
int yydebug=5;
automaton_program_syntax* parsed_program = NULL;
%}
%union{
	char* 							text;
	int32_t 						integer;
	automaton_expression_syntax* 	expr;
	automaton_label_syntax* 		label;
	automaton_set_syntax* 			set;
	automaton_set_def_syntax* 		set_def;
	automaton_fluent_syntax* 		fluent;
	automaton_index_syntax* 		index;
	automaton_indexes_syntax* 		indexes;
	automaton_trace_label_atom_syntax*	trace_label_atom;
	automaton_trace_label_syntax*	trace_label;
	automaton_transition_syntax*	transition;
	automaton_transitions_syntax*	transitions;
	automaton_state_syntax*			state;
	automaton_state_label_syntax*	state_label;
	automaton_states_syntax*		states;
	automaton_composition_syntax*	composition;
	automaton_components_syntax*	components;
	automaton_component_syntax*		component;
	automaton_statement_syntax*		statement;
	automaton_program_syntax*		program;
};
%token	t_INTEGER t_IDENT t_UPPER_IDENT t_STRING t_CONST t_RANGE t_SET t_FLUENT
%left '+' '-' ','
%left '*' '/'


%type<text> 				t_STRING t_IDENT t_UPPER_IDENT t_CONST t_RANGE t_SET t_FLUENT
%type<integer>				t_INTEGER
%type<expr>					exp exp2 exp3 exp4 constDef range rangeDef ltsTransitionPrefix
%type<label>				label concurrentLabel 
%type<set>					set setExp concurrentLabels  labels fluentSet
%type<set_def>				setDef
%type<fluent>				fluentDef
%type<index>				index
%type<indexes>				indexes
%type<trace_label_atom>		ltsSimpleTraceLabel
%type<trace_label>			ltsTraceLabel
%type<transition>			ltsTrace ltsTransition
%type<state>				ltsState
%type<state_label>			ltsStateLabel
%type<transitions>			ltsTransitions
%type<states>				ltsStates
%type<composition>			compositionDef composition
%type<components>			compositionExp
%type<component>			compositionExp2
%type<statement>			statement
%type<program>				program statements		
%%
program:
	statements								{parsed_program = $1; $$ = $1;}
	;
statements:
	statement statements					{$$ = automaton_program_syntax_add_statement($2, $1);}
	|statement								{$$ = automaton_program_syntax_create($1);}
	;
statement:
	import									{$$ = automaton_statement_syntax_create(IMPORT_AUT, NULL, NULL, NULL, NULL, NULL);}
	|menu									{$$ = automaton_statement_syntax_create(MENU_AUT, NULL, NULL, NULL, NULL, NULL);}
	|constDef								{$$ = automaton_statement_syntax_create(CONST_AUT, NULL, NULL, $1, NULL, NULL);}
	|rangeDef								{$$ = automaton_statement_syntax_create(RANGE_AUT, NULL, $1, NULL, NULL, NULL);}
	|fluentDef								{$$ = automaton_statement_syntax_create(FLUENT_AUT, NULL, NULL, NULL, $1, NULL);}
	//|assertionDef							
	|setDef									{$$ = automaton_statement_syntax_create(SET_AUT, NULL, NULL, NULL, NULL, $1);}
	|compositionDef							{$$ = automaton_statement_syntax_create(COMPOSITION_AUT, $1, NULL, NULL, NULL, NULL);}
	//|goalDef
	;
label:
	concurrentLabel							{$$ = $1;}
	|set									{$$ = automaton_label_syntax_create(true, $1, NULL);}
	|t_IDENT								{$$ = automaton_label_syntax_create(false, NULL, $1);}
	;
labels:
	labels ',' label						{$$ = automaton_set_syntax_concat_labels($1,$3);}
	|label									{$$ = automaton_set_syntax_create_from_label($1);}
	;
concurrentLabel:
	'<' concurrentLabels '>'			{$$ = automaton_label_syntax_create(true, $2, NULL);}
	;
concurrentLabels:
	concurrentLabels ',' t_IDENT			{$$ = automaton_set_syntax_concat_concurrent($1, $3);}
	|t_IDENT								{$$ = automaton_set_syntax_create_concurrent($1);}
	;
setDef:
	t_SET t_UPPER_IDENT '=' setExp			{$$ = automaton_set_def_syntax_create($4, $2);}
	;
setExp:
	set										{$$ = $1;}
	|setExp "\\" set						{$$ = $1;/*TODO set diff*/}
	|setExp '+' set							{$$ = $1;/*TODO set add*/}
	;
set:
	'{' labels '}'						{$$ = $2;}
	;
/*TODO: ref defs of set was removed because it introduced ambiguity as label in the state trace
set:
	t_UPPER_IDENT							{$$ = automaton_set_syntax_create_from_ident($1);}
	| '{' labels '}'						{$$ = $2;}
	;
	*/
fluentDef:
	t_FLUENT t_UPPER_IDENT '=' '<' fluentSet ',' fluentSet '>'	{$$ = automaton_fluent_syntax_create($2, $5, $7);}
	;
fluentSet:
	t_IDENT									{$$ = automaton_set_syntax_create_concurrent($1);}
	|setExp									{$$ = $1;}
	;	
import:
	"import" t_UPPER_IDENT '=' t_STRING
	;
menu:
	"menu" t_UPPER_IDENT '=' t_STRING	
	;
indexes:
	indexes index							{$$ = automaton_indexes_syntax_add_index($1, $2);}
	|index									{$$ = automaton_indexes_syntax_create($1);}
	|										{$$ = NULL;}
	;
index:
	'[' exp ']'								{$$ = automaton_index_syntax_create(true, false, $2, NULL, NULL);}
	|'[' t_IDENT ':' t_UPPER_IDENT ']'		{$$ = automaton_index_syntax_create(false, false, NULL, $2, $4);}
	|'[' t_IDENT ':' range ']'				{$$ = automaton_index_syntax_create(false, true, $4, $2, NULL);}
	;	
rangeDef:
	t_RANGE t_UPPER_IDENT '=' range			{$$ = automaton_expression_syntax_create(RANGE_DEF_TYPE_AUT, $4, NULL, $2, 0, NOP_AUT);}
	;
range:
	exp ".." exp							{$$ = automaton_expression_syntax_create(RANGE_TYPE_AUT, $1, $3, NULL, 0, RANGE_OP_AUT);}
	;
constDef:
	t_CONST t_UPPER_IDENT '=' exp			{$$ = automaton_expression_syntax_create(CONST_TYPE_AUT, $4, NULL, $2, 0, NOP_AUT);}
	;
exp:
	exp2									{$$ = $1;}
	| exp "==" exp2							{$$ = automaton_expression_syntax_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, EQ_OP_AUT);}
	| exp "!=" exp2							{$$ = automaton_expression_syntax_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, NEQ_OP_AUT);}
	| exp ">=" exp2							{$$ = automaton_expression_syntax_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, GE_OP_AUT);}
	| exp "<=" exp2							{$$ = automaton_expression_syntax_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, LE_OP_AUT);}
	| exp ">" exp2							{$$ = automaton_expression_syntax_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, GT_OP_AUT);}
	| exp "<" exp2							{$$ = automaton_expression_syntax_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, LT_OP_AUT);}
	;
exp2:
	exp3									{$$ = $1;}
	|exp2 '+' exp3							{$$ = automaton_expression_syntax_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, PLUS_OP_AUT);}
	|exp2 '-' exp3							{$$ = automaton_expression_syntax_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, MINUS_OP_AUT);}
	;
exp3:
	exp4									{$$ = $1;}
	|exp3 '*' exp4							{$$ = automaton_expression_syntax_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, MUL_OP_AUT);}
	|exp3 '/' exp4							{$$ = automaton_expression_syntax_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, DIV_OP_AUT);}
	|exp3 '%' exp4							{$$ = automaton_expression_syntax_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, MOD_OP_AUT);}
	;
exp4:
	t_UPPER_IDENT							{$$ = automaton_expression_syntax_create(UPPER_IDENT_TERMINAL_TYPE_AUT, NULL, NULL, $1, 0, NOP_AUT);}
	|t_INTEGER								{$$ = automaton_expression_syntax_create(INTEGER_TERMINAL_TYPE_AUT, NULL, NULL, NULL, $1, NOP_AUT);}
	|'(' exp ')'							{$$ = automaton_expression_syntax_create(PARENTHESIS_TYPE_AUT, $2, NULL, NULL, 0, NOP_AUT);}
	;
ltsStates:
	ltsStates ',' ltsState					{$$ = automaton_states_syntax_add_state($1, $3);}
	| ltsState								{$$ = automaton_states_syntax_create($1);}
	;
ltsStateLabel:
	t_UPPER_IDENT indexes					{$$ = automaton_state_label_syntax_create($1, $2);}
	;
ltsState:
	ltsStateLabel '=' '(' ltsTransitions ')'{$$ = automaton_state_syntax_create(false, $1, NULL, $4);free($4);}
	| ltsStateLabel '=' ltsStateLabel		{$$ = automaton_state_syntax_create(true, $1, $3, NULL);}
	;
ltsTransitions:
	ltsTransition							{$$ = automaton_transitions_syntax_create($1);}
	| ltsTransitions '|' ltsTransition		{$$ = automaton_transitions_syntax_add_transition($1, $3);}
	;
ltsTransition:
	ltsTransitionPrefix ltsTrace '-''>' ltsStateLabel {$$ = automaton_transition_syntax_finish($1, $2, $5);}
	;
ltsTransitionPrefix:
	"when" '(' exp ')'						{$$ = $3;}
	|										{$$ = NULL;}
	;
ltsTrace:
	ltsTraceLabel							{$$ = automaton_transition_syntax_create_from_trace($1);}
	| ltsTrace '-''>' ltsTraceLabel			{$$ = automaton_transition_syntax_add_trace($1, $4);}
	;
ltsTraceLabel:
	ltsSimpleTraceLabel						{$$ = automaton_trace_label_syntax_create($1);}
	|ltsTraceLabel '.' ltsSimpleTraceLabel	{$$ = automaton_trace_label_syntax_add_atom($1, $3);}
	;
ltsSimpleTraceLabel:
	label indexes							{$$ = automaton_trace_label_atom_syntax_create($1, $2);}
	| index									{$$ = automaton_trace_label_atom_syntax_create_from_index($1);}
	;
compositionDef:
	composition '.'							{$$ = $1;}
	;
composition:
	ltsStates								{$$ = automaton_composition_syntax_create_from_states($1); free($1);}
	| '|' '|' t_UPPER_IDENT '=' '(' compositionExp ')'	{$$ = automaton_composition_syntax_create_from_ref($3, $6); free($6);}
	;
compositionExp:
	compositionExp2							{$$ = automaton_components_syntax_create($1);}
	|compositionExp '|''|' compositionExp2	{$$ = automaton_components_syntax_add_component($1, $4);}
	;
compositionExp2:
	t_UPPER_IDENT							{$$ = automaton_component_syntax_create($1, NULL, NULL, NULL);}
	|t_IDENT indexes ':' t_UPPER_IDENT		{$$ = automaton_component_syntax_create($4, $1, NULL, $2);}
	|index indexes ':' t_UPPER_IDENT		{$$ = automaton_component_syntax_create($4, NULL, $1, $2);}
	;
/*
compositionDef:
	compositionKind composition '.'
	;
compositionKind:
	"clousure"
	|"abstract"
	|"deterministic"
	|"minimal"
	|"compose"
	|"property"
	|"optimistic"
	|"pessimistic"
	|"component"
	|"controller"
	|"starenv"
	|"plant"
	|"checkCompatibility"
	|"probabilistic"
	|"mdp"
	|
	;
composition:
	ltsStates
	| "||" t_UPPER_IDENT '=' '(' compositionExp ')' compositionSuffix
	;
compositionSuffix:
	'~' '{' t_UPPER_IDENT '}'
	|'@' set
	|
	;                      
compositionExp:
	compositionExp2
	|compositionExp '|''|' compositionExp2
	|compositionExp "++" compositionExp2
	|compositionExp "+ca" compositionExp2
	|compositionExp "+cr" compositionExp2
	;
compositionExp2:
	t_UPPER_IDENT
	|t_IDENT indexes ':' t_UPPER_IDENT
	|index indexes ':' t_UPPER_IDENT
	;	
*/	

/*
assertionDef:
	assertionKind assertion
	;
assertionKind:
	"assert"
	| "constraint"
	| "ltl_property"
	;
assertion:
	t_UPPER_IDENT '=' ltlExp assertionSuffix
	;
assertionSuffix:
	'+' set
	|
	;
goalDef:
	"controllerSpec" t_UPPER_IDENT '=' '{' goalBody '}'
	;
goalBody:
	goalSafety goalFailure goalAssumption goalLiveness goalControllable
	;
goalSafety:
	"safety" '=' goalExp
	|
	;
goalFailure:
	"failure" '=' goalExp
	|
	;
goalAssumption:
	"assumption" '=' goalExp
	|
	;
goalLiveness:
	"liveness" '=' goalExp
	|
	;
goalControllable:
	"controllable" '=' goalExp
	;
goalExp:
	'{' goalExp2 '}'
	;
goalExp2:
	t_UPPER_IDENT
	| goalExp2 ',' t_UPPER_IDENT
	|
	;
ltlExp:
	ltlExp
	|ltlExp '|''|' ltlExp2
	|ltlExp '+''+' ltlExp2
	|ltlExp '+''c''a' ltlExp2
	|ltlExp '+''c''r' ltlExp2
	|ltlExp 'U' ltlExp2
	|ltlExp 'W' ltlExp2
	|ltlExp '-''>' ltlExp2
	|ltlExp '<''-''>' ltlExp2
	|ltlExp '&''&' ltlExp2
	;
ltlExp2:
	label
	|'!' ltlExp2
	|'X' ltlExp2
	|'<''>' ltlExp2
	|'[]' ltlExp2
	|'(' ltlExp ')'
	;
	*/
%%
 //int main (void) {return yyparse ( );}

 void yyerror (char *s) {fprintf (stderr, "%s\n", s);}
