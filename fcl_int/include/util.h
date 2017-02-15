/*
    util.h is part of FCL_INT
    util.h - A simple collection of utility macros.
    
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

#ifndef UTIL_H
#define UTIL_H


/*****************************************************************************/


/**
 * Store data type.
typedef struct store {
    char *key;
    int value;

    struct store *next;
} store;
 */

/*
 * Decomposed store structure. The key struct is a linked list of variable
 * identifiers, the value struct a linked list of integer values.
 */
typedef struct key {
    char *id;
    
    struct key * next;
} key;

typedef struct value {
    int intval;

    struct value *next;
} value;


/*****************************************************************************/


/*
 * For annotating calls for CMIX one may use the special macro __CMIX(),
 * however if compiling normally these annotations break the code. This macro
 * will remove the annotations when compiling without CMIX.
 */

#ifndef __CMIX
#define __CMIX(x)
#endif


/*****************************************************************************/


/**
 * Prototypes.
 */

/*
store *mkstore (char *, int);
store *build_store (int, char **, int *);
 */

void update (char *, int);


#endif
