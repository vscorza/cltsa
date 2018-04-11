%{
#include <stdio.h>
#include "parser_utils.h"
int yylex(void);
void yyerror(char *);
int sym[26];
%}
%union{
	char* text;
	int32_t integer;
	automaton_expression* expr;
};
%token	t_INTEGER t_IDENT t_UPPER_IDENT t_STRING
%left '+' '-'
%left '*' '/'

%type<text> t_STRING t_IDENT t_UPPER_IDENT
%type<integer> t_INTEGER
%type<expr> exp exp2 exp3 exp4
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
	"import" t_UPPER_IDENT '=' t_STRING
	;
menu:
	"menu" t_UPPER_IDENT '=' t_STRING		{}
	;
rangeDef:
	"range" t_UPPER_IDENT '=' range			{}
	;
range:
	exp ".." exp							{}
	;
constDef:
	"const" t_UPPER_IDENT '=' exp			{}
	;
exp:
	exp2									{$$ = $1;}
	| exp "==" exp2							{$$ = automaton_expression_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, EQ_OP_AUT);}
	| exp "!=" exp2							{$$ = automaton_expression_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, NEQ_OP_AUT);}
	| exp ">=" exp2							{$$ = automaton_expression_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, GE_OP_AUT);}
	| exp "<=" exp2							{$$ = automaton_expression_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, LE_OP_AUT);}
	| exp ">" exp2							{$$ = automaton_expression_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, GT_OP_AUT);}
	| exp "<" exp2							{$$ = automaton_expression_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, LT_OP_AUT);}
	;
exp2:
	exp3									{$$ = $1;}
	|exp2 '+' exp3							{$$ = automaton_expression_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, PLUS_OP_AUT);}
	|exp2 '-' exp3							{$$ = automaton_expression_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, MINUS_OP_AUT);}
	;
exp3:
	exp4									{$$ = $1;}
	|exp3 '*' exp4							{$$ = automaton_expression_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, MUL_OP_AUT);}
	|exp3 '/' exp4							{$$ = automaton_expression_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, DIV_OP_AUT);}
	|exp3 '%' exp4							{$$ = automaton_expression_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, MOD_OP_AUT);}
	;
exp4:
	t_IDENT									{$$ = automaton_expression_create(IDENT_TERMINAL_TYPE_AUT, NULL, NULL, $1, 0, 0);}
	|t_UPPER_IDENT							{$$ = automaton_expression_create(UPPER_IDENT_TERMINAL_TYPE_AUT, NULL, $1, NULL, 0, 0);}
	|t_INTEGER								{$$ = automaton_expression_create(INTEGER_TERMINAL_TYPE_AUT, NULL, NULL, NULL, $1, 0);}
	|'(' exp ')'							{$$ = automaton_expression_create(PARENTHESIS_TYPE_AUT, $2, NULL, NULL, 0, 0);}
	;
fluentDef:
	"fluent" t_UPPER_IDENT '=' '<' fluentSet ',' fluentSet '>'
	;
fluentSet:
	t_IDENT
	|setExp
	;
setDef:
	"set" t_UPPER_IDENT '=' setExp
	;
setExp:
	set
	|setExp "\\" set
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
	ltsTransitionPrefix ltsTrace "->" ltsStateLabel
	;
ltsTransitionPrefix:
	"when" '(' exp ')'
	|
	;
ltsTrace:
	ltsTraceLabel
	| ltsTraceLabel "->" ltsTraceLabel
	;
ltsTraceLabel:
	ltsSimpleTraceLabel
	|ltsTraceLabel '.' ltsSimpleTraceLabel
	;
ltsSimpleTraceLabel:
	label indexes
	| index
	;
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
	;                      |
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
