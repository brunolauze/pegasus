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
#include <Pegasus/Common/Time.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Logger.h>
#include "UNIX_OperatingSystem.h"

#include <fcntl.h>
#include <kvm.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/vmmeter.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utmpx.h>
#include <time.h>


PEGASUS_USING_STD;


static kvm_t *kd;

UNIX_OperatingSystem::UNIX_OperatingSystem(void)
{
}

UNIX_OperatingSystem::~UNIX_OperatingSystem(void)
{
}

static Boolean _getOSName(String& osName)
{
    struct utsname unameInfo;

    // Call uname and check for any errors.
    if (uname(&unameInfo) < 0)
    {
       return false;
    }

    osName.assign(unameInfo.sysname);

    return true;
}

static Boolean getUtilGetHostName(String& csName)
{
     char    hostName[PEGASUS_MAXHOSTNAMELEN + 1];

     if (gethostname(hostName, sizeof(hostName)) != 0)
     {
         return false;
     }
     hostName[sizeof(hostName)-1] = 0;

     // Now get the official hostname.  If this call fails then return
     // the value from gethostname().
     /*
     struct  hostent *he;
     if ((he=gethostbyname(hostName)))
     {
         csName.assign(he->h_name);
     }
     else
     {
         csName.assign(hostName);
     }
     */
     csName.assign(hostName);
     return true;
}

Boolean UNIX_OperatingSystem::getCSName(String& csName)
{
    return getUtilGetHostName(csName);
}

Boolean UNIX_OperatingSystem::getName(String& osName)
{
    return _getOSName(osName);
}

Boolean UNIX_OperatingSystem::getCaption(String& caption)
{
    caption.assign("The current Operating System");

    return true;
}

Boolean UNIX_OperatingSystem::getDescription(String& description)
{
      description.assign("This instance reflects the Operating System"
           " on which the CIMOM is executing (as distinguished from instances"
           " of other installed operating systems that could be run).");

   return true;
}

Boolean UNIX_OperatingSystem::getInstallDate(CIMDateTime& installDate)
{

	struct tm* clock;			// create a time structure
	struct stat attrib;			// create a file attribute structure
	stat("/mnt", &attrib);		// get the attributes mnt
	clock = gmtime(&(attrib.st_birthtime));	// Get the last modified time and put it into the time structure

	installDate = CIMDateTime(
			clock->tm_year + 1900, 
			clock->tm_mon + 1, 
			clock->tm_mday,
			clock->tm_hour,
			clock->tm_min,
			clock->tm_sec,
			0,0,
			clock->tm_gmtoff
			);
    return true;
}

Boolean UNIX_OperatingSystem::getStatus(String& status)
{
   status.assign("OK");

   return true;
}

Boolean UNIX_OperatingSystem::getVersion(String& osVersion)
{
    struct utsname  unameInfo;

    // Call uname and check for any errors.

    if (uname(&unameInfo) < 0)
    {
       return false;
    }

    osVersion.assign(unameInfo.release);

    return true;
}

Boolean UNIX_OperatingSystem::getOSType(Uint16& osType)
{
    osType = FreeBSD;
    return true;
}

Boolean UNIX_OperatingSystem::getOtherTypeDescription(String& otherTypeDescription)
{
    struct utsname  unameInfo;
    char version[sizeof(unameInfo.release) + sizeof(unameInfo.version) + 1];

    // Call uname and check for any errors.

    if (uname(&unameInfo) < 0)
    {
       return false;
    }

    sprintf(version, "%s %s", unameInfo.version, unameInfo.release);
    otherTypeDescription.assign(version);

    return true;
}

static CIMDateTime time_t_to_CIMDateTime(time_t *time_to_represent)
{
   struct tm* clock = gmtime(time_to_represent);
   return CIMDateTime(
			clock->tm_year + 1900, 
			clock->tm_mon + 1, 
			clock->tm_mday,
			clock->tm_hour,
			clock->tm_min,
			clock->tm_sec,
			0,0,
			clock->tm_gmtoff
			);
}

Boolean UNIX_OperatingSystem::getLastBootUpTime(CIMDateTime& lastBootUpTime)
{
    int mib[2] = { CTL_KERN, KERN_BOOTTIME };
    struct timeval   tv;
    size_t len = sizeof(tv);

    if (sysctl(mib, 2, &tv, &len, NULL, 0) == -1)
    {
        return false;
    }
    lastBootUpTime = time_t_to_CIMDateTime(&(tv.tv_sec));

    return true;
}

Boolean UNIX_OperatingSystem::getLocalDateTime(CIMDateTime& localDateTime)
{
   time_t now;
   now = time(NULL);
   localDateTime = time_t_to_CIMDateTime(&now);
   return true;
}

Boolean UNIX_OperatingSystem::getCurrentTimeZone(Sint16& currentTimeZone)
{
   	time_t now;
   	now = time(NULL);
    struct tm* clock = gmtime(&(now));
    currentTimeZone = -clock->tm_gmtoff;
    return true;
}

Boolean UNIX_OperatingSystem::getNumberOfLicensedUsers(Uint32& numberOfLicensedUsers)
{
    return false;
}

Boolean UNIX_OperatingSystem::getNumberOfUsers(Uint32& numberOfUsers)
{
    struct utmpx * utmpp;

    numberOfUsers = 0;

    while ((utmpp = getutxent()) != NULL)
    {
        if (utmpp->ut_type == USER_PROCESS)
        {
            numberOfUsers++;
        }
    }

    endutxent();
    return true;
}


Boolean UNIX_OperatingSystem::getNumberOfProcesses(Uint32& numberOfProcesses)
{
    size_t length = 0;

    static const int name[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };

    // Call sysctl with a NULL buffer to get proper length
    if (sysctl((int *)name, (sizeof(name) / sizeof(*name)) - 1, NULL, &length, NULL, 0) != -1)
    {
    	int proc_count = length / sizeof(struct kinfo_proc);
		numberOfProcesses = Uint32(proc_count);
    	return true;
	}
	return false;
}

Boolean UNIX_OperatingSystem::getMaxNumberOfProcesses(Uint32& mMaxProcesses)
{
    long pid_max;
    size_t ret_len = sizeof(pid_max);
    if (sysctlbyname("kern.pid_max", &pid_max, &ret_len, NULL, 0) != -1)
    {
    	mMaxProcesses = Uint32(pid_max);
    	return true;
    }
    return false;
}

Boolean UNIX_OperatingSystem::getTotalSwapSpaceSize(Uint64& mTotalSwapSpaceSize)
{
    long retavail = 0;
	int n;
	int hlen = 0;
	long blocksize = 0;
	getbsize(&hlen, &blocksize);
	int pagesize = getpagesize();
	struct kvm_swap swapary[1];
	n = kvm_getswapinfo(kd, swapary, 1, 0);
	#define CONVERT(v)	((quad_t)(v) * pagesize)
	if (n < 0 || swapary[0].ksw_total == 0) {
		mTotalSwapSpaceSize = 0;
	}
	else {
		retavail = CONVERT(swapary[0].ksw_total);
		//retfree = CONVERT(swapary[0].ksw_total - swapary[0].ksw_used);
		mTotalSwapSpaceSize = Uint64(retavail * 32);
	}
    return true;
}

Boolean UNIX_OperatingSystem::getTotalVirtualMemorySize(Uint64& total)
{
    /*
    int mib[2] = { CTL_HW, HW_REALMEM };
    size_t len = sizeof(total);

    if (sysctl(mib, 2, &total, &len, NULL, 0) == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
	*/
  struct vmtotal vm_info;
  int mib[2];
  
  mib[0] = CTL_VM;
  mib[1] = VM_TOTAL;

  size_t len = sizeof(vm_info);
  sysctl(mib, 2, &vm_info, &len, NULL, 0);

  total = Uint64(vm_info.t_vm);
  return true;
}

Boolean UNIX_OperatingSystem::getFreeVirtualMemory(Uint64& freeVirtualMemory)
{
  struct vmtotal vm_info;
  int mib[2];
  
  mib[0] = CTL_VM;
  mib[1] = VM_TOTAL;

  size_t len = sizeof(vm_info);
  sysctl(mib, 2, &vm_info, &len, NULL, 0);

  freeVirtualMemory = Uint64(vm_info.t_free);
  return true;
}

Boolean UNIX_OperatingSystem::getFreePhysicalMemory(Uint64& total)
{
    size_t len = sizeof(total);
    long val = 0;
    long blocksize = 0;
    int hlen = 0;
    getbsize(&hlen, &blocksize);
    if (sysctlbyname("vm.stats.vm.v_free_count", &val, &len, NULL, 0) == -1)
    {
        return false;
    }
    else
    {
    	total = Uint64(val * blocksize);
        return true;
    }
   return false;
}

Boolean UNIX_OperatingSystem::getTotalVisibleMemorySize(Uint64& memory)
{
    int mib[2] = { CTL_HW, HW_REALMEM };
    size_t len = sizeof(memory);

    if (sysctl(mib, 2, &memory, &len, NULL, 0) == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

Boolean UNIX_OperatingSystem::getSizeStoredInPagingFiles(Uint64& total)
{
    return false;
}

Boolean UNIX_OperatingSystem::getFreeSpaceInPagingFiles(
    Uint64& freeSpaceInPagingFiles)
{
    return false;
}

Boolean UNIX_OperatingSystem::getMaxProcessMemorySize(Uint64& maxProcessMemorySize)
{
    long val = 0;
    size_t len = sizeof(val);
    long blocksize = 0;
    int hlen = 0;
    getbsize(&hlen, &blocksize);
    if (sysctlbyname("kern.maxdsiz", &val, &len, NULL, 0) == -1)
    {
        return false;
    }
    else
    {
    	maxProcessMemorySize = Uint64(val);
        return true;
    }
}

Boolean UNIX_OperatingSystem::getDistributed(Boolean& distributed)
{
    distributed = false;
    return true;
}

Boolean UNIX_OperatingSystem::getMaxProcsPerUser(Uint32& maxProcsPerUser)
{
    int mib[2] = { CTL_KERN, KERN_MAXPROCPERUID };
    long maxp = 0;
    size_t len = sizeof(maxp);

    if (sysctl(mib, 2, &maxp, &len, NULL, 0) == -1)
    {
        return false;
    }
    maxProcsPerUser = Uint32(maxp);
    return true;
}

Boolean UNIX_OperatingSystem::getSystemUpTime(Uint64& mUpTime)
{
    int mib[2] = { CTL_KERN, KERN_BOOTTIME };
    struct timeval   tv;
    size_t len = sizeof(tv);

    if (sysctl(mib, 2, &tv, &len, NULL, 0) == -1)
    {
        return false;
    }
    time_t now;
    now = time(NULL);
    mUpTime = difftime(now,tv.tv_sec);
    return true;
}

Boolean UNIX_OperatingSystem::getOperatingSystemCapability(String& scapability)
{
    char capability[80];
    void *ptr;
    int ptr_bits;
    ptr_bits = 8*sizeof(ptr);
    sprintf (capability, "%d bit", ptr_bits);
    scapability.assign(capability);
    return true;

}

Uint32 UNIX_OperatingSystem::_reboot()
{
   return false;
}

Uint32 UNIX_OperatingSystem::_shutdown()
{
   return false;
}