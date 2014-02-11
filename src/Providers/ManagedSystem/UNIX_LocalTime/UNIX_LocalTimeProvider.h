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
#ifndef _LOCALTIMEPROVIDER_H
#define _LOCALTIMEPROVIDER_H

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
#define LOCALTIMEPROVIDERNAME	"UNIX_LocalTimeProvider"

#define PROPERTY_DAY 			"Day"
#define PROPERTY_DAYOFWEEK		"DayOfWeek"
#define PROPERTY_HOUR			"Hour"
#define PROPERTY_MILLISECONDS 	"Milliseconds"
#define PROPERTY_MINUTE			"Minute"
#define PROPERTY_MONTH			"Month"
#define PROPERTY_QUARTER		"Quarter"
#define PROPERTY_SECOND			"Second"
#define PROPERTY_WEEKINMONTH	"WeekInMonth"
#define PROPERTY_YEAR			"Year"
#define PROPERTY_TIMEZONE_OFFSET "TimeZoneOffset"


PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

class UNIX_LocalTimeProvider :
    public CIMInstanceProvider,
    public CIMMethodProvider
{
public:
    UNIX_LocalTimeProvider();
    ~UNIX_LocalTimeProvider();

    //-- CIMInstanceProvider methods
    /** Given a reference to an instance of the CIM class, fills in the data
     *  elements of the class with the details gleaned from the system. */
    void getInstance(
        const OperationContext& context,
        const CIMObjectPath& ref,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList& propertyList,
        InstanceResponseHandler& handler);

    /** Returns filled instances for all instances of the CIM class detected
     *  on the system. */
    void enumerateInstances(
        const OperationContext& context,
        const CIMObjectPath& ref,
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList& propertyList,
        InstanceResponseHandler& handler);

    /** Produces a list of references to all instances of the CIM class
     *  detected on the system, but does not fill the instances
     *  themselves. */
    void enumerateInstanceNames(
        const OperationContext& context,
        const CIMObjectPath& ref,
        ObjectPathResponseHandler& handler);

    /** Currently unimplemented in the Pegasus source, this is a no-op
     *  here. */
    void modifyInstance(
        const OperationContext& context,
        const CIMObjectPath& ref,
        const CIMInstance& instanceObject,
        const Boolean includeQualifiers,
        const CIMPropertyList& propertyList,
        ResponseHandler& handler);

    /** Currently unimplemented in the Pegasus source, this is a no-op
     *  here. */
    void createInstance(
        const OperationContext& context,
        const CIMObjectPath& ref,
        const CIMInstance& instanceObject,
        ObjectPathResponseHandler& handler);

    /** Currently unimplemented in the Pegasus source, this is a no-op
     *  here. */
    void deleteInstance(
        const OperationContext& context,
        const CIMObjectPath& ref,
        ResponseHandler& handler);

    void initialize(CIMOMHandle& handle);
    void terminate();

    void invokeMethod(
        const OperationContext& context,
        const CIMObjectPath& objectReference,
        const CIMName& methodName,
        const Array<CIMParamValue>& inParameters,
        MethodResultResponseHandler& handler);
    
    CIMInstance testBuildInstance(const CIMObjectPath &objectReference);

	CIMObjectPath testReference(const CIMNamespaceName &nameSpace,
        const CIMName &className);
private:
    // store off for future use
    CIMOMHandle _cimomhandle;

    // Builds a filled-in instance.
    CIMInstance _build_instance(const CIMObjectPath &objectReference);

    // Builds a reference (a set of Key,Value pairs)
    CIMObjectPath _fill_reference(
        const CIMNamespaceName &nameSpace,
        const CIMName &className);


    // uninitialized; will be set during initialize() processing
    String _hostName;
};

#endif
