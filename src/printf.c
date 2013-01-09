/*************************************************************************

      Copyright (c) Skirrid Systems 2006 - 2012

     Module Name: printf.c
    Description : Cut-down version of printf for embedded use.

Implementation notes:
Contains a substantially cut down, integer-only version of the printf family.
The following print specifiers are supported :-
    %c  single character
    %d  signed decimal int
    %u  unsigned decimal int
    %x  unsigned hex int
    %s  string (in RAM, not Flash)
Leading padding can be specified for numbers by inserting a width after the
percentage sign. If the width starts with 0, the pad character is 0, otherwise
space is used.
Trailing space padding can be specified for strings in the same way.
Any other character following the percentage sign is passed through unchanged.

This version supports integers, but not longs.
******************************************************************************/

// System and config header files are included by the parent stub file.
#include <stdarg.h>
#include <string.h>
#include "printf.h"
#include "printf_cfg.h"

/* Define default macro to access the format string using a pointer. */
#ifndef GET_FORMAT
    #define GET_FORMAT(p)   (*(p))
#endif

/* Define default function for printf output. */
#ifndef PUTCHAR_FUNC
    #define PUTCHAR_FUNC    putchar
#endif

/* Renames the functions if they have been defined as macros in printf.h */
#ifdef printf
    #undef  printf
    #define printf printf_rom
#endif

#ifdef sprintf
    #undef  sprintf
    #define sprintf sprintf_rom
#endif

#define BUFMAX  16      // Size of buffer for formatting numbers into

#define FL_LEFT_JUST    (1<<0)
#define FL_ZERO_PAD     (1<<1)
#define FL_SPECIAL      (1<<2)
#define FL_PLUS         (1<<3)
#define FL_SPACE        (1<<4)
#define FL_NEG          (1<<5)

static int doprnt(char *ptr, void (*func)(char c), const char *fmt, va_list ap)
{
    int value;
    unsigned uvalue;
    unsigned base;
    int width;
    int fwidth;
    int precision;
    char convert, c;
    char *p;
    char buffer[BUFMAX+1];
    int  count = 0;
    unsigned char flags;

    buffer[BUFMAX] = '\0';

    while ((convert = GET_FORMAT(fmt)) != 0)
    {
        p = buffer + BUFMAX;
        width = 0;
        precision = -1;
        flags = 0;

        if (convert == '%')
        {
            // Extract flag chars
            for (;;)
            {
                convert = GET_FORMAT(++fmt);
                if (convert == '0')
                {
                    flags |= FL_ZERO_PAD;
                }
                else if (convert == '+')
                {
                    flags |= FL_PLUS;
                }
                else if (convert == '-')
                {
                    flags |= FL_LEFT_JUST;
                }
                else if (convert == ' ')
                {
                    flags |= FL_SPACE;
                }
                else if (convert == '#')
                {
                    flags |= FL_SPECIAL;
                }
                else
                    break;
            }
            // Extract width
            if (convert == '*')
            {
                width = va_arg(ap, int);
                convert = GET_FORMAT(++fmt);
            }
            else
            while (convert >= '0' && convert <= '9')
            {
                width = width * 10 + convert - '0';
                convert = GET_FORMAT(++fmt);
            }

            // Extract precision
            if (convert == '.')
            {
                precision = 0;
                convert = GET_FORMAT(++fmt);
                if (convert == '*')
                {
                    precision = va_arg(ap, int);
                    convert = GET_FORMAT(++fmt);
                }
                else
                while (convert >= '0' && convert <= '9')
                {
                    precision = precision * 10 + convert - '0';
                    convert = GET_FORMAT(++fmt);
                }
            }

            switch (convert)
            {
            case 'c':
                width = 0;
                *--p = (char) va_arg(ap, int);
                break;
            case 'd':
            case 'i':
                value = va_arg(ap, int);
                base = 10;
                if (value < 0)
                {
                    flags |= FL_NEG;
                    value = -value;
                }
                uvalue = (unsigned) value;
                goto number;
            case 'u':
                uvalue = va_arg(ap, unsigned);
                base = 10;
                goto number;
            case 'o':
                uvalue = va_arg(ap, unsigned);
                base = 8;
                goto number;
            case 'x':
            case 'X':
                uvalue = va_arg(ap, unsigned);
                base = 16;
            number:
                // Set default precision
                if (precision == -1) precision = 1;
                // Make sure options are valid.
                if (base != 10) flags &= ~(FL_PLUS|FL_NEG|FL_SPACE);
                else            flags &= ~FL_SPECIAL;
                // Generate the number without any prefix yet.
                fwidth = width;
                // Avoid formatting buffer overflow.
                if (fwidth > BUFMAX) fwidth = BUFMAX;
                while (uvalue || precision > 0)
                {
                    c = (char) ((uvalue % base) + '0');
                    if (c > '9')
                    {
                        // Hex digits
                        if (convert == 'X') c += 'A' - '0' - 10;
                        else                c += 'a' - '0' - 10;
                    }
                    *--p = c;
                    uvalue /= base;
                    --fwidth;
                    --precision;
                }
                // Allocate space for the sign bit.
                if (flags & (FL_PLUS|FL_NEG|FL_SPACE)) --fwidth;
                // Allocate space for special chars if required.
                if (flags & FL_SPECIAL)
                {
                    if (convert == 'o') fwidth -= 1;
                    else fwidth -= 2;
                }
                // Add leading zero padding if required.
                if ((flags & FL_ZERO_PAD) && !(flags & FL_LEFT_JUST))
                {
                    while (fwidth > 0)
                    {
                        *--p = '0';
                        --fwidth;
                    }
                }
                // Add special prefix if required.
                if (flags & FL_SPECIAL)
                {
                    if (convert != 'o') *--p = convert;
                    *--p = '0';
                }
                // Add the sign prefix
                if      (flags & FL_NEG)    *--p = '-';
                else if (flags & FL_PLUS)   *--p = '+';
                else if (flags & FL_SPACE)  *--p = ' ';
                // Precision is not used to limit number output.
                precision = -1;
                break;
            case 's':
                p = va_arg(ap, char *);
                break;
            default:
                *--p = convert;
                break;
            }
        }
        else
        {
            *--p = convert;
        }

        // Check width of formatted text.
        fwidth = strlen(p);
        // Copy formatted text with leading or trailing space.
        // A positive value for precision will limit the length of p used.
        while ((*p && precision != 0) || width > 0)
        {
            if (((flags & FL_LEFT_JUST) || width <= fwidth) && *p && precision != 0) c = *p++;
            else c = ' ';
            if (ptr != 0) *ptr++ = c;
            else func(c);
            ++count;
            --width;
            if (precision > 0) --precision;
        }
        ++fmt;
    }

    if (ptr != 0) *ptr = '\0';
    
    return count;
}

// printf replacement - writes to serial output using putchar.
int printf(const char *fmt, ...)
{
    va_list ap;
    int Count;

    va_start(ap, fmt);
    Count = doprnt((char *)0, (void (*)(char))PUTCHAR_FUNC, fmt, ap);
    va_end(ap);
    
    return Count;
}

//  sprintf replacement - writes into buffer supplied.
int sprintf(char *buf, const char *fmt, ... )
{
    va_list ap;
    int Count;

    va_start(ap, fmt);
    Count = doprnt(buf, 0, fmt, ap);
    va_end(ap);
    
    return Count;
}
