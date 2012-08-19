/*
    main.c is part of MASI
    main.c - MASI interpreter main program
    
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

    Written by Vivek Shah <bonii@kompiler.org>, Tim van Deurzen <tim(at)kompiler.org>

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "ast.h"
#include "masi.h"
#include "pretty_print.h"

extern void interpret (char *, value []);

#ifdef __MIX
extern void mix (int *);
#endif


/*
 * Display a simple usage message.
 */
void usage (char *pgm)
{
    printf("Usage: %s <options>\n\n", pgm);

#ifdef __COGEN
    printf("\t-a reg=[0,1],..\tTell the compiler generator which registers will"
           "be static (1) or dynamic (0).\n");
#else
    printf("\t-a var=val,..\tSet registers to their specialization values.\n");
#endif
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


int main(int argc, char **argv) {
    block_list *blocks, 
               *res_blocks;

    char *filename,
         *cp,
         *var,
         *delimiters;

    int option_index, 
        c,
        index, 
        i,
        val;


    value registers[NUM_REGISTERS];

    option_index = 0;
    index = 0;
    filename = NULL;
    delimiters = ",=";
    outfile = stdout;

    if (argc == 1) {
        usage(argv[0]);
    }

    i = 0;

    for (; i < NUM_REGISTERS; i++) {
        registers[i] = -1;
    }

    /* Parse the commandline using getopt. */
    while ((c = getopt(argc, argv, "a:o:i:d:")) != -1) {
        switch (c) {
            case 'a':
                /* We `asume' the format <var> = <value>, ... */
                var = strtok(optarg, delimiters);
                val = atoi(strtok(NULL, delimiters));

                if (sscanf(var, "r%d", &i) != 1) {
                    sscanf(var, "R%d", &i);
                }

                registers[i] = val;

                while ((var = strtok(NULL, delimiters)) != NULL) {
                    val = atoi(strtok(NULL, delimiters));

                    if (sscanf(var, "r%d", &i) != 1) {
                        sscanf(var, "R%d", &i);
                    }

                    registers[i] = val;
                }
            break;

            case 'o':
                outfile = fopen(optarg, "w");
            break;

            case 'i':
                filename = (char *) malloc(strlen(optarg) * sizeof(char));
                strcpy(filename, optarg);
            break;

            case 'd':
                /*DBUG_PUSH(optarg);*/
            break;
            
            default:
                abort();
        }
    }

    for (index = optind; index < argc; index++) {
        filename = argv[index];
    }


#ifdef __COGEN
    cmixGenInit();
    cogen(filename);
    cmixGenExit(outfile);
#else
    #ifdef __MIX
        mix(registers);
    #else
        interpret(filename, registers);
    #endif
#endif

    return 0;
}
