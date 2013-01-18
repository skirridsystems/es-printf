#*************************************************************************
# es-printf  -  configurable printf for embedded systems
#
# Makefile for testing on a PC and AVR
#
#*************************************************************************
# Copyright (c) 2006 - 2013 Skirrid Systems
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#*****************************************************************************/

# Define the main target to build
MCU = atmega8
TRG = Test
SRC = test.c
INC = printf.c printf.h printf_cfg.h

# Tool chain used for this project.
AVRGCC = /winavr-20100110

# Tool chain used for the PC test version
PC_GCC = gcc

#**************************  Boilerplate *********************************

# Tools and directories
OUT     = AVR_out
CC      = $(AVRGCC)/bin/avr-gcc
RM      = $(AVRGCC)/utils/bin/rm -rf
HEX     = $(AVRGCC)/bin/avr-objcopy
LIBDIR  = $(AVRGCC)/avr/lib
AVRSIZE = $(AVRGCC)/bin/avr-size

# Libraries
LIB = $(AVRGCC)/avr/lib/libc.a

# Compiler flags
CPMISC  = -g -Wa,-adhlms=$(@:.o=.lst) -mmcu=$(MCU)
CPCODE  = -Os -ffreestanding -fshort-enums -fpack-struct -fno-jump-tables -ffunction-sections -fdata-sections
CPWARN  = -Wall -Wstrict-prototypes -Wundef -Wfatal-errors -Werror
CPFLAGS = $(CPMISC) $(CPCODE) $(CPWARN) -DTEST_AVR

# Linker flags
LDFLAGS = -mmcu=$(MCU) -Wl,-Map=$(basename $@).map,--cref -Wl,--relax -Wl,--gc-sections

# Derive all project specific object files
OBJ = $(addprefix $(OUT)/,$(SRC:.c=.o))

#-----------------------------------------------------------------

# Implicit rules for creating object from source.
$(OUT)/%.o : %.c makefile $(INC)
	$(CC) -c $(CPFLAGS) -I. $*.c -o $@

#-----------------------------------------------------------------
# This section defines the aims of the make process

all: $(TRG).hex $(TRG).exe

# Make sure the output directory exists
$(OBJ): | $(OUT)

$(OUT):
	mkdir -p $(OUT)

$(TRG).hex: $(OUT)/$(TRG).elf
	$(HEX) -O ihex $^ $@

# Link main app object files into an ELF output file.
$(OUT)/$(TRG).elf: $(OBJ)
	$(CC) $(OBJ) $(LIB) $(LDFLAGS) -o $@
	$(AVRSIZE) $@

# Generate PC executable
$(TRG).exe: $(SRC) $(INC) makefile
	$(PC_GCC) -o $@ $(SRC)

#---------------------- MAKE CLEAN -------------------------------
#make instruction to delete created files
clean:
	$(RM) \*.hex \*.exe $(OUT)
