/*************************************************************************

      Copyright (c) Skirrid Systems 2006 - 2008

     Module Name: printf.c
    Description : Cut-down version of printf for embedded use.

*************************************************************************/

#ifndef PRINTF_H
#define PRINTF_H

// These functions are in printf.c
extern void putchar(char c);
extern int sprintf_rom(char *, const char *, ...);
extern int printf_rom(const char *, ...);

// Macros to force the format string into flash rather than RAM.
#define sprintf(buf, format, args...)   sprintf_rom(buf, PSTR(format), ## args)
#define printf(format, args...)         printf_rom(PSTR(format), ## args)

#endif
