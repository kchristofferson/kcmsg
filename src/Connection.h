/*
 * Connection.h
 *
 *  Created on: Jun 5, 2017
 *      Author: kurt
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <stdint.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <array>
#include <vector>

#include <log4cplus/loggingmacros.h>

#include "Message.h"

namespace kcmsg {

#define MAXADDRLEN 104

struct addr_storage
{
	uint32_t length;
	uint8_t addr[MAXADDRLEN];
	uint16_t port;
};

struct connection_storage
{
	int fd;
	addr_storage addr;
};

class Connection {
private:
	int listen_max;
	int family;
	int protocol_type;
	int protocol;
	connection_storage conn;
	std::vector<connection_storage> clients;
//	log4cplus::Logger logger_;

	std::string formatAddress(void);
	std::string formatErrno(int err);

public:
	/*  Parameters
	 *    fam:  Protocol family
	 *       AF_INET  - IP version 4
	 *       AF_INET6 - IP version 6
	 *       AF_LOCAL - UNIX domain protocol
	 *       AF_ROUTE - Routing protocol
	 *       AF_KEY   - Key management socket
	 *    t: Protocol type
	 *       SOCK_STREAM    - Streaming protocol (TCP)
	 *       SOCK_DGRAM     - Datagram protocol  (UDP)
	 *       SOCK_SEQPACKET - Stream Control Transmission protocol (SCTP)
	 *       SOCK_RAW       - Raw socket.  No transport formating (ICMP, IGMP, Routing, nmap, etc.)
	 *    p: protocol
	 *       IPPROTO_TCP    - Streaming TCP
	 *       IPPROTO_UDP    - Datagram UDP
	 *       IPPROTO_SCTP   - Association SCTP
	 *
	 *  Connection is passed values for the underlying socket call.
	 *                  |   AF_INET   |   AF_INET6  |   AF_LOCAL  |   AF_ROUTE  |    AF_KEY   |
	 *                  =======================================================================
	 *   SOCK_STREAM    |  (TCP/SCTP) |  (TCP/SCTP) |  SUPPORTED  |  NOT VALID  |  NOT VALID  |
	 *   SOCK_DGRAM     |     UDP     |     UDP     |  SUPPORTED  |  NOT VALID  |  NOT VALID  |
	 *   SOCK_SEQPACKET |     SCTP    |     SCTP    |  SUPPORTED  |  NOT VALID  |  NOT VALID  |
	 *   SOCK_RAW       |     IPv4    |     IPv6    |  NOT VALID  |  SUPPORTED  |  SUPPORTED  |
	 *
	 */
	Connection(sa_family_t fam, int t, int p, int maxtheads, std::string loginstance);
	virtual ~Connection();

	int resolveHost(std::string host, std::string service);
	int resolveHost(std::wstring host, std::wstring service);
	void setAddress(std::string a);
	void setAddress(uint32_t a);
	void setService(std::string p);
	void setService(uint16_t p);
	void setListenerQueueSize(uint32_t s);
	bool isValidateAddress(std::string a);
	bool isValidatePort(std::string p);
	bool isValidatePort(int p);
	int getDescriptor(void);

    /* Accept()
     *
	 *
	 * Each successful Accept adds a connection_storage to the end of the
	 * vector of clients.
     */
	bool Accept(void);

	/* Bind()  Binds a local transport layer address to a Connection().
	 *         Upon creation of a connection, you can bind immediately.
	 *         The default addresses are
	 *                           Host IP      Service
	 *                       ==========================
	 *         IP version 4  |  INADDR_ANY  |    0    |
	 *         IP version 6  |  in6addr_any |    0    |
	 *         (i.e. the kernel will choose IP address and port number)
	 *
	 * The application must call either setAddress() and setService() or
	 * resolveHost() prior to bind() to listen or use a specific address
	 * and service port number.
	 */
	void Bind(void);

	/* Close () Terminates a TCP connection.  Close marks the socket as closed
	 * and returns immediately.  The descriptor is no longer available to the
	 * process.  Any data already accepted but not yet sent will be sent to the
	 * other side.
	 */
	void Close(int sockfd);

	/* Connect() connects a Connection() to the transport layer address of
	 *           a target host and service. After creation of a connection,
	 *           you can connect() immediately. The default addresses are
	 *                           Host IP      Service
	 *                       =============================
	 *         IP version 4  | Localhost | kernel chooses |
	 *         IP version 6  | Localhost | kernel chooses |
	 *         (i.e. the kernel will choose IP address and port number)
	 *
	 * The application must call setAddress() and setService() prior to bind() to listen or
	 * use a specific address, including "loopback", and service port number.
	 */
	void Connect(void);

	/* Listen() converts an unconnected Connection into a passive listening socket.
	 *          The application may want to set the listen_max member variable prior to
	 *          calling Listen().  On creation of a Connection(), the default value is
	 *          DEFAULT_LISTEN_MAX (10).
	 *          listen_max is used as the queue size or "backlog" value for TCP/IP's listen()
	 *          call.  Note actual queue size based on this number is OS specific.
	 */
	void Listen(void);

	/* Shutdown() Alternative method to Close().  The behavior of the function depends
	 *            upon the "howto" parameter.  Valid howto values are
	 *            SHUT_RD - The read half of the connection is closed.  All data currently
	 *                      in the receive buffer is discarded.  No more read() calls can
	 *                      be made to the socket.  All data received from the other side
	 *                      to this socket is acknowledged and discarded.
	 *            SHUT_WR - The write half of the connection is closed.  Data currently in
	 *                      the send buffer is sent. The process can no longer send data,
	 *                      irrespective of the descriptor's reference count.
	 *          SHUT_RDWR - Both halves are closed.  This is equivalent to two calls to
	 *                      shutdown for RD and then for WR.
	 */
	void Shutdown(int sockfd, int howto);

	size_t Readn(char *msg, size_t nbytes);
	size_t ReadMessage(kcmsg::Message *msg, size_t nbytes);
	size_t Writen(char *msg, size_t nbytes);
	size_t WriteMessage(kcmsg::Message *msg);
};

} /* namespace kcmsg */

#endif /* CONNECTION_H_ */
