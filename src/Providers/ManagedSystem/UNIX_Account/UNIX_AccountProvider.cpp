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
#include "UNIX_AccountProvider.h"



PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

UNIX_AccountProvider::UNIX_AccountProvider()
{
	
}

UNIX_AccountProvider::~UNIX_AccountProvider()
{
}



CIMInstance UNIX_AccountProvider::_constructInstance(
    const CIMName &className,
    const CIMNamespaceName &nameSpace,
    const UNIX_Account &_p)
{
  CIMProperty p;

  CIMInstance inst(className);

  // Set path
  inst.setPath(CIMObjectPath(String(""), // hostname
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
  //CIM_Account
  if (_p.getUserID(p)) inst.addProperty(p);
  if (_p.getObjectClass(p)) inst.addProperty(p);
  if (_p.getDescriptions(p)) inst.addProperty(p);
  if (_p.getHost(p)) inst.addProperty(p);
  if (_p.getLocalityName(p)) inst.addProperty(p);
  if (_p.getOrganizationName(p)) inst.addProperty(p);
  if (_p.getOU(p)) inst.addProperty(p);
  if (_p.getSeeAlso(p)) inst.addProperty(p);
  if (_p.getUserCertificate(p)) inst.addProperty(p);
  if (_p.getUserPassword(p)) inst.addProperty(p);
  if (_p.getUserPasswordEncryptionAlgorithm(p)) inst.addProperty(p);
  if (_p.getOtherUserPasswordEncryptionAlgorithm(p)) inst.addProperty(p);
  if (_p.getComplexPasswordRulesEnforced(p)) inst.addProperty(p);
  if (_p.getInactivityTimeout(p)) inst.addProperty(p);
  if (_p.getLastLogin(p)) inst.addProperty(p);
  if (_p.getMaximumSuccessiveLoginFailures(p)) inst.addProperty(p);
  if (_p.getPasswordExpiration(p)) inst.addProperty(p);
  if (_p.getPasswordHistoryDepth(p)) inst.addProperty(p);
  if (_p.getUserPasswordEncoding(p)) inst.addProperty(p);

  return inst;

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

Array<CIMKeyBinding> UNIX_AccountProvider::_constructKeyBindings(const UNIX_Account& _p)
{
    Array<CIMKeyBinding> keys;

    keys.append(CIMKeyBinding(
        PROPERTY_SYSTEM_CREATION_CLASS_NAME,
        BASE_CLASS_CIM_NAME.getString(),
        CIMKeyBinding::STRING));
    keys.append(CIMKeyBinding(
        PROPERTY_SYSTEM_NAME,
        _hostName,
        CIMKeyBinding::STRING));
    keys.append(CIMKeyBinding(
        PROPERTY_CREATION_CLASS_NAME,
        CLASS_IMPLEMENTATION_CIM_NAME.getString(),
        CIMKeyBinding::STRING));
    keys.append(CIMKeyBinding(
        PROPERTY_NAME,
        _p.getHandle(),
        CIMKeyBinding::STRING));

    return keys;
}
   

#include <CIM_ManagedSystemElementProvider.cpp>