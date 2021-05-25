%{

/* Declarations section */
#include "helper.hpp"
#include "hw3_output.hpp"
#include "parser.tab.hpp"


using namespace std;
using namespace output;
%}

%option yylineno 
%option noyywrap
%x comment
%x thisistheend


relop           (>=|<=|<|>)
equalop			(==|!=)
mult 			([*])
div				([/])
plus			([+])
minus			([-])
digit   		([0-9])
letter			([a-zA-Z])
id  			([a-zA-Z][a-zA-Z0-9]*)
whitespace		([\r\t\n ])

%%
{whitespace}+				;
void                        {
							yylval.type = "VOID"; 
							return VOID;}
int                        {
							yylval.type = "INT"; 
							return INT;}
byte                        {
							yylval.type = "BYTE"; 
							return BYTE;}
b                        {
						yylval.type = "BYTE"; 
							return B;}
bool                        {
							yylval.type = "BOOL"; 
							return BOOL;}
and                        {return AND;}
or                        {return OR;}
not                        {return NOT;}
true                        {return TRUE;}
false                     {return FALSE;}
return                        {return RETURN;}
if                        {return IF;}
else                        {return ELSE;}
while                        {return WHILE;}
break                        {return BREAK;}
continue                        {return CONTINUE;}
switch                        {return SWITCH;}
case                        {return CASE;}
default                        {return DEFAULT;}
:                        {return COLON;}
;                        {return SC;}
,                        {return COMMA;}
\(                        {return LPAREN;}
\)                        {return RPAREN;}
\{                        {return LBRACE;}
\}                        {return RBRACE;}
=                        {return ASSIGN;}
{relop}                        {return RELOP;}
{equalop}						{return EQUALOP;}
{mult}							{return MULT;}
{div}							{return DIV;}
{plus}							{return PLUS;}
{minus}							{return MINUS;}
"//"[^\r|\n]*         BEGIN(comment);
<comment>{
	\r		{BEGIN(INITIAL);}
	\n		{BEGIN(INITIAL);}
	\r\n 	{BEGIN(INITIAL);}
	<<EOF>>	{BEGIN(INITIAL);}
	}
	
{id}+ {
		yylval.name = yytext; 
		return ID;}
0 {	
	yylval.val = 0; 
	return NUM;}
[1-9]+{digit}* {	
					yylval.val = atoi(yytext); 
					return NUM;}
\"([^\n\r\"\\]|\\[rnt"\\])+\" 	{	
									yylval.type = "STRING"; 
									return STRING;}
.		{
			errorLex(yylineno);
			exit(1);
		}
<INITIAL><<EOF>>        {BEGIN(thisistheend);return ENDOFFILE;}
<thisistheend><<EOF>>	{return 0;}
%%
/* c code */


