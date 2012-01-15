/*************************************************************************

      Copyright (c) Skirrid Systems 2006 - 2012

     Module Name: printf.h
    Description : Cut-down version of printf for embedded use.

*************************************************************************/

#ifndef PRINTF_H
#define PRINTF_H

/*************************************************************************
Memory access definitions

Some micros such as the AVR can only support storing strings in flash
memory by wrapping the string in a macro. To make this transparent we can
define the printf function itself as a macro which performs the wrap and
calls a renamed version of printf with a _rom suffix.
*************************************************************************/

/* Example for AVR micros using WinAVR (GCC) compiler

#define sprintf(buf, format, args...)   sprintf_rom(buf, PSTR(format), ## args)
#define printf(format, args...)         printf_rom(PSTR(format), ## args)

extern int sprintf_rom(char *, const char *, ...);
extern int printf_rom(const char *, ...);
*/

/* The standard declarations of printf and sprintf. */
#ifndef printf
extern int printf(const char *, ...);
#endif

#ifndef sprintf
extern int sprintf(char *, const char *, ...);
#endif

#endif
