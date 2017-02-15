/*
    fcl.h is part of FCL_INT
    fcl.h - Datatype definitions and prototypes for the FCL interpreter.
    
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

#ifndef FCL_H
#define FCL_H


#include "ast.h"


/*****************************************************************************/


/**
 * Global variables.
 */

key *env_keys;
value *env_values;

program *pgm;


/*****************************************************************************/


/**
 * Prototypes.
 */

int lookup (char *);

basic_block *find_basic_block (char *);

int eval_entry_block (basic_block *, varlist *, int, int *);
int eval_block (basic_block *);
int eval_expression (expr *);
int eval_n_ary_op (otype, expr *);
int eval_binary_op (otype, expr *);
int eval_jump (jump *);

int interpret (char *, int, int *);

#endif
