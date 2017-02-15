/*
    ast.c is part of FCL_INT
    ast.c - Basic implementation of the AST data structures.
    
    Copyright(C) 2012 Vivek Shah <bonii(at)kompiler.org>, Tim van Deurzen <tim(at)kompiler.org>

    FCL_INT is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FCL_INT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FCL_INT.  If not, see <http://www.gnu.org/licenses/>.

    Written by Vivek Shah <bonii(at)kompiler.org>, Tim van Deurzen <tim(at)kompiler.org>

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "ast.h"


/*****************************************************************************/


/**
 * Build an integer expression.
 */
expr *mkint (int value)
{
    expr *result;

    result = (expr *) malloc(sizeof (expr));

    result->type = INT;
    result->intval = value;

    return result;
}


/**
 * Build a variable expression;
 */
expr *mkvar (char *varname)
{
    expr *result;

    result = (expr *) malloc(sizeof (expr));

    result->type = VAR;

    result->id = (char *) malloc(strlen(varname) * sizeof (char));
    strcpy(result->id, varname);

    return result;
}


/**
 * Build a primitive operation expression.
 */
expr *mkop (otype type, expr *es)
{ 
    expr *result;

    result = (expr *) malloc(sizeof (expr));

    result->type = OP;

    result->op = malloc(sizeof (__op));
    result->op->type = type;
    result->op->operands = es;

    return result;
}


/**
 * Build a unconditional jump.
 */
jump *mkgoto (char *label)
{
    jump *result;

    result = (jump *) malloc(sizeof (jump));

    result->tag = GOTO;
    result->label = (char *) malloc(strlen(label) * sizeof(char));
    strcpy(result->label, label);

    return result;
}


/**
 * Build a conditional jump.
 */
jump *mkcnd (expr *cond, char *l_then, char *l_else)
{
    jump *result;

    result = (jump *) malloc(sizeof (jump));

    result->tag = CND;

    result->conditional = (__cnd *) malloc(sizeof (__cnd));
    result->conditional->condition = cond;

    result->conditional->l_then = 
        (char *) malloc(strlen(l_then) * sizeof(char));

    strcpy(result->conditional->l_then, l_then);

    result->conditional->l_else = 
        (char *) malloc(strlen(l_else) * sizeof(char));

    strcpy(result->conditional->l_else, l_else);

    return result;
}


/**
 * Build a return jump.
 */
jump *mkreturn (expr *ret)
{
    jump *result;

    result = (jump *) malloc(sizeof (jump));

    result->tag = RETURN;
    result->return_stmt = ret;

    return result;
}


/**
 * Build an assignment.
 */
__assign *mkassign (char *variable, expr *e)
{
    __assign *result;

    result = (__assign *) malloc(sizeof (__assign));

    result->variable = malloc(strlen(variable) * sizeof (char));

    strcpy(result->variable, variable);

    result->expression = e;

    return result;
}


/**
 * Build a variable list item.
 */
varlist *mkreadvar (char *variable)
{
    varlist *result;

    result = (varlist *) malloc(sizeof (varlist));

    result->id = (char *) malloc(strlen(variable) * sizeof (char));
    strcpy(result->id, variable);

    result->next = NULL;

    return result;
}


/**
 * Build a basic block.
 */
basic_block * mkbasic_block(char *l, __assign *a, jump *j)
{
    basic_block *result;

    result = (basic_block *) malloc(sizeof (basic_block));
    
    if (result == NULL) {
        fprintf(stderr, "Not enough memory!");
        exit(1);
    }

    result->label = (char *) malloc(strlen(l) * sizeof (char));
    strcpy(result->label, l);

    result->assignments = a;
    result->jump = j;

    return result;
}


/**
 * Build the program representation.
 */
program *mkprogram (char *entry_label, basic_block *blocks)
{
    program *result;
    int i;

    result = (program *) malloc(sizeof (program));

    if (result == NULL) {
        fprintf(stderr, "Not enough memory!\n");
        exit(1);
    }

    result->entry_label = (char *) malloc(strlen(entry_label) * sizeof (char));
    strcpy(result->entry_label, entry_label);

    result->blocks = blocks;

    return result;
}
