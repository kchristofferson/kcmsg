/*
 *  NetworkInterface.cpp
 *  \
 *  Created on : June 25, 2018
 *       Author: Kurt Christofferson
 */

#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include <cstring>
#include <cerrno>
#include <iostream>


#include "NetworkInterface.h"

namespace kcmsg {

NetworkInterface::NetworkInterface( struct ifaddrs *ifa )
{
	int fd, res;
	struct ifreq ifr;
	struct ethtool_link_settings els;

	m_Data = nullptr;
	m_IsBound = m_IsWireless = m_IsBroadcast = false;
	m_DriverSupportsStatistics = m_DriverSupportsTest = false;
	m_DriverSupportsEEPROMAccess = m_DriverSupportsRegisterDump = false;
	m_DriverSupportsPrivledgeFlags = false;

	m_Duplex = 0xFF;					// Unknown
	m_Port = 0x00;						// Internal
	m_PhysicalAddress = 0x00;			// TODO: Need to understand
	m_AutoNegociate = 0X00;				// Disabled
	m_MDIOSupport = 0X00;				// TODO: Need to understand
	m_EthernetTP_MDIX = 0X00;			// TODO: Need to understand
	m_EthernetTP_MDIXControl = 0X00;	// Status unknown : Control unsupported
	m_Family = 0x00;					// Unknown
	m_Speed = 0x00;						// Unknown

	m_LinkModeMasksNWords = 0x00;		//TODO: Need to understand

	m_Reserved = m_LinkModeMasksSupported = 0x00;
	m_LinkModeMasksAdvertising = m_LinkModeMasksLPAdvertising = 0x00;

	size_t st = sizeof( sockaddr_storage );
	char addr[st];
	std::memset( &addr, 0, st );

	std::memset( &m_NetworkAddress, 0, sizeof( sockaddr_storage ) );
	std::memset( &m_NetworkMask, 0, sizeof( sockaddr_storage ) );
	std::memset( &m_NetworkDestination, 0, sizeof( sockaddr_storage ) );

	m_Name = ifa->ifa_name;
	m_Flags = ifa->ifa_flags;

	/* if the interface is a wireless interface, m_Protocol is set
	 * by the static test function
	 */
	if( ( m_IsWireless = testIsWireless() ) != true ) // assume a wired network
	{
		// Incorporate as much information as possible in the static function

	} else {  // We have a wireless network

	}




	if( ( fd = socket( AF_INET, SOCK_DGRAM, IPPROTO_IP ) ) == -1 )
	{
			std::cout << "Socket on " << m_Name << " failed: " << std::strerror( errno );
	} else {
		std::strncpy( ifr.ifr_ifrn.ifrn_name, m_Name.c_str(), m_Name.length() );
		els.cmd = ETHTOOL_GLINKSETTINGS;
		ifr.ifr_ifru.ifru_data = &els;
		if( ( res = ioctl( fd, SIOCETHTOOL, &els ) ) != -1 )
		{


			struct ethtool_link_settings {
				__u32	cmd;
				__u32	speed;
				__u8	duplex;
				__u8	port;
				__u8	phy_address;
				__u8	autoneg;
				__u8	mdio_support;
				__u8	eth_tp_mdix;
				__u8	eth_tp_mdix_ctrl;
				__s8	link_mode_masks_nwords;
				__u32	reserved[8];
				__u32	link_mode_masks[0];
				/* layout of link_mode_masks fields:
				 * __u32 map_supported[link_mode_masks_nwords];
				 * __u32 map_advertising[link_mode_masks_nwords];
				 * __u32 map_lp_advertising[link_mode_masks_nwords];
				 */
			};











		}



	}

	switch( ifa->ifa_addr->sa_family )
	{
	case AF_INET:
		std::memcpy( &m_NetworkAddress, ifa->ifa_addr, sizeof( sockaddr_in ) );
//		inet_ntop( ifa->ifa_addr->sa_family, m_NetworkAddress, addr, sizeof( sockaddr_in ) );
		std::memset( &addr, 0, sizeof( sockaddr_storage ) );
		m_IsBound = true;
		m_Family = AF_INET;
		break;
	case AF_INET6:
		std::memcpy( &m_NetworkAddress, ifa->ifa_addr, sizeof( sockaddr_in6 ) );
//		inet_ntop( ifa->ifa_addr->sa_family, ( const void * ) m_NetworkAddress, addr, sizeof( sockaddr_in6 ) );
		std::memset( &addr, 0, sizeof( sockaddr_storage ) );
		m_IsBound = true;
		m_Family = AF_INET;
		break;
	default:
		break;
	}

	/* Only collect address data and user data if the interface is bound. */
	if( m_IsBound )
	{

		switch( ifa->ifa_netmask->sa_family )
		{
		case AF_INET:
			std::memcpy( &m_NetworkMask, ifa->ifa_netmask, sizeof( sockaddr_in ) );
	//		inet_ntop( ifa->ifa_netmask->sa_family, ( const void * ) m_NetworkMask, addr, sizeof( sockaddr_in ) );
			std::memset( &addr, 0, sizeof( sockaddr_storage ) );
			break;
		case AF_INET6:
			std::memcpy( &m_NetworkMask, ifa->ifa_netmask, sizeof( sockaddr_in6 ) );
	//		inet_ntop( ifa->ifa_netmask->sa_family, ( const void * ) m_NetworkMask, addr, sizeof( sockaddr_in6 ) );
			std::memset( &addr, 0, sizeof( sockaddr_storage ) );
			break;
		default:
			break;
		}

		if( ifa->ifa_flags && IFF_BROADCAST )
		{
			m_IsBroadcast = true;
			switch( ifa->ifa_ifu.ifu_broadaddr->sa_family )
			{
			case AF_INET:
				std::memcpy( &m_NetworkDestination, ifa->ifa_ifu.ifu_broadaddr, sizeof( sockaddr_in ) );
//				inet_ntop( ifa->ifa_ifu.ifu_broadaddr->sa_family, ( const void * ) m_NetworkDestination, addr, sizeof( sockaddr_in ) );
				std::memset( &addr, 0, sizeof( sockaddr_storage ) );
				break;
			case AF_INET6:
				std::memcpy( &m_NetworkDestination, ifa->ifa_ifu.ifu_broadaddr, sizeof( sockaddr_in6 ) );
//				inet_ntop( ifa->ifa_ifu.ifu_broadaddr->sa_family, ( const void * ) m_NetworkDestination, addr, sizeof( sockaddr_in6 ) );
				std::memset( &addr, 0, sizeof( sockaddr_storage ) );
				break;
			default:
				break;
			}
		}
		else if( ifa->ifa_flags && IFF_POINTOPOINT )
		{
			m_IsBroadcast = false;
			switch( ifa->ifa_ifu.ifu_dstaddr->sa_family )
			{
			case AF_INET:
				std::memcpy( &m_NetworkDestination, ifa->ifa_ifu.ifu_dstaddr, sizeof( sockaddr_in ) );
//				inet_ntop( ifa->ifa_ifu.ifu_dstaddr->sa_family, ( const void * ) m_NetworkDestination, addr, sizeof( sockaddr_in ) );
				std::memset( &addr, 0, sizeof( sockaddr_storage ) );
				break;
			case AF_INET6:
				std::memcpy( &m_NetworkDestination, ifa->ifa_ifu.ifu_dstaddr, sizeof( sockaddr_in6 ) );
//				inet_ntop( ifa->ifa_ifu.ifu_dstaddr->sa_family, ( const void * ) m_NetworkDestination, addr, sizeof( sockaddr_in6 ) );
				std::memset( &addr, 0, sizeof( sockaddr_storage ) );
				break;
			default:
				break;
			}
		}
		if( ifa->ifa_data )
		{
			size_t st = sizeof( ifa->ifa_data );
			if( (m_Data = malloc( st ) ) != NULL )
			{
				std::memcpy (m_Data, ifa->ifa_data, st );
			}
		}
	}



	/*
	 * Get the rest of the link settings
	 */



}

NetworkInterface::~NetworkInterface( void )
{
	if( m_Data )
		free( m_Data );
}

std::string NetworkInterface::getName( void )
{
	return m_Name;
}

std::string NetworkInterface::getProtocol( void )
{
	return m_Protocol;
}

unsigned int NetworkInterface::getFlags( void )
{
	return m_Flags;
}

std::string NetworkInterface::getAddress( void )
{
	return m_Address;
}

std::string NetworkInterface::getMask( void )
{
	return m_Mask;
}

std::string NetworkInterface::getDestination( void )
{
	return m_Destination;
}

struct sockaddr_storage NetworkInterface::getNetworkAddress( void )
{
	return m_NetworkAddress;
}

struct sockaddr_storage NetworkInterface::getNetworkMask( void )
{
	return m_NetworkMask;
}

struct sockaddr_storage NetworkInterface::getNetworkDestination( void )
{
	return m_NetworkDestination;
}

bool NetworkInterface::isWireless( void )
{
	return m_IsWireless;
}

bool NetworkInterface::isBroadcast( void )
{
	return m_IsBroadcast;
}

bool NetworkInterface::isBound( void )
{
	return m_IsBound;
}

/*
 * Private methods
 */

bool NetworkInterface::testIsWireless( void )
{
	int sock = -1;
	struct iwreq pwrq;

	memset( &pwrq, 0, sizeof( pwrq ) );
	strncpy( pwrq.u.name, m_Name.c_str(), IFNAMSIZ );

	if( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
	{
		m_IsWireless = false;
		return false;
	}

	if( ioctl( sock, SIOCGIWNAME, &pwrq ) != -1 )
	{
		m_Protocol = pwrq.u.name;
		m_IsWireless = true;
		close( sock );
		return true;
	}

	m_IsWireless = false;
	close( sock );
	return false;
}

}
