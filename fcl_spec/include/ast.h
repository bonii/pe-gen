/*
    ast.h is part of FCL_SPEC
    ast.h - Utilities and declarations for AST
    
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

#ifndef AST_H
#define AST_H


/*****************************************************************************/


/*** JUMPS ***/

/* For conditional jumps. */
#define JMP_COND(b) b->jump->conditional->condition
#define JMP_THEN(b) b->jump->conditional->l_then
#define JMP_ELSE(b) b->jump->conditional->l_else

/* For return jumps. */
#define JMP_RETN(b) b->jump->return_stmt

/* For unconditional jumps. */
#define JMP_LABL(b) b->jump->label


/*** EXPRESSIONS ***/

/* For operators */

#define EXP_OPER(e) e->op->operands


/*****************************************************************************/


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
typedef enum otype { PLUS, MIN, MUL, DIV, EQ, LT, GT, LTE, GTE, MOD } otype;

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


typedef struct varlist {
    char *variable;
    struct varlist *next;
} varlist;


/*****************************************************************************/

/**
 * A basic block consists of a label, a number of assignments and a jump.
 */
typedef struct basic_block { 
    char *label;
    varlist *use_variables;

    __assign *assignments;
    jump *jump;

    struct basic_block *next;
} basic_block;


/*****************************************************************************/


/**
 * Program data type.
 */
typedef struct program {
    int     argc;
    char    *entry_label;
    varlist *variables;
    basic_block *blocks;
} program;


/*****************************************************************************/


typedef struct block_list {
    char *block_name;
    basic_block *block;
    struct store *env;

    struct block_list *next;
} block_list;


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
program *mkprogram (char *, varlist *, basic_block *);

block_list *mkblock_list(char *, basic_block *, struct store *);
basic_block *copy_block(basic_block *);
void prepend_list(block_list **, char *, basic_block *, struct store *);
void append_list(block_list **, char *, basic_block *, struct store *);

#endif
