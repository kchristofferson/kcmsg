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
	std::string m_name;
	std::string m_sprotocol;
	std::string m_saddr;
	std::string m_smask;
	std::string m_sdest;
	unsigned int m_flags;
	unsigned int m_family;
	struct sockaddr_storage m_NetworkAddress;
	struct sockaddr_storage m_NetworkMask;
	struct sockaddr_storage m_NetworkDestination;
	void *m_data;
	bool m_isWireless;
	bool m_isBroadcast;

	bool testIsWireless(void);

public:
	NetworkInterface( struct ifaddrs *ifa );
	virtual ~NetworkInterface( void );
	unsigned int getFlags( void );
	struct sockaddr_storage getNetworkAddress( void );
	struct sockaddr_storage getNetworkMask( void );
	struct sockaddr_storage getNetworkDestination( void );

	bool isWireless( void );
	bool isBroadcast( void );
};

} /* namespace kcmsg */

#endif /* NETWORKINTERFACE_H_*/
