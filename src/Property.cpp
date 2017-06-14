/*
 * Property.cpp
 *
 *  Created on: May 18, 2017
 *      Author: kurt
 */

#include <cstdint>
#include <string>
#include <cstring>
#include <array>
#include <memory>
#include <iostream>
#include <vector>

#include "Property.h"

namespace kcmsg {

Property::Property() {
  value_type = kcmsg::PROP_TYPE_NULL;
  value = nullptr;
}

Property::Property(const uint8_t ptype,  void *v ) {
  value_type = ptype;
  value = v;
}

Property::~Property() {
  // TODO Auto-generated destructor stub
  if( value != nullptr )
  {
    int i = value_type;

     switch ( i )
    {
      case kcmsg::PROP_TYPE_STRING :
      {
        std::string *sp;
        sp = static_cast<std::string*>(value);
        delete sp;
      }
        break;
      case kcmsg::PROP_TYPE_STRING_ARRAY :
      {
    	  std::vector<std::string> *vp;
    	  vp = static_cast<std::vector<std::string>*>(value);
    	  delete vp;
      }
        break;
      default:
        break;
    }
  }
}

void Property::setType(uint8_t ptype)
{
	value_type = ptype;
}

void Property::setValue(std::string s)
{
  std::string *sp = static_cast<std::string*>(value);
  *sp = s;
}

void Property::setValueVector(std::vector<std::string> values)
{
	std::vector<std::string> *vp = &values;
	value = static_cast<void *>(vp);
}

void Property::dumpProperty( void )
{
	std::string pt;
	switch ( value_type )
	{
	case PROP_TYPE_NULL :
		pt.assign("PROP_TYPE_NULL");
		break;
	case PROP_TYPE_STRING :
		pt.assign("PROP_TYPE_STRING");
		break;
	case PROP_TYPE_STRING_ARRAY :
		pt.assign("PROP_TYPE_STRING_ARRAY");
		break;
	default:
		pt.assign("PROP_TYPE_UNKNOWN");
		break;
	}

	std::cout << "( " << pt << " ) ";

	switch ( value_type )
	{
	case PROP_TYPE_STRING :
	{
		std::string *s;
		s = static_cast<std::string *>(value);
		std::cout << "\"" << *s << "\"" << std::endl << std::endl;
	}
		break;
	case PROP_TYPE_STRING_ARRAY :
	{
		std::vector<std::string> *v;
		v = static_cast<std::vector<std::string> *>(value);
		uint i = 0;
		for( auto it = v->begin(); it != v->end(); it++ )
		{
			if( i == 0 )
			{
				std::cout << "- [" << i << "] \"" << *it << "\"" << std::endl;
			} else
			{
			std::cout << "                              " << "- [" << i << "] \"" << *it << "\"" << std::endl;
			}
		}
		std::cout << std::endl;
	}
		break;
	default:
	{
		std::cout << " ????? " << std::endl;
	}
		break;
	}

}

uint8_t Property::getType( void )
{
	return value_type;
}

std::string Property::getValue(void)
{
  std::string *sp = static_cast<std::string*>(value);
  return *sp;
}

std::vector<std::string> Property::getValueVector(void)
{
	std::vector<std::string> *ret = static_cast<std::vector<std::string> *>(value);
	return *ret;
}

} /* namespace kcmsg */
