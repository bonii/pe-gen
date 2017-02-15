/*
    main.c is part of FCL_INT
    main.c - Simple Flow Chart Language interpreter.
    
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


/*****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "fcl.h"
#include "ast.h"


/*****************************************************************************/


#pragma cmix pure spectime: spectime_declare()
extern void spectime_declare(char *, key *);
extern void parse_program(program **, varlist **, int *, char *);
extern int yydebug;


/*****************************************************************************/




/**
 * Lookup function for finding a variables value in the store.
 */
int lookup (char *key_)
{
    key *current_key;
    value *current_value;

    current_key = env_keys;
    current_value = env_values;

    while (current_key != NULL) {
        if (strcmp(current_key->id, key_) == 0 && current_value != NULL) {
            return current_value->intval;
        }

        current_key = current_key->next;
        current_value = current_value->next;
    }

    return 0;
}


/*
 * Evaluate the entry block of the program.
 *
 * To evaluate the entry block we must first get the values of the program
 * arguments into the environment. At this stage the environment should only
 * contain the variable names, with the values uninitialized. This creates a
 * potential bug when too few arguments are supplied. However, we should not
 * crash as lookup will simply return
 */
int eval_entry_block (basic_block *b, varlist *argn, int argc, int *argv)
{
    key *curr_key;

    value *curr_val;

    int i;

    curr_key = env_keys;
    curr_val = env_values;

    /*
     * Build the initial store as two concurrent linked lists of keys and
     * values.
     */
    
    curr_key = (key *) malloc(sizeof(key));
    curr_val = (value *) malloc(sizeof(value));

    while (argn != NULL) {

        curr_key->id = (char *) malloc(strlen(argn->id) * sizeof(char));

        strcpy(curr_key->id, argn->id);
        curr_key->next = (key *) malloc(sizeof(key));

        curr_val->intval = argv[i++];
        curr_val->next = (value *) malloc(sizeof(value));

        curr_key = curr_key->next;
        curr_val = curr_val->next;

        argn = argn->next;
    }

    return eval_block(b);
}


/*
 * Evaluate a single basic block.
 *
 * First evaluate all assignments then evaluate the jump.
 */
int eval_block (basic_block *b)
{
    __assign *current;

    current = b->assignments;
    while (current != NULL) {
        __CMIX(pure) update( current->variable
                           , eval_expression(current->expression) 
                           );

        current = current->next;
    }

    return eval_jump(b->jump);
}


/*
 * Evaluate a single expression.
 */
int eval_expression (expr *e)
{
    int result;

    result = 0;

    switch (e->type) {
        case INT:
            result = e->intval;
        break;

        case VAR:
            result = lookup(e->id);
        break;

        case OP:
            switch (e->op->type) {
                case PLUS:
                case MIN:
                case MUL:
                case DIV:
                    result = eval_n_ary_op(e->op->type, e->op->operands);
                break;

                case EQ:
                case LT:
                case LTE:
                case GT:
                case GTE:
                    result = eval_binary_op(e->op->type, e->op->operands);
                break;
            }
        break;
    }

    return result;
}


/*
 * Evaluate a n-ary operator.
 */
int eval_n_ary_op (otype type, expr *operands)
{
    expr *current;
    int accumulator,
        temp;

    
    if (operands == NULL || operands->next == NULL) {
        __CMIX(pure) fprintf(stderr, "Not enough operands for operation!");
        __CMIX(pure) exit(1);
    }

    temp = 0;
    current = operands;

    if (type == MUL || type == DIV) {
        accumulator = 1;
    } else if (type == MIN) {
        accumulator = eval_expression(current);

        current = current->next;
    } else {
        accumulator = 0;
    }

    while (current != NULL) {
        temp = eval_expression(current);

        switch (type) {
            case PLUS:
                accumulator += temp;
            break;

            case MIN:
                accumulator -= temp;
            break;

            case MUL:
                accumulator *= temp;
            break;

            case DIV:
                if (temp == 0) {
                    __CMIX(pure) fprintf(stderr, "Division by zero!\n");
                    __CMIX(pure) exit(1);
                }

                accumulator /= temp;
            break;
        }

        current = current->next;
    }

    return accumulator;
}


/*
 * Evaluate a binary operator, throwing an error of there are not enough
 * operands.
 */
int eval_binary_op (otype type, expr *operands)
{
    int a, b, result;

    a = b = result = 0;
    
    if (operands == NULL || operands->next == NULL) {
        __CMIX(pure) fprintf(stderr, "Not enough operands!\n");
        __CMIX(pure) exit(1);
    }
    
    a = eval_expression(operands);
    b = eval_expression(operands->next);

    switch (type) {
        case EQ:
            result = (a == b) ? 1 : 0;
        break;

        case LT:
            result = (a < b) ? 1 : 0;
        break;

        case LTE:
            result = (a <= b) ? 1 : 0;
        break;

        case GT:
            result = (a > b) ? 1 : 0;
        break;

        case GTE:
            result = (a >= b) ? 1 : 0;
        break;
    }

    return result;
}


/*
 * Evaluate a jump instruction, throwing an error if the target label can not
 * be found.
 */
int eval_jump (jump *j) 
{
    basic_block *target;

    switch (j->tag) {
        case GOTO:
            target = find_basic_block(j->label);
        break;

        case CND:
            if (eval_expression(j->conditional->condition) == 1) {
                target = find_basic_block( j->conditional->l_then);
            } else {
                target = find_basic_block( j->conditional->l_else);
            }
        break;

        case RETURN:
            return eval_expression(j->return_stmt);
        break;
    }

    if (target == NULL) {
        __CMIX(pure) fprintf(stderr, "Couldn't find target block, exiting.\n");
        __CMIX(pure) exit(1);
    }

    return eval_block(target);
}


/*
 * Look for a basic block with a specific label.
 */
basic_block *find_basic_block (char *label) 
{
    basic_block *current;

    current = pgm->blocks;
    while (current != NULL) {
        if (strcmp(label, current->label) == 0) {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

#pragma cmix spectime: parse_program()
/*#pragma cmix generator: compile specializes interpret($1, ?, ?) producing("main")*/
#pragma cmix goal: interpret($1, ?)

/*
 * Interpret a program.
 */
int interpret (char *filename, int argc, int *argv)
{
    basic_block *entry;

    varlist *argn;

    int i, pgm_argc, result;


    env_keys = NULL;
    env_values = NULL;
    pgm = NULL;

    /* Parse the program getting the program and a list of input arguments. */
    parse_program(&pgm, &argn, &pgm_argc, filename);

    entry = find_basic_block(pgm->entry_label);

    if (entry == NULL) {
        __CMIX(pure) fprintf(stderr, "Couldn't find entry label, exiting.");
        __CMIX(pure) exit(1);
    }

    result = eval_entry_block(entry, argn, argc, argv);

    printf(">> %d\n", result);

    return 0;
}

#ifdef FCL_MAIN
int main(int argc, char **argv)
{
    /*yydebug = 1;*/

    int i, j;

    int *input;

    input = (int *) malloc((argc - 2) * sizeof(int));
    
    i = 0;
    j = 2;
    for (; i < argc - 2; i++) {
        input[i] = atoi(argv[j++]);
    }

    if (argc > 1) {
        interpret(argv[1], argc - 2, input);
    } else {
        __CMIX(pure) printf("Usage: %s <filename>\n", argv[0]);
    }

    
    return 0;
}
#endif
