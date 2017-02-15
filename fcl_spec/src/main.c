/*
    main.c is part of FCL_SPEC
    main.c - FCL specializer main program
    
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


/*****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>


#ifdef __DEBUG_ON
    #include "dbug.h"
#else
    #define DBUG_ENTER(a1)
    #define DBUG_RETURN(a1) return(a1)
    #define DBUG_VOID_RETURN return
    #define DBUG_EXECUTE(keyword,a1) {}
    #define DBUG_PRINT(keyword,arglist) {}
    #define DBUG_PUSH(a1) {}
    #define DBUG_POP() {}
    #define DBUG_PROCESS(a1) {}
    #define DBUG_FILE (stderr)
    #define DBUG_SETJMP setjmp
    #define DBUG_LONGJMP longjmp
    #define DBUG_DUMP(keyword,a1,a2) {}
    #define DBUG_IN_USE 0
    #define DEBUGGER_OFF
    #define DEBUGGER_ON
    #define DBUG_my_pthread_mutex_lock_FILE
    #define DBUG_my_pthread_mutex_unlock_FILE
    #define DBUG_ASSERT(A) {}
#endif

#include "util.h"
#include "fcl.h"
#include "ast.h"
#include "pretty_print.h"


/*****************************************************************************/


extern void parse_program(program **, char *);
extern int yydebug;

/*****************************************************************************/


/*
 * Checks if two enviroments are the same.
 * 
 * Two environments are identical if they have the same size, variables and 
 * all variables have the same value. The number of variables that need to be 
 * checked may be constrained by the list 'block_use_variables' which contains 
 * the variables that are actually in use.
 *
 * Return 0 when env1 and env2 are equal and -1 otherwise.
 */
int are_env_same(store *env1, store *env2, varlist *block_use_variables) 
{
    varlist *use_variables;
    DBUG_ENTER("are_env_same");
    DBUG_PRINT("params", ( "env1: %p, env2: %p, block_use_variables %p\n"
                         , env1, env2, block_use_variables
                         ));

    if (env1 == NULL && env2 == NULL || env1 == env2) {
        DBUG_RETURN(0);
    } else if (env1 == NULL && env2 != NULL || env2 == NULL && env1 != NULL) {
        DBUG_RETURN(-1);
    }
    
    use_variables = (GLOBAL_USE_VARIABLES) ? global_use_variables 
                                           : block_use_variables;

    while (use_variables != NULL) {
      
        /* Variable exists in both environments and is unequal or variable
         * exists in one environment but not the other.
         */
        if (!((   exists_in_env(use_variables -> variable, env1) != -1
               && exists_in_env(use_variables -> variable, env2) != -1 
               && lookup(use_variables -> variable, env1) 
               == lookup(use_variables -> variable, env2)) 
                  || (exists_in_env(use_variables -> variable, env1) == -1 
                  &&  exists_in_env(use_variables -> variable, env2) == -1))) {

            DBUG_RETURN(-1);
        }

        use_variables = use_variables -> next;
    }
    
    DBUG_RETURN(0);
}


/*
 * Deep copy an expression.
 *
 * Iteratively and recursively copy all parts of an expression.
 * We iterate over all operands and recurse if the operand is itself an
 * expression.
 */
expr *copy_expression(expr *expression) 
{
    expr *new_expression,
         *current;

    expr *prev_op,
         *new_op;

    DBUG_ENTER("copy_expression");
    DBUG_PRINT("params", ("expression: %p\n", expression));

    if(expression == NULL) {
        DBUG_RETURN(NULL);
    }

    switch(expression -> type) {
        case INT:
            new_expression = mkint(expression -> intval);
            new_expression -> next = NULL;
        break;

        case VAR:
            new_expression = mkvar(expression -> id);
            new_expression -> next = NULL;
        break;

        case OP:
            current = EXP_OPER(expression) -> next;

            new_op = copy_expression(EXP_OPER(expression));

            new_expression = mkop(expression -> op -> type, new_op);

            prev_op = new_op;

            while(current != NULL) {
                new_op = copy_expression(current);
                new_op -> next = NULL;
                prev_op -> next = new_op;
                prev_op = new_op;
                current = current -> next;
            }
        break;
    }

    DBUG_RETURN(new_expression);
}


/*
 * Append the assignments of the SRC basic block to the assignment list of the
 * DEST basic block. This function facilitates part of transition compression
 * between basic blocks.
 */
void append_assign_statement_migrate_jump ( basic_block *dest_block
                                          , basic_block *src_block
                                          ) 
{
    __assign *prev_dest_assign,
             *current_dest_assign;

    DBUG_ENTER("append_assign_statement_migrage_jump");
    DBUG_PRINT("params", ( "dest_block: %p, src_block: %p\n"
                         , dest_block, src_block
                         ));

    prev_dest_assign = NULL;
    current_dest_assign = dest_block -> assignments;

    while (current_dest_assign != NULL) {
        prev_dest_assign = current_dest_assign;
        current_dest_assign = current_dest_assign -> next;
    }

    if (prev_dest_assign == NULL) {
        dest_block -> assignments = src_block -> assignments;
    } else {
        prev_dest_assign -> next = src_block -> assignments;
    }

    /*
     * Migrate the jump.
     */
    dest_block -> jump = src_block -> jump;

    DBUG_VOID_RETURN;
}


/*
 * Check if the block 'block_name' has been evaluated in environment 'env' 
 * before.
 *
 * Return the block if it was evaluated before and NULL otherwise.
 */
block_list *evaluated_before( char *block_name
                            , store *env
                            , varlist *block_use_variables
                            ) 
{
    block_list *current;

    DBUG_ENTER("evaluated_before");
    DBUG_PRINT("params", ( "block_name: %s, env: %p, block_use_variables: %p\n"
                         , block_name, env, block_use_variables
                         ));

    current = seen_list;
    while (current != NULL) {
        if (strcmp(current -> block_name, block_name) == 0 
            && are_env_same(env, current -> env, block_use_variables) == 0) {

            DBUG_RETURN(current);
        }

        current = current -> next;
    }    

   DBUG_RETURN(NULL);
}


/*
 * Extend the seen list with a new block.
 *
 * The new block is prepended to the list, the most efficient way of extending
 * the list.
 */
void prepend_seen_list(char *block_name, basic_block *result, store *env) 
{
    block_list *new_entry;

    DBUG_ENTER("prepend_seen_list");
    DBUG_PRINT("params", ( "block_name: %s, result: %p, env: %p\n"
                         , block_name, result, env
                         ));

    new_entry = mkblock_list(block_name, result, env);

    new_entry -> next = seen_list;
  
    seen_list = new_entry;
    
    DBUG_VOID_RETURN;
}


/*
 * Update a block in the list of evaluated blocks or insert the block if it
 * did not exist.
 */
void update_seen_list( char *block_name
                     , basic_block *result
                     , store *env
                     , varlist *use_variables
                     ) 
{
    block_list *current;
    int found;

    DBUG_ENTER("update_seen_list");
    DBUG_PRINT("params", ( "block_name: %s, result: %p, env: %p, use_variables: %p\n"
                         , block_name, result, env, use_variables
                         ));

    current = seen_list;
    found = 0;
    while (current != NULL) {
      if (strcmp(current -> block_name, block_name) == 0 
          && are_env_same(current -> env, env, use_variables) == 0) {

          current -> block = result;
          found = 1;
        }
        
        current = current -> next;
    }

    if(!found) {
        prepend_list(&seen_list, block_name, result, env);

        /*
        prepend_seen_list(block_name, result, env);
         */
    }
    
    DBUG_VOID_RETURN;
}


/*
 * Lookup if a variable is available in the store.
 *
 * Returns 0 on success -1 on failure.
 */
int exists_in_env(char *key, store *env) 
{
    store *current;

    DBUG_ENTER("exists_in_env");
    DBUG_PRINT("params", ("key: %s, env: %p\n"
                         , key, env
                         ));

    current = env;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            DBUG_PRINT(INFO, ("lookup::DEFINED %s = %d\n"
                             , key, current->value));
            DBUG_RETURN(0);
        }
        current = current->next;
    }

    DBUG_PRINT(INFO, ("lookup::UNDEFINED\n"));
    DBUG_RETURN(-1);
}


/*
 * Deep copy a store.
 */
store *copy_store(store *env) 
{
    store *new_store, 
          *prev_store,
          *new_env,
          *current;

    int first;

    DBUG_ENTER("copy_store");
    DBUG_PRINT("params", ("env: %p\n", env));

    new_env = NULL;
    current = env;
    prev_store = NULL;

    first = 1;
    while (current != NULL) {
        new_store = mkstore(current->key, current->value);

        if (first) {
            new_env = new_store;
            first = 0;
        }

        if (prev_store != NULL) {
            prev_store -> next = new_store;
        }

        prev_store = new_store;
        current = current -> next;
    }

    DBUG_RETURN(new_env);
}


/*
 * Allocate a new store.
 */
store *mkstore(char *key, int value) 
{
    store *result;

    DBUG_ENTER("mkstore");
    DBUG_PRINT("params", ("key: %s, value: %d\n", key, value));

    result = (store *) malloc(sizeof(store));

    result->key = (char *) malloc(strlen(key) * sizeof(char));
    strcpy(result->key, key);

    result->value = value;
    result->next = NULL;

    DBUG_RETURN(result);
}


/*****************************************************************************/


/*
 * Lookup function for finding a variables value in the store.
 */
int lookup(char *key, store *env) 
{
    store *current;

    DBUG_ENTER("lookup");
    DBUG_PRINT("params", ("key: %s, store: %p\n", key, env));

    current = env;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            DBUG_RETURN(current->value);
        }

        current = current->next;
    }

    DBUG_RETURN(0);
}


/*
 * Update function to set a variable in the store to a new value;
 *
 * If the env parameter is NULL this function implicitly creates the initial
 * environment.
 */
store *update(store *env, char *key, int value) 
{
    store *current, *result;

    DBUG_ENTER("update");
    DBUG_PRINT("params", ("env: %p, key: %s, value: %d\n", env, key, value));

    /*
     * Search through the store to see if the key exists and update if the key
     * is found.
     */
    current = env;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            DBUG_PRINT(INFO, ( "update:: %s = %d\n"
                             , current->key
                             , current->value
                             ));
            DBUG_RETURN(env);
        }

        current = current->next;
    }

    /*
     * Key was not found prepend a new (key, value) object and set it's next
     * value to the previous environment.
     *
     * This will create the initial environment if the env parameter is NULL.
     */
    result = mkstore(key, value);
    result->next = env;

    DBUG_PRINT(INFO, ( "update::NEW %s = %d\n" , result->key , result->value));
    DBUG_RETURN(result);
}


/*
 * Remove a variable from the store, freeing its memory.
 */
store *remove_variable_from_store(store *env, char *key) 
{
    store *current, 
          *temp,
          *return_value;

    DBUG_ENTER("remove_variable_from_store");
    DBUG_PRINT("params", ("env: %p, key: %s\n", env, key));

    current = env;
    return_value = current;

    if (strcmp(current -> key, key) == 0) {
        return_value = current -> next;

        free(current);

        DBUG_RETURN(return_value);
    }

    while (current -> next != NULL) {
        if (strcmp(current -> next -> key, key) == 0) {
            temp = current -> next;
            current -> next = current -> next -> next;

            free(temp);

            /* Since this should be the only occurence of the variable in the
             * store we return directly.  
             */
            DBUG_RETURN(return_value);
        }

        current = current -> next;
    }
}


/*
 * Create a unique block name using the original name and a local static
 * counter.
 */
char *get_block_relocated_name(char *block_name)
{
    static int program_relocator = 0;
    char *relocated_name;
    
    DBUG_ENTER("get_block_relocated_name");
    DBUG_PRINT("params", ("block_name: %s\n", block_name));

    /* Given a maximum of 10000 labels (0 - 9999, i.e. 4 digits) and an
     * underscore we need 5 + length of the block_name characters for the new
     * name.
     */
    relocated_name = (char *) malloc((5 + strlen(block_name)) * sizeof(char)); 

    /* The format forces the maximum of 10000 unique labels, padding with 0's
     * to always fill up to 4 digits. 
     */
    sprintf(relocated_name, "%s_%04d", block_name, (++program_relocator));

    DBUG_RETURN(relocated_name);
}


/*
 * Specialize the assignments of a block.
 */
__assign *eval_assignments(__assign *assignments, store **env)
{
    __assign *previous_new,
             *current_new,
             *current_old,
             *result;

    expr *spec_expr;

    int first;

    DBUG_ENTER("eval_assignments");
    DBUG_PRINT("params", ("assignments: %p, env: %p", assignments, *env));

    previous_new = NULL;
    current_new = NULL;
    current_old = NULL;
    result = NULL;

    first = 1;
    current_old = assignments;
    while (current_old != NULL) {
        spec_expr = eval_expression(current_old -> expression, *env);

        switch (spec_expr -> type) {
            case INT:
                *env = update(*env, current_old -> variable, spec_expr -> intval);
            break;

            case OP:
            case VAR:
                /* This means the RHS of the assignment is residual. */
                if (exists_in_env(current_old -> variable, *env) != -1) {
                    *env = remove_variable_from_store( *env
                                                     , current_old -> variable);
                }

                current_new = mkassign(current_old -> variable, spec_expr);

                if (first) { 
                    first = 0;
                    result = current_new;
                } else {
                    previous_new -> next = current_new;
                }

                previous_new = current_new;

            break;
        }

        current_old = current_old -> next;
    }

    DBUG_RETURN(result);
}


/*
 * Evaluate a single basic block.
 *
 * First evaluate all assignments then evaluate the jump.
 */
basic_block *eval_block( basic_block *b
                       , program *p
                       , store *env
                       , char *relocated_block_name
                       )
{
    store *new_env,
          *copy_env;

    expr *conditional_expr;

    basic_block *new_block, 
                *next_block,
                *target;

    block_list *evaluated_before_target;

    char *left_block_name, 
         *right_block_name;

    jump *new_jump;
    __cnd *new_cnd;

    DBUG_ENTER("eval_block");

    DBUG_PRINT("params", ("b: %p, p: %p, env: %p, relocated_block_name: %s"
                         , p, b, env, relocated_block_name
                         ));

    DBUG_PRINT("block", ("Evaluating block <%s>.", b->label));

    copy_env = copy_store(env);
    
    
    prepend_list(&seen_list, b -> label, NULL, copy_env);

    /*
    prepend_seen_list(b -> label, NULL, copy_env);
     */

    evaluated_before_target = NULL;

    new_block = (basic_block *) malloc(sizeof(basic_block));

    if (strlen(relocated_block_name) > 0) {
        new_block -> label = (char *) malloc(strlen(relocated_block_name) *
                                             sizeof(char));

        strcpy(new_block -> label, relocated_block_name);

    } else {
        new_block -> label = (char *) malloc(strlen(b -> label) * 
                                             sizeof(char));

        strcpy(new_block -> label, b -> label);
    }

    /*************************************************************************/

    /*** ASSIGNMENTS ***/

    new_block -> next = NULL;
    new_block -> assignments = eval_assignments(b -> assignments, &env);


    /*************************************************************************/

    /*** JUMP ***/

    next_block = NULL;

    target = NULL;
    conditional_expr = NULL;

    left_block_name = NULL; 
    right_block_name = NULL;

    

    switch (b -> jump -> tag) {
        case GOTO:
            target = find_basic_block(JMP_LABL(b), p -> blocks);

            evaluated_before_target = evaluated_before( JMP_LABL(b)
                                                      , env
                                                      , target -> use_variables
                                                      );

            if(evaluated_before_target == NULL) {

                /* For an unconditional jump we can merge the two basic blocks,
                 * while also keeping the target block (as it may be a target of
                 * some other jump).
                 */

                if (target == NULL) {
                    fprintf(stderr, "Couldn't find target block, exiting.\n");
                    exit(1);
                }

                next_block = eval_block(target, p, env, "");
                append_assign_statement_migrate_jump(new_block, next_block);

            } else{
                if(evaluated_before_target -> block != NULL) {

                    append_assign_statement_migrate_jump(
                            new_block
                          , evaluated_before_target -> block
                          );

                } else {
                    new_block -> jump = __CMIX(residual) mkgoto(JMP_LABL(b));
                    append_list(&residual_jump_block_name_list,b-> label,b,env);
                }
            }

        break;

        case CND:

            conditional_expr = eval_expression(JMP_COND(b), env);

            if (conditional_expr -> type == INT) {

                DBUG_PRINT(INFO, ("Evaluating static conditional."));

                if (conditional_expr -> intval == 1) {
                    DBUG_PRINT(INFO, ("Static conditional evaluated to TRUE."));

                    target = find_basic_block(JMP_THEN(b), p -> blocks);

                    if (target == NULL) {
                        fprintf(stderr, "Couldn't find target block, exiting.\n");
                        exit(1);
                    }

                } else {
                    DBUG_PRINT(INFO, ("Static conditional evaluated to FALSE."));

                    target = find_basic_block(JMP_ELSE(b), p -> blocks);

                    if (target == NULL) {
                        fprintf(stderr, "Couldn't find target block, exiting.\n");
                        exit(1);
                    }
                }

                evaluated_before_target = evaluated_before( 
                      target -> label
                    , env
                    , target -> use_variables
                    );

                if (evaluated_before_target == NULL) {
                    next_block = eval_block(target, p, env, "");

                    append_assign_statement_migrate_jump(new_block, next_block);

                } else {
                    if(evaluated_before_target -> block != NULL) {
                        append_assign_statement_migrate_jump(
                              new_block
                            , evaluated_before_target -> block
                            );
                    } else {
                        new_block -> jump = __CMIX(residual) mkgoto(target -> label);
                        append_list(&residual_jump_block_name_list,target -> label,target,env);
                    }
                }
            } else {

                DBUG_PRINT(INFO, ("Evaluating dynamic conditional."));

                new_block -> jump = mkcnd(conditional_expr, "", "");

                target = find_basic_block(JMP_THEN(b), p -> blocks);
                
                if(evaluated_before( JMP_THEN(b)
                                   , env
                                   , target -> use_variables) == NULL) {
                    
                    left_block_name = get_block_relocated_name(JMP_THEN(b));

                    if (target == NULL) {
                        fprintf(stderr, "Couldn't find target block, exiting.\n");
                        exit(1);
                    }

                    append_list( &eval_list
                                 , left_block_name
                                 , target
                                 , copy_store(env)
                        );

                    JMP_THEN(new_block) = left_block_name;
                } else {
                    JMP_THEN(new_block) = (char *) malloc(strlen(JMP_THEN(b)) * 
                                                          sizeof(char));

                    strcpy(JMP_THEN(new_block), JMP_THEN(b));
                    append_list(&residual_jump_block_name_list,target -> label,target,env);
                }

                target = find_basic_block(JMP_ELSE(b), p -> blocks);

                if(evaluated_before( JMP_ELSE(b)
                                   , env
                                   , target -> use_variables) == NULL) {

                    right_block_name = get_block_relocated_name(JMP_ELSE(b));

                    if (target == NULL) {
                        fprintf(stderr, "Couldn't find target block, exiting.\n");
                        exit(1);
                    }

                    append_list( &eval_list
                                 , right_block_name
                                 , target
                                 , copy_store(env)
                        );
                    
                    JMP_ELSE(new_block) = right_block_name;

                } else {
                    JMP_ELSE(new_block) = (char *) malloc(strlen(JMP_ELSE(b)) * 
                                                          sizeof(char));

                    strcpy(JMP_ELSE(new_block), JMP_ELSE(b));
                    append_list(&residual_jump_block_name_list,target -> label,target, env);
                }
            }
        break;

        case RETURN:
            new_block -> jump = mkreturn(eval_expression(JMP_RETN(b), env));

        break;
    }

    update_seen_list(b -> label, new_block, copy_env, b -> use_variables);

    DBUG_RETURN(new_block);
}

/*
 * Evaluate a single expression.
 */
expr *eval_expression(expr *e, store *env) 
{
    expr *newexpr;

    DBUG_ENTER("eval_expression");
    DBUG_PRINT("params", ("e: %p, env: %p\n", e, env));

    newexpr = (expr *) malloc(sizeof(expr));

    DBUG_EXECUTE(INFO, debug_print = 1; )
    DBUG_PRINT(INFO, ("Evaluating expression."));
    DBUG_EXECUTE(INFO, printf("----------\n");)
    DBUG_EXECUTE(INFO, print_expression(e, 1);)
    DBUG_EXECUTE(INFO, printf("----------\n");)

    switch (e->type) {
        case INT:
            newexpr -> type = INT;
            newexpr -> intval = e -> intval;
            newexpr -> next = NULL;
            newexpr -> op = NULL;

        break;

        case VAR:
            if (exists_in_env(e->id, env) != -1) {
                newexpr -> type = INT;
                newexpr -> intval = lookup(e->id, env);
                newexpr -> next = NULL;

            } else {
                newexpr -> type = VAR;
                newexpr -> id = (char *) malloc(strlen(e -> id) * sizeof(char));
                strcpy(newexpr -> id, e -> id);
                newexpr -> next = NULL;
                newexpr -> op = NULL;

            }
        break;

        case OP:
            DBUG_EXECUTE(INFO, debug_print = 0; )
            switch (e->op->type) {
                case PLUS:
                case MIN:
                case MUL:
                case DIV:
                    DBUG_RETURN(eval_n_ary_op(e->op->type, e->op->operands, env));
                break;

                case EQ:
                case LT:
                case LTE:
                case GT:
                case MOD:
                case GTE:
                    DBUG_RETURN(eval_binary_op(e->op->type, e->op->operands, env));
                break;
            }
        break;
    }

    DBUG_PRINT(INFO, ("Evaluated expression to."));
    DBUG_EXECUTE(INFO, printf("----------\n");)
    DBUG_EXECUTE(INFO, print_expression(newexpr, 1); )
    DBUG_EXECUTE(INFO, printf("----------\n");)

    DBUG_EXECUTE(INFO, debug_print = 0; )
    DBUG_RETURN(newexpr);
}


/*
 * Evaluate a n-ary operator.
 */
expr *eval_n_ary_op(otype type, expr *operands, store *env) 
{
    expr *current, 
         *new_expr_prev, 
         *temp_expr,
         *start_expr,
         *new_expr, 
         *spec_expr,
        *new_current;

    int accumulator, 
        temp, 
        first, 
        accumulator_executed, 
        first_operand_in_accumulator,
        variable_found,
        invert_accumulator;
    
    __op *new_op;

    DBUG_ENTER("eval_n_ary_op");
    DBUG_PRINT("params", ("type: %d, operands: %p, env: %p\n"
                         , type, operands, env
                         ));

    new_expr = NULL;
    start_expr = NULL;

    first = 1, accumulator_executed = 0, variable_found = 0;

    /*    if (operands == NULL || operands->next == NULL) {
        fprintf(stderr, "Not enough operands for operation!");
        exit(1);
    }*/

    temp = 0;
    current = operands;
    variable_found = 0;

    /*
     * Setup the accumulator to be the correct identity value for the operator.
     */

    accumulator = (type == MIN || type == PLUS) ? 0 : 1;
    
    if(current -> type == OP) {
        new_current = eval_expression(current,env);
    } else {
        new_current = current;
    }
    
    invert_accumulator = 0;
    first_operand_in_accumulator = 1;
    if (new_current -> type == VAR && exists_in_env(new_current -> id, env) == 0) {
        accumulator = lookup(new_current -> id, env);
        accumulator_executed = 1;
        /* The following current is not a bug, we want to skip if the first
         * expression can be evaluated to a static value and is stored in the
         * accumulator.
         */
        current = current -> next;
    } else if (new_current -> type == INT) {
        accumulator = new_current -> intval;
        accumulator_executed = 1;
        current = current -> next;
    } else {
        first_operand_in_accumulator = 0;
        if(type == MIN || type == DIV) {
            invert_accumulator = 1;
        }
    }

    /*
     * Iterate over the expressions operands.
     */

    while (current != NULL) {
        if(current -> type == OP) {
            spec_expr = eval_expression(current, env);
            DBUG_PRINT(INFO,("Nested op evaluates to : "));
            DBUG_EXECUTE(INFO, print_expression(spec_expr, 1););
        } else {
            spec_expr = current;
        }
        /*
         * For variables that are *not* in the environment we create a new
         * operand expression.
         */
        if (spec_expr -> type == OP 
            || (spec_expr -> type == VAR 
                && exists_in_env(spec_expr -> id, env) == -1)) {

            variable_found = 1;
            new_expr = copy_expression(spec_expr);
            new_expr -> next = NULL;
            
            if (first) {
                first = 0;
                start_expr = new_expr;
            } else {
                new_expr_prev -> next = new_expr;
            }
            
            new_expr_prev = new_expr;
            
        } else {
            
            /*
             * For variables or constants we set the temporary value to the
             * constant value or lookup the variables value in the store.
             */
            if (spec_expr -> type == INT) {
                temp = spec_expr -> intval;
            } else if (spec_expr -> type == VAR 
                       && exists_in_env(spec_expr->id, env) == 0) {
                temp = lookup(spec_expr->id, env);
            }

            accumulator_executed = 1;

            switch (type) {
                case PLUS:
                    accumulator += temp;
                break;

                case MIN:
                    if(invert_accumulator) {
                        accumulator += temp;
                    } else {
                        accumulator -= temp;
                    }
                break;

                case MUL:
                    accumulator *= temp;
                break;

                case DIV:
                    if(invert_accumulator) {
                        accumulator *= temp;
                    } else {
                        if (temp == 0) {
                            fprintf(stderr, "Division by zero!\n");
                            exit(1);
                        }
                    accumulator /= temp;
                    }
                break;
            }
        }

        current = current->next;
    }

    /*
     * We have reduced the expression and we create a result 'constant'
     * expression.
     */
    if (accumulator_executed) {
        
        new_expr = mkint(accumulator);
        new_expr -> next = NULL;

        if (start_expr == NULL) {
            start_expr = new_expr;
        } else {
            /* The first operand was added to the accumulator */
            if(first_operand_in_accumulator) {
                new_expr -> next = start_expr;
                start_expr = new_expr;
            } else {
                new_expr_prev -> next = new_expr;
            }
        }
    }

    /*
     * We were not able to reduce the expression, so, we create a new resulting
     * expression with the same operation and a new list of operands (parts of
     * which may have been reduced).
     */
    if (variable_found) {
        new_op = (__op *) malloc(sizeof(__op ));

        new_op -> type = type;
        new_op -> operands = start_expr;

        new_expr = (expr *) malloc(sizeof(expr));

        new_expr -> op = new_op;
        new_expr -> type = OP;
        new_expr -> next = NULL;

        DBUG_RETURN(new_expr);
    }

    DBUG_RETURN(start_expr);
}


/*
 * Evaluate a binary operator, throwing an error of there are not enough
 * operands.
 */
expr *eval_binary_op(otype type, expr *operands, store *env) 
{
    int a, b, result;

    expr *newexpr,
         *left_of_operator,
         *right_of_operator;

    __op *newop;

    DBUG_ENTER("eval_binary_op");
    DBUG_PRINT("params", ("type: %d, operands: %p, env: %p\n"
                         , type, operands, env
                         ));

    newexpr = (expr *) malloc(sizeof(expr));
    a = b = result = 0;

    if (operands == NULL || operands->next == NULL) {
        fprintf(stderr, "Not enough operands!\n");
        exit(1);
    }

    right_of_operator = eval_expression(operands -> next, env);
    left_of_operator = eval_expression(operands, env);

    if (left_of_operator -> type == INT && right_of_operator -> type == INT) {

        a = left_of_operator -> intval;
        b = right_of_operator -> intval;

        DBUG_PRINT(INFO, ("a: %d, b: %d\n", a, b));

        switch (type) {
            case EQ:
                result = (a == b) ? 1 : 0;
            break;

            case LT:
                result = (a < b) ? 1 : 0;
            break;

            case LTE:
                result = (a <= b) ? 1 : 0;
            break;

            case GT:
                result = (a > b) ? 1 : 0;
            break;

            case GTE:
                result = (a >= b) ? 1 : 0;
            break;

            case MOD:
                if(b == 0) {
                    fprintf(stderr, "Division by zero!\n");
                    exit(1);
                }
                result = a % b;
            break;

        }
        
        newexpr -> type = INT;
        newexpr -> intval = result;
        newexpr -> next = NULL;

    } else {
        newexpr -> type = OP;

        newop = (__op *) malloc(sizeof(__op ));

        newop -> type = type;
        newop -> operands = left_of_operator;
        newop -> operands -> next = right_of_operator;

        newexpr -> op = newop;
        newexpr -> next = NULL;
    }

    DBUG_RETURN(newexpr);
}


/*
 * Look for a basic block with a specific label.
 */
basic_block *find_basic_block(char *label, basic_block *blocks) 
{
    basic_block *current;

    DBUG_ENTER("find_basic_block");
    DBUG_PRINT("params", ("label: %s, blocks: %p\n", label, blocks));

    current = blocks;
    while (current != NULL) {
        if (strcmp(label, current -> label) == 0) {
            DBUG_RETURN(current);
        }

        current = current -> next;
    }

    DBUG_RETURN(NULL);
}

varlist *append_block_use_variables( varlist *src_varlist
                                   , varlist *to_be_appended_list
                                   )
{
    varlist *src_list_iterator,
            *new_list_iterator;
    
    int exists;

    DBUG_ENTER("append_block_use_variables");
    DBUG_PRINT("params", ("src_varlist: %p, to_be_appended_list: %p\n"
                         , src_varlist, to_be_appended_list
                         ));

    if (src_varlist == NULL) {
        DBUG_RETURN(to_be_appended_list);
    } else if (to_be_appended_list == NULL) {
        DBUG_RETURN(src_varlist);
    }
    
    /* For each variable in the list ... */
    new_list_iterator = to_be_appended_list;
    while (new_list_iterator != NULL) {
        exists = 0;

        /* ... check if it can be found in the destination list. */
        src_list_iterator = src_varlist;
        while (src_list_iterator -> next != NULL) {
            if (strcmp( new_list_iterator -> variable
                      , src_list_iterator -> variable) == 0) {
                exists = 1;
            }

            src_list_iterator = src_list_iterator -> next;
        }
        
        /* Check the last value. */
        if (strcmp( new_list_iterator -> variable
                  , src_list_iterator -> variable) != 0 && !exists) {
            /* Append the new variable to the destination list. */
            src_list_iterator -> next = mkreadvar(new_list_iterator -> variable);
        }

        new_list_iterator = new_list_iterator -> next;
    }

    DBUG_RETURN(src_varlist); 

}

varlist *get_use_variables_from_expression(expr *expression) 
{
    varlist *new_varlist,
            *new_entry;

    expr *operands;

    DBUG_ENTER("get_use_variables_from_expression");
    DBUG_PRINT("params", ("expression: %p\n", expression));

    new_varlist = NULL;
    new_entry = NULL;

    if(expression != NULL) {
        switch (expression -> type) {
            case VAR:
                new_entry = (varlist *)malloc(sizeof(varlist));

                /* We are annotating the src program so no need 
                 * to duplicate structures 
                 */
                new_entry -> variable = expression -> id;
                new_entry -> next = new_varlist;
                new_varlist = new_entry;
            break;

            case OP:
                operands = expression -> op -> operands;
                while(operands != NULL) {
                    new_entry = get_use_variables_from_expression(operands);
                    new_varlist =
                        append_block_use_variables(new_varlist,new_entry);
                    operands = operands -> next;
                }
            break;
        }
    }

    DBUG_RETURN(new_varlist);
}

program *analyse(program *program)
{
    basic_block *blocks; 

    __assign *assignments;

    varlist *use_variables;

    DBUG_ENTER("analyse");
    DBUG_PRINT("params", ("program: %p\n", program));

    blocks = program -> blocks;
    
    global_use_variables = NULL;

    while(blocks != NULL) {
        assignments = blocks -> assignments;
        blocks -> use_variables = NULL;

        while(assignments != NULL) {
            use_variables = 
                get_use_variables_from_expression(assignments -> expression);

            blocks -> use_variables = 
                append_block_use_variables( blocks -> use_variables
                                          , use_variables
                                          );
            if(GLOBAL_USE_VARIABLES) {
                global_use_variables = append_block_use_variables(global_use_variables,use_variables);
            }
            assignments = assignments -> next;
        }

        if(blocks -> jump -> tag == RETURN) {
            use_variables = get_use_variables_from_expression(blocks -> jump -> return_stmt);
            if(GLOBAL_USE_VARIABLES) {
                global_use_variables = append_block_use_variables(global_use_variables,use_variables);
            }
            blocks -> use_variables = append_block_use_variables(blocks -> use_variables, use_variables);                    
        } else if(blocks -> jump -> tag == CND) {
            use_variables = get_use_variables_from_expression(blocks -> jump -> conditional -> condition);
            blocks -> use_variables = append_block_use_variables(blocks -> use_variables, use_variables);                    
            if(GLOBAL_USE_VARIABLES) {
                global_use_variables = append_block_use_variables(global_use_variables,use_variables);
            }
        }

        blocks = blocks -> next;
    }
    
    
    DBUG_RETURN(program);
}

int exists_in_block_name_list(char *block_name, block_list *search_list) 
{
    DBUG_ENTER("exists_in_block_name_list");
    block_list *current;
    current = search_list;
    while(current != NULL) {
        if(strcmp(block_name,current -> block_name) == 0) {
            DBUG_RETURN(1);
        }
        current = current -> next;
    }
    DBUG_RETURN(0);
}

#pragma cmix spectime: parse_program()
#pragma cmix goal: specialize($1, ?) producing("main")

/*
 * specialize a program.
 */
program *specialize(char *filename, store *env) 
{
    program *pgm;

    basic_block *entry, 
                *new_residual_block;

    varlist *argname, 
            *newarg;

    program *new_program;
    block_list *residual_jump_printed_block_name_list,*current;

    DBUG_ENTER("specialize");
    DBUG_PRINT("params", ("filename: %s, env: %p\n", filename, env));

    parse_program(&pgm, filename);
    pgm = analyse(pgm);

    new_program = (program *) malloc(sizeof(program));

    new_program -> variables = NULL;
    new_program -> entry_label = pgm -> entry_label;
    new_program -> blocks = NULL;

    /* Create the input variables for the new program, using only those that
     * have not been specified for specialization.
     */

    argname = pgm -> variables;
    while (argname != NULL) {
        if (exists_in_env(argname -> variable, env) == -1) {
            newarg = (varlist *) malloc(sizeof(varlist));

            newarg -> variable = argname -> variable;
            newarg -> next = new_program -> variables;

            new_program -> variables = newarg;
        }

        argname = argname -> next;
    }

    entry = find_basic_block(pgm->entry_label, pgm->blocks);

    eval_list = mkblock_list(pgm -> entry_label, entry, env);

    eval_list -> next = NULL;

    if (entry == NULL) {
        fprintf(stderr, "Couldn't find entry label, exiting.");
        exit(1);
    }

    current = eval_list;
    while (current != NULL && current -> block != NULL) {

        new_residual_block = eval_block( current -> block
                                       , pgm
                                       , current -> env
                                       , current -> block_name
                                       );

        new_residual_block -> next = new_program -> blocks;
        append_list(&residual_jump_printed_block_name_list, new_residual_block -> label, new_residual_block,NULL);
        new_program -> blocks = new_residual_block;

        current = current -> next;
    }

    current = residual_jump_block_name_list;
    while(current != NULL) {
        if(!exists_in_block_name_list(current -> block_name, residual_jump_printed_block_name_list)) {
            new_residual_block = copy_block(current -> block);
            new_residual_block -> next = new_program -> blocks;
            new_program -> blocks = new_residual_block;
            append_list(&residual_jump_printed_block_name_list,new_residual_block -> label,new_residual_block, NULL);
        }
        current = current -> next;
    }

    DBUG_RETURN(new_program);
}

void usage (char *pgm)
{
    printf("Usage: %s <options>\n\n", pgm);

    printf("\t-a var=val,..\tSet variables to their specialization values.\n");
    printf("\t-v           \tEnable verbose output.\n");
    printf("\t-o <filename>\tSet the output filename.\n");
    printf("\t-i <filename>\tSet the input filename.\n");
    printf("\t-d <DBUG_FMT>\tEnable debugging using the DBUG format string.\n");
    printf("\t             \tRead the manual for a specification of the"
           " options.\n");
    printf("\t-g           \tEnable global use variables for enviroment equality\n");
    printf("\n");

    exit(0);
}

#ifdef SPEC_MAIN
int main(int argc, char **argv) {
    program *prog, 
            *res_prog;

    store *start_env;

    char *filename,
         *var,
         *cp,
         *delimiters;

    int option_index, 
        val, 
        c,
        index;

    DBUG_PROCESS("FCL_SPEC");

    DBUG_ENTER("main");
    DBUG_PRINT("params", ("argc: %d, argv: %p\n", argc, argv));

    /* Defaults */

    outfile = stdout;
    option_index = 0;
    index = 0;
    filename = NULL;
    delimiters = ",=";
    start_env = NULL;
    
    if (argc == 1) {
        usage(argv[0]);
    }

    /* Parse the commandline using getopt. */
    while ((c = getopt(argc, argv, "a:vo:i:d:g")) != -1) {
        switch (c) {
            case 'a':
                /* We `asume' the format <var> = <value>, ... */

                var = strtok(optarg, delimiters);
                val = atoi(strtok(NULL, delimiters));

                start_env = update(start_env, var, val);

                while ((var = strtok(NULL, delimiters)) != NULL) {
                    val = atoi(strtok(NULL, delimiters));

                    start_env = update(start_env, var, val);
                }
            break;

            case 'v':
                DBUG_PUSH("d,info");
            break;

            case 'o':
                outfile = fopen(optarg, "w");
            break;

            case 'i':
                filename = (char *) malloc(strlen(optarg) * sizeof(char));
                strcpy(filename, optarg);
            break;

            case 'd':
                DBUG_PUSH(optarg);
            break;
            
            case 'g':
                GLOBAL_USE_VARIABLES = 1;
                break;
            default:
                abort();
        }
    }

    for (index = optind; index < argc; index++) {
        filename = argv[index];
    }

    res_prog = specialize(filename, start_env);
    print_program(res_prog);

    fclose(outfile);

    DBUG_PRINT(INFO, ("Exit specializer."));

    DBUG_RETURN(0);
}
#endif
