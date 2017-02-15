/*
    ast.c is part of FCL_SPEC
    ast.c - Functions to manipulate AST
    
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "ast.h"


/*****************************************************************************/


/*
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


/*
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


/*
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


/*
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


/*
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


/*
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


/*
 * Build an assignment.
 */
__assign *mkassign (char *variable, expr *e)
{
    __assign *result;

    result = (__assign *) malloc(sizeof (__assign));

    result->variable = malloc(strlen(variable) * sizeof (char));

    strcpy(result->variable, variable);

    result->expression = e;
    result->next = NULL;

    return result;
}


/*
 * Build a variable list item.
 */
varlist *mkreadvar (char *variable)
{
    varlist *result;

    result = (varlist *) malloc(sizeof (varlist));

    result->variable = (char *) malloc(strlen(variable) * sizeof (char));
    strcpy(result->variable, variable);

    result->next = NULL;

    return result;
}


/*
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


/*
 * Build the program representation.
 */
program *mkprogram (char *entry_label, varlist *read, basic_block *blocks)
{
    program *result;
    varlist *current;
    int i;

    result = (program *) malloc(sizeof (program));

    if (result == NULL) {
        fprintf(stderr, "Not enough memory!\n");
        exit(1);
    }

    result->entry_label = (char *) malloc(strlen(entry_label) * sizeof (char));
    strcpy(result->entry_label, entry_label);

    i = 0;
    current = read;
    while (current != NULL) {
        i++;
        current = current->next;
    }

    result->argc = i;
    result->variables = read;
    result->blocks = blocks;


    return result;
}


/*
 * Build a new block list link.
 */
block_list *mkblock_list(char *block_name, basic_block *block, struct store *env)
{
    block_list *result;

    result = (block_list *) malloc(sizeof(block_list));

    if (result == NULL) {
        fprintf(stderr, "Not enough memory!\n");
        exit(1);
    }
  
    result->block_name = block_name;
    result->block = block;
    result->env = env;
    result->next = NULL;

    return result;
}

/*
 * Builds a new block using shallow copy
 */
basic_block *copy_block(basic_block *old_block) 
{
    if(old_block == NULL) {
        return NULL;
    }
    basic_block *result;
    result = (basic_block *) malloc(sizeof(basic_block));
    result -> label = old_block -> label;
    result -> assignments = old_block -> assignments;
    result -> jump = old_block -> jump;
    result -> use_variables = old_block -> use_variables;
    return result;
}

/*
 * Extend a block list by prepending a new block to it.
 */
void prepend_list( block_list **root
                 , char *block_name
                 , basic_block *block
                 , struct store *env
                 )
{
    block_list *new_entry;

    new_entry = mkblock_list(block_name, block, env);

    new_entry->next = *root;

    *root = new_entry;
}

/*
 * Extend a block list by appending a new block to it.
 */
void append_list( block_list **root
                 , char *block_name
                 , basic_block *block
                 , struct store *env
                 )
{
    block_list *new_entry, *current;

    current = *root;
    new_entry = mkblock_list(block_name, block, env);
    new_entry -> next = NULL;

    if (current == NULL) {
        *root = new_entry;
    } else {
        while(current -> next != NULL) {
            current = current -> next;
        }
        current -> next = new_entry;
    }
}
