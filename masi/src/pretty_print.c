/*
    pretty_print.c is part of MASI
    pretty_print.c - Functions to print AST in MASI
    
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


#include "pretty_print.h"
#include "ast.h"

void print_instructions (instr *instructions)
{
    while (instructions != NULL) {
        switch (instructions->tag) {
            case MOV:
                fprintf(stdout, "\tMOV r%d, r%d\n", instructions->operands[0]
                                                  , instructions->operands[1]);
            break;

            case MOVI:
                fprintf(stdout, "\tMOVI %d, r%d\n", instructions->operands[0]
                                                  , instructions->operands[1]);
            break;

            case ADD:
                fprintf(stdout, "\tADD r%d, r%d, r%d \n", instructions->operands[0]
                                                        , instructions->operands[1]
                                                        , instructions->operands[2]);
            break;

            case SUB:
                fprintf(stdout, "\tSUB r%d, r%d, r%d \n", instructions->operands[0]
                                                        , instructions->operands[1]
                                                        , instructions->operands[2]);
            break;

            case MUL:
                fprintf(stdout, "\tMUL r%d, r%d, r%d \n", instructions->operands[0]
                                                        , instructions->operands[1]
                                                        , instructions->operands[2]);
            break;

            case DIV:
                fprintf(stdout, "\tDIV r%d, r%d, r%d \n", instructions->operands[0]
                                                        , instructions->operands[1]
                                                        , instructions->operands[2]);
            break;

            case BEQ:
                fprintf(stdout, "\tBEQ %s, r%d, r%d \n", instructions->jump_target
                                                       , instructions->operands[0]
                                                       , instructions->operands[1]);
            break;

            case HALT:
                fprintf(stdout, "\tHALT\n");
            break;
        }

        instructions = instructions->next;
    }
}

void print_blocks (block_list *blocks)
{
    while (blocks != NULL) {
        if (blocks->label == NULL) {
            break;
        }

        fprintf(stdout, "%s:\n", blocks->label);

        print_instructions(blocks->instructions);

        blocks = blocks->next;
    }
}
