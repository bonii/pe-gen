/*
    spec.c is part of MASS
    spec.c - Simple online MAS Specializer.
    
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "mass.h"
#include "pretty_print.h"


/*****************************************************************************/


extern void parse_program(block_list **, char *);

#ifndef __CMIX
#define __CMIX(x)
#endif


/*****************************************************************************/


#pragma cmix spectime: parse_program()
/*#pragma cmix spectime: math_spec::bta
#pragma cmix spectime: math_spec::old_bta*/
#pragma cmix spectime: poly_spec::bta
#pragma cmix spectime: eval_math_operation::bta
#pragma cmix spectime: specialize::bt
#pragma cmix spectime: specialize::bta
#pragma cmix residual: print_blocks()
/*#pragma cmix residual: mkblock_list(), mkinstr(), print_blocks()*/
#pragma cmix generator: cogen specializes specialize($1,$2,?) producing("mix")


/*
 * Custom strcpy function.
 *
 * CMix does not trust the standard library version of this function making
 * anything that uses it residual. With this definition CMix knows exactly what
 * happens and is more friendly.
 *
 */
void strcpy_(char *dest, const char *src) 
{    
    while(*src) {
        *dest++ = *src++;
    }

    *dest = '\0';
}


/*
 * Deep copy an instruction.
 *
 * This function is fairly pedantic as we have to show CMix how to read the src
 * instruction and how to construct the result instruction from that.
 */
instr *copy_instruction(instr *src)
{
    instr *dest;

    dest = (instr *) malloc(sizeof (instr));

    switch (src -> tag) {
        case MOV:
            dest -> tag = MOV;
        break;
        
        case MOVI:
            dest -> tag = MOVI;
        break;
        
        case ADD:
            dest -> tag = ADD;
        break;
        
        case SUB: 
            dest -> tag = SUB;
        break;
        
        case MUL:
            dest -> tag = MUL;
        break;
        
        case DIV:
            dest -> tag = DIV;
        break;
        
        case BEQ:
            dest -> tag = BEQ;
        break;
        
        case HALT:
            dest -> tag = HALT;
        break;
    }

    dest -> jump_target = (char *) malloc(strlen(src -> jump_target) 
                                          * sizeof (char));
    
    strcpy_(dest -> jump_target, src -> jump_target);

    dest -> operands[0] = src -> operands[0];
    dest -> operands[1] = src -> operands[1];
    dest -> operands[2] = src -> operands[2];

    dest -> next = NULL;

    return dest;
}


/*
 * Copy a list of instructions.
 *
 * This function takes a list of instructions and builds an identical copy of
 * it.
 */
instr *copy_instruction_list (instr *src)
{
    instr *result,
          *cinstr;
    
    int first;

    first = 1;
    while (src != NULL) {
        if (first) {
            first = 0;
            result = copy_instruction(src);
            cinstr = result;
        } else {
            cinstr -> next = copy_instruction(src);
            cinstr = cinstr -> next;
        }
        
        src = src -> next;
    }

    return result;
}


instr *mkinstr (itype tag, char *target, int op1, int op2, int op3)
{
    instr *result;

    result = (instr *) malloc(sizeof (instr));

    if (result == NULL) {
        fprintf(stderr, "Not enough memory!\n");
        exit(1);
    }

    result->tag = tag;

    if (target != NULL) {
        result->jump_target = (char *) malloc(strlen(target) * sizeof (char));
        strcpy(result->jump_target, target);
    } else {
        result->jump_target = NULL;
    }

    result->operands[0] = op1;

    result->operands[1] = op2;

    result->operands[2] = op3;

    result->next = NULL;

    return result;
}


block_list *mkblock_list (char *label, char *new_label, instr *instructions, value regs[])
{
    block_list *result;

    result = (block_list *) malloc(sizeof (block_list));

    if (result == NULL) {
        fprintf(stderr, "Not enough memory!\n");
        exit(1);
    }

    result->label = malloc(strlen(label) * sizeof (char));
    strcpy(result->label, label);
    
    if (new_label != NULL) {
        result->new_label = malloc(strlen(new_label) * sizeof (char));
        strcpy(result->new_label, new_label);
    }

    result->instructions = instructions;

    result->regs = regs;

    result->next = NULL;

    return result;
}


/*
 * Copy a list of blocks.
 *
 * Similar to copying a list of instructions.
 */
block_list *copy_block_list (block_list *src)
{
    block_list *cblk,
               *last,
               *dest;

    int first;

    last = NULL;

    cblk = (block_list *) malloc(sizeof (block_list));

    first = 1;
    while (src != NULL) {
        cblk -> label = (char *) malloc((strlen(src -> label) + 1) 
                                         * sizeof (char));
        
        strcpy_(cblk -> label, src -> label);

        cblk -> instructions = copy_instruction_list(src -> instructions);

        cblk -> next = (block_list *) malloc(sizeof (block_list));

        if (first) {
            first = 0;
            dest = cblk;
        }
        
        last = cblk;
        cblk = cblk -> next;
        src = src -> next;
    }

    if (last != NULL) {
        last -> next = NULL;
    }

    return dest;
}


int *copy_int_array(int src[])
{
    int *result,
        i;

    result = (int *) malloc(NUM_REGISTERS * sizeof(binding_time));

    i = 0;
    for (; i < NUM_REGISTERS; i++) {
        result[i] = src[i];
    }

    return result;
}


instr *eval_math_operation(instr *old_instruction, binding_time bta[], value regs[])
{
    instr *new_instruction;
    new_instruction = NULL;
    
    if(old_instruction == NULL) {
        return NULL;
    }

    if(bta[old_instruction -> operands[0]] >= 1 && 
       bta[old_instruction -> operands[1]] >= 1) {

        /* TODO Find out why this is necessary. */
        /* regs[old_instruction -> operands[2]] = -1; */
        switch (old_instruction -> tag) {
            case ADD:
                regs[old_instruction -> operands[2]] = 
                    regs[old_instruction -> operands[0]]
                    + regs[old_instruction -> operands[1]];
            break;

            case SUB:
                if (regs[old_instruction -> operands[0]] >= 
                        regs[old_instruction -> operands[1]]) {

                    regs[old_instruction -> operands[2]] = 
                        regs[old_instruction -> operands[0]]
                        - regs[old_instruction -> operands[1]];

                } else {
                    __CMIX(pure) fprintf(stderr, 
                            "Result of subtraction is less than 0!\n");

                    __CMIX(pure) exit(1);
                }
            break;

            case MUL:
                regs[old_instruction -> operands[2]] = 
                    regs[old_instruction -> operands[0]]
                    * regs[old_instruction -> operands[1]];
            break;

            case DIV:
                if (regs[old_instruction -> operands[1]] != 0) {
                    regs[old_instruction -> operands[2]] = 
                        regs[old_instruction -> operands[0]]
                        / regs[old_instruction -> operands[1]];
                } else {
                    __CMIX(pure) fprintf(stderr, "Division by 0!\n");
                    __CMIX(pure) exit(1);
                }
            break;
        }
    } else {
        new_instruction = mkinstr(MOVI, NULL, 0, 0, 0);

        if (bta[old_instruction -> operands[0]] < 0 && 
            bta[old_instruction -> operands[1]] < 0) {

            /* If both parts of the operation were dynamic, we do not create a
             * MOVI instruction.
             */
            new_instruction = NULL;
        } else if (bta[old_instruction -> operands[0]] < 0) {

            new_instruction -> operands[0] = regs[old_instruction -> operands[1]];
            new_instruction -> operands[1] = old_instruction -> operands[1];

        } else {

            new_instruction -> operands[0] = regs[old_instruction -> operands[0]];
            new_instruction -> operands[1] = old_instruction -> operands[0];
        }
    }

    return new_instruction;
}
    

#if 0
/*
 * Peform simple specialization of mathematical instructions.
 *
 * For `copy' and `math' instructions where all registers are known we simply
 * update the registers. When we encounter a dynamic instruction that uses a
 * static register we generate a MOVI instruction for the constant value of
 * that register.
 */
block_list *math_spec ( block_list *blocks
                      , binding_time old_bta[]
                      , value regs[]
                      )
{
    block_list *result,
               *prev_blk,
               *nblk,
               *cblk;

    instr *cinstr,
          *ninstr,
          *prev,
          *new;

    int first,
        first_block,i;

    binding_time bta[NUM_REGISTERS];
    for(i=0; i < NUM_REGISTERS; bta[i] = old_bta[i], i++);

    cblk = blocks;
    first_block = 1;
    while (cblk != NULL) {

        first = 1;
        cinstr = cblk -> instructions;
        ninstr = NULL;

        while (cinstr != NULL) {
            switch (cinstr -> tag) {
                case MOV:
                    if (bta[cinstr -> operands[0]] >= 1) {

                        regs[cinstr -> operands[1]] = 
                            regs[cinstr -> operands[0]];
                        bta[cinstr -> operands[1]] = 1;

                    } else {
                        bta[cinstr -> operands[1]] = -1;
                        new = copy_instruction(cinstr);
                        if (first) {
                            first = 0;
                            ninstr = new;
                        } else {
                            prev -> next = new;
                        }
                        prev = new;
                    }
                    break;
                
                case MOVI:
                    bta[cinstr -> operands[1]] = 1;
                    regs[cinstr -> operands[1]] = cinstr -> operands[0];
                    break;
                
                case ADD:
                case SUB:
                case MUL:
                case DIV:
                    new = eval_math_operation(cinstr, bta, regs);

                    /* Since the bta we pass in the previous line is not modified
                     * we need to do it now 
                     */
                    if(bta[cinstr -> operands[0]] >= 1 && bta[cinstr -> operands[1]] >= 1) {
                        bta[cinstr -> operands[2]] = 1;
                    } else {
                        bta[cinstr -> operands[2]] = -1;
                    }

                    if(new != NULL) {
                        if (first) {
                            first = 0;
                            ninstr = new;
                        } else {
                            prev -> next = new;
                        }

                        prev = new;
                    }

                    if(bta[cinstr -> operands[2]] < 0) {
                        new = copy_instruction(cinstr);
                        if (first) {
                            first = 0;
                            ninstr = new;
                        } else {
                            prev -> next = new;
                        }
                        prev = new;
                    }

                break;

                case HALT:
                    for(i=0;i<NUM_REGISTERS;i++) {
                        if(bta[i] >= 1) {
                            new = mkinstr (MOVI, NULL, regs[i], i, -1);

                            if (first) {
                                first = 0;
                                ninstr = new;
                            } else {
                                prev -> next = new;
                            }

                            prev = new;
                        }
                    }

                    new = copy_instruction(cinstr);

                    if (first) {
                        first = 0;
                        ninstr = new;
                    } else {
                        prev -> next = new;
                    }

                    prev = new;
                break;

                default:
                    __CMIX(pure) fprintf(stderr, "Unknown instruction type 0!\n");
                    exit(1);
                    
            }

            cinstr = cinstr -> next;
        }

        nblk = (block_list *) malloc(sizeof (block_list));
        nblk -> label = malloc(strlen(cblk->label) * sizeof (char));
        strcpy_(nblk -> label, cblk -> label);
        nblk -> instructions = ninstr;
        nblk -> next = NULL;

        if (first_block) {
            first_block = 0;
            result = nblk;
            prev_blk = nblk;
        } else {
            prev_blk -> next = nblk;
            prev_blk = prev_blk -> next;
        }

        cblk = cblk -> next;
    }

    return result;
}
#endif


/*
 * Compare to sets of register values.
 *
 * If they are equal return 0, else return 1.
 */
int compare_registers (value regsA[], value regsB[])
{
    int i;

    i = 0;
    for (; i < NUM_REGISTERS; i++) {
        if (regsA[i] != regsB[i]) {
            return 1;
        }
    }

    return 0;
}


/*
 * Find a block in the original program.
 */
block_list *find_block (char *label, block_list *pgm)
{
    block_list *current;

    current = pgm;

    while (current != NULL) {
        if (strcmp(current->label, label) == 0) {
            return current;
        }

        current = current -> next;
    }

    return NULL;
}


/*
 * Find a block in the residual program.
 */
block_list *find_residual_block (char *label, value regs[], block_list *pgm)
{
    block_list *current;

    current = pgm;

    while (current != NULL) {
        if (strcmp(current->label, label) == 0 &&
                compare_registers(current->regs, regs) == 0) {
            return current;
        }

        current = current -> next;
    }

    return NULL;
}


/*
 * Find the block marked by `label' and update it with a new set of
 * instructions.
 */
void update_block_instr(char *label, value regs[], instr *ninstr, block_list *pgm)
{
    block_list *to_update;

    to_update = find_residual_block(label, regs, pgm);

    if (to_update != NULL) {
        to_update -> instructions = ninstr;
    }
}

/*
 * Generated a unique block name using local static counter.
char *get_block_relocated_name (char *block_name)
{
    static int program_relocator = 0;
    char *relocated_name;

    relocated_name = (char *) malloc((5 + strlen(block_name)) * sizeof(char));

    __CMIX(pure) sprintf(relocated_name, "%s_%04d", block_name, (++program_relocator));

    return relocated_name;
}
 */


/*
 * Append a block to a block list.
 */
block_list *append_block ( char *label
                         , char *new_label
                         , instr *instructions
                         , value regs[]
                         , block_list *pgm
                         )
{
    block_list *new,
               *current;

    new = mkblock_list(label, new_label, instructions, regs);

    if (pgm == NULL) {
        return new;
    }

    current = pgm;
    while (current -> next != NULL) {
        current = current -> next;
    }
    
    current -> next = new;

    return pgm;
}


/*
 * Implementation of recursive polyvariant specialization.
 */
block_list *poly_spec ( char *label
                      , value regs[]
                      , binding_time bt[]
                      , block_list *pgm
                      , block_list *residual_pgm
                      )
{

    block_list *current_blk,
               *residual_blk,
               *spec_blk;

    instr *cinstr,
          *ninstr,
          *prev,
          *new;

    binding_time *bta;
    value *new_regs;

    int first,
        i,
        block_finished;

    char *label_copy;

    bta = copy_int_array(bt);
    new_regs = copy_int_array(regs);

    residual_blk = find_residual_block(label, new_regs, residual_pgm);

    if (residual_blk == NULL) {
        strcpy_(label_copy, label);
        residual_pgm = append_block( label_copy
                                   , label_copy
                                   , NULL
                                   , regs
                                   , residual_pgm
                                   );

        current_blk = find_block(label, pgm);

        block_finished = 0;
        first = 1;
        cinstr = current_blk -> instructions;
        ninstr = NULL;

        while (cinstr != NULL && !block_finished) {
            switch (cinstr -> tag) {
                case MOV:
                    if (bta[cinstr -> operands[0]] >= 1) {

                        new_regs[cinstr -> operands[1]] = 
                            new_regs[cinstr -> operands[0]];

                        bta[cinstr -> operands[1]] = 1;

                    } else {
                        bta[cinstr -> operands[1]] = -1;
                        new = copy_instruction(cinstr);

                        if (first) {
                            first = 0;
                            ninstr = new;
                        } else {
                            prev -> next = new;
                        }

                        prev = new;
                    }
                break;
                
                case MOVI:
                    bta[cinstr -> operands[1]] = 1;
                    new_regs[cinstr -> operands[1]] = cinstr -> operands[0];
                break;

                case ADD:
                case SUB:
                case MUL:
                case DIV:
                    new = eval_math_operation(cinstr, bta, new_regs);

                    /* Since the bta we pass in the previous line is not modified
                     * we need to do it now 
                     */
                    if (bta[cinstr -> operands[0]] >= 1 &&
                        bta[cinstr -> operands[1]] >= 1) {

                        bta[cinstr -> operands[2]] = 1;
                    } else {
                        bta[cinstr -> operands[2]] = -1;
                    }

                    if (new != NULL) {
                        if (first) {
                            first = 0;
                            ninstr = new;
                        } else {
                            prev -> next = new;
                        }

                        prev = new;
                    }

                    if (bta[cinstr -> operands[2]] < 0) {
                        new = copy_instruction(cinstr);
                        if (first) {
                            first = 0;
                            ninstr = new;
                        } else {
                            prev -> next = new;
                        }
                        prev = new;
                    }

                break;

                case BEQ:
                    if (bta[cinstr -> operands[0]] >= 1 &&
                        bta[cinstr -> operands[1]] >= 1) {
                        
                        if (new_regs[cinstr -> operands[0]] == 
                                new_regs[cinstr -> operands[1]]) {

                            residual_pgm = poly_spec( cinstr -> jump_target
                                                    , new_regs
                                                    , bta
                                                    , pgm
                                                    , residual_pgm
                                                    );
                            block_finished = 1;
                        }
                    } else {
                        new = NULL;

                        if (bta[cinstr -> operands[0]] >= 1) {
                            new = mkinstr( MOVI
                                         , NULL
                                         , new_regs[cinstr -> operands[0]]
                                         , cinstr -> operands[0]
                                         , -1
                                         );

                        } else if (bta[cinstr -> operands[1]] >= 1) {
                            new = mkinstr( MOVI
                                         , NULL
                                         , new_regs[cinstr -> operands[1]]
                                         , cinstr -> operands[1]
                                         , -1
                                         );
                        }

                        if(new != NULL) {
                            if (first) {
                                first = 0;
                                ninstr = new;
                            } else {
                                prev -> next = new;
                            }
                            prev = new;
                        }

                        new = copy_instruction(cinstr);
                        
                        if (first) {
                            first = 0;
                            ninstr = new;
                        } else {
                            prev -> next = new;
                        }

                        prev = new;

                        residual_pgm = poly_spec( cinstr -> jump_target
                                                , new_regs
                                                , bta
                                                , pgm
                                                , residual_pgm
                                                );

                        spec_blk = find_residual_block( cinstr -> jump_target
                                                      , new_regs
                                                      , residual_pgm
                                                      );

                        strcpy_(new -> jump_target, spec_blk -> new_label);
                    }
                break;

                case HALT:
                    for(i = 0; i < NUM_REGISTERS; i++) {
                        if(bta[i] >= 1) {
                            new = mkinstr(MOVI, NULL, new_regs[i], i, -1);

                            if (first) {
                                first = 0;
                                ninstr = new;
                            } else {
                                prev -> next = new;
                            }

                            prev = new;
                        }
                    }

                    new = copy_instruction(cinstr);

                    if (first) {
                        first = 0;
                        ninstr = new;
                    } else {
                        prev -> next = new;
                    }

                    prev = new;
                break;

                default:
                    __CMIX(pure) fprintf(stderr, "Unknown instruction type 0!\n");
                    __CMIX(pure) exit(1);
            }

            cinstr = cinstr -> next;
        }

        update_block_instr(label, regs, ninstr, residual_pgm);

        if (current_blk -> next != NULL && !block_finished) {
            return poly_spec( current_blk -> next -> label
                            , new_regs
                            , bta
                            , pgm
                            , residual_pgm
                            );
        }
    }

    return residual_pgm;
}


/*
 * Entry function to our specializer.
 *
 * From here each phase of specialization is started and the result of phase n
 * is subsequently used by phase n + 1.
 *
 */
void specialize ( char *filename
                , binding_time bt[]
                , value regs[]
                )
{
    block_list *blocks,
               *phase1,
               *result;

    binding_time bta[NUM_REGISTERS];
    value new_regs[NUM_REGISTERS];
    
    int i;

    parse_program(&blocks, filename);

    for (i = 0; i < NUM_REGISTERS; new_regs[i] = regs[i], bta[i] = bt[i], i++);

    /* result = math_spec(blocks, bta, new_regs); */
    result = poly_spec(blocks->label, new_regs, bta, blocks, NULL);

    /*
    __CMIX(pure) printf("===========================\n");
    __CMIX(pure) printf("   BTA & REGISTER DUMP.  \n");
    __CMIX(pure) printf("===========================\n");

    i = 0;
    for (; i < NUM_REGISTERS; i++) {
        __CMIX(pure) printf("bta[%d] = %d\t", i, bta[i]);
        __CMIX(pure) printf("reg[%d] = %d\n", i, regs[i]);
    }

    __CMIX(pure) printf("===========================\n\n"); 
    */

    __CMIX(residual) print_blocks(result);


}
