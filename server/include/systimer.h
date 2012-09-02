/**----------------------------------------------------------------------------
Проект:     SDK-2.0 
Название:   Драйвер системного таймера
Файл:       systimer.h
Версия:     1.0.2

Описание:   

Автор:      Ключев А.О.
Изменения:
-------------------------------------------------------------------------------
N     Дата      Версия      Автор           Описание   
-------------------------------------------------------------------------------
1   01.02.05    1.0.1   Ключев А.О.     Базовая версия
2	12.01.06	1.0.2	Петров Е.В.		Микросекундный таймер (Timer 1)
-----------------------------------------------------------------------------*/

#ifndef __SYSTIMER_H
#define __SYSTIMER_H

#include <settings.h>

extern void init_systimer0( void );
extern void init_systimer1( void );
extern void delay_ms( unsigned long ms );
extern void delay_mcs( unsigned long ms );
extern void start_timer0( void );
extern void start_timer1( void );
extern void reset_timer1( void );
extern unsigned long dtime( unsigned long t2 );
extern unsigned long dtime_mcs( unsigned long t2 );

extern unsigned long clock( void );
extern unsigned long dtime( unsigned long t2 );


#endif // __SYSTIMER_H
