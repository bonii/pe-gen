#    run_tests.sh is part of FCL_SPEC
#    run_tests.sh - Shell script to run test programs and compare with standard results
#    
#    Copyright(C) 2012 Vivek Shah <bonii(at)kompiler.org>, Tim van Deurzen <tim(at)kompiler.org>
#
#    MASI is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    MASI is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Mem-Store.  If not, see <http://www.gnu.org/licenses/>.
#
#    Written by Vivek Shah <bonii@kompiler.org>, Tim van Deurzen <tim(at)kompiler.org>
#
#
#!/bin/bash

### Test harness configuration.

TEST_CASES="forty_two
            fibonacci
            power
            perfect_numbers
            nesting_and_looping
            euclid
            euclid_mod
           "

forty_two_ARGS="x=1,y=2,z=3"
fibonacci_ARGS="n=5"
power_ARGS="e=5"
perfect_numbers_ARGS="num=1 -g"
nesting_and_looping_ARGS="x=10"
euclid_ARGS="a=5,b=2"
euclid_mod_ARGS="a=16,b=12"

TEST="./specialized/manual"
OUT="./specialized/automatic"

FCLSPEC="./bin/fcl_spec"

### Individual testing function.

function specialize {
    if [ -z $1 ] || [ -z $2 ]
    then
        echo -e "[$FUNCNAME] Not enough arguments!"
    else
        eval var=\$${2}
        echo -e "ARGS [${var}]\n"
        `./bin/fcl_spec -i test/$1.fcl -o ${OUT}/$1.fcl -a $var`
    fi
}


### Running the tests and checking their results.

if [ -e ./bin/fcl_spec ]
then
    SUCCEEDED=0
    FAILED=0
    
    if [ ! -d ${OUT} ]
    then
        mkdir ${OUT}
    fi

    echo -e "=================================================="
    echo; echo

    for x in ${TEST_CASES}
    do
        echo -e "SPECIALIZING [${x}]\n"

        specialize ${x} ${x}_ARGS

        echo -e "CHECKING [${x}]\n"

        diff -wBq ${TEST}/${x}.fcl ${OUT}/${x}.fcl > /dev/null

        RET=$?
        if [ $RET -eq 0 ]
        then
            echo -e "\e[1;32mFiles are equivalent (ignoring whitespace).\e[0m"
            SUCCEEDED=$(( $SUCCEEDED + 1))
        else
            echo -e "\e[1;31mFiles are NOT equivalent (ignoring whitespace).\e[0m"
            FAILED=$(( $FAILED + 1))
        fi

        echo; echo
        echo -e "=================================================="
        echo; echo
    done

    echo -e "\e[1;32mSUCCESSFUL:\e[0m ${SUCCEEDED}, \e[1;31mFAILED:\e[0m ${FAILED}\n"
else
    echo -e "No specializer, build specializer first!"
    exit 1
fi
