/**----------------------------------------------------------------------------
������:     SDK-2.0 
��������:   ������� ������� � PHY ��� ����������� LAN91C111
����:       eth_mii_phy.h
������:     1.0.1
------------------------------------------------------------------------------*/

#ifndef __ETH_MII_PHY
#define __ETH_MII_PHY

extern void init_eth_phy( void );
extern void write_phy_reg( unsigned char addr, unsigned short data );
extern unsigned short read_phy_reg( unsigned char addr );

#define PHY_CONTROL     0
#define PHY_STATUS      1
#define PHY_ID1         2
#define PHY_ID2         3
#define PHY_AUTONEG_ADV 4
#define PHY_AUTONEG_REM 5
#define PHY_CONF1       16
#define PHY_CONF2       17
#define PHY_STATUS_OUT  18
#define PHY_MASK        19


#endif // __ETH_MII_PHY
