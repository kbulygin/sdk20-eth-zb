/**----------------------------------------------------------------------------
Проект:     SDK-2.0 
Название:   Драйвер последовательного канала
Файл:       serial.h
Версия:     1.0.1

Описание:   Реализация байтового ввода вывода через контроллеры UART0 и UART1
            для микроконтроллера Philips LPC2294

Автор:      Ключев А.О.
Изменения:
-------------------------------------------------------------------------------
N     Дата      Версия      Автор           Описание   
-------------------------------------------------------------------------------
1   31.01.05    1.0.1   Ключев А.О.      Базовая версия
-----------------------------------------------------------------------------*/

#ifndef __SERIAL_H
#define __SERIAL_H

#include <error.h>
#include <common.h>
#include <settings.h>

extern void init_serial( void );

extern void          wsio0( unsigned char ch );
extern unsigned char rsio0( void );
extern int           rsiostat0( void );

extern void          wsio1( unsigned char ch );
extern unsigned char rsio1( void );
extern int           rsiostat1( void );

extern void          rs485_direction( int mode );


extern char buf[80];


#endif // __SERIAL_H
