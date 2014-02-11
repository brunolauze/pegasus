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

#ifndef UNIX_LOGICALDISK_FREEBSD_H
#define UNIX_LOGICALDISK_FREEBSD_H

/* ==========================================================================
   Includes.
   ========================================================================== */
#include <dirent.h>
#include <fcntl.h>    // for O_RDONLY
#include <stdio.h>    // for sprintf
#include <unistd.h>   // for close, read and getpagesize
#include <string.h>   // for strchr
#include <pthread.h>  // for pthreads and spinlocks
#include <UNIX_Common.h>


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

#define CLASS_IMPLEMENTATION_NAME			"UNIX_LogicalDisk"
#define CLASS_IMPLEMENTATION				UNIX_LogicalDisk
#define BASE_CLASS_NAME 					"CIM_LogicalDisk"
#define ELEMENT_NAME						"LogicalDisk"
#define NUMKEYS_CLASS_IMPLEMENTATION		4



//EnabledLogicalElement
#define PROPERTY_ENABLED_STATE				"EnabledState"
#define PROPERTY_OTHER_ENABLED_STATE		"OtherEnabledState"
#define PROPERTY_REQUESTED_STATE			"RequestedState"
#define PROPERTY_ENABLED_DEFAULT			"EnabledDefault"
#define PROPERTY_TIME_OF_LAST_STATE_CHANGE	"TimeOfLastStateChange"
#define PROPERTY_AVAILABLE_REQUESTED_STATES	"AvailableRequestedStates"
#define PROPERTY_TRANSITIONING_TO_STATE		"TransitioningToState"

//LogicalDevice
#define PROPERTY_DEVICE_ID					"DeviceID"
#define PROPERTY_POWER_MANAGEMENT_SUPPORTED	"PowerManagementSupported"
#define PROPERTY_POWER_MANAGEMENT_CAPABILITIES "PowerManagementCapabilities"
#define PROPERTY_AVAILABILITY				"Availability"
#define PROPERTY_STATUS_INFO				"StatusInfo"
#define PROPERTY_LAST_ERROR_CODE			"LastErrorCode"
#define PROPERTY_ERROR_DESCRIPTION			"ErrorDescription"
#define PROPERTY_ERROR_CLEARED				"ErrorCleared"
#define PROPERTY_OTHER_IDENTIFYING_INFO		"OtherIdentifyingInfo"
#define PROPERTY_POWER_ON_HOURS				"PowerOnHours"
#define PROPERTY_TOTAL_POWER_ON_HOURS		"TotalPowerOnHours"
#define PROPERTY_IDENTIFYING_DESCRIPTIONS	"IdentifyingDescriptions"
#define PROPERTY_ADDITIONAL_AVAILABILITY	"AdditionalAvailability"
#define PROPERTY_MAX_QUIESCE_TIME			"MaxQuiesceTime"
#define PROPERTY_DATA_ORGANIZATION			"DataOrganization"
#define PROPERTY_PURPOSE					"Purpose"
#define PROPERTY_ACCESS						"Access"
#define PROPERTY_ERROR_METHODOLOGY			"ErrorMethodology"
#define PROPERTY_BLOCK_SIZE					"BlockSize"
#define PROPERTY_NUMBER_OF_BLOCKS			"NumberOfBlocks"
#define PROPERTY_CONSUMABLE_BLOCKS			"ConsumableBlocks"
#define PROPERTY_IS_BASED_ON_UNDERLYING_REDUNDANCY "IsBasedOnUnderlyingRedundancy"
#define PROPERTY_SEQUENTIAL_ACCESS			"SequentialAccess"
#define PROPERTY_EXTENT_STATUS				"ExtentStatus"
#define PROPERTY_NO_SINGLE_POINT_OF_FAILURE "NoSinglePointOfFailure"
#define PROPERTY_DATA_REDUNDANCY			"DataRedundancy"
#define PROPERTY_PACKAGE_REDUNDANCY			"PackageRedundancy"
#define PROPERTY_DELTA_RESERVATION			"DeltaReservation"
#define PROPERTY_PRIMORDIAL					"Primordial"
#define PROPERTY_NAME_FORMAT				"NameFormat"
#define PROPERTY_NAME_NAMESPACE				"NameNamespace"
#define PROPERTY_OTHER_NAME_NAMESPACE		"OtherNameNamespace"
#define PROPERTY_OTHER_NAME_FORMAT			"OtherNameFormat"
#define PROPERTY_COMPRESSED		 			"Compressed" 
#define PROPERTY_ISOLATED					"Isolated"
#define PROPERTY_CHECKSUM					"Checksum"
#define PROPERTY_EXECUTABLE					"Executable"
#define PROPERTY_DEDUPLICATION_ENABLED		"DeduplicationEnabled"

#define NAME_FORMAT ""

typedef struct pl_disk_info {
	char* name;
	char* path;
	char* description;
	char* status;
	time_t creationDate;
	Uint16 enabledState;
	char* otherEnabledState;
	Uint16 requestedState;
	Uint16 enabledDefault;
	time_t timeOfLastStateChange;
	Array<Uint16> availableRequestedStates;
	Uint16 transitioningToState;
	const char* deviceID;
	bool powerManagementSupported;
	Array<Uint16> powerManagementCapabilities;
	Uint16 availability;
	Uint16 statusInfo;
	Uint32 lastErrorCode;
	char* errorDescription;
	bool errorCleared;
	Array<String> otherIdentifyingInfo;
	Uint64 powerOnHours;
	Uint64 totalPowerOnHours;
	Array<String> identifyingDescriptions;
	Array<Uint16> additionalAvailability;
	Uint64 maxQuiesceTime;
	Uint16 dataOrganization;
	const char* purpose;
	Uint16 access;
	char* errorMethodology;
	Uint64 blockSize;
	Uint64 consumableBlocks;
	bool isBasedOnUnderlyingRedundancy;
	bool sequentialAccess;
	Array<Uint16> extentStatus;
	bool noSinglePointOfFailure;
	Uint16 dataRedundancy;
	Uint16 packageRedundancy;
	Uint8 deltaReservation;
	bool primordial;
	bool compressed;
	bool isolated;
	bool checksum;
	bool executable;
	bool deduplicationEnabled;

    struct pl_disk_info *l, *r; // linked list pointers
} pl_disk_t;

bool get_disk(pl_disk_t* pInfo, int &pIndex);

class UNIX_LogicalDisk
{
public:

  UNIX_LogicalDisk();
  ~UNIX_LogicalDisk();

  #include <CIM_EnabledLogicalElement.h>

  //LogicalDevice
  Boolean getDeviceID(CIMProperty&) const;
  Boolean getPowerManagementSupported(CIMProperty&) const;
  Boolean getPowerManagementCapabilities(CIMProperty&) const;
  Boolean getAvailability(CIMProperty&) const;
  Boolean getStatusInfo(CIMProperty&) const;
  Boolean getLastErrorCode(CIMProperty&) const;
  Boolean getErrorDescription(CIMProperty&) const;
  Boolean getErrorCleared(CIMProperty&) const;
  Boolean getOtherIdentifyingInfo(CIMProperty&) const;
  Boolean getPowerOnHours(CIMProperty&) const;
  Boolean getTotalPowerOnHours(CIMProperty&) const;
  Boolean getIdentifyingDescriptions(CIMProperty&) const;
  Boolean getAdditionalAvailability(CIMProperty&) const;
  Boolean getMaxQuiesceTime(CIMProperty&) const;
  Boolean getDataOrganization(CIMProperty&) const;
  Boolean getPurpose(CIMProperty&) const;
  Boolean getAccess(CIMProperty&) const;
  Boolean getErrorMethodology(CIMProperty&) const;
  Boolean getBlockSize(CIMProperty&) const;
  Boolean getNumberOfBlocks(CIMProperty&) const;
  Boolean getConsumableBlocks(CIMProperty&) const;
  Boolean getIsBasedOnUnderlyingRedundancy(CIMProperty&) const;
  Boolean getSequentialAccess(CIMProperty&) const;
  Boolean getExtentStatus(CIMProperty&) const;
  Boolean getNoSinglePointOfFailure(CIMProperty&) const;
  Boolean getDataRedundancy(CIMProperty&) const;
  Boolean getPackageRedundancy(CIMProperty&) const;
  Boolean getDeltaReservation(CIMProperty&) const;
  Boolean getPrimordial(CIMProperty&) const;
  Boolean getNameFormat(CIMProperty&) const;
  Boolean getNameNamespace(CIMProperty&) const;
  Boolean getOtherNameNamespace(CIMProperty&) const;
  Boolean getOtherNameFormat(CIMProperty&) const;
  Boolean getCompressed(CIMProperty&) const;
  Boolean getIsolated(CIMProperty&) const;
  Boolean getChecksum(CIMProperty&) const;
  Boolean getExecutable(CIMProperty&) const;
  Boolean getDeduplicationEnabled(CIMProperty&) const;

private:
  pl_disk_t pInfo;
};

#endif  /* #ifndef UNIX_LOGICALDISK_FREEBSD_H */