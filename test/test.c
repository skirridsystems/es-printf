/*************************************************************************
es-printf  -  configurable printf for embedded systems

Test program to compare output with the standard library function
and to generate AVR code for embedded testing.

$Id$

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

#ifdef TEST_AVR
    #include "../src/printf.h"
    #include <avr/io.h>
    #include <avr/pgmspace.h>
    #define GET_FORMAT(p)   pgm_read_byte(p)
    void outchar(char c) {}
    #define PUTCHAR_FUNC    outchar
    #define NO_DOUBLE_PRECISION
#else
    #include <stdio.h>
    #define EXP_3_DIGIT
#endif
    #include "float.h"

/* Force printf.c to rename the functions so we can use both
   the standard library and our own version for side-by-side tests.
*/
#define printf x
#define sprintf x

/* Include the printf source here so we can control the definitions
   used in the test environment. You would not normally need to do this.
*/
#include "../src/printf.c"

#ifdef TEST_AVR
    /* In the AVR test environment define a new macro to use our function
       with strings in flash.
    */
    #define tprintf(format, args...)        printf_rom(PSTR(format), ## args)
  #ifdef BASIC_PRINTF_ONLY
    #define tsprintf(buf, format, args...)  printf_rom(PSTR(format), ## args)
  #else
    #define tsprintf(buf, format, args...)  sprintf_rom(buf, PSTR(format), ## args)
  #endif
#else
    /* In the PC test environment reinstate printf to call the normal library function.
       Define a new macro which calls both the library and our own versions of printf.
    */
    #undef printf
    #undef sprintf
    #define tprintf(format, args...)        do { printf(format, ## args); printf_rom(format, ## args); } while(0)
  #ifdef BASIC_PRINTF_ONLY
    #define tsprintf(buf, format, args...)  do { printf(format, ## args); printf_rom(format, ## args); } while(0)
  #else
    #define tsprintf(buf, format, args...)  do { printf(format, ## args); sprintf_rom(buf, format, ## args); printf("%s", buf); } while(0)
  #endif
#endif


#define PI  3.14159265358979323844
#define F6  123450.0
#define FL  1.2345e10
#define FS  1.2345e-10
#define N   -23
#define X   0x5a
#define O   023
#define S   "Abcde"

int main(int argc, char *argv[])
{
#ifdef SIZE
    #ifndef SIZEN
    tprintf("");
    #endif
#else
#ifndef BASIC_PRINTF_ONLY
    char buf[32];
#endif
#if FEATURE(USE_FLOAT)
    double one = 1.0;
#endif
    tsprintf(buf, "Hello world %x %% %z\n", 0x123);
#if FEATURE(USE_SPACE_PAD) || FEATURE(USE_ZERO_PAD)
  #if FEATURE(USE_SIGNED)
    tprintf("Int %d %4d %04d %+04d % 4d %-4d. %+3d %+3d % 3d % 3d\n", N, N, N, N, N, N, 0, -1, 0, -1);
  #endif
  #if FEATURE(USE_HEX_UPPER) || FEATURE(USE_HEX_LOWER)
    tprintf("Hex %x %4x %018X %+x % x %-4x. %+#x\n", X, X, X, X, X, X, X);
  #endif
  #if FEATURE(USE_OCTAL)
    tprintf("Oct %o %4o %04o %+o % o %-4o. %+#o\n", O, O, O, O, O, O, O);
  #endif
    tprintf("Str [%s] [%8s] [%-8s]\n", S, S, S);
  #if FEATURE(USE_INDIRECT)
    tprintf("Ind %*d %*s.\n", 4, N, 8, S);
  #endif
  #if FEATURE(USE_PRECISION)
    tprintf("Prec %.d %.2d %.2d %.2d %-5.3s %.3s.\n", 0, 1, 0, -1, S, S);
  #endif
#else
  #if FEATURE(USE_SIGNED)
    tprintf("Int %d %+d %+d % d % d\n", N, 0, -1, 0, -1);
  #endif
  #if FEATURE(USE_HEX_UPPER) || FEATURE(USE_HEX_LOWER)
    tprintf("Hex %x %+x % x\n", X, X, X);
  #endif
  #if FEATURE(USE_OCTAL)
    tprintf("Oct %o %+o % o\n", O, O, O);
  #endif
    tprintf("Str [%s]\n", S);
  #if FEATURE(USE_PRECISION)
    tprintf("Prec %.d %.2d %.2d %.2d %.3s.\n", 0, 1, 0, -1, S);
  #endif
#endif
#if FEATURE(USE_FLOAT)
    tprintf("pi = %f %e %g\n", PI, PI, PI);
    tprintf("f6dig = %.0f %e %g\n", F6, F6, F6);
    tprintf("flarge = %f %e %g\n", FL, FL, FL);
    tprintf("fsmall = %f %e %g\n", FS, FS, FS);
    tprintf("%%g = %g %g %g %G\n", 1.23e-5, 1.23e-4, 1.23e5, 1.23e6);
    tprintf("Max/Min = %e %e\n", DBL_MAX, DBL_MIN);
    tprintf("NaN/Inf = %f %f\n", sqrt(-1), one / (one - 1.0));
#endif
#endif
    return 0;
}
