/*
    mass.h is part of MASS
    mass.h - Function prototypes for the MAS Specializer (MASS).
    
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

#ifndef MASS_H
#define MASS_H

#include "ast.h"



/*****************************************************************************/


void strcpy_(char *, const char *);

instr *copy_instruction(instr *);

instr *copy_instruction_list (instr *);

block_list *copy_block_list (block_list *);

void update_block_instr(char *, value [], instr *, block_list *);

block_list *find_residual_block (char *, value [], block_list *);

block_list *find_block (char *, block_list *);

block_list *prepend_block (char *, instr *, value [], block_list *);

int compare_registers (value *, value *);

block_list *append_block (char *, char *, instr *, value *, block_list *);

block_list *math_spec (block_list *, binding_time *, value *);

block_list *poly_spec ( char *, value *, binding_time *, block_list *
                      , block_list *
                      );

binding_time *copy_binding_times(binding_time *);

binding_time *binding_time_analysis (block_list *, binding_time *);

void specialize (char *, binding_time[], value *);


#endif
