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

const uint32_t MAX_MSG_DATA = 0xFFFF; // 64K Bytes Maximum Message Data Size

const uint16_t MSG_FLAG_ONCE_AND_ONLY_ONCE = 1<<0;
const uint16_t MSG_FLAG_QUICK_DEATH = 1<<1;
const uint16_t MSG_FLAG_FRAGMENT = 1<<2;

struct MessageHeader
{
	uint32_t source_ident;          // source endpoint identifier
	uint16_t source_organization;   // source organization identifier
	uint32_t target_ident;          // target endpoint identifier
	uint16_t target_organization;          // target endpoint identifier
	uint16_t transaction_ident;     // message type identifier
	uint16_t transaction_application;    // target application identifier
	uint16_t transaction_organization;   // target organization identifier
	uint32_t ttl;                   // time to live in seconds
	uint16_t flags;
};

class Message {
private:
	char *data;		// complete message (header and data)
	size_t offset;		// current pointer into the message
	size_t data_length;	// current length of the complete message
	kcmsg::MessageHeader hdr;

	void readHeader(void);
	void writeHeader(void);
	void updateMessageLength(std::size_t delta);

//	void readMessage(void);
//	void writeMessage(void);
// protected:






public:
	Message();
	virtual ~Message();

	void readMessageLength(void);

	void setSourceIdentifier(uint32_t id);
	uint32_t getSourceIdentifier(void);
	void setSourceOrganization(uint16_t id);
	uint16_t getSourceOrganization(void);
	void setTargetIdentifier(uint32_t id);
	uint32_t getTargetIdentifier(void);
	void setTargetOrganization(uint16_t id);
	uint16_t getTargetOrganization(void);
	void setTransactionIdentifier(uint16_t id);
	uint16_t getTransactionIdentifier(void);
	void setTransactionApplication(uint16_t id);
	uint16_t getTransactionApplication(void);
	void setTransactionOrganization(uint16_t id);
	uint16_t getTransactionOrganization(void);
	void setTTL(uint32_t val);
	uint32_t getTTL(void);
	void setOnceAndOnlyOnce(bool val);
	bool isOnceAndOnlyOnce(void);
	void setQuickDeath(bool val);
	bool isQuickDeath(void);
	void setMessageFragment(bool val);
	bool isMessageFragment(void);
	size_t getMessageLength(void);

	void putBool(bool val);
	void putByte(int8_t val);
	void putShort(int16_t val);
	void putInt(int32_t val);
	void putLong(int32_t val);
	void putLongLong(int64_t val);
	void putFloat(float val);
	void putDouble(double val);
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
	float getFloat(void);
	double getDouble(void);
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

//	void message_send(void);
	void debugMessageSerialize(std::string fo);
	void debugMessagePrint(void);

	void updateDataLength(std::size_t delta);
	void setFlags(uint16_t val);
	uint16_t getFlags(void);

	uint8_t getDataType(void);


};

} /* namespace kcmsg */

#endif /* MESSAGE_H_ */
