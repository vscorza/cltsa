%{
#include <stdio.h>
#include "parser_utils.h"
#include "automaton_common_structs.h"
#include "obdd.h"
int yylex(void);
void yyerror(char *);
int sym[26];
int yydebug=5;
automaton_program_syntax* parsed_program = NULL;
%}
%locations
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
	automaton_gr1_game_syntax*		gr1_game;
	obdd* 							ltl_aut_expression;
	ltl_rule_syntax*				ltl_rule;
	ltl_fluent_syntax*				ltl_fluent;
	automaton_equivalence_check_syntax*		equal_expression;
	automaton_import_syntax*		import_syntax;
	automaton_synchronization_type_syntax composition_type;
	automaton_vstates_syntax*		v_states;
	automaton_vstates_fluent_syntax*	vstates_fluent;
	automaton_serialization_syntax*	lts_serialization;
	automaton_relabel_set_syntax*	relabel_set;
	automaton_hide_set_syntax*		hide_set;
	automaton_relabel_pair_syntax*	relabel_pair;
};
%token	t_INTEGER t_IDENT t_UPPER_IDENT t_STRING t_CONST t_RANGE t_SET t_FLUENT t_DOTS t_WHEN t_GAME_COMPOSE t_GAME_COMPOSE_NO_MIXED_STATES t_PARALLEL t_GR_1 t_INITIALLY t_LTL t_ENV t_SYS t_RHO t_THETA t_IN t_THEN t_IFF t_AND t_NEXT t_CONCURRENT t_SYNCH t_ORDER t_EQUALS t_IMPORT t_EXPORT t_VAL_STATE t_SEQ_LTS t_SEQ_TICK_LTS t_INTERLVD_LTS t_INTERLVD_TICK_LTS t_EXPORT_METRICS 
%left '+' '-' ','
%left '*' '/'


%type<text> 				t_STRING t_IDENT t_UPPER_IDENT t_CONST t_RANGE t_SET t_FLUENT t_DOTS t_WHEN t_GAME_COMPOSE t_GAME_COMPOSE_NO_MIXED_STATES t_PARALLEL t_GR_1 t_INITIALLY t_LTL t_ENV t_SYS t_RHO t_THETA t_IN t_THEN t_IFF t_AND t_NEXT t_CONCURRENT t_SYNCH t_EQUALS t_IMPORT t_EXPORT t_VAL_STATE t_SEQ_LTS t_SEQ_TICK_LTS t_INTERLVD_LTS t_INTERLVD_TICK_LTS t_EXPORT_METRICS
%type<integer>				t_INTEGER fluentInitialCondition compositionType
%type<expr>					exp exp2 exp3 exp4 constDef range rangeDef ltsTransitionPrefix
%type<label>				label concurrentLabel 
%type<set>					set setExp concurrentLabels  labels fluentSet
%type<set_def>				setDef
%type<fluent>				fluentDef
%type<index>				index
%type<indexes>				indexes
%type<trace_label_atom>		ltsSimpleTraceLabel
%type<trace_label>			ltsTraceLabel
%type<trace_label>			ltsTraceLabelSet
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
%type<gr1_game>				gr1
%type<ltl_aut_expression>	ltlAutExp ltlAutExp2
%type<ltl_rule>				ltlAutRule
%type<ltl_fluent>			ltlFluent
%type<equal_expression>		equalsExp
%type<import_syntax>		import
%type<import_syntax>		export
%type<import_syntax>		metrics
%type<vstates_fluent>			stateFluent
%type<v_states>				vstates		
%type<lts_serialization> ltsConversion
%type<relabel_set>	relabelSet relabelExp
%type<hide_set>	hideExp
%type<relabel_pair> relabelPair
%%
program:
	statements								{parsed_program = $1; $$ = $1;}
	;
statements:
	statement statements					{$$ = automaton_program_syntax_add_statement($2, $1);}
	|statement								{$$ = automaton_program_syntax_create($1);}
	;
statement:
	import									{$$ = automaton_statement_syntax_create(IMPORT_AUT, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, $1, NULL, NULL);}
	|export									{$$ = automaton_statement_syntax_create(EXPORT_AUT, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, $1, NULL, NULL);}
	|metrics								{$$ = automaton_statement_syntax_create(METRICS_AUT, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, $1, NULL, NULL);}
	|menu									{$$ = automaton_statement_syntax_create(MENU_AUT, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);}
	|constDef								{$$ = automaton_statement_syntax_create(CONST_AUT, NULL, NULL, $1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);}
	|rangeDef								{$$ = automaton_statement_syntax_create(RANGE_AUT, NULL, $1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);}
	|fluentDef								{$$ = automaton_statement_syntax_create(FLUENT_AUT, NULL, NULL, NULL, $1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);}
	|setDef									{$$ = automaton_statement_syntax_create(SET_AUT, NULL, NULL, NULL, NULL, $1, NULL, NULL, NULL, NULL, NULL, NULL, NULL);}
	|orderDef								{automaton_program_add_obdd_primed_variables();}
	|compositionDef							{$$ = automaton_statement_syntax_create(COMPOSITION_AUT, $1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);}
	|gr1									{$$ = automaton_statement_syntax_create(GR_1_AUT, NULL, NULL, NULL, NULL, NULL, $1, NULL, NULL, NULL, NULL, NULL, NULL);}
	|ltlAutRule								{$$ = automaton_statement_syntax_create(LTL_RULE_AUT, NULL, NULL, NULL, NULL, NULL, NULL, $1, NULL, NULL, NULL, NULL, NULL);}
	|ltlFluent								{$$ = automaton_statement_syntax_create(LTL_FLUENT_AUT, NULL, NULL, NULL, NULL, NULL, NULL, NULL, $1, NULL, NULL, NULL, NULL);}
	|stateFluent							{$$ = automaton_statement_syntax_create(VSTATES_FLUENT_AUT, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, $1, NULL);}	
	|equalsExp								{$$ = automaton_statement_syntax_create(EQUIV_CHECK_AUT, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, $1, NULL, NULL, NULL);}
	|ltsConversion							{$$ = automaton_statement_syntax_create(LTS_SEQ_AUT, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, $1);}
	;
label:
	concurrentLabel							{$$ = $1;}
//	|set									{$$ = automaton_label_syntax_create(true, false, $1, NULL, NULL);}
	|t_IDENT indexes						{$$ = automaton_label_syntax_create(false, false, NULL, $1, $2);free($1);}
	|t_UPPER_IDENT indexes					{$$ = automaton_label_syntax_create(false, false, NULL, $1, $2);free($1);}
	;
labels:
	labels ',' label						{$$ = automaton_set_syntax_concat_labels($1,$3);}
	|label									{$$ = automaton_set_syntax_create_from_label($1);}
	|										{$$	= NULL;}
	;
concurrentLabel:
	'<' concurrentLabels '>' indexes		{$$ = automaton_label_syntax_create(true, true, $2, NULL, $4);}
	|'<' '>'										{$$ = automaton_label_syntax_create_empty();}
	;
concurrentLabels:
	concurrentLabels ',' t_IDENT indexes			{$$ = automaton_set_syntax_concat_concurrent($1, $3, $4);free($3);}
	|t_IDENT indexes								{$$ = automaton_set_syntax_create_concurrent($1, $2);free($1);}
	;
orderDef:
	t_ORDER '=' orderExp					{}
	;
orderExp:
	'{' orderedVariables '}'				{}
	;
orderedVariables:
	t_IDENT '<' orderedVariables			{
	
												uint32_t var_ID = dictionary_add_entry(parser_get_obdd_mgr()->vars_dict, $1);
												char prime_name[255];
												strcpy(prime_name, $1);
												strcat(prime_name, SIGNAL_PRIME_SUFFIX);
												parser_add_primed_variables(dictionary_add_entry(parser_get_obdd_mgr()->vars_dict, prime_name), var_ID);
												free($1);
											}											
	|t_IDENT								{
	
												uint32_t var_ID = dictionary_add_entry(parser_get_obdd_mgr()->vars_dict, $1);
												char prime_name[255];
												strcpy(prime_name, $1);
												strcat(prime_name, SIGNAL_PRIME_SUFFIX);
												parser_add_primed_variables(dictionary_add_entry(parser_get_obdd_mgr()->vars_dict, prime_name), var_ID);
												free($1);
											}
	|										{}
	;
setDef:
	t_SET t_UPPER_IDENT '=' setExp			{$$ = automaton_set_def_syntax_create($4, $2);free($1); free($2);}
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
	t_UPPER_IDENT							{$$ = automaton_set_syntax_create_from_ident($1);free($1);}
	| '{' labels '}'						{$$ = $2;}
	;
	*/
fluentDef:
	t_FLUENT t_UPPER_IDENT indexes '=' '<' fluentSet ',' fluentSet '>' fluentInitialCondition	{$$ = automaton_fluent_syntax_create($2, $6, $8, $10, $3);free($1); free($2);}
	;
fluentInitialCondition:
	t_INITIALLY t_INTEGER					{$$ = $2; free($1);}
	|										{$$ = NULL;}
	;
fluentSet:
	label									{$$ = automaton_set_syntax_create_from_label($1);}
	|setExp									{$$ = $1;}
	;	
import:
	t_UPPER_IDENT t_IMPORT  t_STRING '.'	{$$ = automaton_import_syntax_create($1,$3);free($1);free($2);free($3);}
	;
export:
	t_UPPER_IDENT t_EXPORT  t_STRING '.'	{$$ = automaton_import_syntax_create($1,$3);free($1);free($2);free($3);}
	;
metrics:
	t_UPPER_IDENT t_EXPORT_METRICS  t_STRING '.'	{$$ = automaton_import_syntax_create($1,$3);free($1);free($2);free($3);}
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
	|'[' t_IDENT ':' t_UPPER_IDENT ']'		{$$ = automaton_index_syntax_create(false, false, NULL, $2, $4);free($2);free($4);}
	|'[' t_IDENT ':' range ']'				{$$ = automaton_index_syntax_create(false, true, $4, $2, NULL);free($2);}
	;	
rangeDef:
	t_RANGE t_UPPER_IDENT '=' range			{$$ = automaton_expression_syntax_create(RANGE_DEF_TYPE_AUT, $4, NULL, $2, 0, NOP_AUT);free($1); free($2);}
	;
range:
	exp t_DOTS exp							{$$ = automaton_expression_syntax_create(RANGE_TYPE_AUT, $1, $3, NULL, 0, RANGE_OP_AUT);free($2);}
	;
constDef:
	t_CONST t_UPPER_IDENT '=' exp			{$$ = automaton_expression_syntax_create(CONST_TYPE_AUT, $4, NULL, $2, 0, NOP_AUT);free($1); free($2);}
	;
exp:
	exp2									{$$ = $1;}
	| exp '=''=' exp2							{$$ = automaton_expression_syntax_create(BINARY_TYPE_AUT, $1, $4, NULL, 0, EQ_OP_AUT);}
	| exp '!''=' exp2							{$$ = automaton_expression_syntax_create(BINARY_TYPE_AUT, $1, $4, NULL, 0, NEQ_OP_AUT);}
	| exp '>''=' exp2							{$$ = automaton_expression_syntax_create(BINARY_TYPE_AUT, $1, $4, NULL, 0, GE_OP_AUT);}
	| exp '<''=' exp2							{$$ = automaton_expression_syntax_create(BINARY_TYPE_AUT, $1, $4, NULL, 0, LE_OP_AUT);}
	| exp '>' exp2							{$$ = automaton_expression_syntax_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, GT_OP_AUT);}
	| exp '<' exp2							{$$ = automaton_expression_syntax_create(BINARY_TYPE_AUT, $1, $3, NULL, 0, LT_OP_AUT);}
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
	t_IDENT									{$$ = automaton_expression_syntax_create(IDENT_TERMINAL_TYPE_AUT, NULL, NULL, $1, 0, NOP_AUT);free($1);}
	|t_UPPER_IDENT							{$$ = automaton_expression_syntax_create(UPPER_IDENT_TERMINAL_TYPE_AUT, NULL, NULL, $1, 0, NOP_AUT);free($1);}
	|t_INTEGER								{$$ = automaton_expression_syntax_create(INTEGER_TERMINAL_TYPE_AUT, NULL, NULL, NULL, $1, NOP_AUT);}
	|'(' exp ')'							{$$ = automaton_expression_syntax_create(PARENTHESIS_TYPE_AUT, $2, NULL, NULL, 0, NOP_AUT);}
	;
ltsStates:
	ltsStates ',' ltsState					{$$ = automaton_states_syntax_add_state($1, $3);}
	| ltsState								{$$ = automaton_states_syntax_create($1);}
	;
ltsStateLabel:
	t_UPPER_IDENT indexes					{$$ = automaton_state_label_syntax_create($1, $2);free($1);}
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
	ltsTransitionPrefix ltsTrace t_THEN ltsStateLabel {$$ = automaton_transition_syntax_finish($1, $2, $4);free($3);}
	;
ltsTransitionPrefix:
	t_WHEN '(' exp ')'						{$$ = $3;free($1);}
	|										{$$ = NULL;}
	;
ltsTrace:
	ltsTraceLabel							{$$ = automaton_transition_syntax_create_from_trace($1);}
	| ltsTrace t_THEN ltsTraceLabel			{$$ = automaton_transition_syntax_add_trace($1, $3);free($2);}
	;
ltsTraceLabel:
	ltsSimpleTraceLabel						{$$ = automaton_trace_label_syntax_create($1);}
	| '{' ltsTraceLabelSet '}'				{$$ = $2;}
	
ltsTraceLabelSet:
	ltsSimpleTraceLabel						{$$ = automaton_trace_label_syntax_create($1);}
	| ltsTraceLabelSet ',' ltsSimpleTraceLabel	{$$ = automaton_trace_label_syntax_add_atom($1, $3);}
	;
ltsSimpleTraceLabel:
	label indexes							{$$ = automaton_trace_label_atom_syntax_create($1, $2);}
	| index									{$$ = automaton_trace_label_atom_syntax_create_from_index($1);}
	;
compositionDef:
	composition '.'							{$$ = $1;}
	;
composition:
	ltsStates								{$$ = automaton_composition_syntax_create_from_states($1,NULL); free($1);}
	| ltsStates '+' '{' labels '}'			{$$ = automaton_composition_syntax_create_from_states($1, $4); free($1);}
	| t_PARALLEL t_UPPER_IDENT '=' '(' compositionExp ')'	{$$ = automaton_composition_syntax_create_from_ref($2, $5, false, false); free($1);free($2);free($5);}
	| t_GAME_COMPOSE t_UPPER_IDENT '=' '(' compositionExp ')'	{$$ = automaton_composition_syntax_create_from_ref($2, $5, true, false); free($1);free($2);free($5);}
	| t_GAME_COMPOSE_NO_MIXED_STATES t_UPPER_IDENT '=' '(' compositionExp ')'	{$$ = automaton_composition_syntax_create_from_ref($2, $5, true, true); free($1);free($2);free($5);}		
	;
ltsConversion:
	t_SEQ_LTS t_UPPER_IDENT t_IN t_UPPER_IDENT '.'	{$$ = automaton_serialization_syntax_create_from_ref($2, $4, true, false); free($1);free($2);free($3);free($4);}
	|t_SEQ_TICK_LTS t_UPPER_IDENT t_IN t_UPPER_IDENT '.'	{$$ = automaton_serialization_syntax_create_from_ref($2, $4, true, true); free($1);free($2);free($3);free($4);}
	|t_INTERLVD_LTS t_UPPER_IDENT t_IN t_UPPER_IDENT '.'	{$$ = automaton_serialization_syntax_create_from_ref($2, $4, false, false); free($1);free($2);free($3);free($4);}
	|t_INTERLVD_TICK_LTS t_UPPER_IDENT t_IN t_UPPER_IDENT '.'	{$$ = automaton_serialization_syntax_create_from_ref($2, $4, false, true); free($1);free($2);free($3);free($4);}	
gr1:
	t_GR_1 '<' set '>' '<' set '>' t_UPPER_IDENT '=' t_UPPER_IDENT '.'			{$$ = automaton_gr1_game_syntax_create($8, $10, $3, $6); free($1);free($8); free($10);}
compositionExp:
	compositionExp2							{$$ = automaton_components_syntax_create($1);}
	| '(' compositionExp ')'				{$$ = $2;}
	|compositionExp t_PARALLEL compositionType compositionExp2	{$$ = automaton_components_syntax_add_component($1, $4, $3);free($2);}
	;
compositionExp2:
	t_UPPER_IDENT indexes compositionType relabelExp hideExp	{$$ = automaton_component_syntax_create($1, NULL, NULL, $2, $3, $4, $5);free($1);}
	|t_IDENT indexes ':' t_UPPER_IDENT relabelExp hideExp		{$$ = automaton_component_syntax_create($4, $1, NULL, $2, ASYNCH_AUT, $4, $5);free($1); free($4);}
	|index indexes ':' t_UPPER_IDENT relabelExp hideExp		{$$ = automaton_component_syntax_create($4, NULL, $1, $2, ASYNCH_AUT, $4, $5);free($4);}
	;
relabelExp:
	'/' '{' relabelSet '}'					{$$ = $3;}
	|										{$$ = NULL;}
	;
hideExp:
	'@' '{' labels '}'					{$$ = automaton_hide_set_syntax_create($3, false);}
	|"\\" '{' labels '}'						{$$ = automaton_hide_set_syntax_create($3, true);}
	|									{$$ = NULL;}
	;
	
relabelSet:
	relabelSet ',' relabelPair				{$$ = automaton_relabel_set_concat_labels($1,$3);}
	|relabelPair							{$$ = automaton_relabel_set_create_from_label($1);}
	;	 
	
relabelPair:
	label '/' label							{$$ = automaton_relabel_pair_create($1, $3);}
	;

compositionType:
	t_CONCURRENT							{$$ = CONCURRENT_AUT;free($1);}
	|t_SYNCH								{$$ = SYNCH_AUT; free($1);}
	| 										{$$ = ASYNCH_AUT;}
	;
equalsExp:
	t_EQUALS t_UPPER_IDENT '(' t_UPPER_IDENT ',' t_UPPER_IDENT ')' '.'	{$$ = automaton_equality_check_syntax_create($2,$4,$6); free($1); free($2);free($4);free($6);}

vstates:
	vstates ',' ltsStateLabel						{$$ = automaton_vstates_syntax_concat_state($1,$3);}
	|vstates ',' t_INTEGER						{$$ = automaton_vstates_syntax_concat_int($1,$3);}	
	|ltsStateLabel									{$$ = automaton_vstates_syntax_create_from_state($1);}
	|t_INTEGER										{$$ = automaton_vstates_syntax_create_from_int($1);}	
	|										{$$	= NULL;}
	;	
stateFluent:
	t_VAL_STATE t_FLUENT t_UPPER_IDENT t_IN t_UPPER_IDENT  '=' '{' vstates '}' '.'	{$$ = automaton_vstates_fluent_syntax_create($3, $5, $8);free($1); free($2);free($3);free($4);free($5);}
ltlFluent:
	t_LTL t_FLUENT t_UPPER_IDENT '=' ltlAutExp '.'	{$$ = automaton_ltl_fluent_syntax_create($3, $5);free($1); free($2);free($3);}
ltlAutRule:
	t_LTL t_ENV t_THETA t_UPPER_IDENT t_IN t_UPPER_IDENT '=' ltlAutExp '.'			{$$ = ltl_rule_syntax_create(true, true, $4, $6, $8);free($1);free($2);free($3);free($4);free($5);free($6); }
	|t_LTL t_SYS t_THETA t_UPPER_IDENT t_IN t_UPPER_IDENT '=' ltlAutExp '.'			{$$ = ltl_rule_syntax_create(true, false, $4, $6, $8);free($1);free($2);free($3);free($4);free($5);free($6); }
	|t_LTL t_ENV t_RHO t_UPPER_IDENT t_IN t_UPPER_IDENT '=' '['']' ltlAutExp '.'		{$$ = ltl_rule_syntax_create(false, true, $4, $6, $10);free($1);free($2);free($3);free($4);free($5);free($6); }
	|t_LTL t_SYS t_RHO t_UPPER_IDENT t_IN t_UPPER_IDENT '=' '['']' ltlAutExp '.'		{$$ = ltl_rule_syntax_create(false, false, $4, $6, $10);free($1);free($2);free($3);free($4);free($5);free($6); }
	;

ltlAutExp:
	ltlAutExp2								{$$ = $1;}	
	|ltlAutExp t_THEN ltlAutExp2			{obdd* obdd_not = obdd_apply_not($1); $$ = obdd_apply_or(obdd_not, $3);obdd_destroy(obdd_not);obdd_destroy($1);obdd_destroy($3);free($2);}
	|ltlAutExp t_IFF ltlAutExp2				{$$ = obdd_apply_equals($1, $3);obdd_destroy($1);obdd_destroy($3);free($2);}
	|ltlAutExp t_AND ltlAutExp2				{$$ = obdd_apply_and($1, $3);obdd_destroy($1);obdd_destroy($3);free($2);}
	|ltlAutExp t_PARALLEL ltlAutExp2		{$$ = obdd_apply_or($1, $3);obdd_destroy($1);obdd_destroy($3);free($2);}
	;	
ltlAutExp2:
	t_IDENT									{
												//we force the addition of the prime variable to the mgr dictionary in order to keep both orders (primed and non primed) consistent
												//in order to then just replace the indexes when applying the next operator
												$$ = obdd_mgr_var(parser_get_obdd_mgr(), $1);
												
												char prime_name[255];
												strcpy(prime_name, $1);
												strcat(prime_name, SIGNAL_PRIME_SUFFIX);
												//dictionary_add_entry(parser_get_obdd_mgr()->vars_dict, prime_name);
												parser_add_primed_variables(dictionary_add_entry(parser_get_obdd_mgr()->vars_dict, prime_name), $$->root_obdd->var_ID);
												
												free($1);
											}
	|'!' ltlAutExp2							{$$ = obdd_apply_not($2);obdd_destroy($2);}
	|t_NEXT ltlAutExp2						{$$ = obdd_apply_next($2);free($1);}
	|'(' ltlAutExp ')'						{$$ = $2;}
	;
%%
 //int main (void) {return yyparse ( );}

void yyerror(char *s)
{
    fprintf(stderr,"Error | Line: %d\n%s\n",yylloc.first_line,s);
}

