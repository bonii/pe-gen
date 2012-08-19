#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct env {
    char *name;
    int value;
} env;


typedef struct arg {
    int val;

    struct arg *next;
} arg;

typedef struct name {
    char *name;

    struct name *next;
} name;

typedef struct Func {
    char *name;
    name  *params;

    struct Func *next;
} Func;

typedef struct Prog {
    Func *funcs;
} Prog;

#pragma cmix spectime: readProg()
extern void readProg(char *, Prog **p);
static int dyncond;

name *global;
arg *_global;

int test3() 
{
    name *currentg,
         *newg;
    arg *currentv,
        *newv;


    currentg = global;
    currentv = _global;

    while(currentg != NULL) {
        __CMIX(pure) printf("%s = ", currentg->name);
        __CMIX(pure) printf("%d\n", currentv->val);

        currentv->val += 2;

        currentg = currentg->next;
        currentv = currentv->next;
    }

    if (dyncond) {
        newg = (name *) malloc(sizeof(name));
        newg->name = (char *) malloc(2 * sizeof(char));
        strcpy(newg->name, "a\0");
        
        newv = (arg *) malloc(sizeof(arg));
        newv->val = _global->val + 20;

        newg->next = global;
        newv->next = _global;

        global = newg;
        _global = newv;
    }

    return 2 + _global->val;
}

int test2(Func *f, arg *argv, Prog *p)
{
    name *param;
    name *currentg;
    arg *currentv;
    
    currentg = global;
    currentv = _global;

    param = f->params;
    while (param) {
        currentg->name = param->name;
        currentg->next = (name *) malloc(sizeof(name));
        currentg = currentg->next;

        currentv->val = argv->val;
        currentv->next = (arg *) malloc(sizeof(arg));
        currentv = currentv->next;

        param = param->next;
    }

    return test3();
}

#pragma cmix goal: test($1, ?)
int test(char *file, arg *argv)
{
    Prog *p;
    Func *f;

    readProg(file, &p);

    f = p->funcs;

    while (f != NULL && strcmp(f->name, "main") != 0) {
        f = f->next;
    }

    if (strcmp(f->name, "main") == 0) {
        return test2(f, argv, p);
    }

    return -1;
}
