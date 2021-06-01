%{
#include <stdlib.h>
#include "parser_utils.h"
#include "y.tab.h"
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;
#define BEBUG_LEX 0
char *p;
%}
%option yylineno
%x C_COMMENT
digit		[0-9]
alpha		[a-fA-F]
hextail		({digit}|{alpha}){1,8}
hex			0[xX]{hextail}
lower 		[a-z■]
upper 		[A-Z]
octalDigit	[0-7]
decDigit	[0-9]
identChars	{lower}|{upper}|{digit}|[_?]
lowerIdentChars	{lower}|{upper}|{digit}|[_?\.]
number 		0{octalDigit}*|{decDigit}+|{hex}+
ident 		{lower}{lowerIdentChars}*
upperIdent 	{upper}{identChars}*
string 		\"(\\.|[^"\\])*\"
keyword		set|range|const|\.\.|when|\|f\||\|\||\|gr1\|initially|ltl|env|sys|rho|theta|in|order|equals|vstate
%{
#include <stdlib.h>
void yyerror(char*);
#include "y.tab.h"
%}
%%
"/*"            { BEGIN(C_COMMENT); }
<C_COMMENT>"*/" { BEGIN(INITIAL); }
<C_COMMENT>\n   { }
<C_COMMENT>.    { }
\/\/[^\r\n]*	{ }
set				{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_SET);
				}
range				{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_RANGE);
				}
const				{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_CONST);
				}
initially		{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_INITIALLY);
				}
\|\|				{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_PARALLEL);
				}
\.\.				{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_DOTS);
				}
\|s\|			{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_SEQ_LTS);
				}
\|ts\|			{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_SEQ_TICK_LTS);
				}
\|i\|			{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_INTERLVD_LTS);
				}
\|ti\|			{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_INTERLVD_TICK_LTS);
				}												
\|f\|			{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_GAME_COMPOSE);
				}
\|gr1\|			{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_GR_1);
				}		
fluent				{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_FLUENT);
				}
when			{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_WHEN);
				}
equals			{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_EQUALS);
				}
vstate				{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_VAL_STATE);
				}
ltl				{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_LTL);
				}
order			{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_ORDER);
				}				
env				{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_ENV);
				}
sys				{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_SYS);
				}
rho				{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_RHO);
				}				
theta			{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_THETA);
				}
in				{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_IN);
				}
-\>				{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_THEN);
				}
\<-\>			{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_IFF);
				}
\<\<			{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_IMPORT);
				}
\>\>			{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_EXPORT);
				}				
&&				{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_AND);
				}
X				{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_NEXT);
				}
_c			{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_CONCURRENT);
				}				
_s			{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[keyword: <%s>]\n", yytext);
					#endif
					yylval.text=p;
					return(t_SYNCH);
				}				
{ident}			{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[t_IDENT: <%s>]", yytext);
					#endif
					yylval.text=p;
					return(t_IDENT);
				}
{upperIdent}	{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					#if DEBUG_LEX
					printf("[t_UPPER_IDENT: <%s>]\n", yytext);
					#endif
					yylval.text=p;
					return(t_UPPER_IDENT);
				}
{string}	{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					#if DEBUG_LEX
					printf("[t_STRING: <%s>]", yytext);
					#endif
					strcpy(p,yytext);
					yylval.text=p;
					return(t_STRING);
				}
{number}		{
					#if DEBUG_LEX
					printf("[t_INTEGER: <%s>]", yytext);
					#endif
					yylval.integer = atoi(yytext);
					return t_INTEGER;
				}
[-+()=/*]	{return *yytext;}
[ \t\n]	; /*skip whitespace*/
.		{return *yytext;}
%%
int yywrap(void){
	return 1;
}
