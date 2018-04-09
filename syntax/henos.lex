digit		[0-9]
alpha		[a-fA-F]
hextail		({digit}|{alpha}){1,8}
hex			0[xX]{hextail}
lower 		[a-z]
upper 		[A-Z]
octalDigit	[0-7]
decDigit	[1-9]
identChars	{lower}{upper}{digit}[_?]
number 		0{octalDigit}* | {decDigit}+ | {hex}+
ident 		{lower}{identChars}*
upperIdent 	{upper}{identChars}*
string 		\"(\\.|[^"\\])*\"

%{
#include <stdlib.h>
void yyerror(char*);
#include "y.tab.h"
%}
%%

%x C_COMMENT

"/*"            { BEGIN(C_COMMENT); }
<C_COMMENT>"*/" { BEGIN(INITIAL); }
<C_COMMENT>\n   { }
<C_COMMENT>.    { }
\/\/[^\r\n]*

{number}		{yylval	= atoi(yytext); return t_INTEGER;}

{ident}			{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					yylval.text=p;
					return(t_IDENT);
				}

{uppperIdent}	{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					yylval.text=p;
					return(t_UPPER_IDENT);
				}

{string}	{
					p=(char *)calloc(strlen(yytext)+1,sizeof(char));
					strcpy(p,yytext);
					yylval.text=p;
					return(t_STRING);
				}

[-+()=/*\n]	return *yytext;
[ \t]	; /*skip whitespace*/
.		return *yytext;
%%
int yywrap(void){
	return 1;
}
