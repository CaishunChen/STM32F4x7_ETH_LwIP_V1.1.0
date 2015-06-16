#ifndef     __PRINT_H_
#define    __PRINT_H_

#include "serial_debug.h"

void    print(char* fmt, ...);
void    printch(char ch);
void    printdec(int dec);
void    printflt(double flt);
void    printbin(int bin);
void    printhex(int hex);
void    printstr(char* str);

#define console_print(ch)    Debug_Putchar(ch)

#endif    /*#ifndef __PRINT_H_*/

