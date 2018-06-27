/*
 *  NetworkInterface.cpp
 *  \
 *  Created on : June 25, 2018
 *       Author: Kurt Christofferson
 */

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>

