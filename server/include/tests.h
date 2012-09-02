/**----------------------------------------------------------------------------
Проект:     SDK-2.0 
Название:   
Файл:       tests.h
Версия:     1.0.1

Описание:   

Изменения:
-------------------------------------------------------------------------------
N     Дата      Версия      Автор           Описание   
-------------------------------------------------------------------------------
------------------------------------------------------------------------------*/

#ifndef __TESTS_H
#define __TESTS_H

#define VERBOSE                 1
#define NO_VERBOSE              0

#include <error.h>
#include <serial.h>

#define MAIN                    "SDK-2.0"
#define SDK_2_0

extern void run_debug_console( void );

extern void test_sound( void );
extern void test_led( void );
extern void test_kb( void );
extern void test_lcd( void );
extern void test_sw( void );
extern void test_adc_pwm( void );
extern void test_rtc( void );
extern void test_fram( void );
extern void test_wdt( void );
extern void test_bus( void );
extern void test_serial( void );
extern void test_supervisor( void );
extern void test_can( void );
extern void test_parall( void );
extern int  test_memory();
extern void test_power( void );
extern unsigned char test_adc_dac( unsigned char autom );
extern void test_spi( void );
extern void test_zb( void );
extern void test_eth( void );

#endif // __TESTS_H
