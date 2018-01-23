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
	  std::vector<std::string> kp;
	  std::vector<std::string> vp;
	  std::vector<uint> num;
	  std::unordered_map<std::string,kcmsg::Property> properties;

	  void mapProperties( boost::property_tree::ptree pt );
	  void insertProperty( void );
	  void debugPrintPropertyTree( boost::property_tree::ptree pt );

	public:
	  /* Configuration factory which reads in global properties from
	   * a 'config.json' file in the directory '/etc/kcmsg' and user properties
	   * from 'config.json' in the directory '{username}/.kcmsg' */
	  Configuration();

	  /* Configuration factory accepts a string which must contain a valid directory or
	   * subdirectory name.  The expectation is to pass this function argv(0).  If argv(0)
	   * contains periods ("."), the final period and substring to the right of the period
	   * are removed.  Example: passing the procname of "a.out" converts procname to "a".
	   * The function is a configuration factory which reads in global properties from
	   * a 'config.json' file in the directory '/etc/{procname}' and user properties
		   * from 'config.json' in the directory '{username}/.{procname}' */
//	  Configuration(std::string progName);

		  /* Configuration factory accepts two strings which must contain a valid directory or
		   * subdirectory names.  gblDirectory is the directory in which glabal configuration information is
		   * placed.  procName also must be a valid directory or subdirectory name.  */

//	  Configuration(std::string gblDirectory, std::string progName);
	  virtual ~Configuration();

	  void readConfigurationFile(std::string conffile);
	  std::string getProperty(std::string key);
	  std::vector<std::string> getPropertyVector(std::string key);
	  bool propertyExists(std::string key);

	  void dumpProperties( void );
	};

} /* namespace kcmsg */

#endif /* CONFIGURATION_H_ */
