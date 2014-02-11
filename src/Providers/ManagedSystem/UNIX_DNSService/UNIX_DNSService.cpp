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

#include "UNIX_DNSService.h"
#include <sys/stat.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <libutil.h>

UNIX_DNSService::UNIX_DNSService(void)
{
}

UNIX_DNSService::~UNIX_DNSService(void)
{
}


Boolean UNIX_DNSService::getName(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_NAME, getHandle());
	return true; 
}

Boolean UNIX_DNSService::getCaption(CIMProperty &p) const
{
	p = CIMProperty(PROPERTY_CAPTION, getHandle());
    return true;
}

Boolean UNIX_DNSService::getDescription(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_DESCRIPTION, DNS_DESCRIPTION);
    return true;
}

Boolean UNIX_DNSService::getInstanceID(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_INSTANCE_ID, ELEMENT_NAME);
    return true;
}

Boolean UNIX_DNSService::getStartMode(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_STARTED, String("Automatic"));
    return true;
}

Boolean UNIX_DNSService::getStarted(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_STARTED, Boolean(true));
    return true;
}

Boolean UNIX_DNSService::getInstallDate(CIMProperty &p) const
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

Boolean UNIX_DNSService::getAddresses(CIMProperty &p) const
{
    Array<String> as;
	for(int i = 0; i < _res.nscount; i++)
	{
		as.append(inet_ntoa(_res.nsaddr_list[i].sin_addr));
	}
    p = CIMProperty(PROPERTY_ADDRESSES, as);
    return true;
}

Boolean UNIX_DNSService::getSearchList(CIMProperty &p) const
{
    Array<String> as;
	register char **pp;
	if (!String::equal(_res.defdname, String::EMPTY))
	{
		as.append(_res.defdname);
	}
	for (pp = _res.dnsrch; *pp; pp++)
	{
	    if (!String::equal(_res.defdname, *pp))
	    {
	    	as.append(*pp);
	    }
	}
    p = CIMProperty(PROPERTY_SEARCH_LIST, as);
    return true;
    return false;
}

String UNIX_DNSService::getHandle() const
{
	return DNS_CAPTION;
}

#define __initialize_H
Boolean UNIX_DNSService::initialize()
{
   
   return true;
}



Boolean UNIX_DNSService::load(int& pIndex)
{
   if (pIndex == 0)
   {
   		res_init();
   		return true;
   }
   return false;
}


Boolean UNIX_DNSService::find(String& handle)
{

   return false;
}

Boolean UNIX_DNSService::validateKey(CIMKeyBinding &kb) const
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

void UNIX_DNSService::buildKeyHandle(CIMKeyBinding &kb, String &s) const
{
	//IPAddress
	if (kb.getName().equal(PROPERTY_NAME))
	{
		s.append(kb.getValue());
	}
}

#include <CIM_ManagedSystemElement.cpp>
