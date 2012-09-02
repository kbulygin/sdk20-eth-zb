/*-----------------------------------------------------------------------------
Проект:     WOCSCN 
Название:   Драйвер IEEE802.15.4 (zigbee )
Файл:       zb.c
Версия:     1.0.5

Описание:   


Автор:      Ковязина Д.Р., Петров Е.В.
Изменения:
-------------------------------------------------------------------------------
N     Дата      Версия      Автор           Описание   
-------------------------------------------------------------------------------
1   05.06.2006  1.0.1      Ковязина Д.Р.,   Базовая версия SDK-2.0
                           Петров Е.В.
2   08.12.2006  1.0.2      Ковязина Д.Р.    Обновление версии для проекта WOCS_CN
3   XX.02.2007  1.0.3      Упрямов Ю.В.     Обновление версии для проекта WOCS_CN
4   27.02.2007  1.0.4      Андрианов О.Б.   Заработал ZigBee (желательна доработка)
5   06.03.2007  1.0.5      Упрямов Ю.В.     Начало доработки ... 
------------------------------------------------------------------------------*/

//#include <common.h>
#include <zb.h>
#include <spi.h>
//#include <interrupt.h>
#include <systimer.h>
#include <stdio_uart0.h>
#include <common.h>
#include <wdt.h>
#include <sw.h>

#define _FIFO		0x00800000	// P2
#define _FIFOP		0x00010000	// P0
#define _CCA		0x00400000	// P2
#define _SFD		0x00400000	// P0

/*#define _FIFO_IS_1	( IOPIN & _FIFO )
#define _FIFOP_IS_1	( IOPIN & _FIFOP )
#define _CCA_IS_1	( IOPIN & _CCA )
#define _SFD_IS_1	( IOPIN & _SFD )*/


#define CC2420_TO   100

unsigned char packet_last_dest, packet_last_type, packet_last_seq_no;
unsigned char packet_settings;
//unsigned short last_dest_addr = ZB_GPS_ADDRESS;

unsigned short zb_self_addr = 0x1234;
unsigned short zb_dest_addr = 0x5678;

unsigned short zb_error = 0; 

#define ZB_WARNING  10
#define ZB_FAULT    20

#define GET_TIMEOUT	3000


unsigned char packet_buf[ PACKET_BUFFER_SIZE ];
PACKET_INFO_T packet_info;


unsigned char CC2420_Read( void );
void CC2420_Init( void );
int CC2420_Wait_For_Valid_RSSI( void );
unsigned char CC2420_Reg_Read( unsigned char addr_reg, unsigned char *reg );
unsigned char CC2420_Reg_Write( unsigned char addr_reg, unsigned char byte2, unsigned char byte3 );
unsigned char CC2420_Strobe( unsigned char addr_reg, unsigned char *reg );
unsigned char CC2420_Start_Reg_Write( unsigned char addr_reg ); 
unsigned char CC2420_Start_Reg_Read( unsigned char addr_reg ); 
unsigned char CC2420_Start_RAM_Write( unsigned short address );
unsigned char CC2420_Start_RAM_Read( unsigned short address );

unsigned short get_zb_pan_id( void );


/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////
unsigned short get_self_zb_addr( void )
{
	return zb_self_addr;
}

/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////
void set_self_zb_addr( unsigned short addr )
{
	zb_self_addr = addr;
    packet_info.src_addr = addr; // added by k
}

/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////
unsigned short get_dest_zb_addr( void )
{
	return zb_dest_addr;
}

/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////
void set_dest_zb_addr( unsigned short addr )
{
	zb_dest_addr = addr;
    packet_info.dest_addr = addr; // added by k
}

/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////
unsigned short get_zb_pan_id( void )
{
	return 0x8855;
}


/*-----------------------------------------------------------------------------
                 Enable radio reception.
------------------------------------------------------------------------------*/
void zb_receive_on()
{
    unsigned char tmp;

    if ( !( packet_settings & PACKET_RECEIVE ) ) {
        CC2420_Strobe( CC2420_SRXON, &tmp );
        CC2420_Flush_RX();
    }
    packet_settings |= PACKET_RECEIVE;
}



/*-----------------------------------------------------------------------------
                  Disable radio reception.
------------------------------------------------------------------------------*/
void zb_receive_off()
{
    unsigned char tmp;

    if ( packet_settings & PACKET_RECEIVE )
        CC2420_Strobe( CC2420_SRFOFF, &tmp );
    packet_settings &= ~PACKET_RECEIVE;
}


//----------------------------------------------------------
// Return 1 if the CC2420 RX FIFO buffer is empty.
//----------------------------------------------------------
unsigned char CC2420_RX_Empty( void )
{
//printf( "\n" );
//    return !FIFOP_IS_1;
    return !( IO0PIN & _FIFOP );
}



//----------------------------------------------------------
// Read a byte from the CC2420 by SPI.
//----------------------------------------------------------
unsigned char CC2420_Read( void )
{
    unsigned char tmp, err;
    
    err = 1;
    tmp = 0;

    err = spi1_exchange( CC2420_SNOP, &tmp );    

    return tmp; 
}



//----------------------------------------------------------
// Return 1 if the CC2420 RX FIFO has overrun.
//----------------------------------------------------------
unsigned char CC2420_RX_Overrun( void )
{
    return ( ( IO0PIN & _FIFOP ) && !( IO2PIN & _FIFO ) );
}



//----------------------------------------------------------
// Flush the CC2420 RXFIFO buffer.
//----------------------------------------------------------
void CC2420_Flush_RX( void )
{
    unsigned char tmp;

    // According to the documentation, this must be done twice.
    CC2420_Strobe( CC2420_SFLUSHRX, &tmp );
    CC2420_Strobe( CC2420_SFLUSHRX, &tmp );
}

//----------------------------------------------------------
// Wait until the CC2420 RSSI value is valid.
//----------------------------------------------------------
int CC2420_Wait_For_Valid_RSSI( void )
{
    unsigned char tmp = 0;
    unsigned long t = clock();

    while ( !( tmp & CC2420_RSSI_VALID ) )
    {
        CC2420_Strobe( CC2420_SNOP, &tmp );

        if( dtime( t ) >= CC2420_TO )
        {
            printf("CC2420_RSSI(): Timeout\n");
            return ERR;
        }
    }

    return OK;
}



/*-----------------------------------------------------------------------------
         Чтение регистра ZigBee
-----------------------------------------------------------------------------*/
unsigned char CC2420_Reg_Read( unsigned char addr_reg, unsigned char *reg )
{
    unsigned char tmp, err;

    err = OK;
    tmp = 0;

    spi1_select( 1 );
    if( (err = spi1_exchange( ((addr_reg & 0x3F) | 0x40), &tmp )) == OK )    //RAM/~Reg(7), R/~W(6), AddrReg(5..0)
    {
        if( (err = spi1_exchange( 0, &tmp )) == OK )
        {
            reg[ 0 ] = tmp;
            if( (err = spi1_exchange( 0, &tmp )) == OK ) 
            	reg[ 1 ] = tmp;
        }
    }
    spi1_select( 0 );
    
    return err;
}


/*-----------------------------------------------------------------------------
        Write to a 16-bit CC2420 register.
-----------------------------------------------------------------------------*/
unsigned char CC2420_Reg_Write( unsigned char addr_reg, unsigned char byte2, unsigned char byte3 )
{
    unsigned char tmp, err;
    
    err = OK;
    tmp = 0;

    spi1_select( 1 );
    if( (err = spi1_exchange( addr_reg & 0x3F, &tmp )) == OK )       //RAM/~Reg(7), R/~W(6), AddrReg(5..0)
    {
        if( (err = spi1_exchange( byte2, &tmp )) == OK ) err = spi1_exchange( byte3, &tmp );
    }
    spi1_select( 0 );

    return err;
}


/*-----------------------------------------------------------------------------
            Trigger a CC2420 command strobe.
-----------------------------------------------------------------------------*/
unsigned char CC2420_Strobe( unsigned char addr_reg, unsigned char *reg )
{
    unsigned char tmp, err;
    
    err = OK;
    tmp = 0;
    
    spi1_select( 1 );
    if( (err = spi1_exchange( (addr_reg & 0x0F), &tmp )) == OK ) *reg = tmp;
    spi1_select( 0 );

    return err;
}


/*-----------------------------------------------------------------------------
            Begin a write sequence to a CC2420 register.
-----------------------------------------------------------------------------*/
unsigned char CC2420_Start_Reg_Write( unsigned char addr_reg ) 
{
    unsigned char tmp = 0;
    return spi1_exchange( addr_reg, &tmp );       //RAM/~Reg(7), R/~W(6), AddrReg(5..0)
}


/*-----------------------------------------------------------------------------
            Begin a read sequence from a CC2420 register.
-----------------------------------------------------------------------------*/
unsigned char CC2420_Start_Reg_Read( unsigned char addr_reg ) 
{
    unsigned char tmp = 0;
    return spi1_exchange( addr_reg | CC2420_REG_READ, &tmp );     //RAM/~Reg(7), R/~W(6), AddrReg(5..0)
}


/*-----------------------------------------------------------------------------
            Start a write sequence to CC2420 RAM.
-----------------------------------------------------------------------------*/
unsigned char CC2420_Start_RAM_Write( unsigned short address )
{
    unsigned char tmp, err;
    
    err = OK;
    tmp = 0;

    err = spi1_exchange( 0x80 | ( address & 0x7F ), &tmp );  //RAM/~Reg(7), AddrReg(6..0)
    if( err == OK ) err = spi1_exchange( ( address >> 1 ) & 0xC0, &tmp );    //AddrReg(7, 6) R/~W(5) 0(4..0)

    return err;
}


/*-----------------------------------------------------------------------------
            Start a write sequence to CC2420 RAM.
-----------------------------------------------------------------------------*/
unsigned char CC2420_Start_RAM_Read( unsigned short address )
{
    unsigned char tmp, err;
    
    err = OK;
    tmp = 0;

    err = spi1_exchange( 0x80 | ( address & 0x7F ), &tmp );  //RAM/~Reg(7), AddrReg(6..0)
    if( err == OK ) err = spi1_exchange( (( address >> 1 ) & 0xC0) | 0x20, &tmp );   //AddrReg(7, 6) R/~W(5) 0(4..0)

    return err;
}


/*-----------------------------------------------------------------------------
         Установка адресов источника и назначения
------------------------------------------------------------------------------*/
void zb_set_addr( unsigned short s_addr, unsigned short d_addr )
{
    packet_info.src_addr = s_addr;
    packet_info.dest_addr = d_addr;
}



/*============================================================================
------------------------------------------------------------------------------
                          Инициализация ZigBee
------------------------------------------------------------------------------
==============================================================================*/

void init_zb( void )
{
    unsigned char tmp = 0;

    printf("\nInit zb\n");

    packet_settings = 0;

	packet_info.src_addr = get_self_zb_addr();
    packet_info.dest_addr = get_dest_zb_addr();
	packet_info.pan_id = get_zb_pan_id();

    // Initialise radio.
    CC2420_Init();
    
    // Initialise sequence number ( 802.15.4, s7.2.1.2, p.114 )
    packet_last_seq_no = 0;

    // Program device address.
    spi1_select( 0 ); 
    spi1_select( 1 );

    CC2420_Start_RAM_Write( CC2420RAM_SHORTADDR );

    // Write upper byte of address. 
    spi1_exchange( packet_info.src_addr >> 8, &tmp );
    // Write lower byte of address.
    spi1_exchange( packet_info.src_addr, &tmp );

    // Cycle between RAM operations.
    spi1_select( 0 );                 
    spi1_select( 1 );

    // Program PAN identifier.
    CC2420_Start_RAM_Write( CC2420RAM_PANID );
    spi1_exchange( packet_info.pan_id >> 8, &tmp );
    spi1_exchange( packet_info.pan_id, &tmp );
    spi1_select( 0 );
    spi1_select( 1 );

    zb_receive_on();
}


/*============================================================================
------------------------------------------------------------------------------
                           Инициализация CC2420
------------------------------------------------------------------------------
==============================================================================*/
void CC2420_Init()
{
    unsigned char tmp[ 2 ];
    unsigned char err = OK;
//    unsigned char *string;
    unsigned long t;

    spi1_select( 1 ); // CC2420 chip select    

    printf("Start CC2420_init...\n");
    

    CC2420_Reg_Write( CC2420_MAIN, 0x00, 0x00 );
    delay_ms( 10 );



//------------------------------------------------------------------------
//          Использование CC2420 внутреннего кварцевого резонатора
//------------------------------------------------------------------------  
    printf("\nCC2420_MAIN... ");
    if( CC2420_Reg_Write( CC2420_MAIN, 0xF8, 0x00 ) == OK ) 
		printf("OK\n");
	else 
		printf("ERROR\n");

    CC2420_Reg_Read( CC2420_MAIN, tmp );
//	printf("CC2420_MAIN = [0xF800] ");
//	WSIO_ASCII(tmp[0]);
//	WSIO_ASCII(tmp[1]);
    printf("CC2420_MAIN = 0x%.2X%.2X [0xF800]\n", tmp[ 0 ], tmp[ 1 ] );


//-------------------------------------------------------------------------
//           Set modem control register 1 - see CC2420 documentation p63.
//           Using hardware support for 802.15.4 acknowledgements, address
//           recognition and CRC.
//-------------------------------------------------------------------------
	printf("\nCC2420_MDMCTRL0... ");
    if( CC2420_Reg_Write( CC2420_MDMCTRL0,
        (  0x02                         // CCA hysteresis = 4dB
        | CC2420_MDMCTRL0_ADR_DECODE    // Hardware address recognition
        ),      
        (  0xC0                         // CCA mode = 3 ( see documentation )
        //| CC2420_MDMCTRL0_AUTOACK       // Hardware auto acknowledgements
        | CC2420_MDMCTRL0_AUTOCRC       // Hardware CRC checking
        | 0x02                          // Preamble length 2 bytes
        ) 
    ) == OK ) 
    {
		printf("OK\n");
    }
    else
    {
		printf("ERROR\n");
    }
    CC2420_Reg_Read( CC2420_MDMCTRL0, tmp );
//  printf("CC2420_MDMCTRL0 = [0x0AF2] ");
//  WSIO_ASCII(tmp[0]);
//  WSIO_ASCII(tmp[1]);
    printf("CC2420_MDMCTRL0 = 0x%.2X%.2X [0x0AE2]\n", tmp[ 0 ], tmp[ 1 ]);
    

//------------------------------------------------------------------------------
//           Set modem control register 2 - see CC2420 documentation p64.
//------------------------------------------------------------------------------
	printf("\nCC2420_MDMCTRL1...");
    if( CC2420_Reg_Write( CC2420_MDMCTRL1,
        0x05,   // Correlation threshold = 20 ( specified in documentation )
        0x00    // Defaults.
    ) == OK ) 
    {
		printf("OK\n");
    }
    else 
    {
		printf("ERROR\n");
    }

    CC2420_Reg_Read( CC2420_MDMCTRL1, tmp );
//  printf("CC2420_MDMCTRL1 = [0x0500] ");
//  WSIO_ASCII(tmp[0]);
//  WSIO_ASCII(tmp[1]);
    printf("CC2420_MDMCTRL1 = 0x%.2X%.2X [0x0500]\n", tmp[ 0 ], tmp[ 1 ]);
    

//-----------------------------------------------------------------------------------
//       Set CCA threshold - see CC2420 documentation p.64. Default was found to
//       be too low for our environment, probably due to interference from
//       802.11b networks.
//-----------------------------------------------------------------------------------
    printf("\nCC2420_RSSI...");
    if( CC2420_Reg_Write( CC2420_RSSI, 0xF0, 0x00 ) == OK ) 
    {
		printf("OK\n");
    }
    else 
    {
		printf("ERROR\n");
    }

    CC2420_Reg_Read( CC2420_RSSI, tmp );
//  printf("CC2420_RSSI = [0xF000] ");
//  WSIO_ASCII(tmp[0]);
//  WSIO_ASCII(tmp[1]);
    printf("CC2420_RSSI = 0x%.2X%.2X [0xF000]\n", tmp[ 0 ], tmp[ 1 ]);
    

//------------------------------------------------------------------------------------
//           Set maximum FIFOP threshold. We don't want to deal with a packet until
//           we have all of it. See CC2420 documentation p.71.
//------------------------------------------------------------------------------------
	printf("\nCC2420_IOCFG0...");
    if( CC2420_Reg_Write( CC2420_IOCFG0, 0x00, 0x7F ) == OK ) 
    {
		printf("OK\n");
    }
    else 
    {
		printf("ERROR\n");
    }

    CC2420_Reg_Read( CC2420_IOCFG0, tmp );
//  printf("CC2420_IOCFG0 =  [0x007F] ");
//  WSIO_ASCII(tmp[0]);
//  WSIO_ASCII(tmp[1]);
    printf("CC2420_IOCFG0 = 0x%.2X%.2X [0x007F]\n", tmp[ 0 ], tmp[ 1 ]);
    

//----------------------------------------------------------------------------------
//           Disable security features to maximise use of RXFIFO. See CC2420
//           documentation p.69.
//----------------------------------------------------------------------------------
	printf("\nCC2420_SECCTRL0...");
    if( CC2420_Reg_Write( CC2420_SECCTRL0, 0x01, 0xC4 ) == OK ) 
    {
		printf("OK\n");
    }
    else 
    {
		printf("ERROR\n");
    }

    CC2420_Reg_Read( CC2420_SECCTRL0, tmp );
//  printf("CC2420_SECCTRL0 =  [0x01C4] ");
//  WSIO_ASCII(tmp[0]);
//  WSIO_ASCII(tmp[1]);
    printf("CC2420_SECCTRL0 = 0x%.2X%.2X [0x01C4]\n", tmp[ 0 ], tmp[ 1 ]);
    

//------------------------------------------------------------------------------------
//                           Start up oscillators & PLL
//------------------------------------------------------------------------------------
	printf("\nCC2420_SXOSCON...");
    if( CC2420_Strobe( CC2420_SXOSCON, &tmp[ 0 ] ) == OK ) 
    {
		printf("OK\n");
    }
    else 
    {
		printf("ERROR\n");
    }

    // CC2420_Cycle();
    CC2420_Strobe( CC2420_SNOP, tmp );
//  printf("Status1 = ");
//  WSIO_ASCII(tmp[0]);



//---------------------------------------------------------------------------------------
//                          Wait until oscillator is steady
//---------------------------------------------------------------------------------------
    tmp[ 0 ] = tmp[ 1 ] = 0;

	printf("\nStart...");
    t = clock();
    while ( (err = CC2420_Reg_Read( CC2420_FSMSTATE, tmp )) == OK )     // Read state register
    {
        if( dtime( t ) >= CC2420_TO )
        {
            printf("CC2420_Init(): Timeout\n");
            break;
        }

        if( tmp[ 1 ] == 0x01 )
        {
            printf("FSMSTATE = 0x%.2X%.2X\n", tmp[ 0 ], tmp[ 1 ]);
            break;                                  // Loop until state = 0x01
        }
        //CC2420_Strobe( CC2420_SNOP, &tmp[ 0 ] );
//        printf("tmp0 = %d, tmp1 = %d\n", tmp[ 0 ], tmp[ 1 ]);
//      string = "\ntmp0 = ";
//      printf(string);
//      WSIO_ASCII(tmp[0]);
//      string = "  tmp1 = ";
//      printf(string);
//      WSIO_ASCII(tmp[1]);
    }
    if( err == OK ) 
    {
		printf("OK\n");
    }
    else 
    {
		printf("ERROR\n");
    }

//  string = "FSMSTATE = [0x0001] ";
//  printf(string);
//  WSIO_ASCII(tmp[0]);
//  WSIO_ASCII(tmp[1]);



    printf("\nCC2420_STXCAL...");
    if( CC2420_Strobe( CC2420_STXCAL, &tmp[ 0 ] ) == OK ) 
    {
		printf("OK\n");
    }
    else 
    {
		printf("ERROR\n");
    }
    
    CC2420_Strobe( CC2420_SNOP, tmp ); 
//  WSIO_ASCII(tmp[0]);
    printf("Status2 = 0x%.4X\n", tmp[ 0 ]);

    printf("\nFinish CC2420_init...\n");  
}
 




/*============================================================================
------------------------------------------------------------------------------
                        Передача пакета
------------------------------------------------------------------------------
==============================================================================*/
int zb_send_packet(unsigned char * packet_buf, unsigned short len)
{

    unsigned char i = 0;
    unsigned char tmp = 0;
//    unsigned char tmp_fsm[ 2 ];
    unsigned long t;

#ifdef DEBUG_OUT
//    printf("zb_send_packet(): ");
//    printf_packet( packet_buf, len );
#endif
    t = clock();

    while ( ( IO0PIN & _SFD ) || ( IO0PIN & _FIFOP) )
    {
        if( dtime( t ) >= CC2420_TO )
        {
        	printf( "IO2PIN = 0x%08X\n", IO2PIN );
            printf("zb_send_packet(): Timeout SFD_IS_1 || FIFOP\n");
            init_zb();
            return ERR;
        }
    }

    // Record the details of the current transmission. Only one transmission
    // can be in progress at a time, and correspondingly only one
    // acknowledgement is expected since acknowledgements are generated in
    // the receiver hardware in bounded time, waited for at the end of this
    // function.
    packet_last_dest = packet_info.dest_addr; // unused
    packet_last_type = packet_info.type; // unused
    packet_last_seq_no++;

    // We need the receiver on to catch acknowledgements, and to check for
    // a clear channel before transmission.

    if ( !( packet_settings & PACKET_RECEIVE ) )
    {
        CC2420_Strobe( CC2420_SRXON, &tmp );
    }

    // Wait for the RSSI value to become valid, for CCA checks.
    if( OK != CC2420_Wait_For_Valid_RSSI() )
    {
        printf("zb_send_packet(): Invalid RSSI\n");
        init_zb();
        return ERR;
    }


    CC2420_Strobe( CC2420_STXONCCA, &tmp );

    t = clock();

    while ( 1 )
    {
        CC2420_Strobe( CC2420_SNOP, &tmp );
        if( tmp & CC2420_TX_ACTIVE )
            break;
        delay_mcs(100);

        if( dtime( t ) >= CC2420_TO )
        {
            printf("zb_send_packet(): CC2420_SNOP\n");
	        init_zb();
            return ERR;
        }

    }

    disable_interrupts();
    

    // We write out our packet straight to the TX buffer.
    spi1_select( 1 );

    CC2420_Start_Reg_Write( CC2420_TXFIFO );        // Write to TX FIFO

    // Length of packet to be sent - not part of final transmission.
    spi1_exchange( (9 + 1 + len + 2), &tmp ); // 9 bytes 802.15.4 header + our packet
                                                            // type + payload + 2 bytes checksum
    // MAC layer packet header
    spi1_exchange( 0x61, &tmp );                         // Data frame, security off, frame pending off,
                                                        // acknowledge request on, intra-PAN.
    spi1_exchange( 0x88, &tmp );                         // 16-bit source and destination address modes.
    spi1_exchange( packet_last_seq_no, &tmp );           // MAC sequence number
    spi1_exchange( packet_info.pan_id >> 8, &tmp );      // Destination PAN identifer, MSB.
    spi1_exchange( packet_info.pan_id, &tmp );           // Destination PAN identifer, LSB.
    spi1_exchange( packet_info.dest_addr >> 8, &tmp );   // Destination address, MSB.
    spi1_exchange( packet_info.dest_addr, &tmp );        // Destination address, LSB.
    spi1_exchange( packet_info.src_addr >> 8, &tmp );    // Source address, MSB.
    spi1_exchange( packet_info.src_addr, &tmp );         // Source address, LSB.
    
    // Our packet type header.
    spi1_exchange( packet_info.type, &tmp );

    // Write out payload. Note that data_base can be a null pointer and len
    // can be zero.
    for ( i = 0; i < len; i++ ) {
        spi1_exchange( packet_buf[ i ], &tmp );
    }

    enable_interrupts();

   // Wait for the SFD pin to go low when transmission finishes.
    t = clock();

    while ( !( IO0PIN & _SFD ) ) {
        if( dtime( t ) >= 3 ) break;
    }

   // Wait for the SFD pin to go low when transmission finishes.
    t = clock();

    while ( IO0PIN & _SFD ) {
        if( dtime( t ) >= CC2420_TO )
        {
            printf("zb_send_packet(): **Error! Timeout SFD\n");
	        init_zb();
            return ERR;
        }
    }

    
    // We're waiting on an acknowledgement packet now, which should come
    // within a bounded time period.


    spi1_select( 0 );

    //zb_receive_on();

    //delay_ms( 100 );

    return OK;
}


/*============================================================================
------------------------------------------------------------------------------
                    Прием пакета
------------------------------------------------------------------------------
==============================================================================*/
int zb_receive_packet(unsigned char * packet_buf, unsigned short * len_p )
{
    unsigned char pktlen;
    unsigned char byte;
//    unsigned char *buf = &packet_buf[ 0 ];
    unsigned char i;
    unsigned char ack = 0;

    unsigned short mac, src, dest, pan;

//printf("\nEnter zb_receive_packet\n" );

    *len_p = 0;

    spi1_select( 1 );                 // CC2420_Cycle() 
                                                 
    CC2420_Start_Reg_Read( CC2420_RXFIFO );         // ok
                                               
    // First byte is packet length.
    //CC2420_Strobe( CC2420_SNOP, &pktlen );        
    pktlen = CC2420_Read();

    // Read through the packet. We only copy bytes up to the buffer length, but
    // we must read all the way to the end to get the RSSI value and clear the
    // data from the FIFO.


    byte = CC2420_Read(); // [0] Frame control Field    

    if ( ( byte & ZIGBEE_FRAME_TYPE_MASK ) == ZIGBEE_FRAME_TYPE_ACK ) 
    {
        ack = 1;
        // Acknowledgement packet. We need to know this later, see below.
        printf("zb_receive_packet(): **Error! reject ack\n" );
        goto error;
    }

    byte = CC2420_Read(); // [1] Frame control Field    

    byte = CC2420_Read();   mac   = byte;                // [2] 
    byte = CC2420_Read();   pan   = byte; pan <<= 8;     // [3] 
    byte = CC2420_Read();   pan  |= byte;                // [4] 
    byte = CC2420_Read();   dest  = byte; dest <<= 8;    // [5] 
    byte = CC2420_Read();   dest |= byte;                // [6] 
    byte = CC2420_Read();   src   = byte; src <<= 8;     // [7] 
    byte = CC2420_Read();   src  |= byte;                // [8] 


//printf("\ndest = %.5d\n", dest );
//printf("\nsrc  = %.5d\n", src  );

    if( dest != packet_info.src_addr )
    {
        printf("zb_receive_packet(): **Error! incorrect dest_addr, %04X\n", dest );
        goto error; 
    }

    if( src  != packet_info.dest_addr )
    {
        printf("zb_receive_packet(): **Error! incorrect src_addr, %04X\n", src );
        goto error; 
    }

    byte = CC2420_Read(); // [9] 

    if( !ack ) packet_info.type = byte;


    if( (pktlen >= 12) && ( pktlen <= ( PACKET_BUFFER_SIZE + 12 ) ) && (ack == 0) ) 
    {
        pktlen -= 12;

        for ( i = 0; i < pktlen; i++ )
            packet_buf[ i ] = CC2420_Read(); 
        
        byte = CC2420_Read(); 
        byte = CC2420_Read();     

        *len_p = pktlen;
    }
    else
        goto error;
        
    // End read operation.
    spi1_select( 0 );

    return OK;

error:
    spi1_select( 0 );
    CC2420_Flush_RX();
    printf("zb_receive_packet(): reject packet\n");
    return ERR;
}

unsigned char CC2420_RX_Overflow( void )
{
    return ( ( ( IO2PIN & _FIFO ) == 0 ) && ( ( IO0PIN & _FIFOP ) == 1 ) );
}


void inc_zb_error( void )
{
/*
    zb_error++;

    printf("++zb_error(%d)\n", zb_error );

    if( zb_error <    ZB_WARNING     )             return;
    if( zb_error ==   ZB_WARNING     ) init_zb();  return;

    if( zb_error >= ZB_FAULT  )        restart_mcu();  
*/
}

void dec_zb_error( void )
{
/*


    if( zb_error > 0 ) 
    {
        zb_error--;
        printf("--zb_error(%d)\n", zb_error );
    }
*/
}


int zb_get_packet(unsigned char * packet_buf, unsigned short * packet_len_p, unsigned long timeout) {
    unsigned long t = clock();
	
	while(1) {
		if(!CC2420_RX_Empty()) {
			if(CC2420_RX_Overrun()) {
				// Buffer overrun - flush it.
				CC2420_Flush_RX();
				printf( "zb_get_packet(): ZB Overrun\n" );
				break;
			} else {
				// At least one packet is ready in the buffer.
				if(zb_receive_packet(packet_buf, packet_len_p) != OK) {
					printf("zb_get_packet(): packet is not received");
					break;
				} else {
					return OK;
				}
			}
		}

		if(CC2420_RX_Overflow()) {
			printf("zb_get_packet(): ZB Overflow\n");
			CC2420_Flush_RX();
			break;
		}

		if(dtime(t) >= timeout) {
			if(timeout != 0) {
				printf("zb_get_packet(): timeout (%d ms)\n", timeout);
			}
			break;
		}
	}
	return ERROR;
}
