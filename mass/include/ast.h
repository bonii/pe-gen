/*
    ast.h is part of MASS
    ast.h - Data structure definition for MASS.
    
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

    Written by Vivek Shah <bonii@kompiler.org>, Tim van Deurzen <tim(at)kompiler.org>

*/

#ifndef AST_H
#define AST_H


#define MAX_OPERANDS 3
#define MAX_OPERAND_LENGTH 10
#define NUM_REGISTERS 16

/*****************************************************************************/


typedef int key;

typedef int value;

typedef int binding_time;


/*****************************************************************************/


/*
 * The instruction data type.
 */
typedef enum itype { MOV, MOVI, ADD, SUB, MUL, DIV, BEQ, HALT } itype;

typedef struct instr {
    itype tag;
    int operands[MAX_OPERANDS];
    char *jump_target;

    struct instr *next;
} instr;


/*
 * The block list data type.
 */
typedef struct block_list {
    char *label;
    char *new_label;
    instr *instructions;
    value *regs;

    struct block_list *next;
} block_list;



/*****************************************************************************/


instr *mkinstr (itype, char *, int, int, int);

block_list *mkblock_list (char *, char *, instr *, value[]);

#endif
