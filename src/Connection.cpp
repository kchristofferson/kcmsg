/*
 * Connection.cpp
 *
 *  Created on: Jun 5, 2017
 *      Author: kurt
 */

#include "Connection.h"

#include <stdexcept>
#include <cassert>
#include <string>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <locale>
#include <regex>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

namespace kcmsg {

Connection::Connection(sa_family_t fam, int t, int p, int maxthreads, std::string loginstance)
{
	// assert a supported protocol
	assert( ( fam == AF_INET ) || ( fam == AF_INET6 ) );
	assert( ( t == SOCK_STREAM ) || ( t == SOCK_DGRAM ) || ( t == SOCK_SEQPACKET ) || ( t == SOCK_RAW ) );
	assert( ( p == IPPROTO_TCP ) || ( p == IPPROTO_UDP ) || ( p == IPPROTO_SCTP ) );

	// initialize member variables
	// setting all bytes in 'conn' member to 0 makes the default for a
	// connect() or a bind() to be to default addresses
	//                           Host IP      Service
	//                       ==========================
	//         IP version 4  |  INADDR_ANY  |    0    |
	//         IP version 6  |  in6addr_any |    0    |
	//         (i.e. the kernel will choose IP address and port number)

	conn = {0};
	listen_max = maxthreads;

	logger_ = log4cplus::Logger::getInstance( LOG4CPLUS_TEXT(loginstance) );

	//set connection definition
	family = fam;
	protocol_type = t;
	protocol = p;

	//handle rest of initialization as a switch to support future protocols
	switch ( fam )
	{
	case AF_INET :
		conn.addr.length = 16;
		break;
	case AF_INET6 :
		conn.addr.length = 28;
		break;
	default:
		conn.addr.length = sizeof(uint8_t) + MAXADDRLEN;
	}

	// create the socket
	conn.fd = socket(family, protocol_type, protocol);
}

Connection::~Connection() {
	// TODO Auto-generated destructor stub

}

int Connection::resolveHost(std::string host, std::string service)
{
	int res;
	addrinfo hints, *result;
	hints = {0};

	// return hostname in cannonical form and only addresses meeting "hints"
	hints.ai_flags = AI_CANONNAME | AI_ADDRCONFIG;
	hints.ai_family = family;
	hints.ai_socktype = protocol_type;
	hints.ai_protocol = protocol;

	if( (res = getaddrinfo(host.c_str(), service.c_str(), &hints, &result) ) != 0)
	{
		throw std::ios_base::failure("Unable to resolve host/service address");
		return(res);
	}

	switch (family)
	{
	case AF_INET :
		conn.addr.length = sizeof(sockaddr_in);
		std::memcpy( conn.addr.addr, result->ai_addr, sizeof(sockaddr_in) );
		break;
	case AF_INET6 :
		conn.addr.length = sizeof(sockaddr_in6);
		std::memcpy( conn.addr.addr, result->ai_addr, sizeof(sockaddr_in6) );
		break;
	default:
		LOG4CPLUS_ERROR( logger_, LOG4CPLUS_TEXT( "Connection class instantiated with unsupported protocol family: \"" )
				<< family << "\"" );
		throw std::invalid_argument("Unsupported protocol family");
		break;
	}

	LOG4CPLUS_DEBUG( logger_, LOG4CPLUS_TEXT( "Resolved host: \"" ) << LOG4CPLUS_TEXT( host) << "\" and service: \""
			<< LOG4CPLUS_TEXT( service) << "\" to " << formatAddress() );

	freeaddrinfo(result);
	return (res);
}

int Connection::resolveHost(std::wstring host, std::wstring service)
{
	/*
	int res;
	addrinfo hints, *result;
	hints = {0};

	// return hostname in cannonical form and only addresses meeting "hints"
	hints.ai_flags = AI_CANONNAME | AI_ADDRCONFIG;
	hints.ai_family = family;
	hints.ai_socktype = protocol_type;
	hints.ai_protocol = protocol;
    */


	// convert to punycode

	/*
	if( (res = getaddrinfo(host.c_str(), service.c_str(), &hints, &result) ) != 0)
		throw("Unable to resolve host/service address");

	switch (family)
	{
	case AF_INET :
		conn.addr.length = sizeof(sockaddr_in);
		std::memcpy( conn.addr.addr, result->ai_addr, sizeof(sockaddr_in) );
		break;
	case AF_INET6 :
		conn.addr.length = sizeof(sockaddr_in6);
		std::memcpy( conn.addr.addr, result->ai_addr, sizeof(sockaddr_in6) );
		break;
	default:
		throw("Unsupported protocol family");
		break;
	}

	freeaddrinfo(result);
	*/
	return 0;
}

void Connection::setAddress(std::string a)
{
	/*
	hostent h;
	std::regex re4("\d+\.\d+\.\d+\.\d+");
	std::string addr6Cast = "(?: \\
		    (?:                                              (?:[0-9a-f]{1,4}:){6} \\
		    |                                             :: (?:[0-9a-f]{1,4}:){5} \\
		    | (?:                         [0-9a-f]{1,4})? :: (?:[0-9a-f]{1,4}:){4} \\
		    | (?: (?:[0-9a-f]{1,4}:){0,1} [0-9a-f]{1,4})? :: (?:[0-9a-f]{1,4}:){3} \\
		    | (?: (?:[0-9a-f]{1,4}:){0,2} [0-9a-f]{1,4})? :: (?:[0-9a-f]{1,4}:){2} \\
		    | (?: (?:[0-9a-f]{1,4}:){0,3} [0-9a-f]{1,4})? :: (?:[0-9a-f]{1,4}:)    \\
		    | (?: (?:[0-9a-f]{1,4}:){0,4} [0-9a-f]{1,4})? ::                       \\
		    )                                                                      \\
		    (?: [0-9a-f]{1,4} : [0-9a-f]{1,4}                                      \\
		        | (?: (?: 25[0-5] | 2[0-4][0-9] | [01]?[0-9]?[0-9])\\.){3}         \\
		          (?: (?: 25[0-5] | 2[0-4][0-9] | [01]?[0-9]?[0-9]))               \\
		    )                                                                      \\
		    | (?: (?:[0-9a-f]{1,4}:){0,5} [0-9a-f]{1,4})? :: [0-9a-f]{1,4}         \\
		    | (?: (?:[0-9a-f]{1,4}:){0,6} [0-9a-f]{1,4})? ::                       \\
		    )";
	std::remove_if(addr6Cast.begin(), addr6Cast.end(), ::isspace), addr6Cast.end();

	std::regex re6(addr6Cast.c_str());


	//force hex address string to be lower case
	std::locale loc;
	for (std::string::size_type i=0; i<a.length(); ++i)
		std::tolower(a[i],loc);

	//test if the string is a number address
	//i.e. not requiring a hostname translation
	if ( family == AF_INET ) //IPv4
	{
		if (!std::regex_match( a, re4 ) )
				throw("Invalid IPv4 Address");

	}
	*/

}

void Connection::setAddress(uint32_t a)
{

}

void Connection::setService(std::string p)
{

}

void Connection::setService(uint16_t p)
{

}

void Connection::setListenerQueueSize(uint32_t s)
{
	listen_max = s;
}

bool Connection::isValidateAddress(std::string a)
{
	return ( true );
}

bool Connection::isValidatePort(std::string p)
{
	return ( true );
}

bool Connection::isValidatePort(int p)
{
	return ( true );
}

int Connection::getDescriptor(void)
{
	return this->conn.fd;
}

bool Connection::Accept(void)
{
	connection_storage cliaddr = {0};

	switch ( family )
	{
	case AF_INET :
		cliaddr.addr.length = 16;
		break;
	case AF_INET6 :
		cliaddr.addr.length = 28;
		break;
	default:
		cliaddr.addr.length = sizeof(uint8_t) + MAXADDRLEN;
	}

	LOG4CPLUS_DEBUG( logger_, LOG4CPLUS_TEXT( "accepting client ... ") );

	if ( ( cliaddr.fd = accept( conn.fd, ( sockaddr * ) &cliaddr.addr.addr, (unsigned int *) &cliaddr.addr.length ) ) < 0 )
	{
		return (errno);
	}

	LOG4CPLUS_DEBUG( logger_, LOG4CPLUS_TEXT( "Client accepted on the descriptor (" ) << std::to_string( cliaddr.fd ) << ").");

	return (true);
}

void Connection::Bind(void)
{
	int err;
	if ( ( bind(conn.fd, (const struct sockaddr *) &conn.addr.addr, (socklen_t) conn.addr.length) ) < 0 )
	{
		err = errno;
		LOG4CPLUS_ERROR( logger_, LOG4CPLUS_TEXT( "Connection bind failure. ") << LOG4CPLUS_TEXT( formatErrno( err ) ) );
		throw std::ios_base::failure( "Unable to Bind()" );
	}
	else
	{
		LOG4CPLUS_DEBUG( logger_, LOG4CPLUS_TEXT( "Bound socket (") << LOG4CPLUS_TEXT( std::to_string( conn.fd ) )
				<< LOG4CPLUS_TEXT( ") to " ) << LOG4CPLUS_TEXT( formatAddress() ) );
	}
}

void Connection::Close(int sockfd)
{
	close(sockfd);
}

void Connection::Connect(void)
{
	int err;
	if ( ( conn.fd = connect(conn.fd, (const struct sockaddr *) &conn.addr.addr, (socklen_t) conn.addr.length) ) < 0 )
	{
		err = errno;
		LOG4CPLUS_ERROR( logger_, LOG4CPLUS_TEXT( "Connection connect failure. ") << LOG4CPLUS_TEXT( formatErrno( err ) ) );
		throw std::ios_base::failure( "Unable to Connect()" );
	}
	else
	{
		LOG4CPLUS_DEBUG( logger_, LOG4CPLUS_TEXT( "Connected socket (") << LOG4CPLUS_TEXT( std::to_string( conn.fd ) )
				<< LOG4CPLUS_TEXT( ") to " ) << LOG4CPLUS_TEXT( formatAddress() ) );
	}
}

void Connection::Listen(void)
{
	int err;
	if ( ( listen(conn.fd, listen_max) < 0 ) )
	{
		err = errno;
		LOG4CPLUS_ERROR( logger_, LOG4CPLUS_TEXT( "Connection listen failure. ") << LOG4CPLUS_TEXT( formatErrno( err ) ) );
		throw std::ios_base::failure( "Unable to Listen()" );
	}
	else
	{
		LOG4CPLUS_DEBUG( logger_, LOG4CPLUS_TEXT( "Listening on socket descriptor(") << LOG4CPLUS_TEXT( std::to_string( conn.fd ) )
				<< LOG4CPLUS_TEXT( ") at address " ) << LOG4CPLUS_TEXT( formatAddress() ) );
	}
}

void Connection::Shutdown(int sockfd, int howto)
{
	shutdown(sockfd, howto);
}

size_t Connection::Readn(char *msg, size_t nbytes)
{
	size_t nleft;
	ssize_t nread;
	char *ptr;

	ptr = msg;
	nleft = nbytes;

	while( nleft > 0 )
	{
		if( ( nread = read( conn.fd, ptr, nleft ) ) < 0 )
		{
			if( errno == EINTR )
			{
				nread = 0;
			} else
			{
				return (-1);
			}
		} else if( nread == 0 )
			{
				break;
			}
		nleft -= nread;
		ptr += nread;


	}

	return ( nbytes - nleft );
}

size_t Connection::ReadMessage(kcmsg::Message *msg, size_t nbytes)
{
	// nbytes is the size of the buffer needed to read the message
	// msgsize is the actual size of the message which is not known
	//   until we read its length
	size_t nleft, msgsize;
	ssize_t nread;
	char *ptr;

	ptr = (char *) msg;
	nleft = nbytes; // Max size of buffer (msg)
	msgsize = 0;	// We do not know actual message size yet...


	while( nleft > 0 )
	{
		if( ( nread = read( conn.fd, ptr, nleft ) ) < 0 )
		{
			if( errno == EINTR )
			{
				nread = 0;
			} else
			{
				return (-1);
			}
		} else if( nread == 0 )
			{
				break;
			}
		nleft -= nread;
		ptr += nread;

		// set the actual length of the message
		// if we do not know it yet and have read in enough bytes to obtain it
		if ( ( msgsize == 0) && ( ( nbytes - nleft ) >= (MESSAGE_LENGTH_OFFSET + 2) ) )
		{
			msgsize = msg->getLength();
			nleft -= ( nbytes - msgsize );
		}

	}

	return ( nleft );
}

size_t Connection::Writen(char *msg, size_t nbytes)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = msg;
	nleft = nbytes;

	while( nleft > 0 )
	{
		if( ( nwritten = write( conn.fd, ptr, nleft ) ) <= 0 )
		{
			if( nwritten < 0 && errno == EINTR )
			{
				nwritten = 0;
			} else
			{
				return (-1);
			}
		}

		nleft -= nwritten;
		ptr += nwritten;
	}

	return ( nbytes );
}

size_t Connection::WriteMessage(kcmsg::Message *msg)
{
	size_t nleft, retval;
	ssize_t nwritten;
	const char *ptr;

	ptr = (const char *) msg;
	nleft = retval = msg->getLength();

	while( nleft > 0 )
	{
		if( ( nwritten = write( conn.fd, ptr, nleft ) ) <= 0 )
		{
			if( nwritten < 0 && errno == EINTR )
			{
				nwritten = 0;
			} else
			{
				return (-1);
			}
		}

		nleft -= nwritten;
		ptr += nwritten;
	}

	return ( retval );
}

/* private methods */

std::string Connection::formatAddress(void)
{
	/*
	 *
	 */

	std::string ret;
	uint p = 0; // holder for the port number in host order

	switch ( family )
	{
	case AF_INET :
		sockaddr_in *addr;
		char host_addr[INET_ADDRSTRLEN];
		ret = "AF_INET: HOST(";
		addr = (sockaddr_in *) &conn.addr.addr;
		ret += inet_ntop(AF_INET, &(addr->sin_addr), host_addr, INET_ADDRSTRLEN);
		ret += ") PORT(";
		p = ntohs(addr->sin_port);
		ret += std::to_string( p );
		ret += ")";
		break;
	case AF_INET6 :
		sockaddr_in6 *addr6;
		char host6_addr[INET6_ADDRSTRLEN];
		ret = "AF_INET6: HOST(";
		addr6 = (sockaddr_in6 *) &conn.addr.addr;
		ret += inet_ntop(AF_INET6, &(addr6->sin6_addr), host6_addr, INET6_ADDRSTRLEN);
		ret += ") PORT(";
		p = ntohs(addr6->sin6_port);
		ret += std::to_string( p );
		ret += ")";
		break;
	default :
		ret = "Unsupported protocol family";
		break;
	}

	return (ret);
}

std::string Connection::formatErrno(int err)
{
	std::string ret = "ERRNO: (";
	ret += std::to_string(err);
	ret += ") \"";
	switch (err)
	{
	case EPERM :
	   ret += "EPERM - Operation not permitted\"";
	   break;
	case ENOENT :
	   ret += "ENOENT - No such file or directory\"";
	   break;
	case ESRCH :
	   ret += "ESRCH - No such process\"";
	   break;
	case EINTR :
	   ret += "EINTR - Interrupted system call\"";
	   break;
	case EIO :
	   ret += "EIO - I/O error\"";
	   break;
	case ENXIO :
	   ret += "ENXIO - No such device or address\"";
	   break;
	case E2BIG :
	   ret += "E2BIG - Argument list too long\"";
	   break;
	case ENOEXEC :
	   ret += "ENOEXEC - Exec format error\"";
	   break;
	case EBADF :
	   ret += "EBADF - Bad file number\"";
	   break;
	case ECHILD :
	   ret += "ECHILD - No child processes\"";
	   break;
	case EAGAIN :
	   ret += "EAGAIN - Try again\"";
	   break;
	case ENOMEM :
	   ret += "ENOMEM - Out of memory\"";
	   break;
	case EACCES :
	   ret += "EACCES - Permission denied\"";
	   break;
	case EFAULT :
	   ret += "EFAULT - Bad address\"";
	   break;
	case ENOTBLK :
	   ret += "ENOTBLK - Block device required\"";
	   break;
	case EBUSY :
	   ret += "EBUSY - Device or resource busy\"";
	   break;
	case EEXIST :
	   ret += "EEXIST - File exists\"";
	   break;
	case EXDEV :
	   ret += "EXDEV - Cross-device link\"";
	   break;
	case ENODEV :
	   ret += "ENODEV - No such device\"";
	   break;
	case ENOTDIR :
	   ret += "ENOTDIR - Not a directory\"";
	   break;
	case EISDIR :
	   ret += "EISDIR - Is a directory\"";
	   break;
	case EINVAL :
	   ret += "EINVAL - Invalid argument\"";
	   break;
	case ENFILE :
	   ret += "ENFILE - File table overflow\"";
	   break;
	case EMFILE :
	   ret += "EMFILE - Too many open files\"";
	   break;
	case ENOTTY :
	   ret += "ENOTTY - Not a typewriter\"";
	   break;
	case ETXTBSY :
	   ret += "ETXTBSY - Text file busy\"";
	   break;
	case EFBIG :
	   ret += "EFBIG - File too large\"";
	   break;
	case ENOSPC :
	   ret += "ENOSPC - No space left on device\"";
	   break;
	case ESPIPE :
	   ret += "ESPIPE - Illegal seek\"";
	   break;
	case EROFS :
	   ret += "EROFS - Read-only file system\"";
	   break;
	case EMLINK :
	   ret += "EMLINK - Too many links\"";
	   break;
	case EPIPE :
	   ret += "EPIPE - Broken pipe\"";
	   break;
	case EDEADLK :
	   ret += "EDEADLK - Resource deadlock would occur\"";
	   break;
	case ENAMETOOLONG :
	   ret += "ENAMETOOLONG - File name too long\"";
	   break;
	case ENOLCK :
	   ret += "ENOLCK - No record locks available\"";
	   break;
	case ENOSYS :
	   ret += "ENOSYS - Invalid system call number\"";
	   break;
	case ENOTEMPTY :
	   ret += "ENOTEMPTY - Directory not empty\"";
	   break;
	case ELOOP :
	   ret += "ELOOP - Too many symbolic links encountered\"";
	   break;
	case ENOMSG :
	   ret += "ENOMSG - No message of desired type\"";
	   break;
	case EIDRM :
	   ret += "EIDRM - Identifier removed\"";
	   break;
	case ECHRNG :
	   ret += "ECHRNG - Channel number out of range\"";
	   break;
	case EL2NSYNC :
	   ret += "EL2NSYNC - Level 2 not synchronized\"";
	   break;
	case EL3HLT :
	   ret += "EL3HLT - Level 3 halted\"";
	   break;
	case EL3RST :
	   ret += "EL3RST - Level 3 reset\"";
	   break;
	case ELNRNG :
	   ret += "ELNRNG - Link number out of range\"";
	   break;
	case EUNATCH :
	   ret += "EUNATCH - Protocol driver not attached\"";
	   break;
	case ENOCSI :
	   ret += "ENOCSI - No CSI structure available\"";
	   break;
	case EL2HLT :
	   ret += "EL2HLT - Level 2 halted\"";
	   break;
	case EBADE :
	   ret += "EBADE - Invalid exchange\"";
	   break;
	case EBADR :
	   ret += "EBADR - Invalid request descriptor\"";
	   break;
	case EXFULL :
	   ret += "EXFULL - Exchange full\"";
	   break;
	case ENOANO :
	   ret += "ENOANO - No anode\"";
	   break;
	case EBADRQC :
	   ret += "EBADRQC - Invalid request code\"";
	   break;
	case EBADSLT :
	   ret += "EBADSLT - Invalid slot\"";
	   break;
	case ENOSTR :
	   ret += "ENOSTR - Device not a stream\"";
	   break;
	case ENODATA :
	   ret += "ENODATA - No data available\"";
	   break;
	case ETIME :
	   ret += "ETIME - Timer expired\"";
	   break;
	case ENOSR :
	   ret += "ENOSR - Out of streams resources\"";
	   break;
	case ENONET :
	   ret += "ENONET - Machine is not on the network\"";
	   break;
	case ENOPKG :
	   ret += "ENOPKG - Package not installed\"";
	   break;
	case EREMOTE :
	   ret += "EREMOTE - Object is remote\"";
	   break;
	case ENOLINK :
	   ret += "ENOLINK - Link has been severed\"";
	   break;
	case EADV :
	   ret += "EADV - Advertise error\"";
	   break;
	case ESRMNT :
	   ret += "ESRMNT - Srmount error\"";
	   break;
	case ECOMM :
	   ret += "ECOMM - Communication error on send\"";
	   break;
	case EPROTO :
	   ret += "EPROTO - Protocol error\"";
	   break;
	case EMULTIHOP :
	   ret += "EMULTIHOP - Multihop attempted\"";
	   break;
	case EDOTDOT :
	   ret += "EDOTDOT - RFS specific error\"";
	   break;
	case EBADMSG :
	   ret += "EBADMSG - Not a data message\"";
	   break;
	case EOVERFLOW :
	   ret += "EOVERFLOW - Value too large for defined data type\"";
	   break;
	case ENOTUNIQ :
	   ret += "ENOTUNIQ - Name not unique on network\"";
	   break;
	case EBADFD :
	   ret += "EBADFD - File descriptor in bad state\"";
	   break;
	case EREMCHG :
	   ret += "EREMCHG - Remote address changed\"";
	   break;
	case ELIBACC :
	   ret += "ELIBACC - Can not access a needed shared library\"";
	   break;
	case ELIBBAD :
	   ret += "ELIBBAD - Accessing a corrupted shared library\"";
	   break;
	case ELIBSCN :
	   ret += "ELIBSCN - .lib section in a.out corrupted\"";
	   break;
	case ELIBMAX :
	   ret += "ELIBMAX - Attempting to link in too many shared libraries\"";
	   break;
	case ELIBEXEC :
	   ret += "ELIBEXEC - Cannot exec a shared library directly\"";
	   break;
	case ERESTART :
	   ret += "ERESTART - Interrupted system call should be restarted\"";
	   break;
	case ESTRPIPE :
	   ret += "ESTRPIPE - Streams pipe error\"";
	   break;
	case EUSERS :
	   ret += "EUSERS - Too many users\"";
	   break;
	case ENOTSOCK :
	   ret += "ENOTSOCK - Socket operation on non-socket\"";
	   break;
	case EDESTADDRREQ :
	   ret += "EDESTADDRREQ - Destination address required\"";
	   break;
	case EMSGSIZE :
	   ret += "EMSGSIZE - Message too long\"";
	   break;
	case EPROTOTYPE :
	   ret += "EPROTOTYPE - Protocol wrong type for socket\"";
	   break;
	case ENOPROTOOPT :
	   ret += "ENOPROTOOPT - Protocol not available\"";
	   break;
	case EPROTONOSUPPORT :
	   ret += "EPROTONOSUPPORT - Protocol not supported\"";
	   break;
	case ESOCKTNOSUPPORT :
	   ret += "ESOCKTNOSUPPORT - Socket type not supported\"";
	   break;
	case EOPNOTSUPP :
	   ret += "EOPNOTSUPP - Operation not supported on transport endpoint\"";
	   break;
	case EPFNOSUPPORT :
	   ret += "EPFNOSUPPORT - Protocol family not supported\"";
	   break;
	case EAFNOSUPPORT :
	   ret += "EAFNOSUPPORT - Address family not supported by protocol\"";
	   break;
	case EADDRINUSE :
	   ret += "EADDRINUSE - Address already in use\"";
	   break;
	case EADDRNOTAVAIL :
	   ret += "EADDRNOTAVAIL - Cannot assign requested address\"";
	   break;
	case ENETDOWN :
	   ret += "ENETDOWN - Network is down\"";
	   break;
	case ENETUNREACH :
	   ret += "ENETUNREACH - Network is unreachable\"";
	   break;
	case ENETRESET :
	   ret += "ENETRESET - Network dropped connection because of reset\"";
	   break;
	case ECONNABORTED :
	   ret += "ECONNABORTED - Software caused connection abort\"";
	   break;
	case ECONNRESET :
	   ret += "ECONNRESET - Connection reset by peer\"";
	   break;
	case ENOBUFS :
	   ret += "ENOBUFS - No buffer space available\"";
	   break;
	case EISCONN :
	   ret += "EISCONN - Transport endpoint is already connected\"";
	   break;
	case ENOTCONN :
	   ret += "ENOTCONN - Transport endpoint is not connected\"";
	   break;
	case ESHUTDOWN :
	   ret += "ESHUTDOWN - Cannot send after transport endpoint shutdown\"";
	   break;
	case ETOOMANYREFS :
	   ret += "ETOOMANYREFS - Too many references: cannot splice\"";
	   break;
	case ETIMEDOUT :
	   ret += "ETIMEDOUT - Connection timed out\"";
	   break;
	case ECONNREFUSED :
	   ret += "ECONNREFUSED - Connection refused\"";
	   break;
	case EHOSTDOWN :
	   ret += "EHOSTDOWN - Host is down\"";
	   break;
	case EHOSTUNREACH :
	   ret += "EHOSTUNREACH - No route to host\"";
	   break;
	case EALREADY :
	   ret += "EALREADY - Operation already in progress\"";
	   break;
	case EINPROGRESS :
	   ret += "EINPROGRESS - Operation now in progress\"";
	   break;
	case ESTALE :
	   ret += "ESTALE - Stale file handle\"";
	   break;
	case EUCLEAN :
	   ret += "EUCLEAN - Structure needs cleaning\"";
	   break;
	case ENOTNAM :
	   ret += "ENOTNAM - Not a XENIX named type file\"";
	   break;
	case ENAVAIL :
	   ret += "ENAVAIL - No XENIX semaphores available\"";
	   break;
	case EISNAM :
	   ret += "EISNAM - Is a named type file\"";
	   break;
	case EREMOTEIO :
	   ret += "EREMOTEIO - Remote I/O error\"";
	   break;
	case EDQUOT :
	   ret += "EDQUOT - Quota exceeded\"";
	   break;
	case ENOMEDIUM :
	   ret += "ENOMEDIUM - No medium found\"";
	   break;
	case EMEDIUMTYPE :
	   ret += "EMEDIUMTYPE - Wrong medium type\"";
	   break;
	case ECANCELED :
	   ret += "ECANCELED - Operation Canceled\"";
	   break;
	case ENOKEY :
	   ret += "ENOKEY - Required key not available\"";
	   break;
	case EKEYEXPIRED :
	   ret += "EKEYEXPIRED - Key has expired\"";
	   break;
	case EKEYREVOKED :
	   ret += "EKEYREVOKED - Key has been revoked\"";
	   break;
	case EKEYREJECTED :
	   ret += "EKEYREJECTED - Key was rejected by service\"";
	   break;
	case EOWNERDEAD :
	   ret += "EOWNERDEAD - Owner died\"";
	   break;
	case ENOTRECOVERABLE :
	   ret += "ENOTRECOVERABLE - State not recoverable\"";
	   break;
	case ERFKILL :
	   ret += "ERFKILL - Operation not possible due to RF-kill\"";
	   break;
	case EHWPOISON :
	   ret += "EHWPOISON - Memory page has hardware error\"";
	   break;
	default :
		ret += "UNKNOWN - UNKNOWN errno value\"";
		break;
	}

	return (ret);
}

} /* namespace kcmsg */
