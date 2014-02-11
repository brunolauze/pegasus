#include "UNIX_SoftwareElement.h"
#include <fcntl.h>
#include <kvm.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/vmmeter.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

UNIX_SoftwareElement::UNIX_SoftwareElement()
{

}

UNIX_SoftwareElement::~UNIX_SoftwareElement()
{
}

Boolean UNIX_SoftwareElement::getCaption(CIMProperty &p) const
{
	p = CIMProperty(PROPERTY_CAPTION, getHandle());
	return true; 
}

Boolean UNIX_SoftwareElement::getDescription(CIMProperty &p) const
{
	p = CIMProperty(PROPERTY_DESCRIPTION, String(pkg_current.description));
	return true; 
}

Boolean UNIX_SoftwareElement::getInstallDate(CIMProperty &p) const
{
	struct tm* clock;			// create a time structure
	clock = gmtime(&(pkg_current.timestamp));	// Get the last modified time and put it into the time structure

	p = CIMProperty(PROPERTY_INSTALL_DATE,CIMDateTime(
			clock->tm_year + 1900, 
			clock->tm_mon + 1, 
			clock->tm_mday,
			clock->tm_hour,
			clock->tm_min,
			clock->tm_sec,
			0,0,
			clock->tm_gmtoff
			));
    return true;
}

Boolean UNIX_SoftwareElement::getName(CIMProperty &p) const
{
	p = CIMProperty(PROPERTY_NAME, getNameValue());
	return true; 
}

Boolean UNIX_SoftwareElement::getVersion(CIMProperty &p) const
{
	p = CIMProperty(PROPERTY_VERSION, getVersionValue());
	return true; 
}

Boolean UNIX_SoftwareElement::getSoftwareElementState(CIMProperty &p) const
{
	p = CIMProperty(PROPERTY_SOFTWARE_ELEMENT_STATE, getState());
	return true;
}

Boolean UNIX_SoftwareElement::getSoftwareElementID(CIMProperty &p) const
{
	p = CIMProperty(PROPERTY_SOFTWARE_ELEMENT_ID, getHandle());
	return true; 
}

Boolean UNIX_SoftwareElement::getTargetOperatingSystem(CIMProperty &p) const
{
	p = CIMProperty(PROPERTY_TARGET_OPERATING_SYSTEM, Uint16(42));
	/* Check if if it could run on other system */ /* might be hard */
	return true;
}

Boolean UNIX_SoftwareElement::getOtherTargetOS(CIMProperty &p) const
{
	p = CIMProperty(PROPERTY_OTHER_TARGET_OS, String(""));
	return true; 
}

Boolean UNIX_SoftwareElement::getManufacturer(CIMProperty &p) const
{
	p = CIMProperty(PROPERTY_MANUFACTURER, String(pkg_current.maintainer));
	return true; 
}

Boolean UNIX_SoftwareElement::getSerialNumber(CIMProperty &p) const
{
	p = CIMProperty(PROPERTY_SERIAL_NUMBER, String(""));
	return true; 
}
Boolean UNIX_SoftwareElement::getBuildNumber(CIMProperty &p) const
{
	p = CIMProperty(PROPERTY_BUILD_NUMBER, String(""));
	return true; 
}
Boolean UNIX_SoftwareElement::getCodeSet(CIMProperty &p) const
{
	p = CIMProperty(PROPERTY_CODESET, String("UTF-8"));
	return true; 
}
Boolean UNIX_SoftwareElement::getIdentificationCode(CIMProperty &p) const
{
	p = CIMProperty(PROPERTY_IDENTIFICATION_CODE, getHandle());
	return true;
}
Boolean UNIX_SoftwareElement::getLanguageEdition(CIMProperty &p) const
{
	p = CIMProperty(PROPERTY_LANGUAGE_EDITION, String("Multilingual"));
	return true; 
}

Boolean UNIX_SoftwareElement::getInstanceID(CIMProperty &p) const
{
    p = CIMProperty(
        PROPERTY_INSTANCE_ID,
        getHandle());
    return true;
}

String UNIX_SoftwareElement::getNameValue() const
{
	return String(pkg_current.name);
}

String UNIX_SoftwareElement::getVersionValue() const
{
	return String(pkg_current.version);
}

String UNIX_SoftwareElement::getHandle() const
{
	String elementID(getNameValue());
	elementID.append("-");
	elementID.append(getVersionValue());
	return elementID;
}

Uint16 UNIX_SoftwareElement::getState() const
{
	/* Check if it's running */ /* might be hard */ /* 2 = Executable - 3 = Running */
    return Uint16(2);
}


Boolean UNIX_SoftwareElement::find(String &handle)
{
	return false;
}

#define __initialize_H
Boolean UNIX_SoftwareElement::initialize()
{
	PackageInfo_init(&state);
	return true;
}

Boolean UNIX_SoftwareElement::load(int &pIndex)
{
	if (PackageInfo_print_package_names(&state, &pkg_current) == 0)
	{
		//cout << "Name: " << pkg_current.name << " - Version: " << pkg_current.version << endl;
		return true;
	}
	return false;
}

#define __finalize_H
Boolean UNIX_SoftwareElement::finalize()
{
	PackageInfo_finalize(&state);
	return true;
}


Boolean UNIX_SoftwareElement::validateKey(CIMKeyBinding &kb) const
{
	/* Keys  */
	//CreationClassName
	//SystemCreationClassName
	//Name
	//Version
	//SystemName
	//SoftwareElementState
	//SoftwareElementID

	CIMName name = kb.getName();
	if (name.equal(PROPERTY_CREATION_CLASS_NAME) ||
		name.equal(PROPERTY_SYSTEM_NAME) ||
		name.equal(PROPERTY_SYSTEM_CREATION_CLASS_NAME) ||
		name.equal(PROPERTY_NAME) ||
		name.equal(PROPERTY_VERSION) ||
		name.equal(PROPERTY_SOFTWARE_ELEMENT_STATE) ||
		name.equal(PROPERTY_SOFTWARE_ELEMENT_ID))
	{
		return true;
	}
	return false;
}

void UNIX_SoftwareElement::buildKeyHandle(CIMKeyBinding &kb, String &s) const
{
	//Name
	if (kb.getName().equal(PROPERTY_NAME))
	{
		s.append(kb.getValue());
	}
	else if (kb.getName().equal(PROPERTY_VERSION)) {
		s.append("-");
		s.append(kb.getValue());
	}
}

#include <CIM_ManagedSystemElement.cpp>