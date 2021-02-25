/*************************************************************************
es-printf  -  configurable printf for embedded systems

printf.h: Header for consumers of printf functions.

https://github.com/skirridsystems/es-printf

**************************************************************************
Copyright (c) 2006 - 2021 Skirrid Systems

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
*************************************************************************/

#ifndef PRINTF_H
#define PRINTF_H

/*************************************************************************
Number of chars output

Traditionally printf returns the number of chars output. If you are not
interested in that value you can leave PRINTF_T undefined.
On a small micro you can define the return type as unsigned char if you
are sure the total output width will never exceed 255, or unsigned short.
*************************************************************************/

#define PRINTF_T unsigned short

/*************************************************************************
Memory access definitions

Some micros such as the AVR can only support storing strings in flash
memory by wrapping the string in a macro. To make this transparent we can
define the printf function itself as a macro which performs the wrap and
calls a renamed version of printf with an _ suffix and no i.
*************************************************************************/

/*
Example for AVR micros using GCC toolchain from WinAVR or Atmel Studio

#define sprintf(buf, format, args...)   _sprntf(buf, PSTR(format), ## args)
#define printf(format, args...)         _prntf(PSTR(format), ## args)

extern printf_t _sprntf(char *, const char *, ...);
extern printf_t _prntf(const char *, ...);
*/



/*************************************************************************
End of customisations - Stop Editing!

The remainder of this file contains the function declarations.
*************************************************************************/

// Create a type definition for the return value
#ifndef PRINTF_T
typedef void printf_t;
#else
typedef PRINTF_T printf_t;
#endif

// Function declarations, unless macros have been defined above
#ifndef printf
extern printf_t printf(const char *, ...);
#endif

#ifndef sprintf
extern printf_t sprintf(char *, const char *, ...);
#endif

#endif
