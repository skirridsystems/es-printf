/*************************************************************************
es-printf  -  configurable printf for embedded systems

print_cfg.h: Select the build options for printf functions.

**************************************************************************
Copyright (c) 2006 - 2013 Skirrid Systems

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
******************************************************************************/

#ifndef PRINTF_CFG_H
#define PRINTF_CFG_H

/*************************************************************************
Memory access definitions

Some micros such as the AVR can only support storing and accessing strings
in flash memory using special macros and functions. This section can be
used to specify those methods. You may also need to modify printf.h
to get the compiler to place the format strings in flash memory.

The GET_FORMAT(ptr) macro is used to access a character in the printf
format string. By default this does a normal memory pointer access, but
you can configure it to access flash memory if needed.
*************************************************************************/

/* Example for AVR micros using WinAVR (GCC) compiler

#include <avr/io.h>
#include <avr/pgmspace.h>
#define GET_FORMAT(p)   pgm_read_byte(p)
*/

/*************************************************************************
Output configuration

By default printf will use the putchar function. If this is not defined
in your system you can set your own function here by defining
PUTCHAR_FUNC to be the name of that function.
*************************************************************************/

/*
extern void UARTSendByte(char c);
#define PUTCHAR_FUNC    UARTSendByte
*/

/*************************************************************************
Compiler capability configuration

Set some options that the C pre-processor will not tell us about.
*************************************************************************/

// Does the compiler support double precision or silently degrade to single?
// #define NO_DOUBLE_PRECISION

/*************************************************************************
Feature configuration
*************************************************************************/

// Non-zero to include floating point number support
#define USE_FLOAT       1

// Non-zero to include support for long integers
#define USE_LONG        1

// Non-zero to include precision support when floating point is not present.
// Precision is automatically enabled when floating point support is used.
#define USE_PRECISION   1

// Non-zero to include support for the special '#' flag.
#define USE_SPECIAL     1

// Non-zero to include indirect width/precision support e.g. "%*d"
#define USE_INDIRECT    1

#endif
