/******************************************************************************
Проект:     SDK-2.0 
Название:   Функции для обеспечения стандартного ввода-вывода
Файл:       stdio_low.c
Версия:     1.0

Описание:   Модуль предназначен для обеспечения вывода текстовой информации
            с помощью функций printf через UART0.

Автор:      Ключев А.О.
Изменения:
-------------------------------------------------------------------------------
N     Дата      Версия      Автор           Описание   
-------------------------------------------------------------------------------
1   31.01.05    1.0.1   Ключев А.О.      Базовая версия
******************************************************************************/

#include <serial.h>


#define CR                  0x0D
#define LF                  0x0A


char sprintf_buf[100]; // буфер для хранения результата sprintf

/* ----------------------------------------------------------------------------
Вывод символа в последовательный канал
-----------------------------------------------------------------------------*/

int putchar0 ( int ch )  
{                  
    if (ch == '\n') 
    {
        wsio0( CR );
        wsio0( LF );
    }
    else
        wsio0( ch );

    return ch;
}

/* ----------------------------------------------------------------------------
Чтение символа из последовательного канала
-----------------------------------------------------------------------------*/
int getchar0 ( void )  
{                    
    return rsio0();
}

/* ----------------------------------------------------------------------------
Возвравщает признак готовности приемника последовательного канала
-----------------------------------------------------------------------------*/
int kbhit0 ( void )
{
    return rsiostat0();
}

/* ----------------------------------------------------------------------------
Вывод строки в последовательный канал
-----------------------------------------------------------------------------*/
void type0 ( char *buf )
{
	while ( *buf != 0 )
		putchar0(*(buf++));
}
