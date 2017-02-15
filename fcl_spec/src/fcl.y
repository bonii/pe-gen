/*
    fcl.y is part of FCL_SPEC
    fcl.y - Yacc input for FCL
    
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

    Written by Vivek Shah <bonii(at)kompiler.org>, Tim van Deurzen <tim(at)kompiler.org>

*/

%{

#include <stdio.h>
#include <stdlib.h>

#include "ast.h"


/*****************************************************************************/


FILE *yyin;
program *prog    = NULL;

%}

%union {
    char *id;
    int  num;
    expr *expr;
    varlist *vlist;
    jump *jump;
    __assign *assign;
    otype op;
    basic_block *block;
}

%token _READ _GOTO _IF _THEN _ELSE _RETURN _ID _NUMBER _ASSIGN _EQ _ADD _MINUS
       _MUL _DIV _LT _LTE _GT _GTE _ARROW _MOD

%type <id> _ID
%type <num> _NUMBER
%type <expr> expression
%type <expr> expressions
%type <vlist> vars
%type <jump> jump
%type <assign> assignment
%type <assign> assignments
%type <op> operation
%type <block> basicblock
%type <block> basicblocks


%right _ASSIGN
%left '-' '+'
%left '*' '/'

%start program

%debug
%error-verbose

%%

program: _READ vars _ARROW _ID basicblocks      { prog = mkprogram($4, $2, $5); }
       | _READ _ARROW _ID basicblocks           { prog = mkprogram($3, NULL, $4); }


vars: _ID ',' vars                              { $$ = mkreadvar($1); $$->next = $3; }
    | _ID                                       { $$ = mkreadvar($1); $$->next = NULL; }


basicblocks: basicblock basicblocks             { $$ = $1; $$->next = $2; }
           | basicblock                         { $$ = $1; $$->next = NULL; }


basicblock: _ID '{' assignments jump '}'        { $$ = mkbasic_block($1, $3, $4); }
          | _ID '{' jump '}'                    { $$ = mkbasic_block($1, NULL, $3); }


assignments: assignment assignments             { $$ = $1; $$->next = $2; }
           | assignment                         { $$ = $1; $$->next = NULL; }


assignment: _ID _ASSIGN expression              { $$ = mkassign($1, $3); }


jump: _GOTO _ID                                 { $$ = mkgoto($2); }
    | _IF expression _THEN _ID _ELSE _ID        { $$ = mkcnd($2, $4, $6); }
    | _RETURN expression                        { $$ = mkreturn($2); }


expressions: expression expressions             { $$ = $1; $$->next = $2; }
           | expression                         { $$ = $1; $$->next = NULL; }


expression: '(' operation expressions ')'       { $$ = mkop($2, $3); }
          | _NUMBER                             { $$ = mkint($1); }
          | _ID                                 { $$ = mkvar($1); }


operation: _ADD                                 { $$ = PLUS; }
         | _MINUS                               { $$ = MIN; }
         | _MUL                                 { $$ = MUL; }
         | _DIV                                 { $$ = DIV; }
         | _EQ                                  { $$ = EQ; }
         | _LT                                  { $$ = LT; }
         | _GT                                  { $$ = GT; }
         | _LTE                                 { $$ = LTE; }
         | _GTE                                 { $$ = GTE; }
         | _MOD                                 { $$ = MOD;}

%%


/*****************************************************************************/


/**
 * Print a parse error.
 */
yyerror (char *s)
{
    fflush(stdout);
    fprintf (stderr, "error: %s\n", s);
}


/**
 * Parse an FCL program, building up the AST.
 */
void parse_program(program **pgm, char *filename)
{
    if (prog == NULL) {
        yyin = fopen(filename, "r");

        if (yyin == NULL) {
            fprintf(stderr, "Could not open file `%s'!\n", filename);
            exit(1);
        }

        if (yyparse() == 1) {
            fprintf(stderr, "Error parsing file!\n");
            exit(1);
        }

        fclose(yyin);
    }

    *pgm = prog;
}
