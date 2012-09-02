/**----------------------------------------------------------------------------
Проект:     WOCSCN 
Название:   Драйвер ZigBee
Файл:       zb.h
Версия:     1.0.1

Описание:   


Автор:      Ковязина Д.Р., Петров Е.В.
Изменения:
-------------------------------------------------------------------------------
N     Дата      Версия      Автор           	Описание   
-------------------------------------------------------------------------------
1   08.12.2006	1.0.1      Ковязина Д.Р.	Базовая версия
------------------------------------------------------------------------------*/

#ifndef __ZB_H
#define __ZB_H

#include <lpc2292.h>

#define ZB_SELF_ADDRESS					1000
#define ZB_GPS_ADDRESS					2000
#define ZB_CAMERA_ADDRESS				3000

/*
#define FIFOP           				P_16 
#define FIFO            				P_23 
#define SFD             				P_18 
#define CCA             				P_22 

// Pin status
#define FIFO_IS_1       				(IOPIN & FIFO)
#define CCA_IS_1        				(IOPIN & CCA)
#define FIFOP_IS_1      				(IOPIN & FIFOP)
#define SFD_IS_1        				(IOPIN & SFD)
*/

#define	ZIGBEE_FRAME_TYPE_ACK			0x02
#define	ZIGBEE_FRAME_TYPE_MASK			0x07

#define PACKET_POWER_MASK				0x1F
#define PACKET_RECEIVE					0x20

#define PACKET_BUFFER_SIZE				64

#define ZB_SEND_PACKET_TRY_COUNT		3

//            timeout in msec										  				
#define RSSI_TIMEOUT					10
#define STXONCCA_TIMEOUT	   			200


typedef struct {
	unsigned short src_addr;
	unsigned short dest_addr;
	unsigned short pan_id;
	unsigned char type;
	unsigned char length;
} PACKET_INFO_T;



//Конфигурационные регистры CC2420
#define CC2420_SNOP						0x00
#define CC2420_SXOSCON					0x01
#define CC2420_STXCAL					0x02
#define CC2420_SRXON					0x03
#define CC2420_STXON					0x04
#define CC2420_STXONCCA					0x05
#define CC2420_SRFOFF					0x06
#define CC2420_SXOSCOFF					0x07
#define CC2420_SFLUSHRX					0x08
#define CC2420_SFLUSHTX					0x09
#define CC2420_SACK						0x0A
#define CC2420_SACKPEND					0x0B
#define CC2420_SRXDEC					0x0C
#define CC2420_STXENC					0x0D
#define CC2420_SAES						0x0E
#define CC2420_MAIN						0x10
#define CC2420_MDMCTRL0					0x11
#define CC2420_MDMCTRL1					0x12
#define CC2420_RSSI						0x13
#define CC2420_SYNCWORD					0x14
#define CC2420_TXCTRL					0x15
#define CC2420_RXCTRL0					0x16
#define CC2420_RXCTRL1					0x17
#define CC2420_FSCTRL					0x18
#define CC2420_SECCTRL0					0x19
#define CC2420_SECCTRL1					0x1A
#define CC2420_BATTMON					0x1B
#define CC2420_IOCFG0					0x1C
#define CC2420_IOCFG1					0x1D
#define CC2420_MANFIDL					0x1E
#define CC2420_MANFIDH					0x1F
#define CC2420_FSMTC					0x20
#define CC2420_MANAND					0x21
#define CC2420_MANOR					0x22
#define CC2420_AGCCTRL					0x23
#define CC2420_AGCTST0					0x24
#define CC2420_AGCTST1					0x25
#define CC2420_AGCTST2					0x26
#define CC2420_FSTST0					0x27
#define CC2420_FSTST1					0x28
#define CC2420_FSTST2					0x29
#define CC2420_FSTST3					0x2A
#define CC2420_RXBPFTST					0x2B
#define CC2420_FSMSTATE					0x2C
#define CC2420_ADCTST					0x2D
#define CC2420_DACTST					0x2E
#define CC2420_TOPTST					0x2F
#define CC2420_RESERVED					0x30
#define CC2420_TXFIFO					0x3E
#define CC2420_RXFIFO					0x3F
#define CC2420_RAM_SIZE					368
#define CC2420_FIFO_SIZE				128
#define CC2420RAM_TXFIFO				0x000
#define CC2420RAM_RXFIFO				0x080
#define CC2420RAM_KEY0					0x100
#define CC2420RAM_RXNONCE				0x110
#define CC2420RAM_SABUF					0x120
#define CC2420RAM_KEY1					0x130
#define CC2420RAM_TXNONCE				0x140
#define CC2420RAM_CBCSTATE				0x150
#define CC2420RAM_IEEEADDR				0x160
#define CC2420RAM_PANID					0x168
#define CC2420RAM_SHORTADDR				0x16A

#define CC2420_XOSC16M_STABLE			64
#define CC2420_TX_UNDERFLOW				32
#define CC2420_ENC_BUSY					16
#define CC2420_TX_ACTIVE				8
#define CC2420_LOCK						4
#define CC2420_RSSI_VALID				2
#define CC2420_MODE_TX					0
#define CC2420_MODE_RX					1
#define CC2420_MODE_PD					2
#define CC2420_MODE_SLEEP				3
#define	CC2420_MDMCTRL0_AUTOACK			0x10
#define	CC2420_MDMCTRL0_AUTOCRC			0x20
#define	CC2420_MDMCTRL0_ADR_DECODE		0x08
#define	CC2420_REG_READ					0x40

extern unsigned char packet_buf[ PACKET_BUFFER_SIZE ];
extern PACKET_INFO_T packet_info;

extern unsigned short last_dest_addr;

//extern void init_zb( unsigned short self_addr, unsigned short pan_id );
extern void init_zb( void );
//extern void init_zb( unsigned short self_addr, unsigned short pan_id );
extern int zb_receive_packet(unsigned char * packet_buf, unsigned short * len );
extern int zb_get_packet(unsigned char * packet_buf, unsigned short * len, unsigned long timeout);
//extern unsigned char zb_receive_packet( unsigned char* packet_buf, unsigned short* len );
extern int zb_send_packet(unsigned char * packet_buf, unsigned short len);
//extern unsigned char zb_send_packet( unsigned char* packet_buf, unsigned short len );
extern unsigned char zb_send_packet_ex( unsigned char* packet_buf, unsigned short len );

extern unsigned short get_self_zb_addr( void );
extern void set_self_zb_addr( unsigned short addr );
extern unsigned short get_dest_zb_addr( void );
extern void set_dest_zb_addr( unsigned short addr );

extern void zb_set_dest_addr( unsigned short d_addr );
extern void zb_receive_on( void );
extern void zb_receive_off( void );
extern unsigned char CC2420_Reg_Read( unsigned char addr_reg, unsigned char *reg );
extern void CC2420_Init();

extern unsigned char CC2420_RX_Empty( void );
extern unsigned char CC2420_RX_Overrun( void );
extern void CC2420_Flush_RX( void );

//void inc_zb_error( void );
//void dec_zb_error( void );
unsigned char CC2420_RX_Overflow( void );

#endif // __ZB_H
