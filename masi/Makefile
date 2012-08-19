#
#  Makefile is part of MASI
#  Makefile - Recipes for building MASI.
#  
#  Copyright(C) 2012 Vivek Shah <bonii(at)kompiler.org>, Tim van Deurzen <tim(at)kompiler.org>
#
#  MASI is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MASI is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MASI.  If not, see <http://www.gnu.org/licenses/>.
#
#  Written by Tim van Deurzen <tim(at)kompiler.org>
#

### Program configurations.

CC = gcc
YACC = bison
LEX = flex
CMIX = cmix


### Flags

CFLAGS = -ansi -ggdb # -Wall -O2

CPPFLAGS = -I include -I. -I build -D__DEBUG_ON 

MIXCPPFLAGS = -I include -I. -I build -D__COGEN

MIXFLAGS = -ddump-bta -S

# LDFLAGS = 

MIXLDFLAGS = -lcmix


### Macros

OUTPUT = -o $@

COMPILE = $(CC) $(CPPFLAGS) $(CFLAGS) -c 

LINK = $(CC) $(CPPFLAGS) $(CFLAGS) $(OUTPUT) $^ $(LDFLAGS)

MIX = $(CMIX) $(MIXCPPFLAGS) $(MIXFLAGS)


### Path setup for separate folders for .h, .c and .o files.

# All generated files and object files will be put here.
BUILD_DIR = build

# Folder for executables.
LOCAL_BINDIR = bin


VPATH = src include $(BUILD_DIR)


### Main recipes and variables.

# The binary representing our program
PROG = $(LOCAL_BINDIR)/masi

# The object files needed that define our program.
OBJ = $(addprefix $(BUILD_DIR)/, y.tab.o lex.yy.o main_spec.o spec.o ast.o pretty_print.o)


# Main build. 

all: $(PROG)


$(BUILD_DIR)/main_spec.o: main.c | $(BUILD_DIR)
	@echo "    [CC]   main.c"
	@$(COMPILE) $< $(OUTPUT)

$(PROG): $(OBJ) | $(LOCAL_BINDIR)
	@echo "    [LD]   $@"
	@$(LINK)

$(BUILD_DIR)/y.tab.c: masi.y | $(BUILD_DIR)
	@echo "    [YACC] $<"
	@$(YACC) -y --defines=$(BUILD_DIR)/y.tab.h -o $(BUILD_DIR)/y.tab.c $<

$(BUILD_DIR)/lex.yy.c: masi.l | $(BUILD_DIR)
	@echo "    [LEX]  $<"
	@$(LEX) -o $(BUILD_DIR)/lex.yy.c $<

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "    [CC]   $<"
	@$(COMPILE) $< $(OUTPUT)

$(BUILD_DIR): 
	@echo "    [MKDIR] $(BUILD_DIR)"
	@mkdir $(BUILD_DIR)

$(LOCAL_BINDIR): 
	@echo "    [MKDIR] $(LOCAL_BINDIR)"
	@mkdir $(LOCAL_BINDIR)


### MIX recipes and variables.

# Target of CMix application.
MIX_SOURCE = $(addprefix $(BUILD_DIR)/, main-gen.c)

# Source for CMix.
MIX_FILES = spec.c

# The external functionality resides in these object files.
MIX_OBJ = $(addprefix $(BUILD_DIR)/, y.tab.o lex.yy.o ast.o pretty_print.o main_cogen.o)

# Target for compiling the CMix generating extension.
MAS_COGEN = $(LOCAL_BINDIR)/mas_comp

mix: $(MAS_COGEN)

# Call CMix.
$(MIX_SOURCE): $(MIX_FILES) | $(BUILD_DIR)
	@echo "    [MIX]  $<"
	@$(MIX) -ddump-bta $< $(OUTPUT)

$(BUILD_DIR)/main_cogen.o: main.c | $(BUILD_DIR)
	@echo "    [CC]   main.c"
	@$(COMPILE) -D__COGEN $< $(OUTPUT)

$(MAS_COGEN): $(MIX_SOURCE) $(MIX_OBJ) | $(LOCAL_BINDIR)
	@echo "    [LD]   $@"
	@$(LINK) $(MIXLDFLAGS)



### Recipe for creating a main for residual program.


RES = $(BUILD_DIR)/main_res.o

main_res: $(RES)

$(BUILD_DIR)/main_res.o: main.c | $(BUILD_DIR)
	@echo "    [CC]   main.c"
	@$(COMPILE) -D__MIX $< $(OUTPUT)



### Clean up temporary and generated files.

clean:
	@echo "Cleaning temporary and generated files ..."
	@rm -rf build bin


.PHONY: all test mix clean res_main
