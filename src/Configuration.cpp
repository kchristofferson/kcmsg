/*
 * Configuration.cpp
 *
 *  Created on: May 18, 2017
 *      Author: kurt
 */

#include "Configuration.h"
#include "Property.h"
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <string>
#include <iostream>
#include <cerrno>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

namespace kcmsg {

  /* Static Methods */
  void mapProperties( std::vector<std::string> kp, std::vector<std::string> vp, std::vector<uint> num, boost::property_tree::ptree pt, std::unordered_map<std::string,kcmsg::Property> props
 );
  void insertProperty( std::vector<std::string> kp, std::vector<std::string> vp, std::vector<uint> num, 	  std::unordered_map<std::string,kcmsg::Property> props );
  void debugPrintPropertyTree( boost::property_tree::ptree pt );


Configuration::Configuration() {

}

Configuration::~Configuration() {
  // TODO Auto-generated destructor stub
}

int Configuration::readConfigurationFile(std::string conffile)
{
	  boost::property_tree::ptree pt;
	  boost::filesystem::path cpath(conffile);
	  std::vector<std::string> kp;
	  std::vector<std::string> vp;
	  std::vector<uint> num;
	  std::unordered_map<std::string,kcmsg::Property> props;

	  kp.clear();
	  vp.clear();
	  num.clear();

	  if( boost::filesystem::exists( cpath ) )
	  {
		  if( boost::filesystem::is_regular_file(cpath) )
		  {
			  boost::property_tree::read_json(cpath.c_str(),pt);
			  mapProperties(kp, vp, num, pt, props);

		  }
	  }
	  else
		  return ENOENT;

	  properties += props;
	  return 0;
}

std::string Configuration::getProperty(std::string key)
{
	std::string ret;

	auto search = properties.find(key);
	if( search != properties.end() )
	{
		kcmsg::Property *prop = &search->second;
		switch ( prop->getType() )
		{
		case PROP_TYPE_NULL :
		case PROP_TYPE_BOOLEAN :
		case PROP_TYPE_NUMBER :
		case PROP_TYPE_STRING :
		{
			ret = static_cast<std::string>(prop->getValue());
			break;
		}
		case PROP_TYPE_BOOLEAN_ARRAY :
		case PROP_TYPE_NUMBER_ARRAY :
		case PROP_TYPE_STRING_ARRAY :

		default:
			break;
		}
	}

	return ret;
}

std::vector<std::string> Configuration::getPropertyVector(std::string key)
{
	std::vector<std::string> ret;

	auto search = properties.find(key);
	if( search != properties.end() )
	{
		kcmsg::Property *prop = &search->second;
		switch ( prop->getType() )
		{
		case PROP_TYPE_NULL :
		case PROP_TYPE_BOOLEAN :
		case PROP_TYPE_NUMBER :
		case PROP_TYPE_STRING :
			break;
		case PROP_TYPE_BOOLEAN_ARRAY :
		case PROP_TYPE_NUMBER_ARRAY :
		case PROP_TYPE_STRING_ARRAY :
		{
			ret = static_cast<std::vector<std::string>>(prop->getValueVector());
			break;
		}
		default:
			break;
		}
	}

	return ret;
}

bool Configuration::propertyExists(std::string key)
{
	auto search = properties.find(key);
	return ( search != properties.end() );
}

void Configuration::dumpProperties( void )
{
	for( auto it = properties.begin(); it != properties.end(); it++ )
	{
		std::cout << it->first << ": ";
		it->second.dumpProperty();
	}
}

/* Static Methods */

void kcmsg::mapProperties( std::vector<std::string> kp, std::vector<std::string> vp, std::vector<uint> num, const boost::property_tree::ptree pt, std::unordered_map<std::string,kcmsg::Property> props ) {
	int i;

	for (boost::property_tree::ptree::const_iterator it = pt.begin();
			it != pt.end(); it++) {

		num.push_back(it->second.size());
		if (!it->first.empty())
			kp.push_back(it->first);
		if (!it->second.data().empty())
			vp.push_back(it->second.data());

		i = num.back();
		if (i == 0) // I am at a property node
				{
			if (!it->first.empty()) // It is not an array
			{
				kcmsg::insertProperty( kp, vp, num, props );
				kp.pop_back();
				vp.clear();
				num.end()[-2]--;
			} else // I am in an array
			{
				num.end()[-2]--;
				if (num.end()[-2] == 0) // I have all elements of the array
						{
					kcmsg::insertProperty( kp, vp, num, props );
					vp.clear();
				}
			}
			num.pop_back();
		}
		while ( ( !num.empty() ) && (num.back() == 0) ) {
			num.pop_back();
			kp.pop_back();
			if( !num.empty() )
				num.end()[-1]--;
		}
		kcmsg::mapProperties(kp, vp, num, it->second, props);
	}
}

void kcmsg::insertProperty( std::vector<std::string> kp, std::vector<std::string> vp, std::vector<uint> num, 	  std::unordered_map<std::string,kcmsg::Property> props )
{
	std::string key;

	for( std::vector<std::string>::iterator it = kp.begin(); it != kp.end(); it++ )
	{
		if( !key.empty() )
			key.append(".");
		key.append( *it );
	}

	if( vp.size() == 0 ) // should never happen.  It would be an empty value
	{
		kp.pop_back();
	} else if (vp.size() == 1 )  // have a single value
	{
		void *ptr = static_cast<void *>(new std::string(vp.front()));
		kcmsg::Property *p = new kcmsg::Property(PROP_TYPE_STRING, ptr);
		props[key] = *p;
	} else  // I'm dealing with a vector
	{
		void *ptr = static_cast<void *>( new std::vector<std::string>(vp) );
		kcmsg::Property *p = new kcmsg::Property(PROP_TYPE_STRING_ARRAY, ptr );
		props[key] = *p;
	}
}

void kcmsg::debugPrintPropertyTree( boost::property_tree::ptree pt )
{
	for (boost::property_tree::ptree::const_iterator it = pt.begin();
			it != pt.end(); it++) {

		std::cout << "first: " << it->first << std::endl;
		std::cout << "second: " << it->second.data() << std::endl;
		std::cout << "second size: " << it->second.size() << std::endl << std::endl;
	}
}

} /* namespace kcmsg */
