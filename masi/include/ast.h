/*
    ast.h is part of MASI
    ast.h - Data-structures and utilities to deal with the abstract syntax tree
    
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

#ifndef AST_H
#define AST_H


#define MAX_OPERANDS 3
#define MAX_OPERAND_LENGTH 10


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
    instr *instructions;

    struct block_list *next;
} block_list;


/*****************************************************************************/


typedef int key;

typedef int value;

typedef int binding_time;


/*****************************************************************************/


instr *mkinstr (itype, char *, int, int, int);

block_list *mkblock_list(char *, instr *);

#endif
