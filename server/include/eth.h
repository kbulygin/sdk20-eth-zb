/**----------------------------------------------------------------------------
Проект:     SDK-2.0 
Название:   Драйвер контроллера ethernet LAN91C111
Файл:       eth.h
Версия:     1.0.1

Описание:   


Автор:      Ключев А.О.
Изменения:
-------------------------------------------------------------------------------
N     Дата      Версия      Автор           Описание   
-------------------------------------------------------------------------------
1   22.02.05    1.0.1   Ключев А.О.      Базовая версия
------------------------------------------------------------------------------*/

#ifndef __ETH_H
#define __ETH_H

#include <settings.h>


extern volatile unsigned short *eth_reg;

#define BANK0   0
#define BANK1   1
#define BANK2   2
#define BANK3   3

//__IO_REG16(ETH_BANK, 0x8200000E,__READ_WRITE);


#define ETH_REG0		 (*((volatile unsigned short *) 0x81000000))
#define ETH_REG1         (*((volatile unsigned short *) 0x81000002))
#define ETH_REG2         (*((volatile unsigned short *) 0x81000004))
#define ETH_REG3         (*((volatile unsigned short *) 0x81000006))
#define ETH_REG4         (*((volatile unsigned short *) 0x81000008))
#define ETH_REG5         (*((volatile unsigned short *) 0x8100000A))
#define ETH_REG6         (*((volatile unsigned short *) 0x8100000C))
#define ETH_REG7         (*((volatile unsigned short *) 0x8100000E))
/*
__IO_REG16(ETH_REG0     , 0x82000000,__READ_WRITE);
__IO_REG16(ETH_REG1     , 0x82000002,__READ_WRITE);
__IO_REG16(ETH_REG2     , 0x82000004,__READ_WRITE);
__IO_REG16(ETH_REG3     , 0x82000006,__READ_WRITE);
__IO_REG16(ETH_REG4     , 0x82000008,__READ_WRITE);
__IO_REG16(ETH_REG5     , 0x8200000A,__READ_WRITE);
__IO_REG16(ETH_REG6     , 0x8200000C,__READ_WRITE);
__IO_REG16(ETH_REG7     , 0x8200000E,__READ_WRITE);
*/

// BANK0        
#define ETH_TCR         ETH_REG0
#define ETH_EPH_STATUS  ETH_REG1
#define ETH_RCR         ETH_REG2
#define ETH_COUNTER     ETH_REG3
#define ETH_MIR         ETH_REG4
#define ETH_RPCR        ETH_REG5
#define ETH_BANK        ETH_REG7
// BANK1
#define ETH_CONFIG      ETH_REG0
#define ETH_BASE        ETH_REG1
#define ETH_IA_01       ETH_REG2
#define ETH_IA_23       ETH_REG3
#define ETH_IA_45       ETH_REG4
#define ETH_GENERAL     ETH_REG5
#define ETH_CONTROL     ETH_REG6
// BANK2                       
#define ETH_MMU_COMMAND ETH_REG0
#define ETH_PNR         ETH_REG1
#define ETH_PNR_2       (*((volatile unsigned char *) 0x81000002))
#define ETH_PNR_3       (*((volatile unsigned char *) 0x81000003))
#define ETH_FIFO_PORTS  ETH_REG2
#define ETH_POINTER     ETH_REG3
#define ETH_DATA0       ETH_REG4
#define ETH_DATA1       ETH_REG5
#define ETH_INTERRUPT   ETH_REG6
#define ETH_INTERRUPT_C (*((volatile unsigned char *) 0x8100000C))
#define ETH_INTERRUPT_D (*((volatile unsigned char *) 0x8100000D))
// BANK3                       
#define ETH_MT_01       ETH_REG0
#define ETH_MT_23       ETH_REG1
#define ETH_MT_45       ETH_REG2
#define ETH_MT_67       ETH_REG3
#define ETH_MGMT        ETH_REG4
#define ETH_REVISION    ETH_REG5
#define ETH_ERCV        ETH_REG6


/*

__IO_REG16(ETH_TCR         , 0x82000000,__READ_WRITE);
__IO_REG16(ETH_EPH_STATUS  , 0x82000002,__READ_WRITE);
__IO_REG16(ETH_RCR         , 0x82000004,__READ_WRITE);
__IO_REG16(ETH_COUNTER     , 0x82000006,__READ_WRITE);
__IO_REG16(ETH_MIR         , 0x82000008,__READ_WRITE);
__IO_REG16(ETH_RPCR        , 0x8200000A,__READ_WRITE);
           
__IO_REG16(ETH_CONFIG      , 0x82000000,__READ_WRITE);
__IO_REG16(ETH_BASE        , 0x82000002,__READ_WRITE);
__IO_REG16(ETH_IA_01       , 0x82000004,__READ_WRITE);
__IO_REG16(ETH_IA_23       , 0x82000006,__READ_WRITE);
__IO_REG16(ETH_IA_45       , 0x82000008,__READ_WRITE);
__IO_REG16(ETH_GENERAL     , 0x8200000A,__READ_WRITE);
__IO_REG16(ETH_CONTROL     , 0x8200000C,__READ_WRITE);
           
__IO_REG16(ETH_MMU_COMMAND , 0x82000000,__READ_WRITE);
__IO_REG16(ETH_PNR         , 0x82000002,__READ_WRITE);
__IO_REG16(ETH_FIFO_PORTS  , 0x82000004,__READ_WRITE);
__IO_REG16(ETH_POINTER     , 0x82000006,__READ_WRITE);
__IO_REG16(ETH_DATA0       , 0x82000008,__READ_WRITE);
__IO_REG16(ETH_DATA1       , 0x8200000A,__READ_WRITE);
__IO_REG16(ETH_INTERRUPT   , 0x8200000C,__READ_WRITE);
           
__IO_REG16(ETH_MT_01       , 0x82000000,__READ_WRITE);
__IO_REG16(ETH_MT_23       , 0x82000002,__READ_WRITE);
__IO_REG16(ETH_MT_45       , 0x82000004,__READ_WRITE);
__IO_REG16(ETH_MT_67       , 0x82000006,__READ_WRITE);
__IO_REG16(ETH_MGMT        , 0x82000008,__READ_WRITE);
__IO_REG16(ETH_REVISION    , 0x8200000A,__READ_WRITE);
__IO_REG16(ETH_ERCV        , 0x8200000C,__READ_WRITE);
*/

/*
#define ETH_TCR( x )          ETH_BANK = BANK0; __ETH_BANK0_TCR        = x;      
#define ETH_EPH_STATUS( x )   ETH_BANK = BANK0; __ETH_BANK0_EPH_STATUS = x;      
#define ETH_RCR( x )          ETH_BANK = BANK0; __ETH_BANK0_RCR        = x;
#define ETH_COUNTER( x )      ETH_BANK = BANK0; __ETH_BANK0_COUNTER    = x;
#define ETH_MIR( x )          ETH_BANK = BANK0; __ETH_BANK0_MIR        = x;
#define ETH_RPCR( x )         ETH_BANK = BANK0; __ETH_BANK0_RPCR       = x;
                    
#define ETH_CONFIG( x )       ETH_BANK = BANK1; __ETH_BANK1_CONFIG     = x;
#define ETH_BASE( x )         ETH_BANK = BANK1; __ETH_BANK1_BASE       = x;
#define ETH_IA_01( x )        ETH_BANK = BANK1; __ETH_BANK1_IA_01      = x;
#define ETH_IA_23( x )        ETH_BANK = BANK1; __ETH_BANK1_IA_23      = x;
#define ETH_IA_45( x )        ETH_BANK = BANK1; __ETH_BANK1_IA_45      = x;
#define ETH_GENERAL( x )      ETH_BANK = BANK1; __ETH_BANK1_GENERAL    = x;
#define ETH_CONTROL( x )      ETH_BANK = BANK1; __ETH_BANK1_CONTROL    = x;
              
#define ETH_MMU_COMMAN( x )   ETH_BANK = BANK2; __ETH_BANK2_MMU_COMMAND= x;
#define ETH_PNR( x )          ETH_BANK = BANK2; __ETH_BANK2_PNR        = x;
#define ETH_FIFO_PORTS( x )   ETH_BANK = BANK2; __ETH_BANK2_FIFO_PORTS = x;
#define ETH_POINTER( x )      ETH_BANK = BANK2; __ETH_BANK2_POINTER    = x;
#define ETH_DATA0( x )        ETH_BANK = BANK2; __ETH_BANK2_DATA0      = x;
#define ETH_DATA1( x )        ETH_BANK = BANK2; __ETH_BANK2_DATA1      = x;
#define ETH_INTERRUPT( x )    ETH_BANK = BANK2; __ETH_BANK2_INTERRUPT  = x;
                                                                       
#define ETH_MT_01( x )        ETH_BANK = BANK3; __ETH_BANK3_MT_01      = x;
#define ETH_MT_23( x )        ETH_BANK = BANK3; __ETH_BANK3_MT_23      = x;
#define ETH_MT_45( x )        ETH_BANK = BANK3; __ETH_BANK3_MT_45      = x;
#define ETH_MT_67( x )        ETH_BANK = BANK3; __ETH_BANK3_MT_67      = x;
#define ETH_MGMT( x )         ETH_BANK = BANK3; __ETH_BANK3_MGMT       = x;
#define ETH_REVISION( x )     ETH_BANK = BANK3; __ETH_BANK3_REVISION   = x;
#define ETH_ERCV( x )         ETH_BANK = BANK3; __ETH_BANK3_ERCV       = x;
*/

/*
// Банк 0

#define TCR         0
#define EPH_STATUS  1
#define RCR         2
#define COUNTER     3
#define MIR         4
#define RPCR        5
//#define RESERVED  6

#define _BANK       7 


// Банк 1

#define CONFIG      0
#define BASE        1
#define IA_01       2
#define IA_23       3
#define IA_45       4
#define GENERAL     5
#define CONTROL     6


// Банк 2

#define MMU_COMMAND 0
#define PNR         1
#define FIFO_PORTS  2
#define POINTER     3
#define DATA0       4
#define DATA1       5
#define INTERRUPT   6


// Банк 3

#define MT_01       0
#define MT_23       1
#define MT_45       2
#define MT_67       3
#define MGMT        4
#define REVISION    5
#define ERCV        6



#define BANK( bank )   eth_reg[ _BANK ] = bank;

*/




extern unsigned long init_eth( void );
extern int read_eth_pack( unsigned char * buf, unsigned short * len );
extern int write_eth_pack( unsigned char * buf, unsigned short len );


#endif // __ETH_H
