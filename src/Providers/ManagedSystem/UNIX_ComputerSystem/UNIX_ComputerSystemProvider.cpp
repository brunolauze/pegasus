﻿//%LICENSE////////////////////////////////////////////////////////////////
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

// This file should be generic, and while it allows for a
// platform-specific extension to the CIM classes (by
// refering to EXTENDED capabilities), it should never
// explicitly name the extension.

// This provider is registered to support operations at
// several class levels:
//
//  CIM_ComputerSystem
//  CIM_UnitaryComputerSystem
//  <platform>_ComputerSustem
//
// Enumeration operations always return instances from the
// deepest class available.  All other
// operations take note of the specified class.
//
// Operations currently supported:
//
//  enumerateInstanceNames()
//  enumerateInstances()
//  getInstance()

// ==========================================================================
// includes
// ==========================================================================

#include "UNIX_ComputerSystemProvider.h"
#include "UNIX_ComputerSystem.h"

// ==========================================================================
// defines
// ==========================================================================

#define NUMKEYS_COMPUTER_SYSTEM              3

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

UNIX_ComputerSystemProvider::UNIX_ComputerSystemProvider()
{
}

UNIX_ComputerSystemProvider::~UNIX_ComputerSystemProvider()
{
}

void UNIX_ComputerSystemProvider::getInstance(
    const OperationContext& context,
    const CIMObjectPath& ref,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
    InstanceResponseHandler &handler)
{
    CIMName className = ref.getClassName();
    _checkClass(className);

    Array<CIMKeyBinding> keys = ref.getKeyBindings();

    //-- make sure we're the right instance
    unsigned int keyCount = NUMKEYS_COMPUTER_SYSTEM;
    CIMName keyName;
    String keyValue;

    if (keys.size() != keyCount)
        throw CIMInvalidParameterException("Wrong number of keys");

    for (unsigned int ii = 0; ii < keys.size(); ii++)
    {
        keyName = keys[ii].getName();
        keyValue = keys[ii].getValue();

        if (keyName.equal(PROPERTY_CREATION_CLASS_NAME) &&
            (String::equalNoCase(keyValue,CLASS_CIM_COMPUTER_SYSTEM) ||
             String::equalNoCase(keyValue,CLASS_CIM_UNITARY_COMPUTER_SYSTEM) ||
             String::equalNoCase(keyValue,CLASS_EXTENDED_COMPUTER_SYSTEM) ||
             String::equalNoCase(keyValue,String::EMPTY)))
        {
            keyCount--;
        }
        else if (keyName.equal("Name") &&
                 String::equalNoCase(keyValue,_cs.getHostName()))
        {
            keyCount--;
        }
    }

    if (keyCount)
    {
        throw CIMInvalidParameterException(String::EMPTY);
    }

    // return instance of specified class
    CIMInstance instance = _cs.buildInstance(ref.getClassName());

    handler.processing();
    handler.deliver(instance);
    handler.complete();

    return;
}

void UNIX_ComputerSystemProvider::enumerateInstances(
    const OperationContext& context,
    const CIMObjectPath& ref,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
    InstanceResponseHandler& handler)
{
    CIMName className = ref.getClassName();
    _checkClass(className);

    handler.processing();

    // Deliver instance only if request was for leaf class
    if (className.equal (CLASS_EXTENDED_COMPUTER_SYSTEM) ||
    	className.equal (CLASS_CIM_COMPUTER_SYSTEM))
    {
        Array<CIMKeyBinding> keys;
        keys.append(CIMKeyBinding(
            PROPERTY_CREATION_CLASS_NAME,
            CLASS_EXTENDED_COMPUTER_SYSTEM,
            CIMKeyBinding::STRING));
        keys.append(CIMKeyBinding(
            PROPERTY_NAME,
            _cs.getHostName(),
            CIMKeyBinding::STRING));
        CIMObjectPath instanceName(
            String::EMPTY,       // Hostname not required
            CIMNamespaceName(),  // Namespace not required
            CLASS_EXTENDED_COMPUTER_SYSTEM,
            keys);
        CIMInstance instance =
            _cs.buildInstance(CLASS_EXTENDED_COMPUTER_SYSTEM);
        instance.setPath(instanceName);
        handler.deliver(instance);
    }

    handler.complete();
    return;
}

void UNIX_ComputerSystemProvider::enumerateInstanceNames(
    const OperationContext& context,
    const CIMObjectPath &ref,
    ObjectPathResponseHandler& handler)
{
    CIMName className = ref.getClassName();
    _checkClass(className);

    handler.processing();

    // Deliver instance only if request was for leaf class
    if (className.equal(CLASS_EXTENDED_COMPUTER_SYSTEM)
    || className.equal (CLASS_CIM_COMPUTER_SYSTEM))
    {
        Array<CIMKeyBinding> keys;

        keys.append(CIMKeyBinding(
            PROPERTY_CREATION_CLASS_NAME,
            CLASS_CIM_COMPUTER_SYSTEM,
            CIMKeyBinding::STRING));
        keys.append(CIMKeyBinding(
            PROPERTY_NAME,
            _cs.getHostName(),
            CIMKeyBinding::STRING));

        handler.deliver(CIMObjectPath(
            _cs.getHostName(),
            ref.getNameSpace(),
            CLASS_EXTENDED_COMPUTER_SYSTEM,
            keys));
    }

    handler.complete();
    return;
}

void UNIX_ComputerSystemProvider::modifyInstance(
    const OperationContext& context,
    const CIMObjectPath& ref,
    const CIMInstance& instanceObject,
    const Boolean includeQualifiers,
    const CIMPropertyList& propertyList,
    ResponseHandler& handler)
{
    throw CIMNotSupportedException(String::EMPTY);
}

void UNIX_ComputerSystemProvider::createInstance(
    const OperationContext& context,
    const CIMObjectPath& ref,
    const CIMInstance& instanceObject,
    ObjectPathResponseHandler& handler)
{
    throw CIMNotSupportedException(String::EMPTY);
}

void UNIX_ComputerSystemProvider::deleteInstance(
    const OperationContext& context,
    const CIMObjectPath& ref,
    ResponseHandler& handler)
{
    throw CIMNotSupportedException(String::EMPTY);
}

void UNIX_ComputerSystemProvider::execQuery(
       const OperationContext& context,
       const CIMObjectPath& objectPath,
       const QueryExpression& query,
       InstanceResponseHandler& handler)
{
	CIMName className;
    CIMInstance instance;
    CIMObjectPath newref;

    className = objectPath.getClassName();

    // only return instances when enumerate on our subclass, CIMOM
    // will call us as natural part of recursing through subtree on
    // enumerate - if we return instances on enumerate of our superclass,
    // there would be dups
    if (className.equal(CLASS_EXTENDED_COMPUTER_SYSTEM)
    	|| className.equal (CLASS_CIM_COMPUTER_SYSTEM))
    {
    	UNIX_ComputerSystem _cs;
        handler.processing();

        Array<CIMKeyBinding> keys;
        keys.append(CIMKeyBinding(
            PROPERTY_CREATION_CLASS_NAME,
            CLASS_EXTENDED_COMPUTER_SYSTEM,
            CIMKeyBinding::STRING));
        keys.append(CIMKeyBinding(
            PROPERTY_NAME,
            _cs.getHostName(),
            CIMKeyBinding::STRING));
        CIMObjectPath instanceName(
            String::EMPTY,       // Hostname not required
            CIMNamespaceName(),  // Namespace not required
            CLASS_EXTENDED_COMPUTER_SYSTEM,
            keys);

	    instance = _cs.buildInstance(CLASS_EXTENDED_COMPUTER_SYSTEM);
        if (query.evaluate(instance))
	    {
	        instance.setPath(instanceName);
	        handler.deliver(instance);
	    }
        handler.complete();
    }
    else
    {
        throw CIMNotSupportedException("UNIX_OperatingSystemProvider "
                "does not support class " + className.getString());
    }
    return;
}

void UNIX_ComputerSystemProvider::invokeMethod(
    const OperationContext& context,
    const CIMObjectPath& objectReference,
    const CIMName& methodName,
    const Array<CIMParamValue>& inParameters,
    MethodResultResponseHandler& handler)
{
    throw CIMNotSupportedException(
        "UNIX_ComputerSystemProvider does not support invokeMethod");
}


void UNIX_ComputerSystemProvider::initialize(CIMOMHandle& handle)
{
    _ch = handle;
    // platform-specific routine to initialize protected members
    _cs.initialize();
}


void UNIX_ComputerSystemProvider::terminate()
{
    delete this;
}

void UNIX_ComputerSystemProvider::_checkClass(CIMName& className)
{
    if (!className.equal(CLASS_CIM_COMPUTER_SYSTEM) &&
        !className.equal(CLASS_CIM_UNITARY_COMPUTER_SYSTEM) &&
        !className.equal(CLASS_EXTENDED_COMPUTER_SYSTEM))
    {
        throw CIMNotSupportedException(String::EMPTY);
    }
}