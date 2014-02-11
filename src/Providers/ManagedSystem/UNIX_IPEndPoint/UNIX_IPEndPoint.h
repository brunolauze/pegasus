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

// Defines
#define SYSTEM_CREATION_CLASS_NAME CIMName("CIM_UnitaryComputerSystem")
#define CLASS_IMPLEMENTATION_NAME			"UNIX_IPEndPoint"
#define BASE_CLASS_NAME 					"CIM_IPEndPoint"
#define ELEMENT_NAME						"IPEndPoint"
#define NUMKEYS_CLASS_IMPLEMENTATION		4

#define CLASS_IMPLEMENTATION				UNIX_IPEndPoint

#define PROPERTY_NAME_FORMAT				"NameFormat"
#define PROPERTY_ADDRESS					"Address"
#define PROPERTY_SUBNETMASK					"SubnetMask"
#define PROPERTY_PROTOCOL_TYPE				"ProtocolType"
#define PROPERTY_ADDRESS_TYPE				"AddressType"
#define PROPERTY_OTHER_TYPE_DESCRIPTION		"OtherTypeDescription"
#define PROPERTY_IPVERSION_SUPPORT			"IPVersionSupport"

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

typedef struct peg_ipep {
	struct ifaddrs *ifap;
	int addressType;
	const char* address;
	const char* gateway;
	const char* netmask;
	int index;
	const char* flags;
	const char* ifname;
	struct peg_ipep *l, *r; // linked list pointers
} p_ipep_t;

class UNIX_IPEndPoint
{
public:
    UNIX_IPEndPoint();
    ~UNIX_IPEndPoint();

    #include <CIM_ManagedSystemElement.h>

    //IPProtocolEndpoint
    Boolean getNameFormat(CIMProperty&) const;
    Boolean getProtocolType(CIMProperty&) const;
    Boolean getOtherTypeDescription(CIMProperty&) const;
    Boolean getAddress(CIMProperty&) const;
    Boolean getSubnetMask(CIMProperty&) const;
    Boolean getAddressType(CIMProperty&) const;
    Boolean getIPVersionSupport(CIMProperty&) const;

private:
    p_ipep_t pInfo;
};

#endif
