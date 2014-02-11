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

#include "UNIX_SoftwareElementProvider.h"

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

UNIX_SoftwareElementProvider::UNIX_SoftwareElementProvider()
{
	
}

UNIX_SoftwareElementProvider::~UNIX_SoftwareElementProvider()
{
}

CIMInstance UNIX_SoftwareElementProvider::_constructInstance(
    const CIMName &className,
    const CIMNamespaceName &nameSpace,
    const UNIX_SoftwareElement &_p)
{
  CIMProperty p;

  CIMInstance inst(className);
  // Set path
  inst.setPath(CIMObjectPath(String(""), // hostname
                             nameSpace,
                             CLASS_IMPLEMENTATION_CIM_NAME,
                             _constructKeyBindings(_p)));

  
  addManagedSystemElementProperties(inst, _p);
  //SoftwareElement
  if (_p.getVersion(p)) inst.addProperty(p);
  if (_p.getSoftwareElementState(p)) inst.addProperty(p);
  if (_p.getSoftwareElementID(p)) inst.addProperty(p);
  if (_p.getTargetOperatingSystem(p)) inst.addProperty(p);
  if (_p.getOtherTargetOS(p)) inst.addProperty(p);
  if (_p.getManufacturer(p)) inst.addProperty(p);
  if (_p.getSerialNumber(p)) inst.addProperty(p);
  if (_p.getBuildNumber(p)) inst.addProperty(p);
  if (_p.getCodeSet(p)) inst.addProperty(p);
  if (_p.getIdentificationCode(p)) inst.addProperty(p);
  if (_p.getLanguageEdition(p)) inst.addProperty(p);
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

Array<CIMKeyBinding> UNIX_SoftwareElementProvider::_constructKeyBindings(const UNIX_SoftwareElement& _p)
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
	keys.append(CIMKeyBinding(
        PROPERTY_VERSION,
        _p.getVersionValue(),
        CIMKeyBinding::STRING));
	keys.append(CIMKeyBinding(
        PROPERTY_SOFTWARE_ELEMENT_STATE,
        String("2"),
        CIMKeyBinding::NUMERIC));
	keys.append(CIMKeyBinding(
        PROPERTY_SOFTWARE_ELEMENT_ID,
        _p.getHandle(),
        CIMKeyBinding::STRING));
    return keys;
}
   

#include <CIM_ManagedSystemElementProvider.cpp>