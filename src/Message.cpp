/*
 * Message.cpp
 *
 *  Created on: May 18, 2017
 *      Author: kurt
 */

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <boost/endian/conversion.hpp>
#include <boost/endian/buffers.hpp>

#include "Message.h"

namespace kcmsg {

/*
 *                            HEADER FORMAT
 *                            =============
 *
 *  | 00 01 02 03 | 04 05 | 06 07 08 09 | 0A 0B | 0C 0D | 0E 0F |
 *  |   src_id    |src_org|   tgt_id    |tgt_org| trn_id|trn_app|
 *
 *  | 10 11 | 12 13 14 15 | 16 17 | 18 19 1A 1B 1C 1D 1E 1F ....
 *  |trn_org|     ttl     | flags |msg_len|  user data ....
 *
 */
const size_t MESSAGE_LENGTH_OFFSET = 0x18;

const size_t HEADER_SOURCE_IDENT_OFFSET = 0x00;
const size_t HEADER_SOURCE_ORGANIZATION_OFFSET = 0x04;
const size_t HEADER_TARGET_IDENT_OFFSET = 0x06;
const size_t HEADER_TARGET_ORGANIZATION_OFFSET = 0x0A;
const size_t HEADER_TRANSACTION_IDENT_OFFSET = 0x0C;
const size_t HEADER_TRANSACTION_APPLICATION_OFFSET = 0x0E;
const size_t HEADER_TRANSACTION_ORGANIZATION_OFFSET = 0x10;
const size_t HEADER_TTL_OFFSET = 0x12;
const size_t HEADER_FLAGS_OFFSET = 0x16;

/* Supported Data Type Identifiers */
const uint8_t DATA_TYPE = 0x00;
const uint8_t DATA_TYPE_BOOL = 0x01;
const uint8_t DATA_TYPE_BYTE = 0x02;
const uint8_t DATA_TYPE_SHORT = 0x03;
const uint8_t DATA_TYPE_INT = 0x04;
const uint8_t DATA_TYPE_LONG = 0x05;
const uint8_t DATA_TYPE_LONG_LONG = 0x06;
const uint8_t DATA_TYPE_FLOAT = 0x07;
const uint8_t DATA_TYPE_DOUBLE = 0x08;
const uint8_t DATA_TYPE_CHAR = 0x09;
const uint8_t DATA_TYPE_WCHAR = 0x0A;
const uint8_t DATA_TYPE_STRING_1 = 0x0B;
const uint8_t DATA_TYPE_STRING_2 = 0x0C;
const uint8_t DATA_TYPE_WSTRING_1 = 0x0D;
const uint8_t DATA_TYPE_WSTRING_2 = 0x0E;
const uint8_t DATA_TYPE_TIME = 0x0F;
const uint8_t DATA_TYPE_DURATION = 0x10;
const uint8_t DATA_TYPE_BOOL_ARRAY = 0x11;
const uint8_t DATA_TYPE_BYTE_ARRAY = 0x12;
const uint8_t DATA_TYPE_SHORT_ARRAY = 0x13;
const uint8_t DATA_TYPE_INT_ARRAY = 0x14;
const uint8_t DATA_TYPE_LONG_ARRAY = 0x15;
const uint8_t DATA_TYPE_LONG_LONG_ARRAY = 0x16;
const uint8_t DATA_TYPE_FLOAT_ARRAY = 0x17;
const uint8_t DATA_TYPE_DOUBLE_ARRAY = 0x18;
const uint8_t DATA_TYPE_STRING_ARRAY = 0x19;
const uint8_t DATA_TYPE_WSTRING_ARRAY = 0x1A;
const uint8_t DATA_TYPE_TIME_ARRAY = 0x1B;
const uint8_t DATA_TYPE_DURRATION_ARRAY = 0x1C;

/* Private Methods */

void Message::readHeader(void)
{
	boost::endian::little_uint32_buf_t src_id;
	boost::endian::little_uint16_buf_t src_org;
	boost::endian::little_uint32_buf_t tgt_id;
	boost::endian::little_uint16_buf_t tgt_org;
	boost::endian::little_uint16_buf_t trans_id;
	boost::endian::little_uint16_buf_t trans_app;
	boost::endian::little_uint16_buf_t trans_org;
	boost::endian::little_uint32_buf_t ttl;
	boost::endian::little_uint16_buf_t flags;


	memcpy(&src_id, &data[HEADER_SOURCE_IDENT_OFFSET], sizeof(src_id));
	memcpy(&src_org, &data[HEADER_SOURCE_ORGANIZATION_OFFSET], sizeof(src_org));
	memcpy(&tgt_id, &data[HEADER_TARGET_IDENT_OFFSET], sizeof(tgt_id));
	memcpy(&tgt_org, &data[HEADER_TARGET_ORGANIZATION_OFFSET], sizeof(tgt_org));
	memcpy(&trans_id, &data[HEADER_TRANSACTION_IDENT_OFFSET], sizeof(trans_id));
	memcpy(&trans_app, &data[HEADER_TRANSACTION_APPLICATION_OFFSET], sizeof(trans_app));
	memcpy(&trans_org, &data[HEADER_TRANSACTION_ORGANIZATION_OFFSET], sizeof(trans_org));
	memcpy(&ttl, &data[HEADER_TTL_OFFSET], sizeof(ttl));
	memcpy(&flags, &data[HEADER_FLAGS_OFFSET], sizeof(flags));

	hdr.source_ident = src_id.value();
	hdr.source_organization = src_org.value();
	hdr.target_ident = tgt_id.value();
	hdr.target_organization = tgt_org.value();
	hdr.transaction_ident = trans_id.value();
	hdr.transaction_application = trans_app.value();
	hdr.transaction_organization = trans_org.value();
	hdr.ttl = ttl.value();
	hdr.flags = flags.value();
}

void Message::writeHeader(void)
{

	boost::endian::little_uint32_buf_t src_id;
	boost::endian::little_uint16_buf_t src_org;
	boost::endian::little_uint32_buf_t tgt_id;
	boost::endian::little_uint16_buf_t tgt_org;
	boost::endian::little_uint16_buf_t trans_id;
	boost::endian::little_uint16_buf_t trans_app;
	boost::endian::little_uint16_buf_t trans_org;
	boost::endian::little_uint32_buf_t ttl;
	boost::endian::little_uint16_buf_t flags;

	src_id = hdr.source_ident;
	src_org = hdr.source_organization;
	tgt_id = hdr.target_ident;
	tgt_org = hdr.target_organization;
	trans_id = hdr.transaction_ident;
	trans_app = hdr.transaction_application;
	trans_org = hdr.transaction_organization;
	ttl = hdr.ttl;
	flags = hdr.flags;

	memcpy(&data[HEADER_SOURCE_IDENT_OFFSET], &src_id, sizeof(src_id));
	memcpy(&data[HEADER_SOURCE_ORGANIZATION_OFFSET], &src_org, sizeof(src_org));
	memcpy(&data[HEADER_TARGET_IDENT_OFFSET], &tgt_id, sizeof(tgt_id));
	memcpy(&data[HEADER_TARGET_ORGANIZATION_OFFSET], &tgt_org, sizeof(tgt_org));
	memcpy(&data[HEADER_TRANSACTION_IDENT_OFFSET], &trans_id, sizeof(trans_id));
	memcpy(&data[HEADER_TRANSACTION_APPLICATION_OFFSET], &trans_app, sizeof(trans_app));
	memcpy(&data[HEADER_TRANSACTION_ORGANIZATION_OFFSET], &trans_org, sizeof(trans_org));
	memcpy(&data[HEADER_TTL_OFFSET], &ttl, sizeof(ttl));
	memcpy(&data[HEADER_FLAGS_OFFSET], &flags, sizeof(flags));
}

void Message::readMessageLength(void)
{
	boost::endian::little_uint16_buf_t msg_len;

	memcpy(&msg_len, &data[MESSAGE_LENGTH_OFFSET], sizeof(msg_len));
	data_length = msg_len.value();
}

void Message::updateDataLength(std::size_t delta)
{
	boost::endian::little_uint16_buf_t ndl;
	data_length += delta;
	ndl = data_length;
	memcpy(&data[MESSAGE_LENGTH_OFFSET], &ndl, sizeof(ndl));
}

/* Public Methods */

Message::Message()
{
	int maxdata = MAX_MSG_DATA;
	data = (char *) malloc(maxdata);
	assert(data);
	boost::endian::little_uint16_buf_t ndl;

	// zero out the message space
	memset(data, 0, maxdata);

	// set user data in message to end of message header
	data_length = offset = (std::size_t) MESSAGE_LENGTH_OFFSET + 2;

	// store the current length of the message, header plus a two byte length field
	// into the data.  This forces message size max to not be greater than 64K
	ndl = (boost::endian::little_uint16_buf_t) data_length;
	memcpy(&data[MESSAGE_LENGTH_OFFSET], &ndl, sizeof(ndl));
}

Message::~Message()
{
	if ( data )
		free( data );
}

void Message::setSourceIdentifier(uint32_t id)
{
	hdr.source_ident = id;
}

uint32_t Message::getSourceIdentifier(void)
{
	return ( hdr.source_ident );
}

void Message::setSourceOrganization(uint16_t id)
{
	hdr.source_organization = id;
}

uint16_t Message::getSourceOrganization(void)
{
	return ( hdr.source_organization );
}

void Message::setTargetIdentifier(uint32_t id)
{
	hdr.target_ident = id;
}

uint32_t Message::getTargetIdentifier(void)
{
	return ( hdr.target_ident );
}

void Message::setTargetOrganization(uint16_t id)
{
	hdr.target_organization = id;
}

uint16_t Message::getTargetOrganization(void)
{
	return ( hdr.target_organization );
}

void Message::setTransactionIdentifier(uint16_t id)
{
	hdr.transaction_ident = id;
}

uint16_t Message::getTransactionIdentifier(void)
{
	return ( hdr.transaction_ident );
}

void Message::setTransactionApplication(uint16_t id)
{
	hdr.transaction_application = id;
}

uint16_t Message::getTransactionApplication(void)
{
	return ( hdr.transaction_application );
}

void Message::setTransactionOrganization(uint16_t id)
{
	hdr.transaction_organization = id;
}

uint16_t Message::getTransactionOrganization(void)
{
	return ( hdr.transaction_organization );
}

void Message::setTTL(uint32_t val)
{
	hdr.ttl = val;
}

uint32_t Message::getTTL(void)
{
	return ( hdr.ttl );
}

void Message::setOnceAndOnlyOnce(bool val)
{
	if ( val )
		hdr.flags = hdr.flags | MSG_FLAG_ONCE_AND_ONLY_ONCE;
	else
		hdr.flags = hdr.flags & ~MSG_FLAG_ONCE_AND_ONLY_ONCE;
}

bool Message::isOnceAndOnlyOnce(void)
{
	return (hdr.flags & MSG_FLAG_ONCE_AND_ONLY_ONCE) > 0 ? true : false;
}

void Message::setQuickDeath(bool val)
{
	if ( val )
		hdr.flags = hdr.flags | MSG_FLAG_QUICK_DEATH;
	else
		hdr.flags = hdr.flags & ~MSG_FLAG_QUICK_DEATH;
}

bool Message::isQuickDeath(void)
{
	return (hdr.flags & MSG_FLAG_QUICK_DEATH) > 0 ? true : false;
}

void Message::setMessageFragment(bool val)
{
	if ( val )
		hdr.flags = hdr.flags | MSG_FLAG_FRAGMENT;
	else
		hdr.flags = hdr.flags & ~MSG_FLAG_FRAGMENT;
}

bool Message::isMessageFragment(void)
{
	return (hdr.flags & MSG_FLAG_FRAGMENT) > 0 ? true : false;
}

size_t Message::getMessageLength(void)
{
	return ( data_length );
}

void Message::debugMessageSerialize(std::string fo)
{
	FILE *fd;
	if ( ( fd = fopen(fo.c_str(), "wb") ) != nullptr)
	{
		fwrite(&data, data_length, 1, fd);
		fclose(fd);
	}
}

uint8_t Message::getDataType(void)
{
	//  uint8_t don't worry about endianess
	uint8_t val;
	memcpy(&val, &data[offset], sizeof(DATA_TYPE));
	return val;
}

void Message::debugMessagePrint(void)
{
	int dt;
	std::string s;

	std::cout << "Debug message printout" << std::endl;
	std::cout << "======================" << std::endl;
	std::cout << "<message>" << std::endl;
	std::cout << "    <header>" << std::endl;
	std::cout << "        <source_identifier>" << getSourceIdentifier() << "</source_identifier>" << std::endl;
	std::cout << "        <source_organization>" << getSourceOrganization() << "</source_organization>" << std::endl;
	std::cout << "        <target_identifier>" << getTargetIdentifier() << "</target_identifier>" << std::endl;
	std::cout << "        <target_organization>" << getTargetOrganization() << "</target_organization>" << std::endl;
	std::cout << "        <transaction_identifier>" << getTransactionIdentifier() << "</transaction_identifier>" << std::endl;
	std::cout << "        <transaction_application>" << getTransactionApplication() << "</transaction_application>" << std::endl;
	std::cout << "        <transaction_organization>" << getTransactionOrganization() << "</transaction_organization>" << std::endl;
	std::cout << "        <ttl>" << getTTL() << "</ttl>" << std::endl;
	std::cout << "        <is_once>" << ((isOnceAndOnlyOnce())?"true":"false") << "</is_once>" << std::endl;
	std::cout << "        <is_quick_death>" << ((isQuickDeath())?"true":"false") << "</is_quick_death>" << std::endl;
	std::cout << "        <is_fragment>" << ((isMessageFragment())?"true":"false") << "</is_fragment>" << std::endl;
	std::cout << "    </header>" << std::endl;
	std::cout << "    <data>" << std::endl;

	while ( offset < data_length)
	{
		dt = (int) getDataType();
		switch ( dt )
		{
		case DATA_TYPE_BOOL :
			std::cout << "        <bool>" << std::boolalpha << getBool() << "</bool>" << std::endl;
			break;
		case DATA_TYPE_BYTE :
			std::cout << "        <byte>" << std::hex << getByte() << "</byte>" << std::endl;
			break;
		case DATA_TYPE_SHORT :
			std::cout << "        <short>" << getShort() << "</short>" << std::endl;
			break;
		case DATA_TYPE_INT :
			std::cout << "        <int>" << getInt() << "</int>" << std::endl;
			break;
		case DATA_TYPE_LONG :
			std::cout << "        <long>" << getLong() << "</long>" << std::endl;
			break;
		case DATA_TYPE_LONG_LONG :
			std::cout << "        <long_long>" << getLongLong() << "</long_long>" << std::endl;
			break;
		case DATA_TYPE_FLOAT :
			std::cout << "        <float>" << std::fixed << std::setprecision(3) << getFloat() << "</float>" << std::endl;
			break;
		case DATA_TYPE_DOUBLE :
			std::cout << "        <double>" << getDouble() << "</double>" << std::endl;
			break;
		case DATA_TYPE_CHAR :
			std::cout << "        <char>" << getChar() << "</char>" << std::endl;
			break;
		case DATA_TYPE_WCHAR :
			std::cout << "        <wchar>" << getWChar() << "</wchar>" << std::endl;
			break;
		case DATA_TYPE_STRING_1 :
		case DATA_TYPE_STRING_2 :
			std::cout << "        <string>" << getString() << "</string>" << std::endl;
			break;
		case DATA_TYPE_WSTRING_1 :
		case DATA_TYPE_WSTRING_2 :
			std::wcout << "        <wstring>" << getWString() << "</wstring>" << std::endl;
			break;
		case DATA_TYPE_TIME :
			struct tm *time_info;
			time_t t;
			t = getTime();
			time_info = gmtime(&t);
			std::cout << "        <time>" << std::endl;
			std::cout << "            <seconds>" << time_info->tm_sec << "</seconds>" << std::endl;
			std::cout << "            <minutes>" << time_info->tm_min << "</minutes>" << std::endl;
			std::cout << "            <hours>" << time_info->tm_hour << "</hours>" << std::endl;
			std::cout << "            <day_of_month>" << time_info->tm_mday << "</day_of_month>" << std::endl;
			std::cout << "            <month>" << time_info->tm_mon << "</month>" << std::endl;
			std::cout << "            <day_of_week>" << time_info->tm_wday << "</day_of_week>" << std::endl;
			std::cout << "            <day_of_year>" << time_info->tm_yday << "</day_of_year>" << std::endl;
			std::cout << "            <year>" << time_info->tm_year << "</year>" << std::endl;
			std::cout << "            <time_zone>" << time_info->tm_zone << "</time_zone>" << std::endl;
			std::cout << "            <is_dst>" << ((time_info->tm_isdst) ? "true" : "false") << "</is_dst>" << std::endl;
			std::cout << "        </time>" << std::endl;
			break;
		case DATA_TYPE_DURATION :
			std::cout << "        <duration>" << getDuration() << "</duration>" << std::endl;
			break;
		case DATA_TYPE_BOOL_ARRAY :
			break;
		case DATA_TYPE_BYTE_ARRAY :
			break;
		case DATA_TYPE_SHORT_ARRAY :
			break;
		case DATA_TYPE_INT_ARRAY :
			break;
		case DATA_TYPE_LONG_ARRAY :
			break;
		case DATA_TYPE_LONG_LONG_ARRAY :
			break;
		case DATA_TYPE_FLOAT_ARRAY :
			break;
		case DATA_TYPE_DOUBLE_ARRAY :
			break;
		case DATA_TYPE_STRING_ARRAY :
			break;
		case DATA_TYPE_WSTRING_ARRAY :
			break;
		case DATA_TYPE_TIME_ARRAY :
			break;
		case DATA_TYPE_DURRATION_ARRAY :
			break;
		default :
			throw std::domain_error( "Unknown Data Type;" );
		}
	}

	std::cout << "    </data>" << std::endl;
	std::cout << "</message>" << std::endl;
}

/* Protected Methods */

void Message::putBool(bool val)
{
	if ( data_length + sizeof(DATA_TYPE) + sizeof(uint8_t) <= MAX_MSG_DATA )
	{
		memcpy( &data[data_length], &DATA_TYPE_BOOL, sizeof(DATA_TYPE));
		updateDataLength( sizeof( DATA_TYPE ) );

		uint8_t v = ( val ) ? 1 : 0;
		memcpy( &data[data_length], &v, sizeof(v) );
		updateDataLength( sizeof( v ) );
	}
	else
	{
		throw std::domain_error( "exceeded maximum message size" );
	}
}

void Message::putByte(int8_t val)
{
	if ( data_length + sizeof(DATA_TYPE) + sizeof(val) <= MAX_MSG_DATA )
	{
		memcpy( &data[data_length], &DATA_TYPE_BYTE, sizeof(DATA_TYPE));
		updateDataLength( sizeof( DATA_TYPE ) );

		memcpy( &data[data_length], &val, sizeof(val) );
		updateDataLength( sizeof( val ) );
	}
	else
	{
		throw std::domain_error( "exceeded maximum message size" );
	}
}

void Message::putShort(int16_t val)
{
	boost::endian::little_int16_buf_t nval;

	if ( data_length + sizeof(DATA_TYPE) + sizeof(val) <= MAX_MSG_DATA )
	{
		memcpy( &data[data_length], &DATA_TYPE_SHORT, sizeof(DATA_TYPE));
		updateDataLength( sizeof( DATA_TYPE ) );


		nval = val;
		memcpy( &data[data_length], &nval, sizeof(nval) );
		updateDataLength( sizeof( nval ) );
	}
	else
	{
		throw std::domain_error( "exceeded maximum message size" );
	}
}

void Message::putInt(int32_t val)
{
	boost::endian::little_int32_buf_t nval;
	if ( data_length + sizeof(DATA_TYPE) + sizeof(val) <= MAX_MSG_DATA )
	{
		memcpy( &data[data_length], &DATA_TYPE_INT, sizeof(DATA_TYPE));
		updateDataLength( sizeof( DATA_TYPE ) );

		nval = val;
		memcpy( &data[data_length], &nval, sizeof(nval) );
		updateDataLength( sizeof( nval ) );
	}
	else
	{
		throw std::domain_error( "exceeded maximum message size" );
	}
}

void Message::putLong(int32_t val)
{
	boost::endian::little_int32_buf_t nval;
	if ( data_length + sizeof(DATA_TYPE) + sizeof(val) <= MAX_MSG_DATA )
	{
		memcpy( &data[data_length], &DATA_TYPE_LONG, sizeof(DATA_TYPE));
		updateDataLength( sizeof( DATA_TYPE ) );

		nval = val;
		memcpy( &data[data_length], &nval, sizeof(nval) );
		updateDataLength( sizeof( nval ) );
	}
	else
	{
		throw std::domain_error( "exceeded maximum message size" );
	}
}

void Message::putLongLong(int64_t val)
{
	boost::endian::little_int64_buf_t nval;
	if ( data_length + sizeof(DATA_TYPE) + sizeof(val) <= MAX_MSG_DATA )
	{
		memcpy( &data[data_length], &DATA_TYPE_LONG_LONG, sizeof(DATA_TYPE));
		updateDataLength( sizeof( DATA_TYPE ) );

		nval = val;
		memcpy( &data[data_length], &nval, sizeof(nval) );
		updateDataLength( sizeof( nval ) );
	}
	else
	{
		throw std::domain_error( "exceeded maximum message size" );
	}
}








void Message::putFloat(float val)
{
	if ( data_length + sizeof(DATA_TYPE) + sizeof(val) <= MAX_MSG_DATA )
	{
		memcpy( &data[data_length], &DATA_TYPE_FLOAT, sizeof(DATA_TYPE));
		updateDataLength( sizeof( DATA_TYPE ) );

		memcpy( &data[data_length], &val, sizeof(val) );
		updateDataLength( sizeof( val ) );
	}
	else
	{
		throw std::domain_error( "exceeded maximum message size" );
	}
}

void Message::putDouble(double val)
{
	if ( data_length + sizeof(DATA_TYPE) + sizeof(val) <= MAX_MSG_DATA )
	{
		memcpy( &data[data_length], &DATA_TYPE_DOUBLE, sizeof(DATA_TYPE));
		updateDataLength( sizeof( DATA_TYPE ) );

		memcpy( &data[data_length], &val, sizeof(val) );
		updateDataLength( sizeof( val ) );
	}
	else
	{
		throw std::domain_error( "exceeded maximum message size" );
	}
}








void Message::putChar(char val)
{
	if ( data_length + sizeof(DATA_TYPE) + sizeof(val) <= MAX_MSG_DATA )
	{
		memcpy( &data[data_length], &DATA_TYPE_CHAR, sizeof(DATA_TYPE));
		updateDataLength( sizeof( DATA_TYPE ) );

		memcpy( &data[data_length], &val, sizeof(val) );
		updateDataLength( sizeof( val ) );
	}
	else
	{
		throw std::domain_error( "exceeded maximum message size" );
	}
}

void Message::putWChar(wchar_t val)
{
	if ( data_length + sizeof(DATA_TYPE) + sizeof(val) <= MAX_MSG_DATA )
	{
		memcpy( &data[data_length], &DATA_TYPE_WCHAR, sizeof(DATA_TYPE));
		updateDataLength( sizeof( DATA_TYPE ) );

		memcpy( &data[data_length], &val, sizeof(val) );
		updateDataLength( sizeof( val ) );
	}
	else
	{
		throw std::domain_error( "exceeded maximum message size" );
	}
}

void Message::putString(std::string val)
{
	size_t l = val.length();
	uint8_t dt =  l > 255 ? DATA_TYPE_STRING_2 : DATA_TYPE_STRING_1;
	size_t elem_size = (size_t) l * sizeof(char);

	switch ( dt )
	{
	case DATA_TYPE_STRING_1 :
		if ( data_length + sizeof(DATA_TYPE) + sizeof(uint8_t) + elem_size <= MAX_MSG_DATA )
		{
			memcpy( &data[data_length], &dt, sizeof( DATA_TYPE ) );
			updateDataLength( sizeof( DATA_TYPE ) );

			memcpy( &data[data_length], &l, sizeof( uint8_t ) );
			updateDataLength( sizeof( uint8_t ) );

			memcpy( &data[data_length], val.c_str(), elem_size );
			updateDataLength( elem_size );
		}
		else
		{
			throw std::domain_error( "exceeded maximum message size" );
		}
		break;

	case DATA_TYPE_STRING_2 :
		if ( data_length + sizeof(DATA_TYPE) + sizeof(uint16_t) + l * sizeof(char) <= MAX_MSG_DATA )
		{
			boost::endian::little_uint16_buf_t nval;

			memcpy( &data[data_length], &dt, sizeof(DATA_TYPE));
			updateDataLength( sizeof( DATA_TYPE ) );

			nval = (uint16_t) l;
			memcpy( &data[data_length], &nval, sizeof( nval ) );
			updateDataLength( sizeof( nval ) );

			memcpy( &data[data_length], val.c_str(), (size_t)(l * sizeof(char)) );
			updateDataLength( ( l * sizeof( char ) ) );
		}
		else
		{
			throw std::domain_error( "exceeded maximum message size" );
		}
		break;
	default:
		throw std::domain_error( "exceeded maximum message size" );
	}
}

void Message::putWString(std::wstring val)
{
	size_t l = (size_t) val.length();
	uint8_t dt =  l > 255 ? DATA_TYPE_WSTRING_2 : DATA_TYPE_WSTRING_1;

	switch ( dt )
	{
	case DATA_TYPE_WSTRING_1 :
		if ( data_length + sizeof(DATA_TYPE) + sizeof(uint8_t) + l * sizeof(wchar_t) <= MAX_MSG_DATA )
		{
			memcpy( &data[data_length], &dt, sizeof(DATA_TYPE));
			updateDataLength( sizeof( DATA_TYPE ) );

			uint8_t l1 = ( uint8_t ) l;
			memcpy( &data[data_length], &l1, sizeof( l1 ) );
			updateDataLength( sizeof( l1 ) );

			memcpy( &data[data_length], val.c_str(), (size_t)(l * sizeof(wchar_t)) );
			updateDataLength( ( l * sizeof(wchar_t) ) );
		}
		else
		{
			throw std::domain_error( "exceeded maximum message size" );
		}
		break;

	case DATA_TYPE_WSTRING_2 :
		if ( data_length + sizeof(DATA_TYPE) + sizeof(uint16_t) + l * sizeof(wchar_t) <= MAX_MSG_DATA )
		{
			boost::endian::little_uint16_buf_t nval;

			memcpy( &data[data_length], &dt, sizeof(DATA_TYPE));
			updateDataLength( sizeof( DATA_TYPE ) );

			nval = (uint16_t) l;
			memcpy( &data[data_length], &nval, sizeof( nval ) );
			updateDataLength( sizeof( nval ) );

			memcpy( &data[data_length], val.c_str(), (size_t)(l * sizeof(wchar_t)) );
			updateDataLength( ( l * sizeof( wchar_t ) ) );

			data_length += (l * sizeof(wchar_t));
		}
		else
		{
			throw std::domain_error( "exceeded maximum message size" );
		}
		break;
	default:
		throw std::domain_error( "exceeded maximum message size" );
	}
}

void Message::putTime(time_t val)
{
	if ( data_length + sizeof(DATA_TYPE) + sizeof(val) <= MAX_MSG_DATA )
	{
		boost::endian::little_uint64_buf_t nval;

		memcpy( &data[data_length], &DATA_TYPE_TIME, sizeof(DATA_TYPE));
		updateDataLength( sizeof( DATA_TYPE ) );

		nval = val;
		memcpy( &data[data_length], &nval, sizeof(nval) );
		updateDataLength( sizeof( nval ) );
	}
	else
	{
		throw std::domain_error( "exceeded maximum message size" );
	}
}

void Message::putDuration(uint32_t val)
{
	if ( data_length + sizeof(DATA_TYPE) + sizeof(val) <= MAX_MSG_DATA )
	{
		boost::endian::little_uint32_buf_t nval;

		memcpy( &data[data_length], &DATA_TYPE_DURATION, sizeof(DATA_TYPE));
		updateDataLength( sizeof( DATA_TYPE ) );

		nval = val;
		memcpy( &data[data_length], &nval, sizeof(nval) );
		updateDataLength( sizeof( nval ) );
	}
	else
	{
		throw std::domain_error( "exceeded maximum message size" );
	}
}

void Message::putBoolArray(bool *arr)
{

}

void Message::putByteArray(int8_t *arr)
{

}

void Message::putShortArray(int16_t *arr)
{

}

void Message::putIntArray(int32_t *arr)
{

}

void Message::putLongArray(int32_t *arr)
{

}

void Message::putLongLongArray(int64_t *arr)
{

}

void Message::putFloatArray(int32_t *arr)
{

}

void Message::putDoubleArray(int64_t *arr)
{

}

void Message::putStringArray(std::string *arr)
{

}

void Message::putTimeArray(time_t *arr)
{

}

void Message::putDurationArray(uint32_t *arr)
{

}


bool Message::getBool(void)
{
	int8_t data_type = 0;
	int8_t val = 0;
	bool ret_val;

	memcpy( &data_type, &data[offset], sizeof( DATA_TYPE ));
	offset += sizeof( DATA_TYPE );
	assert ( data_type == DATA_TYPE_BOOL );

	memcpy( &val, &data[offset], sizeof( val ) );
	offset += sizeof( val );
	ret_val = (bool) ( val != 0 ) ? true : false;

	return ret_val;
}

int8_t Message::getByte(void)
{
	int8_t data_type = 0;
	int8_t val = 0;

	memcpy( &data_type, &data[offset], sizeof( DATA_TYPE ));
	offset += sizeof( DATA_TYPE );
	assert ( data_type == DATA_TYPE_BYTE );

	memcpy( &val, &data[offset], sizeof( val ) );
	offset += sizeof( val );

	return val;
}

int16_t Message::getShort(void)
{
	int8_t data_type = 0;
	boost::endian::little_int16_buf_t nval;

	memcpy( &data_type, &data[offset], sizeof( DATA_TYPE ));
	offset += sizeof( DATA_TYPE );
	assert ( data_type == DATA_TYPE_SHORT );

	memcpy( &nval, &data[offset], sizeof( nval ) );
	offset += sizeof( nval );

	return ( nval.value() );
}

int32_t Message::getInt(void)
{
	int8_t data_type = 0;
	boost::endian::little_int32_buf_t nval;

	memcpy( &data_type, &data[offset], sizeof( DATA_TYPE ));
	offset += sizeof( DATA_TYPE );
	assert ( data_type == DATA_TYPE_INT );

	memcpy( &nval, &data[offset], sizeof( nval ) );
	offset += sizeof( nval );

	return ( nval.value() );
}

int32_t Message::getLong(void)
{
	int8_t data_type = 0;
	boost::endian::little_int16_buf_t nval;

	memcpy( &data_type, &data[offset], sizeof( DATA_TYPE ));
	offset += sizeof( DATA_TYPE );
	assert ( data_type == DATA_TYPE_LONG );

	memcpy( &nval, &data[offset], sizeof( nval ) );
	offset += sizeof( nval );

	return ( nval.value() );
}

int64_t Message::getLongLong(void)
{
	int8_t data_type = 0;
	boost::endian::little_int64_buf_t nval;

	memcpy( &data_type, &data[offset], sizeof( DATA_TYPE ));
	offset += sizeof( DATA_TYPE );
	assert ( data_type == DATA_TYPE_LONG_LONG );

	memcpy( &nval, &data[offset], sizeof( nval ) );
	offset += sizeof( nval );

	return ( nval.value() );
}








float Message::getFloat(void)
{
	int8_t data_type = 0;
	float val = 0;

	memcpy( &data_type, &data[offset], sizeof( DATA_TYPE ));
	offset += sizeof( DATA_TYPE );
	assert ( data_type == DATA_TYPE_FLOAT );

	memcpy( &val, &data[offset], sizeof( val ) );
	offset += sizeof( val );

	return val;
}

double Message::getDouble(void)
{
	int8_t data_type = 0;
	double val = 0;

	memcpy( &data_type, &data[offset], sizeof( DATA_TYPE ));
	offset += sizeof( DATA_TYPE );
	assert ( data_type == DATA_TYPE_DOUBLE );

	memcpy( &val, &data[offset], sizeof( val ) );
	offset += sizeof( val );

	return val;
}








char Message::getChar(void)
{
	int8_t data_type = 0;
	char val = '\0';

	memcpy( &data_type, &data[offset], sizeof( DATA_TYPE ));
	offset += sizeof( DATA_TYPE );
	assert ( data_type == DATA_TYPE_CHAR );

	memcpy( &val, &data[offset], sizeof( val ) );
	offset += sizeof( val );

	return val;
}

wchar_t Message::getWChar(void)
{
	int8_t data_type = 0;
	wchar_t val = L'\0';

	memcpy( &data_type, &data[offset], sizeof( DATA_TYPE ));
	offset += sizeof( DATA_TYPE );
	assert ( data_type == DATA_TYPE_WCHAR );

	memcpy( &val, &data[offset], sizeof( val ) );
	offset += sizeof( val );

	return val;
}

std::string Message::getString(void)
{
	int8_t data_type = 0;
	boost::endian::little_int8_buf_t s1len;
	boost::endian::little_int16_buf_t s2len;
	size_t string_len = 0;
	size_t elem_size = 0;
	int sv = 0;
	char *ptr = nullptr;
	std::string val;

	memcpy( &data_type, &data[offset], sizeof( DATA_TYPE ));
	offset += sizeof( DATA_TYPE );
	assert ( (data_type == DATA_TYPE_STRING_1) | (data_type == DATA_TYPE_STRING_2) );

	sv = (int) data_type;
	switch (sv)
	{
	case DATA_TYPE_STRING_1 :
		memcpy( &s1len, &data[offset], sizeof( s1len ));
		offset += sizeof( s1len );
		string_len = (size_t) s1len.value();
		break;
	case DATA_TYPE_STRING_2 :
		memcpy( &s2len, &data[offset], sizeof( s2len ));
		offset += sizeof( s2len );
		string_len = (size_t) s2len.value();
		break;
	default :
		break;
	}

	ptr = (char *) malloc( ((string_len + 1)*sizeof(char)) );
	assert ( ptr != nullptr );
	memset( ptr, 0, ((string_len + 1)*sizeof(char)) );

	elem_size = string_len * sizeof(char);
	memcpy( ptr, &data[offset], elem_size );
	offset += elem_size;

	val.assign(ptr);
	return val;
}

std::wstring Message::getWString(void)
{
	int8_t data_type = 0;
	boost::endian::little_int8_buf_t s1len;
	boost::endian::little_int16_buf_t s2len;

	size_t wstring_len;
	int sv = 0;
	wchar_t *ptr = nullptr;
	std::wstring val;

	memcpy( &data_type, &data[offset], sizeof( DATA_TYPE ));
	offset += sizeof( DATA_TYPE );
	assert ( (data_type == DATA_TYPE_WSTRING_1) | (data_type == DATA_TYPE_WSTRING_2) );

	sv = (int) data_type;
	switch (sv)
	{
	case DATA_TYPE_WSTRING_1 :
		memcpy( &s1len, &data[offset], sizeof( s1len ));

		offset += sizeof( s1len );
		wstring_len = (size_t) s1len.value();
		break;
	case DATA_TYPE_WSTRING_2 :
		memcpy( &s2len, &data[offset], sizeof( s2len ));

		offset += sizeof( s2len );
		wstring_len = (size_t) s2len.value();
		break;
	default :
		wstring_len = 0;
		break;
	}

	ptr = (wchar_t *) malloc( ((wstring_len + 1)*sizeof(wchar_t)) );
	memset( ptr, 0, ((wstring_len + 1)*sizeof(char)) );
	assert( ptr != nullptr );

	memcpy( ptr, &data[offset], (size_t)(wstring_len * sizeof(wchar_t)) );
	offset += (wstring_len * sizeof(wchar_t));

	val.assign( ptr );
	return val;
}

time_t Message::getTime(void)
{
	int8_t data_type = 0;
	boost::endian::little_int64_buf_t nval;
	time_t val = 0;

	memcpy( &data_type, &data[offset], sizeof( DATA_TYPE ));
	offset += sizeof( DATA_TYPE );
	assert ( data_type == DATA_TYPE_TIME );

	memcpy( &nval, &data[offset], sizeof( nval ) );
	offset += sizeof( nval );

	val = (time_t) nval.value();
	return val;
}

uint32_t Message::getDuration(void)
{
	int8_t data_type = 0;
	boost::endian::little_int32_buf_t nval;

	memcpy( &data_type, &data[offset], sizeof( DATA_TYPE ));
	offset += sizeof( DATA_TYPE );
	assert ( data_type == DATA_TYPE_DURATION );

	memcpy( &nval, &data[offset], sizeof( nval ) );
	offset += sizeof( nval );

	return ( nval.value() );
}

} /* namespace kcmsg */
