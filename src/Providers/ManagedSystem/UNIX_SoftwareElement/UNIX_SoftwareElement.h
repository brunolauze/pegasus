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

#ifndef UNIX_SOFTWARE_ELEMENT_FREEBSD_H
#define UNIX_SOFTWARE_ELEMENT_FREEBSD_H

#define CLASS_IMPLEMENTATION_NAME			"UNIX_SoftwareElement"
#define CLASS_IMPLEMENTATION				UNIX_SoftwareElement
#define BASE_CLASS_NAME 					"CIM_SoftwareElement"
#define ELEMENT_NAME						"Software"
#define NUMKEYS_CLASS_IMPLEMENTATION		4

/* ==========================================================================
   Includes.
   ========================================================================== */
  
#include <UNIX_Common.h>
#include <PackageInfo.h>

#define PROPERTY_VERSION								"Version"
#define PROPERTY_SOFTWARE_ELEMENT_STATE					"SoftwareElementState"
#define PROPERTY_SOFTWARE_ELEMENT_ID					"SoftwareElementID"
#define PROPERTY_TARGET_OPERATING_SYSTEM				"TargetOperatingSystem"
#define PROPERTY_OTHER_TARGET_OS						"OtherTargetOS"
#define PROPERTY_MANUFACTURER							"Manufacturer"
#define PROPERTY_SERIAL_NUMBER							"SerialNumber"
#define PROPERTY_BUILD_NUMBER							"BuildNumber"
#define PROPERTY_CODESET								"CodeSet"
#define PROPERTY_IDENTIFICATION_CODE					"IdentificationCode"
#define PROPERTY_LANGUAGE_EDITION						"LanguageEdition"

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

class UNIX_SoftwareElement 
{
public:
   	UNIX_SoftwareElement();
   	~UNIX_SoftwareElement();

   	#include <CIM_ManagedSystemElement.h>

   	Boolean getVersion(CIMProperty&) const;
   	Boolean getSoftwareElementState(CIMProperty&) const;
   	Boolean getSoftwareElementID(CIMProperty&) const;
   	Boolean getTargetOperatingSystem(CIMProperty&) const;
   	Boolean getOtherTargetOS(CIMProperty&) const;
   	Boolean getManufacturer(CIMProperty&) const;
   	Boolean getSerialNumber(CIMProperty&) const;
   	Boolean getBuildNumber(CIMProperty&) const;
   	Boolean getCodeSet(CIMProperty&) const;
   	Boolean getIdentificationCode(CIMProperty&) const;
   	Boolean getLanguageEdition(CIMProperty&) const;

   	Uint16 getState() const;
   	String getNameValue() const;
   	String getVersionValue() const;

private:
  	pkg_db_state state;
	pkg_info pkg_current;
};
#endif /* UNIX_SOFTWARE_ELEMENT_FREEBSD_H */