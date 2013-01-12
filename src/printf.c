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
#include <math.h>
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

#define BUFMAX  30      // Size of buffer for formatting numbers into

#define FL_LEFT_JUST    (1<<0)
#define FL_ZERO_PAD     (1<<1)
#define FL_SPECIAL      (1<<2)
#define FL_PLUS         (1<<3)
#define FL_SPACE        (1<<4)
#define FL_NEG          (1<<5)
#define FL_FCVT         (1<<6)
#define FL_GCVT         (1<<7)

#define FLOAT_DIGITS    17
#define MAX_POWER       256

static const double smalltable[] = {
    1e-256, 1e-128, 1e-64,
    1e-32, 1e-16, 1e-8, 1e-4, 1e-2, 1e-1, 1.0
};
static const double largetable[] = {
    1e+256, 1e+128, 1e+64,
    1e+32, 1e+16, 1e+8, 1e+4, 1e+2, 1e+1
};

// Function to trim trailing zeros and DP in 'g' mode.
// Return pointer to new string terminator position.
static char *trim_zeros(char *p)
{
    // Trim trailing 0's in 'g' mode.
    while (*p == '0')
        p--;
    // If last non-zero is DP, trim that as well.
    if (*p != '.') ++p;
    return p;
}

static char *format_float(double number, int ndigits, unsigned char flags, char exp_char, char *buf)
{
    int decpt;
    int power10;
    unsigned char i;
    char *p = buf + 2;
    char *pend;
    
    // Handle special values
    if (isinf(number))
    {
        buf[0] = 'I';
        buf[1] = 'n';
        buf[2] = 'f';
        buf[3] = '\0';
        return buf;
    }
    if (isnan(number))
    {
        buf[0] = 'N';
        buf[1] = 'a';
        buf[2] = 'N';
        buf[3] = '\0';
        return buf;
    }

    // Handle all numbers as positive.
    if (number < 0)
    {
        number = -number;
        flags |= FL_NEG;
    }

    // Digits printed cannot be negative.
    if (ndigits < 0)
        ndigits = 0;

    // Prefill buffer with zeros.
    for (i = 0; i < BUFMAX; i++)
        buf[i] = '0';

    if (number == 0)
    {
        // Special case to correct number of decimals, significant figures,
        // and avoid printing 0E-00.
        ndigits++;
        decpt = 1;
    }
    else
    {
        /* Normalise the number such that it lies in the range 1 <= n < 10.
         * This is done using a binary search, making the largest possible
         * adjustment first and getting progressively smaller. This gets
         * to the answer in the fastest time, with the minimum number of
         * operations to introduce rounding errors.
         */
        decpt = 1;
        // First make small numbers bigger.
        power10 = MAX_POWER;
        i = 0;
        while (number < 1.0)
        {
            while (number < smalltable[i + 1])
            {
                number /= smalltable[i];
                decpt -= power10;
            }
            power10 >>= 1;
            i++;
        }
        // Then make big numbers smaller.
        power10 = MAX_POWER;
        i = 0;
        while (number >= 10.0)
        {
            while (number >= largetable[i])
            {
                number /= largetable[i];
                decpt += power10;
                // Avoid this loop hanging on infinity.
                if (decpt > 1000)
                {
                    buf[0] = 'I';
                    buf[1] = 'n';
                    buf[2] = 'f';
                    buf[3] = '\0';
                    return buf;
                }
            }
            power10 >>= 1;
            i++;
        }
    }
    
    // For g conversions determine whether to use e or f mode.
    if (flags & FL_GCVT)
    {
        /* 'g' format uses 'f' notation where it can and
         * 'e' notation where the exponent is more extreme.
         * Some references indicate that it uses the more
         * compact form but the ANSI standard give an explict
         * definition: Use 'e' when the exponent is < -4
         * or where the exponent is >= ndigits.
         * The exponent is equal to decpt - 1 so this translates
         * to decpt <= -4 || decpt > ndigits
         * http://www.acm.uiuc.edu/webmonkeys/book/c_guide/2.12.html#printf
         * http://www.mkssoftware.com/docs/man3/printf.3.asp
         */
        if (decpt > -4 && decpt <= ndigits)
            flags |= FL_FCVT;
    }
    
    // Sanitise ndigits making sure it fits buffer space.
    if (flags & FL_FCVT)
    {
        if (!(flags & FL_GCVT))
        {
            // For fcvt operation the number of digits is used to
            // refer to decimal places rather than significant digits.
            ndigits += decpt;
            // When there are no significant digits,
            // avoid printing too many 0's.
            if (ndigits < 0)
            {
                decpt -= ndigits;
                ndigits = 0;
            }
        }
        /* For 'f' conversions with positive DP value that would overflow
         * the buffer space, there is no way to display this so fall back
         * to 'e' format. 'f' conversion needs space for sign, point and
         * rounding digit. The point may be forced using the special flag
         * and the rounding digit may be included in the case of maximum
         * overflow.
         */
        if (decpt > BUFMAX-3)
        {
            flags &= ~FL_FCVT;
        }
    }

    if (flags & FL_FCVT)
    {
        // Allow space for sign, point and rounding digit.
        if (ndigits > BUFMAX-3)
            ndigits = BUFMAX-3;
        // Start placing digits after leading 0's
        if (decpt < 1)
        {
            // Number of leading zeros
            int nzero = 1 - decpt;
            // Ensure pointer is not past end of buffer
            if (nzero > BUFMAX-3)
                nzero = BUFMAX-3;
            p += nzero;
        }
    }
    else
    {
        // Allow space for sign, point and up to 3 digit exponent (E+123).
        // Rounding happens in the place where the exponent will go.
        if (ndigits > BUFMAX-7)
            ndigits = BUFMAX-7;
    }
    
    // Format digits one-by-one into the output string.
    // One extra digit is required for rounding.
    for (i = 0; i <= ndigits; i++)
    {
        // Ignore digits beyond the supported precision.
        if (i >= FLOAT_DIGITS)
        {
            *p++ = '0';
        }
        else
        {
            int n = number;
            *p++ = n + '0';
            number = (number - n) * 10;
        }
    }
    // Store a pointer to the last (rounding) digit.
    pend = --p;
    // Round the result directly in the string buffer.
    // Only use the calculated digits, not trailing 0's.
    if (i > FLOAT_DIGITS)
    {
        p -= (i - FLOAT_DIGITS);
    }
    if (*p >= '5')
    {
        for (;;)
        {
            if (i == 0)
            {
                // The rounding has rippled all the way through to
                // the first digit. i.e. 9.999..9 -> 10.0
                // Just replace the first 0 with a 1 and shift the DP.
                *p = '1';
                ++decpt;
                // This increases the displayed digits for 'f' only.
                if ((flags & (FL_FCVT|FL_GCVT)) == FL_FCVT)
                {
                    ++ndigits;
                    ++pend;
                }
                break;
            }
            // Previous digit was a rollover
            *p-- = '0';
            // Increment next digit and break out unless there is a rollover.
            if (*p != '9')
            {
                (*p)++;
                break;
            }
        }
    }

    // Insert the decimal point
    if (flags & FL_FCVT)
    {
        int num;
        num = (decpt > 1) ? decpt : 1;
        p = buf + 1;
        for (i = 0; i < num; i++)
        {
            *p = *(p+1);
            ++p;
        }
        if (p + 1 < pend || (flags & FL_SPECIAL))
        {
            // There are digits after the DP or DP is forced.
            *p = '.';
            // Trim trailing 0's in 'g' mode.
            if ((flags & FL_GCVT) && !(flags & FL_SPECIAL))
            {
                pend = trim_zeros(pend - 1);
            }
        }
        else
        {
            // DP at end is not required.
            --pend;
        }
    }
    else
    {
        // Decimal point is always after first digit.
        buf[1] = buf[2];
        buf[2] = '.';
        // Trim trailing 0's in 'g' mode.
        if ((flags & FL_GCVT) && !(flags & FL_SPECIAL))
        {
            pend = trim_zeros(pend - 1);
        }
        // Add exponent
        *pend++ = exp_char;
        if (--decpt < 0)
        {
            *pend++ = '-';
            decpt = -decpt;
        }
        else
        {
            *pend++ = '+';
        }
#ifndef EXP_3_DIGIT
        // Optional 3rd digit of exponent
        if (decpt > 99)
#endif
        {
            *pend++ = decpt / 100 + '0';
            decpt %= 100;
        }
        // Always print at least 2 digits of exponent.
        *pend++ = decpt / 10 + '0';
        *pend++ = decpt % 10 + '0';
    }
    // Add the sign prefix.
    p = buf + 1;
    if      (flags & FL_NEG)    *--p = '-';
    else if (flags & FL_PLUS)   *--p = '+';
    else if (flags & FL_SPACE)  *--p = ' ';
    
    *pend = '\0';   // Add null terminator
    return p;       // Start of string
}

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
    double fvalue;

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
            case 'f':
                // Set default precision
                if (precision == -1) precision = 6;
                fvalue = va_arg(ap, double);
                flags |= FL_FCVT;
                p = format_float(fvalue, precision, flags, 'e', buffer);
                // Precision is not used to limit number output.
                precision = -1;
                break;
            case 'e':
            case 'E':
                // Set default precision
                if (precision == -1) precision = 6;
                fvalue = va_arg(ap, double);
                p = format_float(fvalue, precision + 1, flags, convert, buffer);
                // Precision is not used to limit number output.
                precision = -1;
                break;
            case 'g':
            case 'G':
                // Set default precision
                if (precision == -1) precision = 6;
                fvalue = va_arg(ap, double);
                flags |= FL_GCVT;
                p = format_float(fvalue, precision, flags, convert - 2, buffer);
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
