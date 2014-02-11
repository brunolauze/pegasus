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


// ==========================================================================
// Includes.
// ==========================================================================

#include "UNIX_ProcessProvider.h"

// ==========================================================================
// Class names.  These values are the names of the classes that
// are common for all of the providers.
//
// We use CIM_UnitaryComputerSystem as the value of the key
// CSCreationClassName, because this class has properties that
// are important for clients of this provider
// ==========================================================================

#define CLASS_CIM_UNITARY_COMPUTER_SYSTEM  "CIM_UnitaryComputerSystem"
#define CLASS_CIM_OPERATING_SYSTEM         "CIM_OperatingSystem"

// ==========================================================================
// Property names.  These values are returned by the provider as
// the property names.
// ==========================================================================


PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;


UNIX_ProcessProvider::UNIX_ProcessProvider()
{
  // cout << "UNIX_ProcessProvider::UNIX_ProcessProvider()" << endl;
}

UNIX_ProcessProvider::~UNIX_ProcessProvider()
{
  // cout << "UNIX_ProcessProvider::~UNIX_ProcessProvider()" << endl;
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

Array<CIMKeyBinding> UNIX_ProcessProvider::_constructKeyBindings(const UNIX_Process& _p)
{
    Array<CIMKeyBinding> keyBindings;

    // Construct the key bindings
    keyBindings.append(CIMKeyBinding(
        PROPERTY_CS_CREATION_CLASS_NAME,
        CLASS_CIM_UNITARY_COMPUTER_SYSTEM,
        CIMKeyBinding::STRING));

    keyBindings.append(CIMKeyBinding(
        PROPERTY_CS_NAME,
        _hostName,
        CIMKeyBinding::STRING));

    keyBindings.append(CIMKeyBinding(
        PROPERTY_OS_CREATION_CLASS_NAME,
        CLASS_CIM_OPERATING_SYSTEM,
        CIMKeyBinding::STRING));

    keyBindings.append(CIMKeyBinding(
        PROPERTY_OS_NAME,
        _osName,
        CIMKeyBinding::STRING));

    keyBindings.append(CIMKeyBinding(
        PROPERTY_CREATION_CLASS_NAME,
        _getClassName(),
        CIMKeyBinding::STRING));

    keyBindings.append(CIMKeyBinding(
        PROPERTY_HANDLE,
        _p.getHandle(),
        CIMKeyBinding::STRING));
    return keyBindings;
}

CIMInstance UNIX_ProcessProvider::_constructInstance(
    const CIMName &className,
    const CIMNamespaceName &nameSpace,
    const UNIX_Process &_p)
{
  CIMProperty p;
  CIMInstance inst(className);

  // Set path

  inst.setPath(CIMObjectPath(String::EMPTY, // hostname
                             nameSpace,
                             CLASS_IMPLEMENTATION_CIM_NAME,
                             _constructKeyBindings(_p)));

  addManagedSystemElementProperties(inst, _p);

  // ===========================================
  // The following properties are in CIM_Process
  // ===========================================

  // The keys for this class are:
  // [ key ] string CSCreationClassName
  // [ key ] string CSName
  // [ key ] string OSCreationClassName
  // [ key ] string OSName
  // [ key ] string CreationClassName
  // [ key ] string Handle

   
//   uint32 Priority
  if (_p.getPriority(p)) inst.addProperty(p);

//   uint16 ExecutionState
  if (_p.getExecutionState(p)) inst.addProperty(p);

//   string OtherExecutionDescription
  if (_p.getOtherExecutionDescription(p)) inst.addProperty(p);

//   datetime CreationDate
  if (_p.getCreationDate(p))
    inst.addProperty(p);

//   datetime TerminationDate
  if (_p.getTerminationDate(p))
    inst.addProperty(p);

//   uint64 KernelModeTime
  if (_p.getKernelModeTime(p))
    inst.addProperty(p);

//   uint64 UserModeTime
  if (_p.getUserModeTime(p))
    inst.addProperty(p);

//   uint64 WorkingSetSize
  if (_p.getWorkingSetSize(p))
    inst.addProperty(p);
  
// =============================================
// return if the requested class was CIM_Process
// =============================================

  if (className.equal (BASE_CLASS_NAME)) return inst;

// ===========================================
// The following properties are in UnixProcess
// ===========================================

//    string ParentProcessID
  if (_p.getParentProcessID(p)) inst.addProperty(p);

//    uint64 RealUserID
  if (_p.getRealUserID(p)) inst.addProperty(p);



//    uint64 ProcessGroupID
  if (_p.getProcessGroupID(p)) inst.addProperty(p);

//    uint64 ProcessSessionID
  if (_p.getProcessSessionID(p)) inst.addProperty(p);

//    string ProcessTTY
  if (_p.getProcessTTY(p)) inst.addProperty(p);

//    string ModulePath
  if (_p.getModulePath(p)) inst.addProperty(p);


//    string Parameters[]
  if (_p.getParameters(p)) inst.addProperty(p);

//    uint32 ProcessNiceValue
  if (_p.getProcessNiceValue(p)) inst.addProperty(p);

//    string ProcessWaitingForEvent
  if (_p.getProcessWaitingForEvent(p)) inst.addProperty(p);

  return inst;
}

#include <CIM_ManagedSystemElementProvider.cpp>