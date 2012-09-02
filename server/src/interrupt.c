/******************************************************************************
������:     SDK-2.0 
��������:   ������ � ������������
����:       interrupt.c
������:     1.0.1
******************************************************************************/

#include <lpc2292.h>

void irq_handler( void ) __attribute__ ((interrupt("IRQ")));

///////////////////////////////////////////////////////////////////////////////
// ���������� ����������
///////////////////////////////////////////////////////////////////////////////
void irq_handler( void )
{
	void (*interrupt_function)( void );
  
	interrupt_function = (void(*)( void ))VICVectAddr;
	(*interrupt_function)();
	  
	VICVectAddr = 0; // Clear interrupt in VIC.
}


void disable_interrupts()
{
	// Disable all interrupts
//	VICIntEnClr = 0xffffffff;
}

void enable_interrupts()
{
	// Enable all interrupts.
//	VICIntEnable = 0xffffffff;
}
