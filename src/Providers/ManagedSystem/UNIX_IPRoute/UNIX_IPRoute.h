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
#ifndef _IPROUTE_H
#define _IPROUTE_H

#include <UNIX_Common.h>


#define CLASS_IMPLEMENTATION_NAME			"UNIX_IPRoute"
#define CLASS_IMPLEMENTATION				UNIX_IPRoute
#define BASE_CLASS_NAME 					"CIM_IPRoute"
#define ELEMENT_NAME						"NetworkRoute"
#define NUMKEYS_CLASS_IMPLEMENTATION		4


#define PROPERTY_ADDRESS_TYPE						"AddressType"
#define PROPERTY_DESTINATION_ADDRESS				"DestinationAddress"
#define PROPERTY_DESTINATION_MASK					"DestinationMask"
#define PROPERTY_IP_DESTINATION_ADDRESS				"IPDestinationAddress"
#define PROPERTY_IP_DESTINATION_MASK				"IPDestinationMask"
#define PROPERTY_SERVICE_CREATION_CLASS_NAME		"ServiceCreationClassName"
#define PROPERTY_SERVICE_NAME						"ServiceName"
#define PROPERTY_NEXT_HOP							"NextHop"
#define PROPERTY_IS_STATIC							"IsStatic"

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

typedef struct peg_ip_route_table {
	char* buffer;
	char* lim;
	size_t length;
	struct peg_ip_route_table *l, *r; // linked list pointers
} p_iproute_table_t;

typedef struct peg_ip_route {
	int addressType;
	const char* destination;
	const char* gateway;
	const char* netmask;
	int index;
	const char* flags;
	const char* ifname;
	unsigned long mtu;
	unsigned long weight;
	unsigned long rtt;
	struct peg_ip_route *l, *r; // linked list pointers
} p_iproute_t;

class UNIX_IPRoute
{
public:
    UNIX_IPRoute();
    ~UNIX_IPRoute();

    #include <CIM_ManagedSystemElement.h>

    Boolean getAddressType(CIMProperty&) const;
    Boolean getDestinationAddress(CIMProperty&) const;
    Boolean getDestinationMask(CIMProperty&) const;
    Boolean getIPDestinationAddress(CIMProperty&) const;
    Boolean getIPDestinationMask(CIMProperty&) const;
    Boolean getNextHop(CIMProperty&) const;
    Boolean getServiceName(CIMProperty&) const;
    Boolean getServiceCreationClassName(CIMProperty&) const;
    Boolean getIsStatic(CIMProperty&) const;
    String getServiceName() const;
    String getServiceCreationClassName() const;
    String getAddressType() const;
    String getIPDestinationAddress() const;
    String getIPDestinationMask() const;
private:
    p_iproute_table_t pRouteTable;
	p_iproute_t pRoute;
};

#endif
