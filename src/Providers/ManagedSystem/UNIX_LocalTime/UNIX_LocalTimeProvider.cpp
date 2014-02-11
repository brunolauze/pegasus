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


#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/ProviderException.h>
#include <fcntl.h>
#include <kvm.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/vmmeter.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utmpx.h>
#include <time.h>


PEGASUS_USING_PEGASUS;

#include "UNIX_LocalTimeProvider.h"

PEGASUS_USING_STD;

#define OSP_DEBUG(X) // cout << "UNIX_LocalTimeProvider" <<  X << endl;

#define BASELOCALTIMECLASS CIMName ("CIM_CurrentTime")
#define STANDARDLOCALTIMECLASS CIMName ("UNIX_CurrentTime")
#define EXTENDEDLOCALTIMECLASS CIMName ("UNIX_LocalTime")
#define CSCREATIONCLASSNAME CIMName ("UNIX_LocalTime")


static Boolean getUtilGetHostName(String& csName)
{
     char    hostName[PEGASUS_MAXHOSTNAMELEN + 1];
     if (gethostname(hostName, sizeof(hostName)) != 0)
     {
         return false;
     }
     hostName[sizeof(hostName)-1] = 0;

     // Now get the official hostname.  If this call fails then return
     // the value from gethostname().
     /*
     struct  hostent *he;
     if ((he=gethostbyname(hostName)))
     {
         csName.assign(he->h_name);
     }
     else
     {
         
     }
     */
     csName.assign(hostName);
     return true;
}

UNIX_LocalTimeProvider::UNIX_LocalTimeProvider()
{
     getUtilGetHostName(_hostName);
}

UNIX_LocalTimeProvider::~UNIX_LocalTimeProvider()
{
}

void UNIX_LocalTimeProvider::getInstance(
    const OperationContext& context,
    const CIMObjectPath& ref,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
    InstanceResponseHandler &handler)
{
    Array<CIMKeyBinding> keys;
    CIMInstance instance;
    CIMName className;

    //-- make sure we're working on the right class
    className = ref.getClassName();
    if (!(className.equal (STANDARDLOCALTIMECLASS)) &&
        !(className.equal (EXTENDEDLOCALTIMECLASS) ||
        !className.equal(BASELOCALTIMECLASS)))
    {
        throw CIMNotSupportedException(
            "UNIX_LocalTimeProvider does not support class " +
                className.getString());
    }

    //-- make sure we're the right instance
    int keyCount;
    CIMName keyName;

    keyCount = 3;
    keys = ref.getKeyBindings();

    if ((unsigned int)keys.size() != (unsigned int)keyCount)
        throw CIMInvalidParameterException("Wrong number of keys");

    // doesn't seem as though this code will handle duplicate keys,
    // but it appears as though the CIMOM strips those out for us.
    // Despite test cases, don't get invoked with 2 keys of the same
    // name.


    for (unsigned int ii = 0; ii < keys.size(); ii++)
    {
         keyName = keys[ii].getName();

         if ((keyName.equal("CSCreationClassName")) &&
             String::equalNoCase(
                 keys[ii].getValue(),
                 EXTENDEDLOCALTIMECLASS.getString()))
         {
             keyCount--;
         }
		else if ((keyName.equal ("CSName")) &&
                  String::equalNoCase(
                      keys[ii].getValue(),
                      _hostName))
         {
             keyCount--;
         }
         else if ((keyName.equal ("CreationClassName")) &&
                  String::equalNoCase(
                      keys[ii].getValue(),
                      BASELOCALTIMECLASS.getString()))
         {
             keyCount--;
         }
         else
         {
             throw CIMInvalidParameterException(
                 "UNIX_LocalTimeProvider unrecognized key " +
                     keyName.getString());
         }
     }

     if (keyCount)
     {
        throw CIMInvalidParameterException("Wrong keys");
     }

    OSP_DEBUG("losp-> getInstance got the right keys");

    handler.processing();

    //-- fill 'er up...
    instance = _build_instance(ref);
    instance.setPath(ref);

    OSP_DEBUG("losp-> getInstance built an instance");

    handler.deliver(instance);
    handler.complete();

    OSP_DEBUG("losp-> getInstance done");
    return;
}

void UNIX_LocalTimeProvider::enumerateInstances(
    const OperationContext& context,
    const CIMObjectPath& ref,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
    InstanceResponseHandler& handler)
{
    CIMName className;
    CIMInstance instance;
    CIMObjectPath newref;

    className = ref.getClassName();

    // only return instances when enumerate on our subclass, CIMOM
    // will call us as natural part of recursing through subtree on
    // enumerate - if we return instances on enumerate of our superclass,
    // there would be dups
    if (className.equal (EXTENDEDLOCALTIMECLASS) ||
    	className.equal(STANDARDLOCALTIMECLASS) ||
    	className.equal(BASELOCALTIMECLASS))
    {
        handler.processing();
        newref = _fill_reference(ref.getNameSpace(), className);
        instance = _build_instance(ref);
        instance.setPath(newref);
        handler.deliver(instance);
        handler.complete();
    }
    else if (className.equal (STANDARDLOCALTIMECLASS))
    {
        handler.processing();
        handler.complete();
    }
    else
    {
        throw CIMNotSupportedException("UNIX_LocalTimeProvider "
                "does not support class " + className.getString());
    }
    return;
}

void UNIX_LocalTimeProvider::enumerateInstanceNames(
    const OperationContext& context,
    const CIMObjectPath &ref,
    ObjectPathResponseHandler& handler)
{
    CIMObjectPath newref;
    CIMName className;

    // only return instances when enumerate on our subclass, CIMOM
    // will call us as natural part of recursing through subtree on
    // enumerate - if we return instances on enumerate of our superclass,
    // there would be dups
    className = ref.getClassName();
    if (className.equal (STANDARDLOCALTIMECLASS))
    {
        handler.processing();
        handler.complete();
        return;
    }
    else if (!className.equal (EXTENDEDLOCALTIMECLASS))
    {
        throw CIMNotSupportedException("UNIX_LocalTimeProvider "
                       "does not support class " + className.getString());
    }

    // so we know it is for EXTENDEDLOCALTIMECLASS
    handler.processing();
    // in terms of the class we use, want to set to what was requested
    newref = _fill_reference(ref.getNameSpace(), className);
    handler.deliver(newref);
    handler.complete();

    return;
}

void UNIX_LocalTimeProvider::modifyInstance(
    const OperationContext& context,
    const CIMObjectPath& ref,
    const CIMInstance& instanceObject,
    const Boolean includeQualifiers,
    const CIMPropertyList& propertyList,
    ResponseHandler& handler)
{
    throw CIMNotSupportedException(
        "UNIX_LocalTimeProvider does not support modifyInstance");
}

void UNIX_LocalTimeProvider::createInstance(
    const OperationContext& context,
    const CIMObjectPath& ref,
    const CIMInstance& instanceObject,
    ObjectPathResponseHandler& handler )
{
    throw CIMNotSupportedException(
        "UNIX_LocalTimeProvider does not support createInstance");
}

void UNIX_LocalTimeProvider::deleteInstance(
    const OperationContext& context,
    const CIMObjectPath& ref,
    ResponseHandler& handler)
{
    throw CIMNotSupportedException(
        "UNIX_LocalTimeProvider does not support deleteInstance");
}

void UNIX_LocalTimeProvider::initialize(CIMOMHandle& handle)
{
   _cimomhandle = handle;  // save off for future use

   // call platform-specific routines to get certain values

}


void UNIX_LocalTimeProvider::terminate()
{
    delete this;
}


CIMInstance UNIX_LocalTimeProvider::_build_instance(
    const CIMObjectPath& objectReference)
{

    CIMInstance instance(objectReference.getClassName());
    CIMName className = objectReference.getClassName();

    //-- fill in all the blanks
    instance.addProperty(CIMProperty(
        "CSCreationClassName", EXTENDEDLOCALTIMECLASS.getString()));

    instance.addProperty(CIMProperty("CSName", _hostName));

    instance.addProperty(CIMProperty(
        "CreationClassName", STANDARDLOCALTIMECLASS.getString()));
    
	// FILL TIME
    time_t now;
   	now = time(NULL);
    struct tm* clock = gmtime(&(now));
    instance.addProperty(CIMProperty(PROPERTY_DAY, Uint32(clock->tm_mday)));
	instance.addProperty(CIMProperty(PROPERTY_DAYOFWEEK, Uint32(clock->tm_wday)));
	instance.addProperty(CIMProperty(PROPERTY_HOUR, Uint32(clock->tm_mday)));
	instance.addProperty(CIMProperty(PROPERTY_MILLISECONDS, Uint32(0)));
	instance.addProperty(CIMProperty(PROPERTY_MINUTE, Uint32(clock->tm_mday)));
	instance.addProperty(CIMProperty(PROPERTY_MONTH, Uint32(clock->tm_mon + 1)));
	instance.addProperty(CIMProperty(PROPERTY_QUARTER, Uint32((clock->tm_mon + 1) / 3 + 1)));
	instance.addProperty(CIMProperty(PROPERTY_SECOND, Uint32(clock->tm_sec)));
	instance.addProperty(CIMProperty(PROPERTY_WEEKINMONTH, Uint32(1)));
	instance.addProperty(CIMProperty(PROPERTY_YEAR, Uint32(clock->tm_year + 1900)));
	instance.addProperty(CIMProperty(PROPERTY_TIMEZONE_OFFSET, Uint32(-clock->tm_gmtoff)));
    return instance;
}

CIMInstance UNIX_LocalTimeProvider::testBuildInstance(const CIMObjectPath &objectReference)
{
	return _build_instance(objectReference);
}

CIMObjectPath UNIX_LocalTimeProvider::testReference(
    const CIMNamespaceName &nameSpace,
    const CIMName &className)
{
    return _fill_reference(nameSpace, className);
}

CIMObjectPath UNIX_LocalTimeProvider::_fill_reference(
    const CIMNamespaceName &nameSpace,
    const CIMName &className)
{
    Array<CIMKeyBinding> keys;

    keys.append(CIMKeyBinding(
        "CSCreationClassName",
        CSCREATIONCLASSNAME.getString(),
        CIMKeyBinding::STRING));
    keys.append(CIMKeyBinding(
        "CSName",
        _hostName,
        CIMKeyBinding::STRING));
    keys.append(CIMKeyBinding(
        "CreationClassName",
        STANDARDLOCALTIMECLASS.getString(),
        CIMKeyBinding::STRING));

    return CIMObjectPath(_hostName, nameSpace, className, keys);
}

void UNIX_LocalTimeProvider::invokeMethod(
    const OperationContext& context,
    const CIMObjectPath& objectReference,
    const CIMName& methodName,
    const Array<CIMParamValue>& inParameters,
    MethodResultResponseHandler& handler)
{
    throw CIMNotSupportedException(
        "UNIX_LocalTimeProvider does not support invokeMethod");
}

