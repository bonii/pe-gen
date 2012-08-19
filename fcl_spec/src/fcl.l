/*
    fcl.l is part of FCL_SPEC
    fcl.l - Lexer input for FCL
    
    Copyright(C) 2012 Vivek Shah <bonii(at)kompiler.org>, Tim van Deurzen <tim(at)kompiler.org>

    FCL_SPEC is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FCL_SPEC is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FCL_SPEC.  If not, see <http://www.gnu.org/licenses/>.

    Written by Vivek Shah <bonii@kompiler.org>, Tim van Deurzen <tim(at)kompiler.org>

*/

%{

#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "y.tab.h"


/* Need this declaration as strdup is not an ANSI standard function so compiling with
 * ansi flags will give errors otherwise
 */

char *strdup(const char *s);

%}

delim  [ \t\n]
ws     {delim}+
letter [A-Za-z]
digit  [0-9]
id     {letter}([_]|{letter}|{digit})*
number {digit}+(\.{digit}+)?(E[+-]?{digit}+)?

%option noyywrap
%x C_COMMENT

%%

{ws}               {}
"/*"               { BEGIN(C_COMMENT); }
<C_COMMENT>"*/"    { BEGIN(INITIAL); }
<C_COMMENT>{delim} {}
<C_COMMENT>.       {}

read               { return (_READ); }
goto               { return (_GOTO); }
if                 { return (_IF); }
then               { return (_THEN); }
else               { return (_ELSE); }
return             { return (_RETURN); }

{id}               { yylval.id  = strdup(yytext); return (_ID); }
{number}           { yylval.num = atoi(yytext); return (_NUMBER); }

":="               { return (_ASSIGN); }
":"                { return (':'); }
";"                { return (';'); }
"+"                { return (_ADD); }
"-"                { return (_MINUS); }
"*"                { return (_MUL); }
"/"                { return (_DIV); }
"="                { return (_EQ); }
"<"                { return (_LT); }
">"                { return (_GT); }
"<="               { return (_LTE); }
">="               { return (_GTE); }
"%"                { return (_MOD); }
"->"               { return (_ARROW); }
","                { return (','); }
"("                { return ('('); }
")"                { return (')'); }
"{"                { return ('{'); }
"}"                { return ('}'); }
.                  {}

%%
