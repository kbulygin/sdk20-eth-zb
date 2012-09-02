/**----------------------------------------------------------------------------
Проект:     SDK-2.0 
Название:   Управление светодиодными индикаторами
Файл:       leds.h
Версия:     1.0.3

Описание:   В данном модуле находится драйвер светодиода "STATUS"
			и линейки светодиодов


Автор:      Ключев А.О.
Изменения:
-------------------------------------------------------------------------------
N     Дата      Версия      Автор           Описание   
-------------------------------------------------------------------------------
1   31.01.05    1.0.1   Ключев А.О.			Базовая версия
2	12.01.06	1.0.2	Петров Е.В.			светодиод STATUS
3	12.01.06	1.0.3	Ковязина Д.Р.		Линейка светодиодов
------------------------------------------------------------------------------*/

#ifndef __LED_H
#define __LED_H

#include <settings.h>

#define LED_ON      1   // Светодиод включен
#define LED_OFF     0   // Светодиод выключен

#define CONFIG_DIR_LED IO2DIR
#define SET_LED IO2SET
#define CLR_LED IO2CLR

#define LED0 P_16 
#define LED1 P_17 
#define LED2 P_18 
#define LED3 P_19

#define SEL_04 P_20
#define SEL_40 P_21

extern void init_led();
extern void init_led_line();
extern void init_led_status();
extern void led ( int led_num, int mode );  
extern void leds(unsigned char light);

#endif // __LED_H
