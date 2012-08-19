#
# Makefile for the efficient C specializer.
#
# author: Tim van Deurzen.
#

### Program configurations.

CC = gcc
YACC = bison
LEX = flex
CMIX = cmix


### Flags

CFLAGS = -ansi -ggdb # -Wall -O2

CPPFLAGS = -I include -I. -I build

MIXFLAGS = -ddump-bta -S

# LDFLAGS = 

MIXLDFLAGS = -lcmix

### Macros

OUTPUT = -o $@

COMPILE = $(CC) $(CPPFLAGS) $(CFLAGS) -c 

LINK = $(CC) $(CPPFLAGS) $(CFLAGS) $(OUTPUT) $^ $(LDFLAGS)

MIX = $(CMIX) $(CPPFLAGS) $(MIXFLAGS)


### Path setup for separate folders for .h, .c and .o files.

# All generated files and object files will be put here.
BUILD_DIR = build

# Folder for executables.
LOCAL_BINDIR = bin


VPATH = src include $(BUILD_DIR)


### Main recipes and variables.

# The binary representing our program
PROG = $(LOCAL_BINDIR)/fcl_int

# The object files needed that define our program.
OBJ = $(addprefix $(BUILD_DIR)/, y.tab.o lex.yy.o main.o ast.o util.o)

.PHONY: all test mix clean


# Main build. 

all: $(PROG)


$(PROG): $(OBJ) | $(LOCAL_BINDIR)
	@echo "    [LD]   $@"
	@$(LINK)

$(BUILD_DIR)/y.tab.c: fcl.y | $(BUILD_DIR)
	@echo "    [YACC] $<"
	@$(YACC) -y --defines=$(BUILD_DIR)/y.tab.h -o $(BUILD_DIR)/y.tab.c $<

$(BUILD_DIR)/lex.yy.c: fcl.l | $(BUILD_DIR)
	@echo "    [LEX]  $<"
	@$(LEX) -o $(BUILD_DIR)/lex.yy.c $<

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "    [CC]   $<"
	@$(COMPILE) -DFCL_MAIN $< $(OUTPUT)

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
MIX_FILES = main.c

# The external functionality resides in these object files.
MIX_OBJ = $(addprefix $(BUILD_DIR)/, y.tab.o lex.yy.o ast.o util.o)

# Target for compiling the CMix generating extension.
FCL2C = $(LOCAL_BINDIR)/fcl2c

mix: $(FCL2C)

# Call CMix.
$(MIX_SOURCE): $(MIX_FILES) | $(BUILD_DIR)
	@echo "    [MIX]  $<"
	@$(MIX) $< $(OUTPUT)

$(FCL2C): $(MIX_SOURCE) $(MIX_OBJ) | $(LOCAL_BINDIR)
	@echo "    [LD]   $@"
	@$(LINK) $(MIXLDFLAGS)


### Clean up temporary and generated files.

clean:
	@echo "Cleaning temporary and generated files ..."
	@rm -rf build bin
