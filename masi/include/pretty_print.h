/*
    pretty_print.h is part of MASI
    pretty_print.h - Prototype functions to print AST
    
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

#ifndef PRETTY_PRINT_H
#define PRETTY_PRINT_H

#include <stdio.h>

#include "ast.h"


/*****************************************************************************/


FILE *outfile;


/*****************************************************************************/


void print_instructions (instr *);

void print_blocks (block_list *);


#endif
