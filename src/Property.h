/*
 * Property.h
 *
 *  Created on: May 18, 2017
 *      Author: kurt
 */

#ifndef PROPERTY_H_
#define PROPERTY_H_

#include <cstdint>
#include <string>
#include <cstring>
#include <array>
#include <memory>
#include <vector>

namespace kcmsg {

const uint8_t PROP_TYPE_NULL = 0x00;
const uint8_t PROP_TYPE_BOOLEAN = 0x01;
const uint8_t PROP_TYPE_NUMBER = 0x02;
const uint8_t PROP_TYPE_STRING = 0x03;
const uint8_t PROP_TYPE_BOOLEAN_ARRAY = 0x04;
const uint8_t PROP_TYPE_NUMBER_ARRAY = 0x05;
const uint8_t PROP_TYPE_STRING_ARRAY = 0x06;

class Property {
private:
  uint8_t value_type;
  void *  value;

public:
  Property();
  Property(const uint8_t ptype, void *v );
  virtual ~Property();

  void setType(uint8_t ptype);
  void setValue(std::string s);
  void setValueVector(std::vector<std::string> values);
  uint8_t getType( void );
  std::string getValue(void);
  std::vector<std::string> getValueVector(void);

  void dumpProperty( void );
};

} /* namespace kcmsg */

#endif /* PROPERTY_H_ */
