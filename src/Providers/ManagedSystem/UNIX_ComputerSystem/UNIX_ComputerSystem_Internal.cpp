
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

#include "UNIX_ComputerSystemProvider.h"
#include "UNIX_ComputerSystem.h"
#include <Pegasus/Common/Logger.h> // for Logger
#include <Pegasus/Common/FileSystem.h>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/param.h>     // for MAXHOSTNAMELEN
#include <sys/utsname.h>   // uname()
#include <sys/sysctl.h>	   // sysctl
#include <sys/stat.h>	   // stat
#include <errno.h>         // for errno
#include <stdlib.h>
#include <string.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

static String _hostName;

UNIX_ComputerSystem::UNIX_ComputerSystem()
{
}

UNIX_ComputerSystem::~UNIX_ComputerSystem()
{
}


Boolean UNIX_ComputerSystem::getCaption(CIMProperty& p)
{
    // hardcoded
    p = CIMProperty(PROPERTY_CAPTION, String(CAPTION));
    return true;
}

Boolean UNIX_ComputerSystem::getDescription(CIMProperty& p)
{
    /*
    char description[256];

    struct utsname nbuf;

    if (uname(&nbuf) != -1)
    {
    	strcpy(description, nbuf.version);
    }

    
    p = CIMProperty(PROPERTY_DESCRIPTION, String(description));
    return true;
    */
    p = CIMProperty(PROPERTY_DESCRIPTION, String(DESCRIPTION));
    return true;
}

Boolean UNIX_ComputerSystem::getInstallDate(CIMProperty& p)
{
	struct tm* clock;			// create a time structure
	struct stat attrib;			// create a file attribute structure
	stat("/mnt", &attrib);		// get the attributes mnt
	clock = gmtime(&(attrib.st_birthtime));	// Get the last modified time and put it into the time structure

	p = CIMProperty(PROPERTY_INSTALL_DATE,CIMDateTime(
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

Boolean UNIX_ComputerSystem::getCreationClassName(CIMProperty& p)
{
    // can vary, depending on class
    p = CIMProperty(
        PROPERTY_CREATION_CLASS_NAME,
        String(CLASS_EXTENDED_COMPUTER_SYSTEM));
    return true;
}


Boolean UNIX_ComputerSystem::getName(CIMProperty& p)
{
    p = CIMProperty(PROPERTY_NAME,String(getHostName()));
    return true;
}

Boolean UNIX_ComputerSystem::getOperationalStatus(CIMProperty& p)
{
    Array<Uint16> opStatus;
    opStatus.append(2); // OK
    p = CIMProperty(PROPERTY_OPERATIONAL_STATUS, opStatus);
    return true;
}

Boolean UNIX_ComputerSystem::getStatusDescriptions(CIMProperty& p)
{
    // not supported.
    return false;
}

Boolean UNIX_ComputerSystem::getStatus(CIMProperty& p)
{
    // hardcoded
    p = CIMProperty(PROPERTY_STATUS,String(STATUS));
    return true;
}

Boolean UNIX_ComputerSystem::getNameFormat(CIMProperty& p)
{
    // hardcoded
    p = CIMProperty(PROPERTY_NAME_FORMAT,String(NAME_FORMAT));
    return true;
}

Boolean UNIX_ComputerSystem::getPrimaryOwnerName(CIMProperty& p)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::setPrimaryOwnerName(const String& name)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::getPrimaryOwnerContact(CIMProperty& p)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::setPrimaryOwnerContact(const String& contact)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::getRoles(CIMProperty& p)
{
    // Defines Roles
    // SSH Server -- check sshd_enable
    // WebServer -- check nginx or apache22
    // Database Server -- check mysql or postgresql
    // Network Switch -- check gateway_enable
    // Firewall -- check pf_enable
    // Cim Server -- check cimserver
    // VPN Server -- check openikve2
    // Mail Server - check dovecot2
    // Developement - check git lab
    // 
    // Desktop check ttys8, kde_enable or gdm_enable

    Array<String> array;
    FILE* pipe = popen("/usr/sbin/sysrc sshd_enable ftpd_enable cimserver_enable samba_server_enable nfsd_enable nginx_enable apache22_enable dovecot2_enable postfix_enable dbus_enable", "r");
    if (!pipe) return false;
    char buffer[256];
    bool webserver = false;
    while(!feof(pipe)) {
    	if (fgets(buffer, 128, pipe) != NULL)
    	{
    		if (strstr(buffer, "sshd_enable: YES") != NULL)
    		{
    			array.append("SSH Server");
    		}
    		if (strstr(buffer, "ftpd_enable: YES") != NULL)
    		{
    			array.append("FTP Server");
    		}
    		if (strstr(buffer, "cimserver_enable: YES") != NULL)
    		{
    			array.append("CIM Server");
    		}
    		if (strstr(buffer, "samba_server_enable: YES") != NULL)
    		{
    			array.append("Directory Server");
    			array.append("File Server");
    		}
    		if (strstr(buffer, "nfsd_enable: YES") != NULL)
    		{
    			array.append("NFS Server");
    		}
    		if (strstr(buffer, "nginx_enable: YES") != NULL)
    		{
    			if (!webserver)
    			{
	    			array.append("Web Server");
	    			webserver = true;
    			}
    		}
    		if (strstr(buffer, "apache22_enable: YES") != NULL)
    		{
    			if (!webserver)
    			{
	    			array.append("Web Server");
	    			webserver = true;
    			}
    		}
    		if (strstr(buffer, "dovecot2_enable: YES") != NULL)
    		{
    			array.append("Mail Server");
    		}
    		if (strstr(buffer, "postfix_enable: YES") != NULL)
    		{
    			array.append("Smtp Server");
    		}
    		if (strstr(buffer, "dbus_enable: YES") != NULL)
    		{
    			array.append("Desktop System");
    		}

    	}
    }
    pclose(pipe);
    p = CIMProperty(PROPERTY_ROLES,array);
    return true;
}

Boolean UNIX_ComputerSystem::getOtherIdentifyingInfo(CIMProperty& p)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::getIdentifyingDescriptions(CIMProperty& p)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::getDedicated(CIMProperty& p)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::getResetCapability(CIMProperty& p)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::getPowerManagementCapabilities(CIMProperty& p)
{
    Array<Uint16> capabilities;

    // hw.acpi.supported_sleep_state

    size_t ret_len;
    if (sysctlbyname("hw.acpi.supported_sleep_state", NULL, &ret_len, NULL, 0) != -1)
    {
    	char ret[ret_len];
    	if (sysctlbyname("hw.acpi.supported_sleep_state", &ret, &ret_len, NULL, 0) != -1)
    	{
	    	if (strstr(ret, "S2") != NULL)
	    	{
	    		capabilities.append(2);
	    	}
	    	if (strstr(ret, "S3") != NULL)
	    	{
	    		capabilities.append(3);
	    	}
	    	if (strstr(ret, "S4") != NULL)
	    	{
	    		capabilities.append(4);
	    	}
	    	if (strstr(ret, "S5") != NULL)
	    	{
	    		capabilities.append(5);
	    	}
	    	p = CIMProperty(PROPERTY_POWER_MANAGEMENT_CAPABILITIES,capabilities);
	    	return true;
    	}
    }

    // Since PowerManagementSupported == FALSE
    // Capabilities must be defined to return "1" which signifies
    // "Not Supported" from Capabilities enum
    capabilities.append(1);
    p = CIMProperty(PROPERTY_POWER_MANAGEMENT_CAPABILITIES,capabilities);
    return true;
}

Boolean UNIX_ComputerSystem::getInitialLoadInfo(CIMProperty& p)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::getLastLoadInfo(CIMProperty& p)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::getPowerManagementSupported(CIMProperty& p)
{
    // hw.acpi.supported_sleep_state

    size_t ret_len;
    if (sysctlbyname("hw.acpi.supported_sleep_state", NULL, &ret_len, NULL, 0) != -1)
    {
    	char ret[ret_len];
    	if (sysctlbyname("hw.acpi.supported_sleep_state", &ret, &ret_len, NULL, 0) != -1)
    	{
	    	bool result = strstr(ret, "S5") != NULL;
	    	p = CIMProperty(PROPERTY_POWER_MANAGEMENT_SUPPORTED,result);
	    	return true;
    	}
    }
    p = CIMProperty(PROPERTY_POWER_MANAGEMENT_SUPPORTED,false); // on PA-RISC!!
    return true;
}

Boolean UNIX_ComputerSystem::getPowerState(CIMProperty& p)
{
    // hardcoded
    /*
        ValueMap {"1", "2", "3", "4", "5", "6", "7", "8"},
        Values {"Full Power", "Power Save - Low Power Mode",
                "Power Save - Standby", "Power Save - Other",
                "Power Cycle", "Power Off", "Hibernate", "Soft Off"}
    */
    p = CIMProperty(PROPERTY_POWER_STATE,Uint16(1));
    return true;
}

Boolean UNIX_ComputerSystem::getWakeUpType(CIMProperty& p)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::getPrimaryOwnerPager(CIMProperty& p)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::setPrimaryOwnerPager(const String& pager)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::getSecondaryOwnerName(CIMProperty& p)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::setSecondaryOwnerName(const String& name)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::getSecondaryOwnerContact(CIMProperty& p)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::setSecondaryOwnerContact(const String& contact)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::getSecondaryOwnerPager(CIMProperty& p)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::setSecondaryOwnerPager(const String& pager)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::getSerialNumber(CIMProperty& p)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::getIdentificationNumber(CIMProperty& p)
{
    // not supported
    return false;
}

Boolean UNIX_ComputerSystem::getElementName(CIMProperty& p)
{
    // We're just going to re-use the caption
    p = CIMProperty(PROPERTY_ELEMENTNAME, String(CAPTION));
    return true;
}

/**
 * initialize primarily functions to initialize static global variables
 * that will not be changed frequently. These variables are currently
 * _hostName.
 *
 */
void UNIX_ComputerSystem::initialize()
{
	 char    hostNameVal[PEGASUS_MAXHOSTNAMELEN + 1];
     struct  hostent *he;

     if (gethostname(hostNameVal, sizeof(hostNameVal)) != 0)
     {
         return;
     }
     hostNameVal[sizeof(hostNameVal)-1] = 0;

     // Now get the official hostname.  If this call fails then return
     // the value from gethostname().

     if ((he=gethostbyname(hostNameVal)))
     {
         _hostName.assign(he->h_name);
     }
     else
     {
         _hostName.assign(hostNameVal);
     }

}

String UNIX_ComputerSystem::getHostName()
{
    return _hostName;
}