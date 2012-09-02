/**----------------------------------------------------------------------------
Проект:     SDK-2.0 
Название:   Драйвер контроллера ethernet LAN91C111
Файл:       eth.c
Версия:     1.0.1

Описание:   


Автор:      Ключев А.О.
Изменения:
-------------------------------------------------------------------------------
N     Дата      Версия      Автор           Описание   
-------------------------------------------------------------------------------
1   22.02.05    1.0.1   Ключев А.О.      Базовая версия
------------------------------------------------------------------------------*/
#include <stdio_uart0.h>
#include <eth.h>
#include <eth_mii_phy.h>
#include <systimer.h>
#include <common.h>

#define ETH_ALLOCATE_TO 3000    // Время ожидания выделения памяти под передаваемый пакет
#define ETH_TRANSMIT_TO 3000    // Время ожидания передачи пакета
#define ETH_RELEASE_TO  3000

#define ETH_INIT_TRY_CNT			2
#define PHY_STATUS_LINK_BIT			0x0004
#define PHY_STATUS_ANEG_ACK_BIT		0x0020

#define PHY_STATUS_OUT_SPDDET_BIT	0x0080
#define PHY_STATUS_OUT_DPLXDET_BIT	0x0040

volatile unsigned short *eth_reg = (unsigned short *)ETH_BASE_ADDRESS;

/* ----------------------------------------------------------------------------
                        Интерфейс модуля
-----------------------------------------------------------------------------*/

unsigned long init_eth( void )
{
	unsigned long i, t;
	unsigned short tmp, tcr;

//    IO3CLR = 0xC0000000;

	delay_ms( 50 );				// задержка 50ms

	// Soft reset
    ETH_BANK = 0; ETH_RCR       = 0x8000;
    ETH_BANK = 0; ETH_RCR       = 0x0000;

	delay_ms( 50 );				// задержка 50ms

	// ANEG = 1
    ETH_BANK = 0; ETH_RPCR      = 0x0810;
    // reset PHY
    write_phy_reg( PHY_CONTROL, 0x8000 );
    // Start Auto_Negatiation process
    write_phy_reg( PHY_CONTROL, 0x1000 );

	printf( "[ ETH ] Wait start Auto_Negatiation process\n" );

    for ( i = 0; i < ETH_INIT_TRY_CNT; i++ ) {
	    delay_ms( 1500 );			// задержка 1.5s

	    t = clock();
	    do {
		    tmp = read_phy_reg( PHY_STATUS );
		    if ( dtime( t ) >= 2000 ) 
		    	break;
	    } while ( ( !( tmp & PHY_STATUS_LINK_BIT ) || !( tmp & PHY_STATUS_ANEG_ACK_BIT ) ) );

	    if ( !( tmp & PHY_STATUS_LINK_BIT ) || !( tmp & PHY_STATUS_ANEG_ACK_BIT ) ) {
	    	printf( "[ ETH ] Restart Auto_Negatiation process\n" );
		    // Restart Auto_Negatiation process
		    write_phy_reg( PHY_CONTROL, 0x3200 );
	    	continue;
	    }

	    tmp = read_phy_reg( PHY_STATUS_OUT );
	    tcr = 0;
	    if ( tmp & PHY_STATUS_OUT_SPDDET_BIT )
	    	printf( "[ ETH ] Device in 100Mbps mode\n" );
	    else
	    	printf( "[ ETH ] Device in 10Mbps mode\n" );

	    if ( tmp & PHY_STATUS_OUT_DPLXDET_BIT ) {
	    	printf( "[ ETH ] Device in Full Duplex mode\n" );
	    	tcr |= 0x8000;
	    } else {
	    	printf( "[ ETH ] Device in Half Duplex mode\n" );
	    }

	    // Set full/half duplex mode
	    ETH_BANK = 0; ETH_TCR       = tcr;
		ETH_BANK = 1; ETH_CONTROL   = 0x0800; // Auto realese = 1
	    write_phy_reg( PHY_CONTROL, 0x3000 );

		ETH_BANK = 2; ETH_INTERRUPT = 0x00FF;

	    return OK;
	}

	return ERROR;
}


int read_eth_pack( unsigned char * buf, unsigned short * len_p )
{
	unsigned short n, i;
	unsigned short interrupt_status, fifo_ports;

    ETH_BANK = 0; ETH_RCR          = 0x0102;

    ETH_BANK = 2; interrupt_status = ETH_INTERRUPT;
    ETH_BANK = 2; fifo_ports       = ETH_FIFO_PORTS;

    if ( ( interrupt_status & 0x0001 ) && !( fifo_ports & 0x8000 ) )
    {  
        ETH_BANK = 2; ETH_POINTER = 0xE000;
        delay_mcs( 10 );
        ETH_BANK = 2; n = ETH_DATA0;		// status word (unused)
        ETH_BANK = 2; n = ETH_DATA1;		// byte count

        *len_p = n - 10;	// !!! +4 байта CRC ???

        n -= 6;
        n += 2;			// + control word

        for(i = 0; i < n; i += 4) {
        	ETH_BANK = 2;
        	*(unsigned short*)&buf[i] = ETH_DATA0;
        	ETH_BANK = 2;
        	*(unsigned short*)&buf[i + 2] = ETH_DATA1;
        }

        // Извлечение "Control Word" и добавка нечетного байта
        if ( n & 0x03 ) {
        	if(*(unsigned short*)&buf[i - 4] & 0x2000)
        		*len_p += 1;
        } else {
        	if(*(unsigned short*)&buf[i - 2] & 0x2000)
        		*len_p += 1;
        }

//        ETH_BANK = 2; w        = ETH_DATA0;		// control word
        ETH_BANK = 2; ETH_MMU_COMMAND = 0x0080; // Release ethernet packet

        return OK;
    }       

	return ERROR;
}


int write_eth_pack( unsigned char * buf, unsigned short len )
{
	unsigned short i;
	unsigned short interrupt_status, allocate_number;
	unsigned long t1;

	ETH_BANK = 2; ETH_MMU_COMMAND = 0x0020; // Allocate memory for packet

	// Make trailer (packet must have at least 60 bytes)
	/* + As a workaround for strange cases when SOME packets with total length 105 are always interpreted as having length 104
	Cf.:
1)	Good:
0000  f0 de f1 1d b7 e4 f0 f0  f0 f0 f0 f0 08 00 45 00   ........ ......E.
0010  00 5b 00 00 40 00 ff 11  66 21 c0 a8 03 8f c0 a8   .[..@... f!......
0020  90 90 00 01 8e fc 00 47  00 00 32 33 34 35 36 37   .......G ..234567
0030  38 39 3a 31 2f 33 34 35  36 37 38 39 3a 31 2f 33   89:1/345 6789:1/3
0040  34 35 36 37 38 39 3a 31  2f 33 34 35 36 37 38 39   456789:1 /3456789
0050  3a 31 2f 33 34 35 36 37  38 39 3a 31 2f 33 34 35   :1/34567 89:1/345
0060  36 37 38 39 3a 31 2f 36  37                        6789:1/6 7
2)	Bad (almost the same):
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
0060  36 37 38 39 3a 31 2f 36  38                        6789:1/6 8
	The last byte (0x38 or '8') is always lost in receiver (only 104 bytes are captured instead of 105)
	*/
	while(len < 60 || len % 2 == 1) {
		buf[len] = 0x00;
		len++;
	}

    // Ждем, когда память под пакет выделится
    t1 = clock();
    while(1) {
        ETH_BANK = 2; interrupt_status = ETH_INTERRUPT;

        if ( interrupt_status & 0x0008 ) 
        	break;

        if ( dtime( t1 ) >= ETH_ALLOCATE_TO ) {
            printf( "[ ETH ] write_eth_pack(): error of allocation memory for transmitting\n" );
            return ERROR;
        }
    }

    ETH_BANK = 2; allocate_number = ETH_PNR >> 8;
	ETH_BANK = 2; ETH_PNR         = allocate_number;

    ETH_BANK = 2; ETH_POINTER     = 0x4000;

    ETH_BANK = 2; ETH_DATA0       = 0x0000;		// status
    ETH_BANK = 2; ETH_DATA0       = len + 6;	// packet len

    // Signitificantly corrected here, cf. original code in programs_user_manual.pdf (15.01.2007), p. 45
    for(i = 0; i < len; i += 4) {
    	ETH_BANK = 2;
    	*(unsigned int*)&ETH_DATA0 = *(unsigned int*)&buf[i];
    }

    ETH_BANK = 2; ETH_DATA0       = 0x0000;        // 

    ETH_BANK = 0; ETH_TCR        |= 0x0001; // TXENA = 1;
	ETH_BANK = 2; ETH_MMU_COMMAND = 0x00C0; // Queue the packet

	// Ждем завершения передачи
    t1 = clock();
    while(1) {
        ETH_BANK = 2; interrupt_status = ETH_INTERRUPT;

        if ( interrupt_status & 0x0002 ) {
			ETH_BANK = 2; ETH_INTERRUPT |= 0x0002;
        	break;
        }
        ETH_BANK = 0; interrupt_status = ETH_EPH_STATUS;
        if ( interrupt_status & 0x0001 ) 
        	break;

        if ( dtime( t1 ) > ETH_TRANSMIT_TO ) {
            printf( "[ ETH ] write_eth_pack(): timeout of transmitting\n" );
            return ERROR;
        }
    }

//	ETH_BANK = 2; ETH_POINTER     = 0x6000;
//	ETH_BANK = 2; w            = ETH_DATA0;

//	printf( "[ ETH ] Send status word = 0x%04X\n", w );

/*	ETH_BANK = 2; ETH_MMU_COMMAND = 0x00A0; // Releasing the transmitted packet
	// Ждем освобождения памяти пакета
    t1 = clock();
    for ( ; ; )
    {
        ETH_BANK = 2; w = ETH_MMU_COMMAND;

        if ( !( w & 0x0001 ) ) break;

        if ( dtime( t1 ) > ETH_RELEASE_TO )
        {
            printf( "[ ETH ] write_eth_pack(): TO of releasing\n" );
            return ERROR;
        }
    }*/

//    printf( "[ ETH ] Send OK\n" );

    return OK;
}


/* ----------------------------------------------------------------------------
                        Локальные функции модуля
-----------------------------------------------------------------------------*/

