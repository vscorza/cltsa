%{
#include <stdlib.h>
#include "parser_utils.h"
#include "y.tab.h"
char *p;
%}

%x C_COMMENT
digit		[0-9]
alpha		[a-fA-F]
hextail		({digit}|{alpha}){1,8}
hex			0[xX]{hextail}
lower 		[a-z]
upper 		[A-Z]
octalDigit	[0-7]
decDigit	[1-9]
identChars	{lower}|{upper}|{digit}|[_?]
number 		0{octalDigit}*|{decDigit}+|{hex}+
ident 		{lower}{identChars}*
upperIdent 	{upper}{identChars}*
string 		\"(\\.|[^"\\])*\"

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
{ident}			{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					printf("[t_IDENT: <%s>]", yytext);
					yylval.text=p;
					return(t_IDENT);
				}
{upperIdent}	{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					printf("[t_UPPER_IDENT: <%s>]\n", yytext);
					yylval.text=p;
					return(t_UPPER_IDENT);
				}
{string}	{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					printf("[t_STRING: <%s>]", yytext);
					strcpy(p,yytext);
					yylval.text=p;
					return(t_STRING);
				}
{number}		{
					printf("[t_INTEGER: <%s>]", yytext);
					yylval.integer = atoi(yytext);
					return t_INTEGER;
				}
[-+()=/*\n]	return *yytext;
[ \t]	; /*skip whitespace*/
.		return *yytext;
%%
int yywrap(void){
	return 1;
}
