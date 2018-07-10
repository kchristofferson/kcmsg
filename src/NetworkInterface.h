/* NetworkInterface.h
 *
 * Created on: June 25, 2018
 *     Author: Kurt Christofferson
 */

#ifndef NETWORKINTERFACE_H_
#define NETWORKINTERFACE_H_

#include <ifaddrs.h>
#include <cstring>
#include <string>
#include <sys/socket.h>

namespace kcmsg {

class NetworkInterface {
private:
	std::string m_Name;
	std::string m_Protocol;
	std::string m_Address;
	std::string m_Mask;
	std::string m_Destination;
	std::string m_Driver;
	std::string m_DriverVersion;
	std::string m_FirmwareVersion;
	std::string m_ExpansionROMVersion;
	std::string m_BusInformation;

	uint8_t m_Duplex;
	uint8_t m_Port;
	uint8_t m_PhysicalAddress;
	uint8_t m_AutoNegociate;
	uint8_t m_MDIOSupport;
	uint8_t m_EthernetTP_MDIX;
	uint8_t m_EthernetTP_MDIXControl;

	int8_t  m_LinkModeMasksNWords;

	uint32_t m_Flags;
	uint32_t m_Family;
	uint32_t m_Speed;
	uint32_t m_Reserved;
	uint32_t m_LinkModeMasksSupported;
	uint32_t m_LinkModeMasksAdvertising;
	uint32_t m_LinkModeMasksLPAdvertising;

	struct sockaddr_storage m_NetworkAddress;
	struct sockaddr_storage m_NetworkMask;
	struct sockaddr_storage m_NetworkDestination;

	void *m_Data;

	bool m_IsWireless;
	bool m_IsBroadcast;
	bool m_IsBound;
	bool m_DriverSupportsStatistics;
	bool m_DriverSupportsTest;
	bool m_DriverSupportsEEPROMAccess;
	bool m_DriverSupportsRegisterDump;
	bool m_DriverSupportsPrivledgeFlags;

	bool testIsWireless( void );

public:
	NetworkInterface( struct ifaddrs *ifa );
	virtual ~NetworkInterface( void );

	std::string getName( void );
	std::string getProtocol( void );
	std::string getAddress( void );
	std::string getMask( void );
	std::string getDestination( void );
	unsigned int getFlags( void );

	/* returns an unsigned int32 as a value of
	 * Mbps
	 */
	uint32_t getSpeed( void );

	/* return values:
	 * HALF_DUPLEX = 0x00
	 * FULL_DUPLEX = 0x01
	 * DUPLEX_UNKNOWN = 0xFF
	 */
	uint8_t getDuplex( void );

	/* return values:
	 * PORT_TP		0x00
     * PORT_AUI		0x01
     * PORT_MII		0x02
     * PORT_FIBRE	0x03
     * PORT_BNC		0x04
     * PORT_DA		0x05
     * PORT_NONE		0xef
     * PORT_OTHER	0xff
	 */
	uint8_t getPort( void );

	/* return values:
	 * XCVR_INTERNAL		0x00   Note: PHY and MAC are in the same package
	 * XCVR_EXTERNAL		0x01   Note: PHY and MAC are in different packages
	 * XCVR_DUMMY1		0x02
	 * XCVR_DUMMY2		0x03
	 * XCVR_DUMMY3		0x04
	 */
	uint8_t getPhysicalAddress( void );

	/* return values:
	 * AUTONEG_DISABLE		0x00
	 * AUTONEG_ENABLE		0x01
	 */
	uint8_t getAutoNegociate( void );

	/*
	 *
	 */
	uint8_t getMDIOSupport( void );

	/* return values:
	 * ETH_TP_MDI_INVALID	0x00 status: unknown; control: unsupported
	 * ETH_TP_MDI			0x01 status: MDI;     control: force MDI
	 * ETH_TP_MDI_X			0x02 status: MDI-X;   control: force MDI-X
	 * ETH_TP_MDI_AUTO		0x03                  control: auto-select
	 */
	uint8_t getEthernetTP_MDIX( void );

//	uint8_t m_EthernetTP_MDIXControl;
//	int8_t  m_LinkModeMasksNWords;
//	uint32_t m_Reserved;
//	uint32_t m_LinkModeMasksSupported;
//	uint32_t m_LinkModeMasksAdvertising;
//	uint32_t m_LinkModeMasksLPAdvertising;


	struct sockaddr_storage getNetworkAddress( void );
	struct sockaddr_storage getNetworkMask( void );
	struct sockaddr_storage getNetworkDestination( void );

	bool isWireless( void );
	bool isBroadcast( void );
	bool isBound( void );
};

} /* namespace kcmsg */

#endif /* NETWORKINTERFACE_H_*/
