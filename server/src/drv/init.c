/**----------------------------------------------------------------------------
Проект:     SDK-2.0 
Название:   Инициализация контроллера
Файл:       init.c
Версия:     1.0.2

Описание:   В данном модуле находится вызов всех функций инициализации


Автор:      Ключев А.О.
Изменения:
-------------------------------------------------------------------------------
N     Дата      Версия      Автор           Описание   
-------------------------------------------------------------------------------
1   01.02.05    1.0.1       Ключев А.О.     Базовая версия
2	13.01.06	1.0.2		Петров Е.В.		Добавлены новфе устройства
-----------------------------------------------------------------------------*/

#include <stdio.h>
#include <serial.h>
#include <leds.h>
#include <systimer.h>
#include <sw.h>
#include <wdt.h>
//#include <i2c.h>
//#include <can.h>
//#include <tests.h>
//#include <adc.h>
//#include <pwm.h>
//#include <kb.h>
//#include <lcd.h>
#include <spi.h>
#include <zb.h>
#include <eth.h>

static void init_pio( void );

static void self_test( void );
static unsigned long test_memory_size( unsigned long addr, unsigned long max_kb );
static void test_wdt_state( void );

/**----------------------------------------------------------------------------
                            init
-------------------------------------------------------------------------------
Инициализация системы

Вход:       нет 
Выход:      нет
Результат:  OK - все нормально, ERROR - есть ошибки
Описание:   
Пример:
-----------------------------------------------------------------------------*/
int init ( void )  
{           
//	unsigned long i;

//	disable_interrupts();

    init_pio();
	
    init_led();
//    init_kb();
//	init_adc();
//	init_pwm();
    init_serial();
//    init_i2c();
//    init_can(CAN0, CANBitrate125k_12MHz); 
//    init_can(CAN1, CANBitrate125k_12MHz); 
    init_systimer0();
    init_systimer1();
    
//	enable_interrupts();

    start_timer0();  
    start_timer1();  
    init_eth();

//    init_lcd();

	leds( 0x55);

	init_spi1();

	packet_info.pan_id = 0x8855;
	packet_info.src_addr = 0x5678;
	packet_info.dest_addr = 0x1234;
	init_zb();

	zb_receive_on();

	/*
	for( ; ; )
	{ 
		if (!CC2420_RX_Empty())
		{
			if (CC2420_RX_Overrun())
			{
				// Buffer overrun - flush it.
				CC2420_Flush_RX();
			}
			else
			{
				// At least one packet is ready in the buffer.
				//zb_receive_packet();
				leds(0x0F);
				delay_ms( 50 );
				leds(0x00);
				delay_ms( 50 );
				leds(0x0F);
				delay_ms( 50 );
				leds(0x00);
				delay_ms( 50 );
				leds(0x0F);
				delay_ms( 50 );
				leds(0x00);
				delay_ms( 50 );
				leds(0x0F);
				delay_ms( 50 );
				leds(0x00);
				delay_ms( 50 );
				leds(0x0F);
				delay_ms( 50 );
				leds(0x00);
				delay_ms( 50 );
			}
		}
	} */

	/*for ( i = 0; i < 20; i++ )
	{
		put_pixel( i, i, 1 );
	}

		delay_ms( 5000 );*/

	//    self_test();
	//    draw_bitmap( 0, 0 );

    return OK;     
}                                

/**----------------------------------------------------------------------------
                            init_pio
-------------------------------------------------------------------------------
Инициализация портов ввода-вывода

Вход:       нет 
Выход:      нет
Результат:  нет
Описание:   порты микроконтроллера переводятся в безопасное состояние
-----------------------------------------------------------------------------*/
static void init_pio( void )
{
	PINSEL0 = 0x80550055;
	PINSEL1 = 0x154540A8;
	PINSEL2 = 0x0D6149D4;

    IO0DIR = 0x011A7D05;
    IO1DIR = 0x00000000;
    IO2DIR = 0x003F0000;
    IO3DIR = 0xC0000000;

    IO0CLR = 0xFFFFFFFF;
    IO1CLR = 0xFFFFFFFF;
    IO2CLR = 0xFFFFFFFF;
    IO3CLR = 0xFFFFFFFF;

    IO0SET = 0x00000000;
    IO1SET = 0x00000000;
    IO2SET = 0x00000000;
    IO3SET = 0x00000000;
}

/**----------------------------------------------------------------------------
                            self_test
-------------------------------------------------------------------------------
Автоматический тест системы

Вход:       нет 
Выход:      нет
Результат:  нет
Описание:   
Пример:
-----------------------------------------------------------------------------*/
/*
static void self_test( void )
{
	int i;
    
    printf("\n--- SELF TEST ----------\n");

    for( i = 0; i < 3; i++ )
    {
        led( P_LED_STATUS, LED_ON );   delay_ms( 50 );
        led( P_LED_STATUS, LED_OFF );  delay_ms( 50 );
    }       

    test_memory(VERBOSE);
    test_memory_size( CS0_BASE_ADDRESS, 8192 );
    test_wdt_state();

    printf("PINSEL0:                0x%.8X\n", PINSEL0 ); 
    printf("PINSEL1:                0x%.8X\n", PINSEL1 ); 
    printf("PINSEL2:                0x%.8X\n", PINSEL2 ); 

    printf("--- END OF SELF TEST ---\n");
}
*/
/*-----------------------------------------------------------------------------
Тестирование состояния WDT
-----------------------------------------------------------------------------*/
static void test_wdt_state( void )
{
    if( get_wdt_state() == WDT_RESTART )
        printf("RESTART STATUS:         [ WDT RESTART ] \n");
    else
        printf("RESTART STATUS:         [ POWER ON RESET ] \n");
}     

/*-----------------------------------------------------------------------------
Тестирование объема памяти
-----------------------------------------------------------------------------*/
static unsigned long test_memory_size( unsigned long addr, unsigned long max_kb )
{
	unsigned long i, count = 0;
	unsigned long index;
	unsigned short *wptr = (unsigned short *)addr;

	for( i = 0, index = 0; i < max_kb; i++ )
	{
		wptr[ index ] = i;
		if ( wptr[ index ] != i ) break;
		if ( i > 0 )
	  	{
	  		if( wptr[ 0 ] == i ) break;
		}
    	count++;
    	index += 512;        
	}
  
  printf("EXTERNAL MEMORY:        %d Kb\n", count );  

  return count;
}

