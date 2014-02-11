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
//%/////////////////////////////////////////////////////////////////////////
#ifndef _DNSSERVICE_H
#define _DNSSERVICE_H

#include <UNIX_Common.h>

// Role definitions
static const String NTP_ROLE_CLIENT("server");

// Defines
#define SYSTEM_CREATION_CLASS_NAME CIMName("CIM_UnitaryComputerSystem")
static const String DNS_CAPTION("NTP Service");
static const String DNS_DESCRIPTION("Describes the Network Time (NTP) Service");

#define CLASS_IMPLEMENTATION_NAME			"UNIX_NTPService"
#define CLASS_IMPLEMENTATION				UNIX_NTPService
#define BASE_CLASS_NAME 					"CIM_Service"
#define ELEMENT_NAME						"NTPService"
#define NUMKEYS_CLASS_IMPLEMENTATION		4

#define PROPERTY_SERVER_ADDRESS				"ServerAddress"

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;



class UNIX_NTPService
{
public:
    UNIX_NTPService();
    ~UNIX_NTPService();

    #include <CIM_ManagedSystemElement.h>

    Boolean getServerAddress(CIMProperty&) const;
private:

};

#endif
