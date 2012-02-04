#ifdef TEST_AVR
    #include "printf.h"
    #include <avr/io.h>
    #include <avr/pgmspace.h>
    #define GET_FORMAT(p)   pgm_read_byte(p)
    void outchar(char c) {}
    #define PUTCHAR_FUNC    outchar
#else
    #include <stdio.h>
#endif

/* Force printf.c to rename the functions so we can use both
   the standard library and our own version for side-by-side tests.
*/
#define printf x   

/* Include the printf source here so we can control the definitions
   used in the test environment. You would not normally need to do this.
*/
#include "printf.c"

#ifdef TEST_AVR
    /* In the AVR test environment define a new macro to use our function
       with strings in flash.
    */
    #define tprintf(format, args...)        printf_rom(PSTR(format), ## args)
#else
    /* In the PC test environment reinstate printf to call the normal library function.
       Define a new macro which calls both the library and our own versions of printf.
    */
    #undef printf
    #define tprintf(format, args...)        do { printf(format, ## args); printf_rom(format, ## args); } while(0)
#endif


#define PI  3.14159265358979323844
#define N   -23
#define X   0x23

int main(int argc, char *argv[])
{
    tprintf("Hello world\n");
    tprintf("Int %d %4d %04d %+04d % 4d %-4d.\n", N, N, N, N, N, N);
    tprintf("pi = %1.6f\n", PI);
    return 0;
}
