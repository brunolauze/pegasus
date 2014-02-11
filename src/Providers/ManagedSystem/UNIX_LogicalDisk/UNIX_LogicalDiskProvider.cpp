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

#include "UNIX_LogicalDiskProvider.h"

// ==========================================================================
// Property names.  These values are returned by the provider as
// the property names.
// ==========================================================================

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;


UNIX_LogicalDiskProvider::UNIX_LogicalDiskProvider()
{
  	
}

UNIX_LogicalDiskProvider::~UNIX_LogicalDiskProvider()
{
  
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

Array<CIMKeyBinding> UNIX_LogicalDiskProvider::_constructKeyBindings(const UNIX_LogicalDisk& _p)
{
    Array<CIMKeyBinding> keyBindings;

    // Construct the key bindings
    keyBindings.append(CIMKeyBinding(
        PROPERTY_SYSTEM_CREATION_CLASS_NAME,
        BASE_CLASS_NAME,
        CIMKeyBinding::STRING));

    keyBindings.append(CIMKeyBinding(
        PROPERTY_SYSTEM_NAME,
        _hostName,
        CIMKeyBinding::STRING));

    keyBindings.append(CIMKeyBinding(
        PROPERTY_CREATION_CLASS_NAME,
        CLASS_IMPLEMENTATION_NAME,
        CIMKeyBinding::STRING));
	
    keyBindings.append(CIMKeyBinding(
        PROPERTY_DEVICE_ID,
        _p.getHandle(),
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
CIMInstance UNIX_LogicalDiskProvider::_constructInstance(
    const CIMName &className,
    const CIMNamespaceName &nameSpace,
    const UNIX_LogicalDisk &_p)
{
  CIMProperty p;

  CIMInstance inst(className);

  // Set path
  inst.setPath(CIMObjectPath(String::EMPTY, // hostname
                             nameSpace,
                             CLASS_IMPLEMENTATION_CIM_NAME,
                             _constructKeyBindings(_p)));

  addManagedSystemElementProperties(inst, _p);
  //EnabledLogicalElement
  if (_p.getEnabledState(p)) inst.addProperty(p);
  if (_p.getOtherEnabledState(p)) inst.addProperty(p);
  if (_p.getRequestedState(p)) inst.addProperty(p);
  if (_p.getEnabledDefault(p)) inst.addProperty(p);
  if (_p.getTimeOfLastStateChange(p)) inst.addProperty(p);
  if (_p.getAvailableRequestedStates(p)) inst.addProperty(p);
  if (_p.getTransitioningToState(p)) inst.addProperty(p);
  //LogicalDevice
  if (_p.getDeviceID(p)) inst.addProperty(p);
  if (_p.getPowerManagementSupported(p)) inst.addProperty(p);
  if (_p.getPowerManagementCapabilities(p)) inst.addProperty(p);
  if (_p.getAvailability(p)) inst.addProperty(p);
  if (_p.getStatusInfo(p)) inst.addProperty(p);
  if (_p.getLastErrorCode(p)) inst.addProperty(p);
  if (_p.getErrorDescription(p)) inst.addProperty(p);
  if (_p.getErrorCleared(p)) inst.addProperty(p);
  if (_p.getOtherIdentifyingInfo(p)) inst.addProperty(p);
  if (_p.getPowerOnHours(p)) inst.addProperty(p);
  if (_p.getTotalPowerOnHours(p)) inst.addProperty(p);
  if (_p.getIdentifyingDescriptions(p)) inst.addProperty(p);
  if (_p.getAdditionalAvailability(p)) inst.addProperty(p);
  if (_p.getMaxQuiesceTime(p)) inst.addProperty(p);
  if (_p.getDataOrganization(p)) inst.addProperty(p);
  if (_p.getPurpose(p)) inst.addProperty(p);
  if (_p.getAccess(p)) inst.addProperty(p);
  if (_p.getErrorMethodology(p)) inst.addProperty(p);
  if (_p.getBlockSize(p)) inst.addProperty(p);
  if (_p.getNumberOfBlocks(p)) inst.addProperty(p);
  if (_p.getConsumableBlocks(p)) inst.addProperty(p);
  if (_p.getIsBasedOnUnderlyingRedundancy(p)) inst.addProperty(p);
  if (_p.getSequentialAccess(p)) inst.addProperty(p);
  if (_p.getExtentStatus(p)) inst.addProperty(p);
  if (_p.getNoSinglePointOfFailure(p)) inst.addProperty(p);
  if (_p.getDataRedundancy(p)) inst.addProperty(p);
  if (_p.getPackageRedundancy(p)) inst.addProperty(p);
  if (_p.getDeltaReservation(p)) inst.addProperty(p);
  if (_p.getPrimordial(p)) inst.addProperty(p);
  if (_p.getNameFormat(p)) inst.addProperty(p);
  if (_p.getNameNamespace(p)) inst.addProperty(p);
  if (_p.getOtherNameNamespace(p)) inst.addProperty(p);
  if (_p.getOtherNameFormat(p)) inst.addProperty(p);
  if (_p.getCompressed(p)) inst.addProperty(p);
  if (_p.getIsolated(p)) inst.addProperty(p);
  if (_p.getChecksum(p)) inst.addProperty(p);
  if (_p.getExecutable(p)) inst.addProperty(p);
  if (_p.getDeduplicationEnabled(p)) inst.addProperty(p);
  return inst;
}


#include <CIM_ManagedSystemElementProvider.cpp>