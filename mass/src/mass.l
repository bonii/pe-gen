%{
/*
    mass.l is part of MASS
    mass.l - Lexer for MAS code.
    
    Copyright(C) 2012 Vivek Shah <bonii(at)kompiler.org>, Tim van Deurzen <tim(at)kompiler.org>

    MASS is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MASS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MASS.  If not, see <http://www.gnu.org/licenses/>.

    Written by Vivek Shah <bonii(at)kompiler.org>, Tim van Deurzen <tim(at)kompiler.org>

*/

#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "y.tab.h"


/* Need this declaration as strdup is not an ANSI standard function so
 * compiling with ansi flags will give errors otherwise
 */

char *strdup(const char *s);

%}

delim  [ \t\n]
ws     {delim}+
letter [A-Za-z]
digit  [0-9]
id     {letter}([_]|{letter})*
number {digit}+(\.{digit}+)?(E[+-]?{digit}+)?

%option noyywrap
%x C_COMMENT

%%

{ws}                {}
"/*"                { BEGIN(C_COMMENT); }
<C_COMMENT>"*/"     { BEGIN(INITIAL); }
<C_COMMENT>{delim}  {}
<C_COMMENT>.        {}

mov                 { return (_MOV_); }
movi                { return (_MOVI_); }           
add                 { return (_ADD_); }
sub                 { return (_SUB_); }
mul                 { return (_MUL_); }
div                 { return (_DIV_); }
beq                 { return (_BEQ_); }
halt                { return (_HALT_); }

r                   { return ('r');  }
{number}            { yylval.num = atoi(yytext); return (_NUM_); }
{id}                { yylval.id = strdup(yytext); return (_ID_); }
[:]                 { return (':'); }
.                   {}

%%
