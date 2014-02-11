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

#include "UNIX_LogicalDisk.h"

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sys/sysctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fstab.h>
#include <paths.h>
#include <pwd.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libutil.h>
#include <time.h>
#include "zfs_client.h"
#include "smartdisk_client.h"

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

#define ZFS_FS "zfs"
#define SWAP_FS "swap"
#define PROC_FS "procfs"
#define PROC_FS_STRING_VAL "Special filesystem that presents information about processes and other system information in a hierarchical file-like structure."
#define DEV_FS "devfs"
#define DEV_FS_STRING_VAL "Specific implementation of a device file system used for presenting device files."
#define LINPROC_FS "linprocfs"
#define LINPROC_FS_STRING_VAL "Special linux emulation layer filesystem that presents information about processes and other system information in a hierarchical file-like structure."
#define LINSYS_FS "linsysfs"
#define LINSYS_FS_STRING_VAL "Special filesystem that emulates a subset of the Linux sys file system and is required for the complete operation of some Linux binaries."
#define FDESC_FS "fdescfs"
#define FDESC_FS_STRING_VAL "File-descriptor file system, or fdescfs, provides access to the per-process file descriptor namespace in the global file system namespace."

#define L__PATH_ROOT 		"/"
#define L__PATH_LIB 		"/lib"
#define L__PATH_TMP 		"/tmp"
#define L__PATH_VAR 		"/var"
#define L__PATH_VAR_LOG		"/var/log"
#define L__PATH_VAR_TMP		"/var/tmp"
#define L__PATH_VAR_RUN		"/var/run"
#define L__PATH_USR 		"/usr"
#define L__PATH_USR_LIB		"/usr/lib"
#define L__PATH_USR_BIN 	"/usr/bin"
#define L__PATH_USR_SBIN	"/usr/sbin"


time_t getOSInstallDate()
{
	struct stat attrib;			// create a file attribute structure
	stat("/mnt", &attrib);		// get the attributes mnt
	return attrib.st_birthtime;	// Get the last modified time and put it into the time structure
}

time_t getLastModified(char* path)
{
	struct stat attrib;			// create a file attribute structure
	stat(path, &attrib);		// get the attributes mnt
	return attrib.st_mtime;	// Get the last modified time and put it into the time structure
}

bool get_disk(pl_disk_t* pInfo, int &pIndex)
{
	//struct fstab *fs;
	int mntsize;
	struct statfs *mntbuf;
	if ((mntsize = getmntinfo(&mntbuf, MNT_NOWAIT)) != 0)
	{
		if (mntsize <= pIndex) return false;
		pInfo->name = mntbuf[pIndex].f_mntfromname;
		pInfo->path = mntbuf[pIndex].f_mntonname;
		pInfo->description = mntbuf[pIndex].f_fstypename;
		pInfo->enabledState = 2; // ENABLED
		char estate[0];
		pInfo->otherEnabledState = estate;
		pInfo->requestedState = 2;
		pInfo->enabledDefault = 2;
		Array<Uint16> availableRequestedStates;
		availableRequestedStates.append(2);availableRequestedStates.append(3);availableRequestedStates.append(4);availableRequestedStates.append(5);availableRequestedStates.append(6);availableRequestedStates.append(7);availableRequestedStates.append(8);availableRequestedStates.append(9);availableRequestedStates.append(10);
		pInfo->availableRequestedStates = availableRequestedStates;
		pInfo->transitioningToState = 12; // NOT APPLICABLE
		pInfo->deviceID = mntbuf[pIndex].f_mntfromname;
		pInfo->powerManagementSupported = true;
		Array<Uint16> powerManagementCapabilities;
		powerManagementCapabilities.append(3); powerManagementCapabilities.append(4); powerManagementCapabilities.append(5); powerManagementCapabilities.append(6); powerManagementCapabilities.append(7);
		pInfo->powerManagementCapabilities = powerManagementCapabilities;
		pInfo->availability = 3; // FULL POWER: TODO
		pInfo->statusInfo = 3; // ENABLED
		pInfo->lastErrorCode = 0;
		pInfo->errorDescription = ""; /* TODO: Will have the  disk error description */
		pInfo->errorCleared = false;
		//Array<String> otherIdentifyingInfo;
		//pInfo->otherIdentifyingInfo = otherIdentifyingInfo;
		Array<String> identifyingDescriptions;
		identifyingDescriptions.append(pInfo->deviceID);
		identifyingDescriptions.append(mntbuf[pIndex].f_mntfromname);
		pInfo->identifyingDescriptions = identifyingDescriptions;
		//uint16_t additionalAvailability[0];
		pInfo->maxQuiesceTime = 0;
		pInfo->access = 0;  // "Unknown", "Readable", "Writeable", "Read/Write Supported", "Write Once";
		pInfo->errorMethodology = ""; // COULD BE SMART
		pInfo->blockSize = mntbuf[pIndex].f_bsize;
		pInfo->consumableBlocks = mntbuf[pIndex].f_blocks;
		pInfo->isBasedOnUnderlyingRedundancy = false; // CHECK RAID
		pInfo->sequentialAccess = false;
		Array<Uint16> extentStatus;
		extentStatus.append(15);
		pInfo->extentStatus = extentStatus;
		pInfo->noSinglePointOfFailure = false;
		pInfo->dataRedundancy = 1;
		pInfo->packageRedundancy = 0;
		pInfo->deltaReservation = 0;
		/*
		cout << "Ending pInfo initialize" << endl;
	    cout << "Mounted From: " << mntbuf[pIndex].f_mntfromname << endl;  // mounted filesystem [MFSNAMELEN] 
		cout << "Mounted To: " << mntbuf[pIndex].f_mntonname << endl;    // directory on which mounted [MFSNAMELEN]
		cout << " Type Name: " << mntbuf[pIndex].f_fstypename << endl; // filesystem type name [MFSNAMELEN]
	    cout << " Version: " << mntbuf[pIndex].f_version << endl;             // structure version number
		cout << " Type: " << mntbuf[pIndex].f_type << endl;                // type of filesystem
		cout << " Flags: " << mntbuf[pIndex].f_flags << endl;               // copy of mount exported flags
		cout << " Fragment Size: " << mntbuf[pIndex].f_bsize << endl;               // filesystem fragment size
		cout << " IO Size: " << mntbuf[pIndex].f_iosize << endl;              // optimal transfer block size
		cout << " Total Size: " << mntbuf[pIndex].f_blocks << endl;              // total data blocks in filesystem
		cout << " Free: " << mntbuf[pIndex].f_bfree << endl;               // free blocks in filesystem
	    cout << " Available: " << mntbuf[pIndex].f_bavail << endl;              // free blocks avail to non-superuser
	    cout << " Total Files: " << mntbuf[pIndex].f_files << endl;               // total file nodes in filesystem
	    cout << " Free Nodes: " << mntbuf[pIndex].f_ffree << endl;               // free nodes avail to non-superuser
	    cout << " Sync Writes: " << mntbuf[pIndex].f_syncwrites << endl;          // count of sync writes since mount
	    cout << " Async Writes: " << mntbuf[pIndex].f_asyncwrites << endl;         // count of async writes since mount
	    cout << " Sync Reads: " << mntbuf[pIndex].f_syncreads << endl;           // count of sync reads since mount
	    cout << " Async Reads: " << mntbuf[pIndex].f_asyncreads << endl;          // count of async reads since mount
	    cout << " Spare: " << mntbuf[pIndex].f_spare << endl;           // unused spare 10
	    cout << " NameMax: " << mntbuf[pIndex].f_namemax << endl;             // maximum filename length
	    cout << " Owner: " << mntbuf[pIndex].f_owner << endl;              // user that mounted the filesystem
	    cout << " FS Id: " << mntbuf[pIndex].f_fsid.val << endl;               // filesystem id
	    cout << " CharSpare: " << mntbuf[pIndex].f_charspare << endl;          // spare string space 80
	    */
	    if (strcmp(mntbuf[pIndex].f_fstypename, ZFS_FS) == 0)
		{
			pInfo->dataOrganization = 3;
			pInfo->purpose = "";
			zfs_client zclient;
			//Get property
			pInfo->creationDate = zclient.getCreationDate(mntbuf[pIndex].f_mntfromname);
			pInfo->compressed = zclient.getCompressed(mntbuf[pIndex].f_mntfromname);
			pInfo->isolated = zclient.getIsolated(mntbuf[pIndex].f_mntfromname);
			pInfo->checksum = zclient.getChecksum(mntbuf[pIndex].f_mntfromname);
			pInfo->executable = zclient.getExecutable(mntbuf[pIndex].f_mntfromname);
			pInfo->deduplicationEnabled = zclient.getDeduplicationEnabled(mntbuf[pIndex].f_mntfromname);
			if (strcmp(mntbuf[pIndex].f_mntonname,  L__PATH_ROOT) == 0 || 
				strcmp(mntbuf[pIndex].f_mntonname,  L__PATH_LIB) == 0 ||
				strcmp(mntbuf[pIndex].f_mntonname,  L__PATH_TMP) == 0 ||
				strcmp(mntbuf[pIndex].f_mntonname,  L__PATH_VAR) == 0 ||
				strcmp(mntbuf[pIndex].f_mntonname,  L__PATH_VAR_LOG) == 0 ||
				strcmp(mntbuf[pIndex].f_mntonname,  L__PATH_VAR_TMP) == 0 ||
				strcmp(mntbuf[pIndex].f_mntonname,  L__PATH_VAR_RUN) == 0 ||
				strcmp(mntbuf[pIndex].f_mntonname,  L__PATH_USR) == 0 ||
				strcmp(mntbuf[pIndex].f_mntonname,  L__PATH_USR_LIB) == 0 ||
				strcmp(mntbuf[pIndex].f_mntonname,  L__PATH_USR_BIN) == 0 ||
				strcmp(mntbuf[pIndex].f_mntonname,  L__PATH_USR_SBIN) == 0)
			{
				pInfo->primordial = true;
			}
		}
		else {
			pInfo->dataOrganization = 2;
			pInfo->creationDate = getOSInstallDate();
			pInfo->compressed = false;
			pInfo->isolated = false;
			pInfo->checksum = false;
			pInfo->executable = true;
			pInfo->deduplicationEnabled = false;
			pInfo->primordial = true;
			if (strcmp(mntbuf[pIndex].f_fstypename, PROC_FS) == 0)
				pInfo->purpose = PROC_FS_STRING_VAL;
			else if (strcmp(mntbuf[pIndex].f_fstypename, FDESC_FS) == 0)
				pInfo->purpose = FDESC_FS_STRING_VAL;
			else if (strcmp(mntbuf[pIndex].f_fstypename, DEV_FS) == 0)
				pInfo->purpose = DEV_FS_STRING_VAL;
			else if (strcmp(mntbuf[pIndex].f_fstypename, LINPROC_FS) == 0)
				pInfo->purpose = LINPROC_FS_STRING_VAL;
			else if (strcmp(mntbuf[pIndex].f_fstypename, LINSYS_FS) == 0)
				pInfo->purpose = LINSYS_FS_STRING_VAL;
			else
				pInfo->purpose = "";
		}

		//SMART VALUES
		smartdisk_client smartcli;
		pInfo->powerOnHours = smartcli.getPowerOnHours(mntbuf[pIndex].f_mntfromname);
		pInfo->totalPowerOnHours = smartcli.getTotalPowerOnHours(mntbuf[pIndex].f_mntfromname);
	}

	pInfo->timeOfLastStateChange = getLastModified(pInfo->path);
	// FSTAB
	//	
	//while ((fs = getfsent()) != NULL) {
	//	cout << fs->fs_spec << " : " << fs->fs_file << endl;
	//}

	return true;
}


UNIX_LogicalDisk::UNIX_LogicalDisk()
{
}

UNIX_LogicalDisk::~UNIX_LogicalDisk()
{
}



//ManagedElement
Boolean UNIX_LogicalDisk::getCaption(CIMProperty& p) const
{
	p = CIMProperty(
        PROPERTY_CAPTION,
        String(pInfo.path));
    return true;
}

Boolean UNIX_LogicalDisk::getDescription(CIMProperty& p) const
{
	p = CIMProperty(
        PROPERTY_DESCRIPTION,
        String(pInfo.description));
    return true;
}

Boolean UNIX_LogicalDisk::getInstanceID(CIMProperty &p) const
{
    p = CIMProperty(
        PROPERTY_INSTANCE_ID,
        String(pInfo.name));
    return true;
}


//ManagedSystemElement
Boolean UNIX_LogicalDisk::getInstallDate(CIMProperty& p) const
{
	struct tm* clock;			// create a time structure
	clock = gmtime(&(pInfo.creationDate));	// Get the last modified time and put it into the time structure

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

Boolean UNIX_LogicalDisk::getName(CIMProperty& p) const
{
    // can vary, depending on class
    p = CIMProperty(
        PROPERTY_NAME,
        String(pInfo.name));
    return true;
}


//EnabledLogicalElement
Boolean UNIX_LogicalDisk::getEnabledState(CIMProperty& p) const
{
    p = CIMProperty(
        PROPERTY_ENABLED_STATE, pInfo.enabledState);
    return true;
}

Boolean UNIX_LogicalDisk::getOtherEnabledState(CIMProperty& p) const
{
    p = CIMProperty(
        PROPERTY_OTHER_ENABLED_STATE,
        String(pInfo.otherEnabledState));
    return true;
}

Boolean UNIX_LogicalDisk::getRequestedState(CIMProperty& p) const
{
    p = CIMProperty(
        PROPERTY_REQUESTED_STATE, pInfo.requestedState);
    return true;
}

Boolean UNIX_LogicalDisk::getEnabledDefault(CIMProperty& p) const
{
    p = CIMProperty(
        PROPERTY_ENABLED_DEFAULT, pInfo.enabledDefault);
    return true;
}

Boolean UNIX_LogicalDisk::getTimeOfLastStateChange(CIMProperty& p) const
{
	struct tm* clock;			// create a time structure
	clock = gmtime(&(pInfo.timeOfLastStateChange));	// Get the last modified time and put it into the time structure
	p = CIMProperty(PROPERTY_TIME_OF_LAST_STATE_CHANGE,CIMDateTime(
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

Boolean UNIX_LogicalDisk::getAvailableRequestedStates(CIMProperty& p) const
{
	p = CIMProperty(
        PROPERTY_AVAILABLE_REQUESTED_STATES, pInfo.availableRequestedStates);
    return true;
}

Boolean UNIX_LogicalDisk::getTransitioningToState(CIMProperty& p) const
{
    p = CIMProperty(
        PROPERTY_TRANSITIONING_TO_STATE, pInfo.transitioningToState);
    return true;
}
//LogicalDevice

Boolean UNIX_LogicalDisk::getDeviceID(CIMProperty& p) const
{
    p = CIMProperty(
        PROPERTY_DEVICE_ID,
        String(pInfo.deviceID));
    return true;
}

Boolean UNIX_LogicalDisk::getPowerManagementSupported(CIMProperty& p) const
{
    p = CIMProperty(
        PROPERTY_POWER_MANAGEMENT_SUPPORTED,
        Boolean(pInfo.powerManagementSupported));
    return true;
}

Boolean UNIX_LogicalDisk::getPowerManagementCapabilities(CIMProperty& p) const
{
    p = CIMProperty(
        PROPERTY_POWER_MANAGEMENT_CAPABILITIES,
        pInfo.powerManagementCapabilities);
    return true;
}

Boolean UNIX_LogicalDisk::getAvailability(CIMProperty& p) const
{
    p = CIMProperty(
        PROPERTY_AVAILABILITY,
        Boolean(pInfo.availability));
    return true;
}

Boolean UNIX_LogicalDisk::getStatusInfo(CIMProperty& p) const
{
    p = CIMProperty(
        PROPERTY_STATUS_INFO,
        pInfo.statusInfo);
    return true;
}

Boolean UNIX_LogicalDisk::getLastErrorCode(CIMProperty& p) const
{
    p = CIMProperty(
        PROPERTY_LAST_ERROR_CODE, pInfo.lastErrorCode);
    return true;
}

Boolean UNIX_LogicalDisk::getErrorDescription(CIMProperty& p) const
{
    p = CIMProperty(PROPERTY_ERROR_DESCRIPTION, 
    		String(pInfo.errorDescription));
    return true;
}

Boolean UNIX_LogicalDisk::getErrorCleared(CIMProperty& p) const
{
    p = CIMProperty(PROPERTY_ERROR_CLEARED, 
    		Boolean(pInfo.errorCleared));
    return true;
}

Boolean UNIX_LogicalDisk::getOtherIdentifyingInfo(CIMProperty& p) const
{
    p = CIMProperty(PROPERTY_OTHER_IDENTIFYING_INFO, 
    		pInfo.otherIdentifyingInfo);
    return true;
}

Boolean UNIX_LogicalDisk::getPowerOnHours(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_POWER_ON_HOURS, 
    		Uint64(pInfo.powerOnHours));
    return true;
}

Boolean UNIX_LogicalDisk::getTotalPowerOnHours(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_TOTAL_POWER_ON_HOURS, 
    		Uint64(pInfo.totalPowerOnHours));
    return true;
}

Boolean UNIX_LogicalDisk::getIdentifyingDescriptions(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_IDENTIFYING_DESCRIPTIONS, 
    		pInfo.identifyingDescriptions);
    return true;
}

Boolean UNIX_LogicalDisk::getAdditionalAvailability(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_ADDITIONAL_AVAILABILITY, 
    		pInfo.additionalAvailability);
    return true;
}

Boolean UNIX_LogicalDisk::getMaxQuiesceTime(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_MAX_QUIESCE_TIME, 
    		pInfo.maxQuiesceTime);
    return true;
}

Boolean UNIX_LogicalDisk::getDataOrganization(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_DATA_ORGANIZATION, 
    		Uint16(pInfo.dataOrganization));
    return true;
}

Boolean UNIX_LogicalDisk::getPurpose(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_PURPOSE, 
    		String(pInfo.purpose));
    return true;
}

Boolean UNIX_LogicalDisk::getAccess(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_ACCESS, 
    		Uint16(pInfo.access));
    return true;
}

Boolean UNIX_LogicalDisk::getErrorMethodology(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_ERROR_METHODOLOGY, 
    		String(pInfo.errorMethodology));
    return true;
}

Boolean UNIX_LogicalDisk::getBlockSize(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_BLOCK_SIZE, 
    		Uint64(pInfo.blockSize));
    return true;
}

Boolean UNIX_LogicalDisk::getNumberOfBlocks(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_NUMBER_OF_BLOCKS, 
    		Uint64(pInfo.consumableBlocks)); /* TODO: REVIEW */
    return true;
}

Boolean UNIX_LogicalDisk::getConsumableBlocks(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_CONSUMABLE_BLOCKS, 
    		Uint64(pInfo.consumableBlocks));
    return true;
}

Boolean UNIX_LogicalDisk::getIsBasedOnUnderlyingRedundancy(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_IS_BASED_ON_UNDERLYING_REDUNDANCY, 
    		pInfo.isBasedOnUnderlyingRedundancy);
    return true;
}

Boolean UNIX_LogicalDisk::getSequentialAccess(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_SEQUENTIAL_ACCESS, 
    		Boolean(pInfo.sequentialAccess));
    return true;
}

Boolean UNIX_LogicalDisk::getExtentStatus(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_EXTENT_STATUS, 
    		pInfo.extentStatus);
    return false;
}

Boolean UNIX_LogicalDisk::getNoSinglePointOfFailure(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_NO_SINGLE_POINT_OF_FAILURE, 
    		Boolean(pInfo.noSinglePointOfFailure));
    return true;
}

Boolean UNIX_LogicalDisk::getDataRedundancy(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_DATA_REDUNDANCY, 
    		Uint16(pInfo.dataRedundancy));
    return true;
}

Boolean UNIX_LogicalDisk::getPackageRedundancy(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_PACKAGE_REDUNDANCY, 
    		Uint16(pInfo.packageRedundancy));
    return true;
}

Boolean UNIX_LogicalDisk::getDeltaReservation(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_DELTA_RESERVATION, 
    		Uint8(pInfo.deltaReservation));
    return true;
}

Boolean UNIX_LogicalDisk::getPrimordial(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_PRIMORDIAL, 
    		Boolean(pInfo.primordial));
    return true;
}

Boolean UNIX_LogicalDisk::getNameFormat(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_NAME_FORMAT, Uint16(12)); /* OS DEVICE NAME */
    return true;
}

Boolean UNIX_LogicalDisk::getNameNamespace(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_NAME_NAMESPACE, Uint16(8));
    return true;
}

Boolean UNIX_LogicalDisk::getOtherNameNamespace(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_OTHER_NAME_NAMESPACE, String(NAME_FORMAT));
    return true;
}

Boolean UNIX_LogicalDisk::getOtherNameFormat(CIMProperty& p) const
{
	p = CIMProperty(PROPERTY_OTHER_NAME_FORMAT, String(NAME_FORMAT));
    return true;
}

Boolean UNIX_LogicalDisk::getCompressed(CIMProperty& p) const
{
    p = CIMProperty(PROPERTY_COMPRESSED, 
    		Boolean(pInfo.compressed));
    return true;
}

Boolean UNIX_LogicalDisk::getIsolated(CIMProperty& p) const
{
    p = CIMProperty(PROPERTY_ISOLATED, 
    		Boolean(pInfo.isolated));
    return true;
}

Boolean UNIX_LogicalDisk::getChecksum(CIMProperty& p) const
{
    p = CIMProperty(PROPERTY_CHECKSUM, 
    		pInfo.checksum);
    return true;
}

Boolean UNIX_LogicalDisk::getExecutable(CIMProperty& p) const
{
    p = CIMProperty(PROPERTY_EXECUTABLE, 
    		pInfo.executable);
    return true;
}

Boolean UNIX_LogicalDisk::getDeduplicationEnabled(CIMProperty& p) const
{
    p = CIMProperty(PROPERTY_DEDUPLICATION_ENABLED, 
    		pInfo.deduplicationEnabled);
    return true;
}

Boolean UNIX_LogicalDisk::load(int &pIndex)
{
	return get_disk(&pInfo, pIndex);
}

Boolean UNIX_LogicalDisk::find(String &handle)
{
	return false;
}

/*
================================================================================
NAME              : getHandle
DESCRIPTION       : Call uname() and get the operating system name.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
String UNIX_LogicalDisk::getHandle() const
{
  return String(pInfo.name);
}

Boolean UNIX_LogicalDisk::validateKey(CIMKeyBinding &kb) const
{
	/* Keys  */
	//CreationClassName
	//SystemCreationClassName
	//Name
	//SystemName

	CIMName name = kb.getName();
	if (name.equal(PROPERTY_CREATION_CLASS_NAME) ||
		name.equal(PROPERTY_SYSTEM_NAME) ||
		name.equal(PROPERTY_SYSTEM_CREATION_CLASS_NAME) ||
		name.equal(PROPERTY_DEVICE_ID))
	{
		return true;
	}
	return false;
}

void UNIX_LogicalDisk::buildKeyHandle(CIMKeyBinding &kb, String &s) const
{
	//Name
	if (kb.getName().equal(PROPERTY_NAME))
	{
		s.append(kb.getValue());
	}
}

#include <CIM_ManagedSystemElement.cpp>