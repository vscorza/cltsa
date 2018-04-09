%{
#include <stdio.h>
int yylex(void);
void yyerror(char *);
int sym[26];
%}
%token	t_INTEGER t_IDENT t_UPPER_IDENT t_STRING
%left '+' '-'
%left '*' '/'
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
	t_IDENT
	|set
	;

labels:
	labels ',' label
	|label
	;

import:
	'import' t_UPPER_IDENT '=' t_STRING
	;

menu:
	'menu' t_UPPER_IDENT '=' t_STRING
	;

rangeDef:
	'range' t_UPPER_IDENT '=' range
	;

range:
	exp '..' exp
	;

constDef:
	'const' t_UPPER_IDENT '=' exp
	;

exp:
	exp2
	| exp '==' exp2
	| exp '!=' exp2
	| exp '>=' exp2
	| exp '<=' exp2
	| exp '>' exp2
	| exp '<' exp2
	;

exp2:
	exp3
	|exp2 '+' exp3
	|exp2 '-' exp3
	;

exp3:
	exp4
	|exp3 '*' exp4
	|exp3 '/' exp4
	|exp3 '%' exp4
	;

exp4:
	t_IDENT
	|t_UPPER_IDENT
	|t_INTEGER
	|'(' exp ')'
	;

fluentDef:
	'fluent' t_UPPER_IDENT '=' '<' fluentSet ',' fluentSet '>'
	;

fluentSet:
	t_IDENT
	|setExp
	;

setDef:
	'set' t_UPPER_IDENT '=' setExp
	;

setExp:
	set
	|setExp '\' set
	|setExp '+' set
	;

set:
	t_UPPER_IDENT
	| '{' labels '}'
	;
	
assertionDef:
	assertionKind assertion
	;

assertionKind:
	'assert'
	| 'constraint'
	| 'ltl_property'
	;

assertion:
	t_UPPER_IDENT '=' ltlExp assertionSuffix
	;

assertionSuffix:
	'+' set
	|
	;

ltsStates:
	ltsStates ',' ltsState
	| ltsState
	;

ltsStateLabel:
	t_UPPER_IDENT indexes
	;	

indexes:
	index indexes
	|
	;

index:
	'[' exp ']'
	|'[' t_IDENT ':' t_UPPER_IDENT ']'
	|'[' t_IDENT ':' range ']'
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
	ltsTransitionPrefix ltsTrace '->' ltsStateLabel
	;

ltsTransitionPrefix:
	'when' '(' exp ')'
	|
	;

ltsTrace:
	ltsTraceLabel
	| ltsTraceLabel '->' ltsTraceLabel
	;

ltsSimpleTraceLabel:
	label indexes
	| index
	;

compositionDef:
	compositionKind composition '.'
	;

compositionKind:
	'clousure'
	|'abstract'
	|'deterministic'
	|'minimal'
	|'compose'
	|'property'
	|'optimistic'
	|'pessimistic'
	|'component'
	|'controller'
	|'starenv'
	|'plant'
	|'checkCompatibility'
	|'probabilistic'
	|'mdp'
	|
	;	
composition:
	ltsStates
	| '||' t_UPPER_IDENT '=' '(' compositionExp ')' compositionSuffix
	;

compositionSuffix:
	'~' '{' t_UPPER_IDENT '}'
	|'@' set
	|
	;
                      |
compositionExp:
	compositionExp2
	|compositionExp '||' compositionExp2
	|compositionExp '++' compositionExp2
	|compositionExp '+ca' compositionExp2
	|compositionExp '+cr' compositionExp2
	;

compositionExp2:
	t_UPPER_IDENT
	|t_IDENT indexes ':' t_UPPER_IDENT
	|index indexes ':' t_UPPER_IDENT
	;

goalDef:
	'controllerSpec' t_UPPER_IDENT '=' '{' goalBody '}'
	;

goalBody:
	goalSafety goalFailure goalAssumption goalLiveness goalControllable
	;

goalSafety:
	'safety' '=' goalExp
	|
	;

goalFailure:
	'failure' '=' goalExp
	|
	;

goalAssumption:
	'assumption' '=' goalExp
	|
	;

goalLiveness:
	'liveness' '=' goalExp
	|
	;

goalControllable:
	'controllable' '=' goalExp
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
	|ltlExp '||' ltlExp2
	|ltlExp '++' ltlExp2
	|ltlExp '+ca' ltlExp2
	|ltlExp '+cr' ltlExp2
	|ltlExp 'U' ltlExp2
	|ltlExp 'W' ltlExp2
	|ltlExp '->' ltlExp2
	|ltlExp '<->' ltlExp2
	|ltlExp '&&' ltlExp2
	;

ltlExp2:
	label
	|'!' ltlExp2
	|'X' ltlExp2
	|'<>' ltlExp2
	|'[]' ltlExp2
	|'(' ltlExp ')'
	;
%%
void yyerror(char* s){
	fprintf(stderr, "%s\n", s);
}
int main(void){
	yyparse();
	return 0;
}

