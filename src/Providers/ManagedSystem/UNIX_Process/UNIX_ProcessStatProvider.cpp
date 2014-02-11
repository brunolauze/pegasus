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


/* ==========================================================================
   Includes.
   ========================================================================== */

#include "UNIX_ProcessStatProvider.h"


// ==========================================================================
// Class names.  These values are the names of the classes that
// are common for all of the providers.
//
// We use CIM_UnitaryComputerSystem as the value of the key
// CSCreationClassName, because this class has properties that
// are important for clients of this provider
// ==========================================================================

#define CLASS_CIM_UNITARY_COMPUTER_SYSTEM "CIM_UnitaryComputerSystem"
#define CLASS_CIM_OPERATING_SYSTEM        "CIM_OperatingSystem"

// Use PG_UnixProcess* until DMTF finalizes CIM_UnixProcess*
#define CLASS_UNIX_PROCESS_STAT           "UNIX_UnixProcessStatisticalInformation"
#define CLASS_UNIX_PROCESS                "UNIX_Process"

/* ==========================================================================
   The number of keys for the classes.
   ========================================================================== */

#define NUMKEYS_UNIX_PROCESS_STAT                    7

/* ==========================================================================
   Property names.  These values are returned by the provider as
   the property names.
   ========================================================================== */

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;


UNIX_ProcessStatProvider::UNIX_ProcessStatProvider()
{
  // cout << "UNIX_ProcessStatProvider::UNIX_ProcessStatProvider()" << endl;
}

UNIX_ProcessStatProvider::~UNIX_ProcessStatProvider()
{
  // cout << "UNIX_ProcessStatProvider::~UNIX_ProcessStatProvider()" << endl;
}

// =============================================================================
// NAME              : _constructKeyBindings
// DESCRIPTION       : Constructs an array of keybindings for process
// ASSUMPTIONS       : None
// PRE-CONDITIONS    :
// POST-CONDITIONS   :
// NOTES             :
// PARAMETERS        : className, Process
// =============================================================================

Array<CIMKeyBinding> UNIX_ProcessStatProvider::_constructKeyBindings(
    const UNIX_Process& _p)
{
    Array<CIMKeyBinding> keyBindings;

    // Construct the key bindings
    keyBindings.append(CIMKeyBinding(PROPERTY_CS_CREATION_CLASS_NAME,
                                  CLASS_CIM_UNITARY_COMPUTER_SYSTEM,
                                  CIMKeyBinding::STRING));
	keyBindings.append(CIMKeyBinding(PROPERTY_CS_NAME,
                                  _hostName,
                                  CIMKeyBinding::STRING));
	
    keyBindings.append(CIMKeyBinding(PROPERTY_OS_CREATION_CLASS_NAME,
                                  CLASS_CIM_OPERATING_SYSTEM,
                                  CIMKeyBinding::STRING));
    keyBindings.append(CIMKeyBinding(PROPERTY_OS_NAME,
                                  _osName,
                                  CIMKeyBinding::STRING));
    keyBindings.append(CIMKeyBinding(PROPERTY_PROCESS_CREATION_CLASS_NAME,
                                  CLASS_IMPLEMENTATION_NAME,
                                  CIMKeyBinding::STRING));
	
    keyBindings.append(CIMKeyBinding(PROPERTY_HANDLE,
                                  _p.getHandle(),
                                  CIMKeyBinding::STRING));
    // We return a time stamp to uniquely identify this
    // instance of UnixProcessStatisticalInformation
    keyBindings.append(CIMKeyBinding(PROPERTY_NAME,
                                  getCurrentTime(),
                                  CIMKeyBinding::STRING));
  return keyBindings;
}


// =============================================================================
// NAME              : _constructInstance
// DESCRIPTION       : Constructs instance by adding its properties. The
//                   : Process instance argument has already been filled in
//                   : with data from an existing process
// ASSUMPTIONS       : None
// PRE-CONDITIONS    :
// POST-CONDITIONS   :
// NOTES             :
// PARAMETERS        : className, Process
// =============================================================================

CIMInstance UNIX_ProcessStatProvider::_constructInstance(
    const CIMName &className,
    const CIMNamespaceName &nameSpace,
    const UNIX_Process &_p)
{
  CIMProperty p;

  CIMInstance inst(className);
  // Set path

  inst.setPath(CIMObjectPath(String::EMPTY, // hostname
                             nameSpace,
                             className,
                             _constructKeyBindings(_p)));
	
  // Add properties

// CIM_ManagedElement

// =================================================
// Keys defined in UnixProcessStatisticalInformation
// =================================================

  // The keys for this class are:
  // [ key ] string CSCreationClassName
  // [ key ] string CSName
  // [ key ] string OSCreationClassName
  // [ key ] string OSName
  // [ key ] string ProcessCreationClassName
  // [ key ] string Handle
  // [ key ] string Name: return a time stamp for client to
  //   be able to distinguish different samples of this data

  // Rather than rebuilding the key properties, we will reuse
  // the values that were inserted for us in the ObjectPath,
  // trusting that this was done correctly

  // Get the keys
  Array<CIMKeyBinding> key = inst.getPath().getKeyBindings();
  // loop through keys, inserting them as properties
  // luckily, all keys for this class are strings, so no
  // need to check key type
  for (unsigned int i=0; i<key.size(); i++)
  {
    // add a property created from the name and value
    inst.addProperty(CIMProperty(key[i].getName(),key[i].getValue()));
  }
  /* Don't call addManagedSystemElementProperties because Name is different */
  #include <CIM_ManagedElementProvider.cpp>
  //ManagedSystemElement
  if (_p.getInstallDate(p)) inst.addProperty(p);
  if (_p.getStatus(p)) inst.addProperty(p);
  if (_p.getCommunicationStatus(p)) inst.addProperty(p);
  if (_p.getDetailedStatus(p)) inst.addProperty(p);
  if (_p.getHealthStatus(p)) inst.addProperty(p);
  if (_p.getOperatingStatus(p)) inst.addProperty(p);
  if (_p.getPrimaryStatus(p)) inst.addProperty(p);
  if (_p.getOperationalStatus(p)) inst.addProperty(p);
  if (_p.getStatusDescriptions(p)) inst.addProperty(p);

// ===============================================
// Properties in UnixProcessStatisticalInformation
// ===============================================
//    uint32 CPUTime
  if (_p.getCPUTime(p)) inst.addProperty(p);

//    uint64 RealText
  if (_p.getRealText(p)) inst.addProperty(p);

//    uint64 RealData
  if (_p.getRealData(p)) inst.addProperty(p);

//    uint64 RealStack
  if (_p.getRealStack(p)) inst.addProperty(p);

//    uint64 VirtualText
  if (_p.getVirtualText(p)) inst.addProperty(p);

//    uint64 VirtualData
  if (_p.getVirtualData(p)) inst.addProperty(p);

//    uint64 VirtualStack
  if (_p.getVirtualStack(p)) inst.addProperty(p);

//    uint64 VirtualMemoryMappedFileSize
  if (_p.getVirtualMemoryMappedFileSize(p)) inst.addProperty(p);

//    uint64 VirtualSharedMemory
  if (_p.getVirtualSharedMemory(p)) inst.addProperty(p);

//    uint64 CpuTimeDeadChildren
  if (_p.getCpuTimeDeadChildren(p)) inst.addProperty(p);

//    uint64 SystemTimeDeadChildren
  if (_p.getSystemTimeDeadChildren(p)) inst.addProperty(p);

//    uint64 RealSpace
  if (_p.getRealSpace(p)) inst.addProperty(p);

  return inst;
} 

#include <CIM_ManagedSystemElementProvider.cpp>