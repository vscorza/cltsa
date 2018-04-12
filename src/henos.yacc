%{
#include <stdio.h>
#include "parser_utils.h"
int yylex(void);
void yyerror(char *);
int sym[26];
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
	automaton_state_syntax*			state;
};
%token	t_INTEGER t_IDENT t_UPPER_IDENT t_STRING
%left '+' '-' ','
%left '*' '/'


%type<text> 	t_STRING t_IDENT t_UPPER_IDENT
%type<integer>	t_INTEGER
%type<expr>		exp exp2 exp3 exp4 constDef range rangeDef ltsTransitionPrefix
%type<label>	label concurrentLabel 
%type<set>		set setExp concurrentLabels  labels fluentSet
%type<set_def>	setDef
%type<fluent>	fluentDef
%type<index>	index
%type<indexes>	indexes
%type<trace_label_atom>		ltsSimpleTraceLabel
%type<trace_label>			ltsTraceLabel
%type<transition>			ltsTrace ltsTransition
%type<state>	ltsStateLabel
%%
program:
	statements
	;
statements:
	statement statements
	|statement
	;
statement:
	import
	|menu
	|constDef
	|rangeDef
	|fluentDef
	|assertionDef
	|setDef
	|compositionDef
	|goalDef
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
	'{' concurrentLabels '}'				{$$ = automaton_label_syntax_create(true, $2, NULL);}
	;
concurrentLabels:
	concurrentLabels '+' t_IDENT			{$$ = automaton_set_syntax_concat_concurrent($1, $3);}
	|t_IDENT								{$$ = automaton_set_syntax_create_concurrent($1);}
	;
setDef:
	"set" t_UPPER_IDENT '=' setExp			{$$ = automaton_set_def_syntax_create($4, $2);}
	;
setExp:
	set										{$$ = $1;}
	|setExp "\\" set						{$$ = $1;/*TODO set diff*/}
	|setExp '+' set							{$$ = $1;/*TODO set add*/}
	;
set:
	t_UPPER_IDENT							{$$ = automaton_set_syntax_create_from_ident($1);}
	| '{' labels '}'						{$$ = $2;}
	;
fluentDef:
	"fluent" t_UPPER_IDENT '=' '<' fluentSet ',' fluentSet '>'	{$$ = automaton_fluent_syntax_create($2, $5, $7);}
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
	index indexes							{$$ = automaton_indexes_syntax_add_index($2, $1);}
	|index									{$$ = automaton_indexes_syntax_create($1);}
	|										{}
	;
index:
	'[' exp ']'								{$$ = automaton_index_syntax_create(true, false, $2, NULL, NULL);}
	|'[' t_IDENT ':' t_UPPER_IDENT ']'		{$$ = automaton_index_syntax_create(false, false, NULL, $2, $4);}
	|'[' t_IDENT ':' range ']'				{$$ = automaton_index_syntax_create(false, true, $4, $2, NULL);}
	;	
rangeDef:
	"range" t_UPPER_IDENT '=' range			{$$ = automaton_expression_syntax_create(RANGE_DEF_TYPE_AUT, $4, NULL, $2, 0, NOP_AUT);}
	;
range:
	exp ".." exp							{$$ = automaton_expression_syntax_create(RANGE_TYPE_AUT, $1, $3, NULL, 0, RANGE_OP_AUT);}
	;
constDef:
	"const" t_UPPER_IDENT '=' exp			{$$ = automaton_expression_syntax_create(CONST_TYPE_AUT, $4, NULL, $2, 0, NOP_AUT);}
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
	t_IDENT									{$$ = automaton_expression_syntax_create(IDENT_TERMINAL_TYPE_AUT, NULL, NULL, $1, 0, NOP_AUT);}
	|t_UPPER_IDENT							{$$ = automaton_expression_syntax_create(UPPER_IDENT_TERMINAL_TYPE_AUT, NULL, NULL, $1, 0, NOP_AUT);}
	|t_INTEGER								{$$ = automaton_expression_syntax_create(INTEGER_TERMINAL_TYPE_AUT, NULL, NULL, NULL, $1, NOP_AUT);}
	|'(' exp ')'							{$$ = automaton_expression_syntax_create(PARENTHESIS_TYPE_AUT, $2, NULL, NULL, 0, NOP_AUT);}
	;
ltsStates:
	ltsStates ',' ltsState
	| ltsState
	;
ltsStateLabel:
	t_UPPER_IDENT indexes
	;
ltsState:
	ltsStateLabel '=' '(' ltsTransitions ')'
	| ltsStateLabel '=' ltsStateLabel
	;
ltsTransitions:
	ltsTransition
	| ltsTransitions '|' ltsTransition
	;
ltsTransition:
	ltsTransitionPrefix ltsTrace "->" ltsStateLabel {$$ = automaton_transition_syntax_finish($1, $2, $4);}
	;
ltsTransitionPrefix:
	"when" '(' exp ')'						{$$ = $3;}
	|										{}
	;
ltsTrace:
	ltsTraceLabel							{$$ = automaton_transition_syntax_create_from_trace($1);}
	| ltsTrace "->" ltsTraceLabel			{$$ = automaton_transition_syntax_add_trace($1, $3);}
	;
ltsTraceLabel:
	ltsSimpleTraceLabel						{$$ = automaton_trace_label_syntax_create($1);}
	|ltsTraceLabel '.' ltsSimpleTraceLabel	{$$ = automaton_trace_label_syntax_add_atom($1, $3);}
	;
ltsSimpleTraceLabel:
	|label indexes							{$$ = automaton_trace_label_atom_syntax_create($1, $2);}
	| index									{$$ = automaton_trace_label_atom_syntax_create_from_index($1);}
	;
compositionDef:
	composition '.'
	;
composition:
	ltsStates
	| "||" t_UPPER_IDENT '=' '(' compositionExp ')'
	;
compositionExp:
	compositionExp2
	|compositionExp "||" compositionExp2
	;
compositionExp2:
	t_UPPER_IDENT
	|t_IDENT indexes ':' t_UPPER_IDENT
	|index indexes ':' t_UPPER_IDENT
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
	|compositionExp "||" compositionExp2
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
	|ltlExp "||" ltlExp2
	|ltlExp "++" ltlExp2
	|ltlExp "+ca" ltlExp2
	|ltlExp "+cr" ltlExp2
	|ltlExp "U" ltlExp2
	|ltlExp "W" ltlExp2
	|ltlExp "->" ltlExp2
	|ltlExp "<->" ltlExp2
	|ltlExp "&&" ltlExp2
	;
ltlExp2:
	label
	|'!' ltlExp2
	|'X' ltlExp2
	|"<>" ltlExp2
	|"[]" ltlExp2
	|'(' ltlExp ')'
	;
%%
 int main (void) {return yyparse ( );}

 int yylex (void) {return getchar ( );}

 void yyerror (char *s) {fprintf (stderr, "%s\n", s);}
