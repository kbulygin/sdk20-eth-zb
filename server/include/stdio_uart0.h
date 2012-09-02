#ifndef __STDIO_UART0_H
#define __STDIO_UART0_H

#include <stdio.h>

extern char sprintf_buf[100];

#define printf(...) do { sprintf(sprintf_buf,__VA_ARGS__); type0(sprintf_buf); } while (0)
#define getchar() getchar0()
#define putchar(ch) putchar0(ch)
#define kbhit(...) kbhit0(__VA_ARGS__)

int  putchar0 ( int ch );
int  getchar0 ( void );
int  kbhit0   ( void );
void type0    ( char * ) ;

#endif // __STDIO_UART0_H
