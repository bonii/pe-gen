/*
    pretty_print.c is part of FCL_SPEC
    pretty_print.c - Functions to print AST
    
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


/*****************************************************************************/


#include <stdio.h>

#include "pretty_print.h"
#include "dbug.h"
#include "ast.h"


/*****************************************************************************/


/*
 * Pretty print an expression recursively printing subexpressions.
 */
void print_expression(expr *expression, int with_new_line) 
{
    expr *current,
         *operands;

    DBUG_ENTER("print_expression");

    current = expression;
    while (current != NULL) {
        switch (current -> type) {
            case INT:
                OUT(" %d ", current -> intval)
            break;

            case VAR:
                OUT(" %s ", current -> id)
            break;

            case OP:
                OUT_(" (")
                switch (current -> op -> type) {
                    case PLUS:
                        OUT_("+")
                    break;

                    case MIN:
                        OUT_("-")
                    break;

                    case MUL:
                        OUT_("*")
                    break;

                    case DIV:
                        OUT_("/")
                    break;

                    case EQ:
                        OUT_("=")
                    break;

                    case LT:
                        OUT_("<")
                    break;

                    case GT:
                        OUT_(">")
                    break;

                    case LTE:
                        OUT_("<=")
                    break;

                    case GTE:
                        OUT_(">=")
                    break;

                    case MOD:
                        OUT_("%%")
                    break;
                }

                print_expression(current -> op -> operands, 0);
                OUT_(")")
        }

        current = current -> next;
    }

    if (with_new_line) {
        OUT_("\n")
    }

    DBUG_VOID_RETURN;
}


/*
 * Pretty print a basic block.
 */
void print_block(basic_block *block) 
{
    __assign *assignments;

    DBUG_ENTER("print_block");

    assignments = block -> assignments;

    OUT("%s", block -> label)
    OUT_("\n{\n")

    while (assignments != NULL) {
        OUT("\t%s := ", assignments -> variable)
        print_expression(assignments -> expression, 1);
        assignments = assignments -> next;
    }

    switch (block -> jump -> tag) {
        case GOTO:
            OUT_("\tgoto ")
            OUT("%s", block -> jump -> label)
        break;

        case RETURN:
            OUT_("\treturn ")
            print_expression(block -> jump -> return_stmt, 0);
        break;

        case CND:
            OUT_("\tif")

            print_expression(block -> jump -> conditional -> condition, 0);

            if (block -> jump -> conditional -> l_then != NULL) {
                OUT("\n\t\tthen %s", block -> jump -> conditional -> l_then)
            }

            if (block -> jump -> conditional -> l_else != NULL) {
                OUT("\n\t\telse %s", block -> jump -> conditional -> l_else)
            }
    }

    OUT_("\n")
    OUT_("\n}\n")

    DBUG_VOID_RETURN;
}


/*
 * Pretty print the program.
 */
void print_program(program *program) 
{
    varlist *vars;
    basic_block *current;
    int first;

    DBUG_ENTER("print_program");
    
    vars = program -> variables;
    current = program -> blocks;
    first = 1;

    OUT_("read ")

    while (vars != NULL) {
        if (first) {
            OUT("%s", vars -> variable)
            first = 0;
        } else {
            OUT(", %s", vars -> variable)
        }
        vars = vars -> next;
    }

    if (program -> entry_label != NULL) {
        OUT(" -> %s", program -> entry_label)
    }

    OUT_("\n")

    while (current != NULL) {
        print_block(current);
        current = current -> next;
    }

    DBUG_VOID_RETURN;
}
