/**----------------------------------------------------------------------------
Проект:     SDK-2.0 
Название:   Драйвер системного таймера
Файл:       systimer.c
Версия:     1.0.2

Описание:   

Автор:      Ключев А.О.
Изменения:
-------------------------------------------------------------------------------
N     Дата      Версия      Автор           Описание   
-------------------------------------------------------------------------------
1   01.02.05    1.0.1   Ключев А.О.     Базовая версия
2   12.01.06    1.0.2   Петров Е.В.     Микросекундный таймер (Timer 1)
-----------------------------------------------------------------------------*/

#include <systimer.h>         
#include <wdt.h>   
#include <leds.h>
//#include <lcd.h>
#include <common.h>

#define VIC_TIMER0_bit (1 << VIC_TIMER0)
#define VIC_TIMER1_bit (1 << VIC_TIMER1)

static unsigned long __systime = 0;

//static void (*timer_function)();
static void TimerInterrupt0(void);

unsigned long clock( void )
{
  return __systime;
}

unsigned long clock_mcs( void )
{
  return T1TC;
}

/* ----------------------------------------------------------------------------
                        Интерфейс модуля
-----------------------------------------------------------------------------*/

void init_systimer( void )
{
    init_systimer0();
    init_systimer1();
}

void init_systimer0( void )
{
  // Disable all interrupts
//  disable_interrupts();
  // Setup interrupt controller.
	VICProtection = 0;

	VICIntSelect &= ~VIC_TIMER0_bit;		// IRQ on timer 0 line.
	VICVectAddr0 = ( unsigned int )&TimerInterrupt0;
	VICVectCntl0 = 0x20 | VIC_TIMER0;		// Enable vector interrupt for timer 0.
	VICIntEnable |= VIC_TIMER0_bit;			// Enable timer 0 interrupt.

	T0TCR = 0;			// Disable timer 0.
						// The prescaler refuses any value other than zero!?
	T0PC  = 0;			// Prescaler is set to no division. Cclk is 48 MHz, pclk is 12 MHz.
	T0MR0 = 12000;		// Count up to this value. Generate 1000 Hz interrupt.
//	T0MR0 = 3000;		// Count up to this value. Generate 4000 Hz interrupt.
	T0MCR = 3;			// Reset and interrupt on MR0 (match register 0).
	T0CCR = 0;			// Capture is disabled.
	T0EMR = 0;			// No external match output.
  // Enable all interrupts
//  enable_interrupts();
}

void init_systimer1( void )
{
  T1TCR = 0; // Disable timer 1.
  T1PR = 11; // 12MHz/(11+1) = 1000000Hz
  T1PC = 0;
  T1TC = 0;
  T1MCR = 0; // Match is disabled.
  T1CCR = 0; // Capture is disabled.
  T1EMR = 0; // No external match output.
}

void start_timer0( void )
{
  T0TCR = 1; // Enable timer 0.
}

void start_timer1( void )
{
  T1TCR = 1; // Enable timer 1.
}

void reset_timer1( void )
{
  T1PC = 0;
  T1TC = 0;
}

unsigned long dtime( unsigned long t2 )
{
    unsigned long t1 = (unsigned long) clock();

    reset_wdt();  
    return t1 - t2;  
}

unsigned long dtime_mcs( unsigned long t2 )
{
    unsigned long t1 = (unsigned long) clock_mcs();

    reset_wdt();  
    return t1 - t2;  
}

void delay_ms( unsigned long ms )
{
    unsigned long t1 = clock();

  	while(1) {
    	if(dtime(t1) > ms) 
    		break;
    	reset_wdt();
  	}  
}

void delay_mcs( unsigned long mcs )
{
    unsigned long t1 = (unsigned long) clock_mcs();

  while( 1 )
  {
    if( dtime_mcs( t1 ) > mcs ) break;
    reset_wdt();
  }  
}


/* ----------------------------------------------------------------------------
                        Локальные функции модуля
-----------------------------------------------------------------------------*/

//
// Interrupt handler.
//

static void TimerInterrupt0(void)
{
    static unsigned char tick4 = 0;
    static unsigned char ind_clock = 0;

    tick4 = ( tick4 + 1 ) & 0x03;
    if ( !tick4 ) __systime++;
/*
    if ( ind_clock )
    {
        IO0SET |= IND_CL;
        ind_clock = 0;
    }
    else
    {
        IO0CLR |= IND_CL;
        ind_clock = 1;
    }
*/
    T0IR = 0xff; // Clear timer 0 interrupt line.
}

