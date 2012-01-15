#*************************************************************************
#
#     Copyright (c) Skirrid Systems 2012
#
#       Project : printf
#    Module Name: makefile
#   Description : For testing printf on an AVR and on the PC
#
#*************************************************************************

# Define the main target to build
MCU = atmega8
TRG = Test
SRC = test.c

# Tool chain used for this project.
AVRGCC = /winavr-20070525

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
$(OUT)/%.o : %.c makefile
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
$(TRG).exe: $(SRC) makefile
	$(PC_GCC) -o $@ $(SRC)

#---------------------- MAKE CLEAN -------------------------------
#make instruction to delete created files
clean:
	$(RM) *.hex *.exe $(OUT)
