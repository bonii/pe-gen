/*
    ast.c is part of MASI
    ast.c - Functions to manipulate AST
    
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

instr *mkinstr (itype tag, char *target, int op1, int op2, int op3)
{
    instr *result;

    result = (instr *) malloc(sizeof (instr));

    if (result == NULL) {
        fprintf(stderr, "Not enough memory!\n");
        exit(1);
    }

    result->tag = tag;

    result->jump_target = (char *) malloc(strlen(target) * sizeof (char));
    strcpy(result->jump_target, target);

    result->operands[0] = op1;

    result->operands[1] = op2;

    result->operands[2] = op3;

    result->next = NULL;

    return result;
}


block_list *mkblock_list (char *label, instr *instructions)
{
    block_list *result;

    result = (block_list *) malloc(sizeof (block_list));

    if (result == NULL) {
        fprintf(stderr, "Not enough memory!\n");
        exit(1);
    }

    result->label = malloc(strlen(label) * sizeof (char));
    strcpy(result->label, label);

    result->instructions = instructions;
    result->next = NULL;

    return result;
}
