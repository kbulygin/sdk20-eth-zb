/**----------------------------------------------------------------------------
Проект:     SDK-2.0
Название:   Описание битовых портов ввода-вывода платы SDK-2.0
Файл:       sdk_2_0.h
Версия:     1.0.1

Описание:   Данный файл позволяет избавиться от привязки драйверов
            к распределению портов для различных вариантов плат.
            Предполагается использование процессора с ядром ARM.

Автор:      Ключев А.О.
Изменения:
-------------------------------------------------------------------------------
N     Дата      Версия      Автор           Описание   
-------------------------------------------------------------------------------
1   31.01.05    1.0.1   Ключев А.О.      Базовая версия
-----------------------------------------------------------------------------*/

#ifndef __SDK_2_0_H
#define __SDK_2_0_H

#include <lpc2292.h>              

// Прикладные имена портов

// SW3

#define P_SW3_DIR IO0DIR // Управление направлением
#define P_SW3_PIN IO0PIN // Считывание значений

#define P_SW3_1 P_04     // Перемычка  "1"
#define P_SW3_2 P_05     // Перемычка  "2"
#define P_SW3_3 P_06     // Перемычка  "3" 
#define P_SW3_4 P_07     // Перемычка  "4"

// SW4

#define P_SW4_DIR_12 IO2DIR // Управление направлением
#define P_SW4_PIN_12 IO2PIN // Считывание значений

#define P_SW4_DIR_3456 IO3DIR // Управление направлением
#define P_SW4_PIN_3456 IO3PIN // Считывание значений

#define P_SW4_DIR_78 IO1DIR // Управление направлением
#define P_SW4_PIN_78 IO1PIN // Считывание значений

#define P_SW4_1 P_28     // Перемычка  "1"
#define P_SW4_2 P_29     // Перемычка  "2"
#define P_SW4_3 P_20     // Перемычка  "3" 
#define P_SW4_4 P_21     // Перемычка  "4"
#define P_SW4_5 P_22     // Перемычка  "5"
#define P_SW4_6 P_23     // Перемычка  "6"
#define P_SW4_7 P_16     // Перемычка  "7" 
#define P_SW4_8 P_17     // Перемычка  "8"

// светодиоды

#define P_LED_DIR   IO0DIR  // Управление напрявлением
#define P_LED_SET   IO0SET  // Установка 1
#define P_LED_CLR   IO0CLR  // Установка 0

#define P_LED_STATUS  P_20  // Светодиод "STATUS"


// направление RS485

#define P_RS485_SET   IO0SET  // Установка 1
#define P_RS485_CLR   IO0CLR  // Установка 0

#define P_RS485_DIR   P_11    // Упрввление направлением: 0 прием, 1 передача

// Бит контроля питания

#define POWER_FAILURE   IO3PIN

// Биты портов ввода-вывода

#define P_00         0x00000001L  
#define P_01         0x00000002L
#define P_02         0x00000004L
#define P_03         0x00000008L
#define P_04         0x00000010L  
#define P_05         0x00000020L
#define P_06         0x00000040L
#define P_07         0x00000080L
#define P_08         0x00000100L  
#define P_09         0x00000200L
#define P_10         0x00000400L
#define P_11         0x00000800L
#define P_12         0x00001000L  
#define P_13         0x00002000L
#define P_14         0x00004000L
#define P_15         0x00008000L
#define P_16         0x00010000L  
#define P_17         0x00020000L
#define P_18         0x00040000L
#define P_19         0x00080000L
#define P_20         0x00100000L
#define P_21         0x00200000L
#define P_22         0x00400000L
#define P_23         0x00800000L
#define P_24         0x01000000L
#define P_25         0x02000000L
#define P_26         0x04000000L
#define P_27         0x08000000L
#define P_28         0x10000000L
#define P_29         0x20000000L
#define P_30         0x40000000L
#define P_31         0x80000000L


// Базовые адреса

#define CS0_BASE_ADDRESS        0x80000000L
#define CS1_BASE_ADDRESS        0x81000000L
#define CS2_BASE_ADDRESS        0x82000000L
#define CS3_BASE_ADDRESS        0x83000000L

#define EXTERNAL_MEMORY_SIZE    524288L
#define EXTERNAL_MEMORY_ADDR    CS0_BASE_ADDRESS

#define ETH_BASE_ADDRESS        CS1_BASE_ADDRESS

#endif // __SDK_2_0_H
