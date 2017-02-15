/*
    masi.y is part of MASI
    masi.y - Yacc input for MASI programs
    
    Copyright(C) 2012 Vivek Shah <bonii(at)kompiler.org>, Tim van Deurzen <tim(at)kompiler.org>

    MASI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MASI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MASI.  If not, see <http://www.gnu.org/licenses/>.

    Written by Vivek Shah <bonii(at)kompiler.org>, Tim van Deurzen <tim(at)kompiler.org>

*/

%{

#include <stdio.h>
#include <stdlib.h>

#include "ast.h"


/*****************************************************************************/


FILE *yyin;
block_list *blocks;

%}

%union {
    char *id;
    int num;
    instr *instr;
    block_list *block_list;
}

%token _MOV_ _MOVI_ _ADD_ _SUB_ _MUL_ _DIV_ _BEQ_ _HALT_ _LABEL_ _NUM_ _ID_

%type <id> _ID_
%type <num> _NUM_
%type <num> reg
%type <instr> mov
%type <instr> movi
%type <instr> add
%type <instr> sub
%type <instr> mul
%type <instr> div
%type <instr> beq
%type <instr> halt
%type <instr> instructions
%type <instr> instruction
%type <block_list> blocks
%type <block_list> block
%type <block_list> program

%start program

%debug
%error-verbose

%%

program: blocks                         { blocks = $1; }

blocks: block blocks                    { $$ = $1; $1->next = $2; }
      | block                           { $$ = $1; }
       
block: _ID_ ':' instructions            { $$ = mkblock_list($1, $3); }

instructions: instruction instructions  { $$ = $1; $1->next = $2; }
            | instruction               { $$ = $1; }

instruction: mov
           | movi
           | add
           | sub
           | mul
           | div
           | beq
           | halt

mov: _MOV_ reg reg                      { $$ = mkinstr(MOV, "", $2, $3, 0); }
  
movi: _MOVI_ _NUM_ reg                  { $$ = mkinstr(MOVI, "",  $2, $3, 0); }

add: _ADD_ reg reg reg                  { $$ = mkinstr(ADD, "", $2, $3, $4); }

sub: _SUB_ reg reg reg                  { $$ = mkinstr(SUB, "", $2, $3, $4); }

mul: _MUL_ reg reg reg                  { $$ = mkinstr(MUL, "", $2, $3, $4); }

div: _DIV_ reg reg reg                  { $$ = mkinstr(DIV, "", $2, $3, $4); }

beq: _BEQ_ _ID_ reg reg                 { $$ = mkinstr(BEQ, $2, $3, $4, 0); }

halt: _HALT_                            { $$ = mkinstr(HALT, "", 0, 0, 0); }

reg: 'r' _NUM_                          { $$ = $2; }
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
void parse_program(block_list **pgm, char *filename)
{
    if (blocks == NULL) {
        yyin = fopen(filename, "r");

        if (yyin == NULL) {
            fprintf(stderr, "Could not open file `%s'!\n", filename);
        }

        if (yyparse() == 1) {
            fprintf(stderr, "Error parsing file!\n");
            exit(1);
        }

        fclose(yyin);
    }

    *pgm = blocks;
}
