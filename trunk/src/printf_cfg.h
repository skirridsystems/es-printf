/*************************************************************************

      Copyright (c) Skirrid Systems 2006 - 2012

     Module Name: printf_cfg.h
    Description : Configuration file for printf.c

*************************************************************************/

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
Feature configuration
*************************************************************************/

#endif
