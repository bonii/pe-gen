/*
    fcl.h is part of FCL_SPEC
    fcl.h - Data structures, global variables and function prototypes for FCL specialiser
    
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

#ifndef FCL_H
#define FCL_H


#include "ast.h"


/*****************************************************************************/


/**
 * Store data type.
 */
typedef struct store {
    char *key;
    int value;

    struct store *next;
} store;


/*****************************************************************************/


/**
 * Global variables.
 */

block_list *eval_list;
block_list *seen_list;
block_list *residual_jump_block_name_list;
varlist *global_use_variables;

const char *INFO = "info";
const char *DEBUG = "debug";
const char *WARN = "warn";
static int GLOBAL_USE_VARIABLES = 0;

/*****************************************************************************/


/**
 * Prototypes.
 */

store *mkstore (char*, int);
store *build_store (int, char **, int*);

int lookup (char*, store*);
int exists_in_env(char *, store *);
store *update (store*, char*, int);

basic_block *find_basic_block (char *, basic_block *);

basic_block *eval_block(basic_block *, program *, store *, char *);
__assign *eval_assignments(__assign *, store **);
expr *eval_expression (expr *, store *);
expr *eval_n_ary_op (otype, expr *, store *);
expr *eval_binary_op (otype, expr *, store *);

program *specialize (char *, store *);


#endif
