/*
    util.c is part of FCL_INT
    util.c - Store utility functions.
    
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "util.h"
#include "fcl.h"


/*****************************************************************************/


/**
 * Function for allocating a store object.
store *mkstore (char *key, int value)
{
    store *result;

    result = (store *) malloc(sizeof (store));

    result->key = malloc(strlen(key) * sizeof (char));
    strcpy(result->key, key);

    result->value = value;
    result->next  = NULL;

    return result;
}
 */


/**
 * Build the store give two arrays of respectively variables and values.
store *build_store(int size, char **variables, int *values)
{
    int i;
    store *result,
          *new;

    result = mkstore(variables[0], values[0]);

    for (i = 1; i < size; i++) {
        new = mkstore(variables[i], values[i]);
        new->next = result;
        result = new;
    }

    return result;
}
 */


/*****************************************************************************/

/**
 * Update function to set a variable in the store to a new value;
 *
 * If the env parameter is NULL this function implicitly creates the initial
 * environment.
 */
void update (char *key_, int value_)
{
    key *current_key,
        *new_key;

    value *current_value,
          *new_value;
    
    current_key = env_keys;
    current_value = env_values;

    /*
     * Search through the store to see if the key exists and update if the key
     * is found.
     */
    while (current_key != NULL) {
        if (strcmp(current_key->id, key_) == 0) {
            current_value->intval = value_;
            return;
        } 

        current_key = current_key->next;
        current_value = current_value->next;
    }

    /*
     * Allocate memory and initialize the new values;
     */
    new_key = (key *) malloc(sizeof(key));
    new_value = (value *) malloc(sizeof(value));

    new_key->id = malloc(strlen(key_) * sizeof(char));
    strcpy(new_key->id, key_);

    new_value->intval = value_;

    new_key->next = NULL;
    new_value->next = NULL;

    /*
     * Prepend the new values and update the global pointers.
     */
    current_key->next = new_key;
    current_value->next = new_value;
}

/**
 * Declare and initialize a variable in the program.
 *
 * This just takes the variable identifier and puts it into the environment
 * with standard value 0.
 */
void spectime_declare (char *key_, key *keys)
{
    key *current_key,
        *new_key;

    current_key = keys;

    /*
     * Allocate memory and initialize the new identifiers;
     */
    new_key = (key *) malloc(sizeof(key));

    new_key->id = malloc(strlen(key_) * sizeof(char));
    strcpy(new_key->id, key_);

    new_key->next = NULL;

    /*
     * Append the new identifier.
     */
    current_key->next = new_key;
}
