/*
 * Message.h
 *
 *  Created on: May 18, 2017
 *      Author: kurt
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <cstdint>
#include <ctime>
#include <cstring>
#include <string>

namespace kcmsg {

const size_t HEADER_LENGTH = 0x20;
const size_t MESSAGE_LENGTH_OFFSET = 0x20;

const uint32_t MAX_MSG_DATA = 0xFFFF; // 64K Bytes Maximum Message Data Size

const uint16_t MSG_FLAG_ONCE_AND_ONLY_ONCE = 1<<0;
const uint16_t MSG_FLAG_QUICK_DEATH = 1<<1;
const uint16_t MSG_FLAG_FRAGMENT = 1<<2;

struct MessageHeader
{
	uint64_t source_ident;          // source endpoint identifier
	uint64_t target_ident;          // target endpoint identifier
	uint32_t transaction_organization;   // target organization identifier
	uint32_t transaction_application;    // target application identifier
	uint32_t ttl;                   // time to live in seconds
	uint16_t transaction_ident;     // message type identifier
	uint16_t flags;
};

class Message {
private:
	char *data;		// complete message (header and data)
	size_t offset;		// current pointer into the message
	size_t data_length;	// current length of the complete message
	uint16_t network_data_length; // value packed into the message

// protected:

public:
	void setSourceIdentity(uint64_t id);
	uint64_t getSourceIdentity(void);
	void setTargetIdentity(uint64_t id);
	uint64_t getTargetIdentity(void);
	void setTargetOrganization(uint32_t id);
	uint32_t getTargetOrganization(void);
	void setTargetApplication(uint32_t id);
	uint32_t getTargetApplication(void);
	void setTTL(uint32_t val);
	uint32_t getTTL(void);
	void setTransactionIdentity(uint16_t id);
	uint16_t getTransactionIdentity(void);
	void setOnceAndOnlyOnce(bool val);
	bool isOnceAndOnlyOnce(void);
	void setQuickDeath(bool val);
	bool isQuickDeath(void);
	void setMessageFragment(bool val);
	bool isMessageFragment(void);

	void putBool(bool val);
	void putByte(int8_t val);
	void putShort(int16_t val);
	void putInt(int32_t val);
	void putLong(int32_t val);
	void putLongLong(int64_t val);
	void putFloat(int32_t val);
	void putDouble(int64_t val);
	void putChar(char val);
	void putWChar(wchar_t val);
	void putString(std::string val);
	void putWString(std::wstring val);
	void putTime(time_t val);
	void putDuration(uint32_t val);
	void putBoolArray(bool *arr);
	void putByteArray(int8_t *arr);
	void putShortArray(int16_t *arr);
	void putIntArray(int32_t *arr);
	void putLongArray(int32_t *arr);
	void putLongLongArray(int64_t *arr);
	void putFloatArray(int32_t *arr);
	void putDoubleArray(int64_t *arr);
	void putWCharArray(wchar_t *arr);
	void putStringArray(std::string *arr);
	void putTimeArray(time_t *arr);
	void putDurationArray(uint32_t *arr);

	bool getBool(void);
	int8_t getByte(void);
	int16_t getShort(void);
	int32_t getInt(void);
	int32_t getLong(void);
	int64_t getLongLong(void);
	int32_t getFloat(void);
	int64_t getDouble(void);
	char getChar(void);
	wchar_t getWChar(void);
	std::string getString(void);
	std::wstring getWString(void);
	time_t getTime(void);
	uint32_t getDuration(void);

	/*
	void put_bool_array(bool *arr);
	void put_byte_array(int8_t *arr);
	void put_short_array(int16_t *arr);
	void put_int_array(int32_t *arr);
	void put_long_array(int32_t *arr);
	void put_long_long_array(int64_t *arr);
	void put_float_array(int32_t *arr);
	void put_double_array(int64_t *arr);
	void put_wchar_array(wchar_t *arr);
	void put_string_array(std::string *arr);
	void put_time_array(time_t *arr);
	void put_duration_array(uint32_t *arr);
	*/

 // public:
	Message();
	virtual ~Message();

	void getHeader(kcmsg::MessageHeader *hdr);
	size_t getLength(void);

	void message_send(void);
	void message_serialize(void);

	void debug_print_message_data(void);

	void updateDataLength(std::size_t delta);
	void setFlags(uint16_t val);
	uint16_t getFlags(void);

	uint8_t getDataType(void);


};

} /* namespace kcmsg */

#endif /* MESSAGE_H_ */
