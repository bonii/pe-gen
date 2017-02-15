/*
    ast.h is part of FCL_INT
    ast.h - Header file for AST data structures.
    
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

#ifndef AST_H
#define AST_H

/**
 * Expression data type.
 */
typedef enum etype { INT, VAR, OP } etype;
 
typedef struct expr {
    etype type;

    int intval;
    char *id;
    struct __op *op;
    struct expr *next;
} expr;


/**
 * Binary operations.
 */
typedef enum otype { PLUS, MIN, MUL, DIV, EQ, LT, GT, LTE, GTE } otype;

typedef struct __op {
    otype type;
    expr *operands;
} __op;


/*****************************************************************************/


typedef struct __assign {
    char *variable;
    expr *expression;

    struct __assign *next;
} __assign;


typedef struct __cnd {
    expr *condition;
    char *l_then;
    char *l_else;
} __cnd;


/**
 * Jump data type.
 */
typedef enum jtype { GOTO, CND, RETURN } jtype;


typedef struct jump {
    jtype tag;
    
    char  *label;
    __cnd *conditional;
    expr  *return_stmt;
} jump;


/**
 * A basic block consists of a label, a number of assignments and a jump.
 */
typedef struct basic_block { 
    char *label;

    __assign *assignments;
    jump *jump;

    struct basic_block *next;
} basic_block;


/*****************************************************************************/


typedef struct varlist {
    char *id;
    struct varlist *next;
} varlist;


/*****************************************************************************/


/**
 * Program data type.
 */
typedef struct program {
    char    *entry_label;
    basic_block *blocks;
} program;


/*****************************************************************************/


expr *mkint (int);
expr *mkvar (char *);
expr *mkop (otype, expr *);

__assign *mkassign (char *, expr *);

jump *mkgoto (char *);
jump *mkcnd (expr *, char *, char *);
jump *mkreturn (expr *);

varlist *mkreadvar (char *);
basic_block *mkbasic_block(char *, __assign *, jump *);
program *mkprogram (char *, basic_block *);

#endif
