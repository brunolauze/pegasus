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

#include "UNIX_NTPService.h"
#include <sys/stat.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <libutil.h>

#define NTP_FILE_CONFIG 		"/etc/ntp.conf"

UNIX_NTPService::UNIX_NTPService(void)
{
}

UNIX_NTPService::~UNIX_NTPService(void)
{
}


Boolean UNIX_NTPService::getName(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_NAME, getHandle());
	return true; 
}

Boolean UNIX_NTPService::getCaption(CIMProperty &p) const
{
	p = CIMProperty(PROPERTY_CAPTION, getHandle());
    return true;
}

Boolean UNIX_NTPService::getDescription(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_DESCRIPTION, DNS_DESCRIPTION);
    return true;
}

Boolean UNIX_NTPService::getInstanceID(CIMProperty &p) const
{
    p = CIMProperty(PROPERTY_INSTANCE_ID, ELEMENT_NAME);
    return true;
}

Boolean UNIX_NTPService::getInstallDate(CIMProperty &p) const
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

Boolean UNIX_NTPService::getServerAddress(CIMProperty &p) const
{
    Array<String> ntpServerAddress;
    Array<String> ntpName;
	
    FILE *fp;
    int i, ps = 0;
    long lstPos = 0;
    char buffer[5000];
    Boolean ok = false,
            okRet = false;
    String strKey,
           strHost,
           strBuffer;

    // Open NTP configuration file
    if((fp = fopen(NTP_FILE_CONFIG, "r")) == NULL)
        return ok;

    memset(buffer, 0, sizeof(buffer));
    while(fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        int bufferLength = strlen(buffer);
        if (bufferLength > 0)
        {
            buffer[bufferLength - 1] = 0;
        }
        strBuffer.assign(buffer);

        ps = strBuffer.find(NTP_ROLE_CLIENT);

        okRet = true;
        if(ps == 0)
        {
            okRet = true;
            fseek(fp, lstPos, SEEK_SET);
            fscanf(fp, "%4999s", buffer);
            strBuffer.assign(buffer);
                        ps = strBuffer.find(NTP_ROLE_CLIENT);
            if(ps < 0) {
                lstPos = ftell(fp);
                continue;
            }

            fscanf(fp, "%4999s", buffer);
            strHost.assign(buffer);

            ok = false;
            // Verify if name server exists in array
            for(i=0; i < ntpServerAddress.size(); i++)
            {
                if(String::equalNoCase(ntpServerAddress[i], strHost))
                {
                    ok = true;
                    break;
                }
            }
            if (!ok)
            {
                ntpServerAddress.append(strHost);
                if (ntpName.size() == 0)
                {
                    // Set ntpName variable with name server, if strHost
                    // variable is an IP address.
                }
            }
        }
        lstPos = ftell(fp);
    }
    fclose(fp);

	p = CIMProperty(PROPERTY_SERVER_ADDRESS, ntpServerAddress);
    return true;
}

String UNIX_NTPService::getHandle() const
{
	return DNS_CAPTION;
}

#define __initialize_H
Boolean UNIX_NTPService::initialize()
{
   
   return true;
}



Boolean UNIX_NTPService::load(int& pIndex)
{
   if (pIndex == 0)
   {
   		res_init();
   		return true;
   }
   return false;
}


Boolean UNIX_NTPService::find(String& handle)
{

   return false;
}

Boolean UNIX_NTPService::validateKey(CIMKeyBinding &kb) const
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

void UNIX_NTPService::buildKeyHandle(CIMKeyBinding &kb, String &s) const
{
	//IPAddress
	if (kb.getName().equal(PROPERTY_NAME))
	{
		s.append(kb.getValue());
	}
}

#include <CIM_ManagedSystemElement.cpp>
