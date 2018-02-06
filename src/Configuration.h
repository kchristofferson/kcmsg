/*
 * Configuration.h
 *
 *  Created on: May 18, 2017
 *      Author: Kurt Christofferson
 *
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_


#include <unordered_map>
#include <cstring>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Property.h"

namespace kcmsg {

class Configuration {
//	  std::vector<std::string> kp;
//	  std::vector<std::string> vp;
//	  std::vector<uint> num;
	  std::unordered_map<std::string,kcmsg::Property> properties;

	//  void mapProperties( boost::property_tree::ptree pt );
	//  void insertProperty( void );
	//  void debugPrintPropertyTree( boost::property_tree::ptree pt );

	public:
	  Configuration();
	  virtual ~Configuration();

	  int readConfigurationFile(std::string conffile);
	  std::string getProperty(std::string key);
	  std::vector<std::string> getPropertyVector(std::string key);
	  bool propertyExists(std::string key);

	  void dumpProperties( void );
	};

} /* namespace kcmsg */

#endif /* CONFIGURATION_H_ */
