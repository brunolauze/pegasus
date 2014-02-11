//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%////////////////////////////////////////////////////////////////////////////

#include "UNIX_IPEndPoint.h"
#include <sys/cdefs.h>

#include <sys/param.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_var.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/radix.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netipx/ipx.h>
#include <netatalk/at.h>
#include <netgraph/ng_socket.h>

#include <sys/sysctl.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <kvm.h>
#include <libutil.h>
#include <netdb.h>
#include <nlist.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>
#include <err.h>

#define _KERNEL 1
#include <netinet/ip_mroute.h>
#undef _KERNEL

#define INET6 1

char*
get_address (struct sockaddr * sockaddr_ptr)
{
	char* s;
    switch(sockaddr_ptr->sa_family) {
        case AF_INET:
        	s = (char*)malloc(INET_ADDRSTRLEN);
            inet_ntop(AF_INET, &(((struct sockaddr_in *)sockaddr_ptr)->sin_addr),
                    s, INET_ADDRSTRLEN);
            break;
        case AF_INET6:
			s = (char*)malloc(INET6_ADDRSTRLEN);
            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sockaddr_ptr)->sin6_addr),
                    s, INET6_ADDRSTRLEN);
            break;
        default:
            return NULL;
    }
    return s;
}


UNIX_IPEndPoint::UNIX_IPEndPoint(void)
{
}

UNIX_IPEndPoint::~UNIX_IPEndPoint(void)
{
}

Boolean UNIX_IPEndPoint::getName(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_NAME, getHandle());
	return true; 
}

Boolean UNIX_IPEndPoint::getCaption(CIMProperty &p) const
{
	p = CIMProperty(PROPERTY_CAPTION, String(pInfo.ifname));
    return true;
}

Boolean UNIX_IPEndPoint::getDescription(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_DESCRIPTION, String(""));
    return true;
}

Boolean UNIX_IPEndPoint::getInstanceID(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_INSTANCE_ID, ELEMENT_NAME);
    return true;
}

Boolean UNIX_IPEndPoint::getInstallDate(CIMProperty &p) const
{
	struct tm* clock;			// create a time structure
	struct stat attrib;			// create a file attribute structure
	stat("/mnt", &attrib);		// get the attributes mnt
	clock = gmtime(&(attrib.st_birthtime));	// Get the last modified time and put it into the time structure

	p = CIMProperty(PROPERTY_INSTALL_DATE, CIMDateTime(
			clock->tm_year + 1900, 
			clock->tm_mon + 1, 
			clock->tm_mday,
			clock->tm_hour,
			clock->tm_min,
			clock->tm_sec,
			0,0,
			clock->tm_gmtoff
			));
    return true;
}
//IPProtocolEndpoint
Boolean UNIX_IPEndPoint::getNameFormat(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_NAME_FORMAT, String("[ifname]_[Address]_[ProtocolType]"));
    return true;
}

Boolean UNIX_IPEndPoint::getProtocolType(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_PROTOCOL_TYPE, Uint16(pInfo.addressType));
    return true;
}

Boolean UNIX_IPEndPoint::getOtherTypeDescription(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_OTHER_TYPE_DESCRIPTION, String(""));
    return true;
}

Boolean UNIX_IPEndPoint::getAddress(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_ADDRESS, String(pInfo.address));
    return true;
}

Boolean UNIX_IPEndPoint::getSubnetMask(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_SUBNETMASK, String(pInfo.netmask));
    return true;
}

Boolean UNIX_IPEndPoint::getAddressType(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_ADDRESS_TYPE, Uint16(pInfo.addressType));
    return true;
}

Boolean UNIX_IPEndPoint::getIPVersionSupport(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_IPVERSION_SUPPORT, Uint16(pInfo.addressType));
    return true;
}

String UNIX_IPEndPoint::getHandle() const
{
    String s;
    s.append(pInfo.ifname);
    s.append("_");
    s.append(pInfo.address);
    s.append("_");
    if (pInfo.addressType == 1)
    {
    	s.append("IPv4");
    }
    else if (pInfo.addressType == 2) {
    	s.append("IPv6");
    }
    else {
    	s.append("Unknown");
    }
	return s;
}

#define __initialize_H
Boolean UNIX_IPEndPoint::initialize()
{
	/*
	 * Retrieve interface list at first
	 * since we need #ifindex -> if_xname match
	 */
	if (getifaddrs(&pInfo.ifap) != 0)
		return false;
		/* err(EX_OSERR, "getifaddrs"); */
   return true;
}



Boolean UNIX_IPEndPoint::load(int& pIndex)
{
   struct ifaddrs *ifa;
   bool found = false;
   for (ifa = pInfo.ifap; ifa; ifa = ifa->ifa_next) {
   		pInfo.ifname = ifa->ifa_name;
   		if (ifa->ifa_addr->sa_family == AF_INET)
   		{
	   		pInfo.address =  get_address(ifa->ifa_addr);
	   		if (ifa->ifa_netmask)
	   			pInfo.netmask = get_address(ifa->ifa_netmask);
	   		else 
				pInfo.netmask = NULL;

   			pInfo.addressType = 1;
   			found = true;
   			break;
		}
   		else if (ifa->ifa_addr->sa_family == AF_INET6)
   		{
   			pInfo.address =  get_address(ifa->ifa_addr);
	   		if (ifa->ifa_netmask)
	   			pInfo.netmask = get_address(ifa->ifa_netmask);
	   		else 
				pInfo.netmask = NULL;

			pInfo.addressType = 2;
			found = true;
			break;
		}
   		else
   		{
			pInfo.addressType = 0;
		}
   }
   if (found) pInfo.ifap = ifa->ifa_next;
   return found;
}


#define __finalize_H
Boolean UNIX_IPEndPoint::finalize()
{
   freeifaddrs (pInfo.ifap);
   return true;
}

Boolean UNIX_IPEndPoint::find(String& handle)
{
   initialize();
   bool found = false;
   for(int i = 0; load(i); i++)
   {
   		if (String::equal(String(pInfo.ifname), handle))
   		{
   			found = true;
   			break;
   		}
   }
   return found;
}

Boolean UNIX_IPEndPoint::validateKey(CIMKeyBinding &kb) const
{
	/* Keys  */
	//CreationClassName
	//SystemCreationClassName
	//Name
	//SystemName

	CIMName name = kb.getName();
	if (name.equal(PROPERTY_CREATION_CLASS_NAME) ||
		name.equal(PROPERTY_SYSTEM_NAME) ||
		name.equal(PROPERTY_SYSTEM_CREATION_CLASS_NAME) ||
		name.equal(PROPERTY_NAME))
	{
		return true;
	}
	return false;
}

void UNIX_IPEndPoint::buildKeyHandle(CIMKeyBinding &kb, String &s) const
{
	//IPAddress
	if (kb.getName().equal(PROPERTY_NAME))
	{
		s.append(kb.getValue());
	}
}

#include <CIM_ManagedSystemElement.cpp>