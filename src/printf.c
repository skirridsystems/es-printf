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

#define BUFLEN  16      // Size of buffer for formatting numbers into

static int doprnt(char *ptr, void (*func)(char c), const char *fmt, va_list ap)
{
    int value;
    unsigned uvalue;
    unsigned base;
    int width;
    char prefix_char;
    char pad_char;
    char convert, c;
    char *p;
    char buffer[BUFLEN];
    int  count = 0;

    buffer[BUFLEN-1] = '\0';

    while ((convert = GET_FORMAT(fmt)) != 0)
    {
        p = buffer + BUFLEN - 1;
        width = 0;
        prefix_char = '\0';
        pad_char = ' ';

        if (convert == '%')
        {
            convert = GET_FORMAT(++fmt);
            if (convert == '0')
            {
                pad_char = '0';
                convert = GET_FORMAT(++fmt);
            }
            while (convert >= '0' && convert <= '9')
            {
                width = width * 10 + convert - '0';
                convert = GET_FORMAT(++fmt);
            }

            switch (convert)
            {
            case 'c':
                width = 0;
                *--p = (char) va_arg(ap, int);
                break;
            case 'd':
                value = va_arg(ap, int);
                base = 10;
                if (value < 0)
                {
                    prefix_char = '-';
                    value = -value;
                }
                uvalue = (unsigned) value;
                goto number;
            case 'u':
                uvalue = va_arg(ap, unsigned);
                base = 10;
                goto number;
            case 'x':
                uvalue = va_arg(ap, unsigned);
                base = 16;
            number:
                if (uvalue == 0)
                {
                    // Avoid printing 0 as ' '
                    *--p = '0';
                    --width;
                }
                while (uvalue || width > 0)
                {
                    c = (char) ((uvalue % base) + '0');
                    if (c > '9') c += 'A' - '0' - 10;   // Hex digits
                    if (uvalue == 0) c = pad_char;
                    *--p = c;
                    uvalue /= base;
                    --width;
                }
                if (prefix_char) *--p = prefix_char;
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

        while (*p || width > 0)
        {
            if (*p) c = *p++;
            else    c = ' ';
            if (ptr != 0) *ptr++ = c;
            else func(c);
            ++count;
            --width;
        }
        ++fmt;
    }

    if (ptr != 0) *ptr = '\0';
    
    return count;
}

// Output a single character performing LF to CRLF conversion.
void printf_func(char c)
{
    if (c == '\n')
        PUTCHAR_FUNC('\r');
    PUTCHAR_FUNC(c);
}

// printf replacement - writes to serial output using putchar.
int printf(const char *fmt, ...)
{
    va_list ap;
    int Count;

    va_start(ap, fmt);
    Count = doprnt((char *)0, printf_func, fmt, ap);
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