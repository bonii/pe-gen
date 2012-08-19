/*
    spec.c is part of MASI
    spec.c - Functions to interpret MASI 
    
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

    Written by Vivek Shah <bonii@kompiler.org>, Tim van Deurzen <tim(at)kompiler.org>

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "masi.h"
#include "pretty_print.h"


/*****************************************************************************/


extern void parse_program(block_list **, char *);

#ifndef __CMIX
#define __CMIX(x)
#endif


/*****************************************************************************/


#pragma cmix spectime: parse_program()
#pragma cmix residual: mkinstr(), print_blocks()
#pragma cmix generator: cogen specializes interpret($1,?) producing("mix")


void strcpy_(char *dest, const char *src) 
{    
    while(*src) {
        *dest++ = *src++;
    }

    *dest = '\0';
}


#pragma cmix residual: copy_instructions::first
instr *copy_instructions (instr *src)
{
    instr *result,
          *current,
          *last;
    
    int first;

    first = 1;
    while (src != NULL) {
        if (first) {
            first = 0;
            result = (instr *) malloc(sizeof(instr));
            current = result;
        }

        current->next = (instr *) malloc(sizeof(instr));

        switch(src->tag) {
            case MOV:
                current->tag = MOV;
            break;
            
            case MOVI:
                current->tag = MOVI;
            break;
            
            case ADD:
                current->tag = ADD;
            break;
            
            case SUB: 
                current->tag = SUB;
            break;
            
            case MUL:
                current->tag = MUL;
            break;
            
            case DIV:
                current->tag = DIV;
            break;
            
            case BEQ:
                current->tag = BEQ;
            break;
            
            case HALT:
                current->tag = HALT;
            break;
        }

        current->jump_target = (char *) malloc((strlen(src->jump_target) + 1) 
                                               * sizeof (char));
        strcpy_(current->jump_target, src->jump_target);
        current->operands[0] = src->operands[0];
        current->operands[1] = src->operands[1];
        current->operands[2] = src->operands[2];

        last = current;
        current = current->next;

        src = src->next;
    }

    last->next = NULL;

    return result;
}

/*
 * Find a block within the program blocks matching the label
 * If not found return NULL
 */
block_list *find_block(char *target_label, block_list *program_blocks) 
{
    block_list *current;
    current = program_blocks;
    while(current != NULL) {
        if(strcmp(target_label,current -> label) == 0) {
            return current;
        }
        current = current -> next;
    }
    return NULL;
}

/* 
 * Evaluate a block and return the pointer to the next block 
 * Return NULL if HALT instruction encountered or end of blocks reached
 */
block_list *eval_block(block_list *block, value regs[], block_list *program_blocks) 
{
    instr *instruction;
    int halt_processing = 0;
    int i;
    instruction = block -> instructions;
    while(instruction != NULL) {
        halt_processing = 0;
        switch (instruction -> tag) {
            case MOV:
                regs[instruction -> operands[1]] = regs[instruction -> operands[0]];
                break;
            case MOVI:
                if(instruction -> operands[0] < 0) {
                    fprintf(stderr, "Moving a negative number to a register\n");
                    exit(1);
                } else {
                    regs[instruction -> operands[1]] = instruction -> operands[0];
                }
                break;
            case ADD:
                if(regs[instruction -> operands[0]] != -1 && regs[instruction -> operands[1]] != -1) {
                    regs[instruction -> operands[2]] = regs[instruction -> operands[0]] + regs[instruction -> operands[1]];
                } else {
                    fprintf(stderr,"Non defined register used\n");
                    exit(1);
                }
                break;
            case SUB:
                if(regs[instruction -> operands[0]] != -1 && regs[instruction -> operands[1]] != -1) {
                    if(regs[instruction -> operands[0]] >= regs[instruction -> operands[1]]) {
                        regs[instruction -> operands[2]] = regs[instruction -> operands[0]] - regs[instruction -> operands[1]];
                    } else {
                        fprintf(stderr,"Result of SUB operation will be negative\n");
                        exit(1);
                    }
                } else {
                    fprintf(stderr,"Non defined register used\n");
                    exit(1);
                }
                break;
            case MUL:
                if(regs[instruction -> operands[0]] != -1 && regs[instruction -> operands[1]] != -1) {
                    regs[instruction -> operands[2]] = regs[instruction -> operands[0]] * regs[instruction -> operands[1]];
                } else {
                    fprintf(stderr,"Non defined register used\n");
                    exit(1);
                }
                break;
            case DIV:
                if(regs[instruction -> operands[0]] != -1 && regs[instruction -> operands[1]] != -1) {
                    if(regs[instruction -> operands[1]] == 0 ) {
                        fprintf(stderr,"Divide by zero\n");
                        exit(1);
                    } else {
                        regs[instruction -> operands[2]] = regs[instruction -> operands[0]] / regs[instruction -> operands[1]];
                    }
                } else {
                    fprintf(stderr,"Non defined register used\n");
                    exit(1);
                }
                break;
            case BEQ:
                if(regs[instruction -> operands[0]] != -1 && regs[instruction -> operands[1]] != -1) {
                    
                    if(regs[instruction -> operands[0]] == regs[instruction -> operands[1]]) {
                        return find_block(instruction -> jump_target, program_blocks);
                    }
                } else {
                    fprintf(stderr,"Non defined register used\n");
                    exit(1);
                }
                break;                    
            case HALT:
                halt_processing = 1;
                break;
        }
        if(halt_processing) {
            /* For now a program without a Halt instruction is considered to be semantically valid
               to one with a Halt instruction at the end */
            return NULL;
        }
        instruction = instruction -> next;
    }
    return (block -> next);
}


block_list *start_interpret ( block_list *blocks
                         , value regs[]
                         )
{
    
    block_list *result,*current;

    instr *new,
        *head;

    int i,first;

    /* We need to copy the blocks struct else it will become residual */
    current = (block_list *) malloc(sizeof (block_list));
    first = 1;
    while (blocks != NULL) {
        current -> label = (char *) malloc((strlen(blocks -> label) + 1)
                                           * sizeof (char));
        
        strcpy_(current -> label, blocks -> label);
        
        current -> instructions = copy_instructions(blocks -> instructions);
        
        if(blocks -> next != NULL)
            current -> next = (block_list *) malloc(sizeof (block_list));
        else
            current -> next = NULL;
        
        if (first) {
            first = 0;
            result = current;
        }
        
        current = current -> next;
        blocks = blocks -> next;
    }
    
    current = result;
    while(current != NULL) {
        current = eval_block(current,regs,result);
    }

    head = NULL;
    for (i = 0; i < NUM_REGISTERS; i++) {
        if (regs[i] != -1) {

            new = (instr *) malloc(sizeof (instr));
            new -> next = head;
            new -> tag = MOVI;
            new -> operands[0] = regs[i];
            new -> operands[1] = i;
            head = new;

        }
    }
    
    result = (block_list *) malloc(sizeof(block_list));
    result -> next = NULL;
    result -> label = INTERPRETER_RESULT_LABEL;
    result -> instructions = new;

    return result;
}

/*
 * A trivial specializer.
 */
void interpret ( char *filename
                , value regs[]
                )
{
    block_list *blocks,
               *result;

    int i=0;

    parse_program(&blocks, filename);
    
    result = start_interpret(blocks, regs);

    __CMIX(residual) print_blocks(result);
}
