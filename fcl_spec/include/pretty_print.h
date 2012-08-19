/*
    pretty_print.h is part of FCL_SPEC
    pretty_print.h - Headers and function definitions for printing AST
    
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

    Written by Vivek Shah <bonii@kompiler.org>, Tim van Deurzen <tim(at)kompiler.org>

*/

#ifndef PRETTY_PRINT_H
#define PRETTY_PRINT_H

#include <stdio.h>

#include "ast.h"


/*****************************************************************************/


#define OUT(fmt, ...)                       \
    if (debug_print) {                      \
        printf(fmt, __VA_ARGS__);           \
    } else {                                \
        fprintf(outfile, fmt, __VA_ARGS__); \
    }

#define OUT_(fmt)               \
    if (debug_print) {          \
        fprintf(stdout, fmt);   \
    } else {                    \
        fprintf(outfile, fmt);  \
    }


/*****************************************************************************/


FILE *outfile;

int debug_print;


/*****************************************************************************/


/**
 * Prototypes.
 */

void print_expression(expr *, int);
void print_block(basic_block *); 
void print_program(program *);


#endif
