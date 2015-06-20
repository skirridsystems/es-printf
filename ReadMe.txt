es-printf is an implementation of the standard C printf function targetted
at small embedded systems. It provides several advantages over the
standard function:

1.  There are many fine-grained customisations which allow you to trim
    the size to absolute minimum so it will fit in limited code space.
    You can elect to have only hexadecimal output, full floating point support
    and any number of options in between.

2.  It does not use any static buffers so it is thread-safe and can
    safely be used within interrupts and in an RTOS environment.

3.  You provide your own output function which can be to a serial port
    or to anything else convenient.

4.  Because the source code is available it is easy to extend the
    formatter to provide custom specifiers for your particular needs.

Full documentation is provided in es-printf.html
