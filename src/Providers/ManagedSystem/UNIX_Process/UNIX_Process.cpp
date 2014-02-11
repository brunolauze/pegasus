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

#include <time.h>               // for time(0)
#include <sys/param.h>          // for MAXPATHLEN
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>           // for struct stat

#include <cstdint>
#include <fcntl.h>
#include <kvm.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <time.h>
#include <paths.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vis.h>
#include "UNIX_ProcessPlatform.h"

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

UNIX_Process::UNIX_Process()
{
}

UNIX_Process::~UNIX_Process()
{
}

/*
================================================================================
NAME              : getCaption
DESCRIPTION       : currently returning the basename of the process
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getCaption(CIMProperty& p) const
{
  p = CIMProperty(PROPERTY_CAPTION,String(pInfo.pst_ucomm));
  return true;
}

/*
================================================================================
NAME              : getDescription
DESCRIPTION       : currently only returns the same thing as getCaption
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getDescription(CIMProperty& p) const
{
  p = CIMProperty(PROPERTY_DESCRIPTION,String(pInfo.pst_cmd));
  return true;
}

Boolean UNIX_Process::getInstanceID(CIMProperty &p) const
{
    p = CIMProperty(
        PROPERTY_INSTANCE_ID,
        getHandle());
    return true;
}

/*
================================================================================
NAME              : getInstallDate
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getInstallDate(CIMProperty& p) const
{
  // doesn't make much sense for a process
  return false;
}

/*
================================================================================
NAME              : getName
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getName(CIMProperty& p) const
{
  // We will return the basename of the executable image,
  // rather than the actual command line, since this is
  // reliably the name of an executable image, unlike the
  // first token on the command line, which could be an
  // alias, or could even be something completely irrelevant
  p = CIMProperty(PROPERTY_NAME,String(pInfo.pst_ucomm));
  return true;
}

/*
================================================================================
NAME              : getPriority
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getPriority(CIMProperty& p) const
{
  p = CIMProperty(PROPERTY_PRIORITY,Uint32(pInfo.pst_pri));
  return true;
}

/*
================================================================================
NAME              : getExecutionState
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getExecutionState(CIMProperty& p) const
{
  /*
     From the MOF for this class:
      [Description (
        "Indicates the current operating condition of the Process. "
        "Values include ready (2), running (3), and blocked (4), "
        "among others."),
       Values {"Unknown", "Other", "Ready", "Running",
               "Blocked", "Suspended Blocked", "Suspended Ready",
               "Terminated", "Stopped", "Growing" },
   */
  Uint16 i16;
  enum
  { Unknown,
    Other,
    Ready,
    Running,
    Blocked,
    Suspended_Blocked,
    Suspended_Ready,
    Terminated,
    Stopped,
    Growing
  };

  switch (pInfo.pst_stat)
  {
  case 'S':
    i16 = Suspended_Ready;
    break;

  case 'R':
    i16 = Running;
    break;

  case 'T':
    i16 = Stopped;
    break;

  case 'Z':
    i16 = Other; // this is coordinated with OtherExecutionDescription
    break;

  case 'I':
    i16 = Ready;
    break;

  case 'O':
    i16 = Other; // This is coordinated with OtherExecutionDescription
    break;

  default:
    i16 = Unknown;
  }
  p = CIMProperty(PROPERTY_EXECUTION_STATE,Uint16(i16));
  return true;
}

/*
================================================================================
NAME              : getOtherExecutionDescription
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getOtherExecutionDescription(CIMProperty& p) const
{
  String s;
  switch (pInfo.pst_stat)
  {
  case 'Z':
    s = "Zombie";
    break;

  case 'O':
    s = "Other";
    break;

  default:
    s = ""; // ExecutionState is not Other
    // In this case, the caller must know to set the
    // property value to NULL (XML: no <VALUE> element)
  }
  if (String::equal(s,String::EMPTY))
  {
    p = CIMProperty(PROPERTY_OTHER_EXECUTION_DESCRIPTION,
                               CIMValue(CIMTYPE_STRING, false));
  }
  else
  {
    p = CIMProperty(PROPERTY_OTHER_EXECUTION_DESCRIPTION,s);
  }
  return true;
}



/*
================================================================================
NAME              : getCreationDate
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getCreationDate(CIMProperty& p) const
{
  struct tm* clock;			// create a time structure
  time_t time = (time_t)pInfo.pst_start;
  clock = gmtime(&(time));	// Get the last modified time and put it into the time structure
  p = CIMProperty(PROPERTY_CREATION_DATE, CIMDateTime(
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

/*
================================================================================
NAME              : getTerminationDate
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getTerminationDate(CIMProperty& p) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getKernelModeTime
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getKernelModeTime(CIMProperty& p) const
{
  // time obtained in seconds from pstat needs to be
  // returned in milliseconds
  p = CIMProperty(PROPERTY_KERNEL_MODE_TIME,Uint64(1000 * pInfo.pst_stime));
  return true;
}

/*
================================================================================
NAME              : getUserModeTime
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getUserModeTime(CIMProperty& p) const
{
  // time obtained in seconds from pstat needs to be
  // returned in milliseconds
  p = CIMProperty(PROPERTY_USER_MODE_TIME,Uint64(1000 * pInfo.pst_utime));
  return true;
}

/*
================================================================================
NAME              : getWorkingSetSize
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getWorkingSetSize(CIMProperty& p) const
{
  /*
     From MOF:
      [Gauge, Description (
        "The amount of memory in bytes that a Process needs to "
        "execute efficiently, for an OperatingSystem that uses "
        "page-based memory management.  If an insufficient amount "
        "of memory is available (< working set size), thrashing "
        "will occur.  If this information is not known, NULL or 0 "
        "should be entered.  If this data is provided, it could be "
        "monitored to understand a Process' changing memory "
        "requirements as execution proceeds."),
       Units ("Bytes") ]
   uint64 WorkingSetSize;
   */
  p = CIMProperty(PROPERTY_WORKING_SET_SIZE,Uint64(0));
  return true;
}

/*
================================================================================
NAME              : getRealUserID
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getRealUserID(CIMProperty& p) const
{
  p = CIMProperty(PROPERTY_REAL_USER_ID,Uint64(pInfo.pst_ruid));
  return true;
}

/*
================================================================================
NAME              : getProcessGroupID
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getProcessGroupID(CIMProperty& p) const
{
  p = CIMProperty(PROPERTY_PROCESS_GROUP_ID,Uint64(pInfo.pst_gid));
  return true;
}

/*
================================================================================
NAME              : getProcessSessionID
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getProcessSessionID(CIMProperty& p) const
{
  p = CIMProperty(PROPERTY_PROCESS_SESSION_ID,Uint64(pInfo.pst_sid));
  return true;
}

/*
================================================================================
NAME              : getProcessTTY
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getProcessTTY(CIMProperty& p) const
{
  char buf[100];
  String s;
  if( pInfo.pst_tty == -1 )
    s = "?";
  else
  {
    sprintf(buf,"%d",pInfo.pst_tty);
    s.assign(buf);
  }
  p = CIMProperty(PROPERTY_PROCESS_TTY,s);
  return true;
}

/*
================================================================================
NAME              : getModulePath
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getModulePath(CIMProperty& p) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getParameters
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getParameters(CIMProperty &p) const
{
  Array<String> as;
  Uint32 idx_new=0, idx_old=0;
  String s = pInfo.pst_cmd;
  while( idx_new != PEG_NOT_FOUND )
  {
    idx_new = pInfo.pst_cmd.find(idx_old, ' ');
    if( idx_new != PEG_NOT_FOUND )
    {
        s = pInfo.pst_cmd.subString(idx_old,idx_new-idx_old);
        as.append(s);
        idx_old = idx_new+1;
    }
    else if (idx_new != 4294967295) /* Seems to return int32_t max when not found instead -1 */
    {
        s = pInfo.pst_cmd.subString(idx_old);
        as.append(s);
        return true;
    }
  }
  p = CIMProperty(PROPERTY_PARAMETERS,as);
  return true;
}

/*
================================================================================
NAME              : getProcessNiceValue
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getProcessNiceValue(CIMProperty& p) const
{
  p = CIMProperty(PROPERTY_PROCESS_NICE_VALUE,Uint32(pInfo.pst_nice));
  return true;
}

/*
================================================================================
NAME              : getProcessWaitingForEvent
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getProcessWaitingForEvent(CIMProperty& p) const
{
  // not supported
  //PROPERTY_PROCESS_WAITING_FOR_EVENT
  return false;
}

/*
================================================================================
NAME              : getCPUTime
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getCPUTime(CIMProperty& p) const
{
  p = CIMProperty(PROPERTY_CPU_TIME,Uint32(pInfo.pst_pctcpu));
  return false;
}

/*
================================================================================
NAME              : getRealText
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getRealText(CIMProperty& p) const
{
  p = CIMProperty(PROPERTY_REAL_TEXT,Uint64(pInfo.pst_tsize * getpagesize() / 1024));
  return true;
}

/*
================================================================================
NAME              : getRealData
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getRealData(CIMProperty& p) const
{
  p = CIMProperty(PROPERTY_REAL_DATA,Uint64(pInfo.pst_dsize * getpagesize() / 1024));
  return true;
}

/*
================================================================================
NAME              : getRealStack
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getRealStack(CIMProperty& p) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getVirtualText
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getVirtualText(CIMProperty& p) const
{
  // not supported
  return false;
}

/*
================================================================================
NAME              : getVirtualData
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getVirtualData(CIMProperty& p) const
{
  p = CIMProperty(PROPERTY_VIRTUAL_DATA,Uint64(pInfo.pst_vdsize * getpagesize() / 1024));
  return true;
}

/*
================================================================================
NAME              : getVirtualStack
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getVirtualStack(CIMProperty& p) const
{
  p = CIMProperty(PROPERTY_VIRTUAL_STACK,Uint64(pInfo.pst_vssize * getpagesize() / 1024));
  return true;
}

/*
================================================================================
NAME              : getVirtualMemoryMappedFileSize
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getVirtualMemoryMappedFileSize(CIMProperty& p) const
{
  p = CIMProperty(PROPERTY_VIRTUAL_MEMORY_MAPPED_FILESIZE,Uint64(pInfo.pst_vmmsize * getpagesize() / 1024));
  return true;
}

/*
================================================================================
NAME              : getVirtualSharedMemory
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getVirtualSharedMemory(CIMProperty& p) const
{
  p = CIMProperty(PROPERTY_VIRTUAL_SHARED_MEMORY, Uint64(pInfo.pst_vshmsize * getpagesize() / 1024));
  return true;
}

/*
================================================================================
NAME              : getCpuTimeDeadChildren
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getCpuTimeDeadChildren(CIMProperty& p) const
{
  p = CIMProperty(PROPERTY_CPU_TIME_DEAD_CHILDREN, Uint64(pInfo.pst_cutime));
  return false;
}

/*
================================================================================
NAME              : getSystemTimeDeadChildren
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getSystemTimeDeadChildren(CIMProperty& p) const
{
  p = CIMProperty(PROPERTY_SYSTEM_TIME_DEAD_CHILDREN, Uint64(pInfo.pst_cstime));
  return false;
}

/*
================================================================================
NAME              : getParentProcessID
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::getParentProcessID(CIMProperty& p) const
{
  char buf[100];
  sprintf(buf,"%d",pInfo.pst_ppid);
  p = CIMProperty(PROPERTY_PARENT_PROCESS_ID, String(buf));
  return true;
}

/*
================================================================================
NAME              : getRealSpace
DESCRIPTION       :
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : returns a modified pIndex that can be used in a
                    subsequent call to get next process structure
================================================================================
*/
Boolean UNIX_Process::getRealSpace(CIMProperty& p) const
{
  CIMProperty(PROPERTY_REAL_SPACE,Uint64((pInfo.pst_dsize +  // real data
         pInfo.pst_tsize)   // real text  may need to figure out how to get
                            // realStackSize
        * getpagesize() / 1024));
  return true;
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
String UNIX_Process::getHandle() const
{
  char buf[100];
  sprintf(buf,"%d",pInfo.pst_pid);
  return String(buf);
}

/*
================================================================================
NAME              : load
DESCRIPTION       : get process info from system into internal data struct
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::load(int &pIndex)
{
  // This routine fills in the protected member pInfo by calling
  // get_proc. Because Linux process entries are not contiguous
  // this routine modifies pIndex so that the caller, after
  // incrementing pIndex, will be able to fetch the next process in
  // a subsequent call.
  // It returns true if it succeeded in fetching a process, otherwise
  // false, incidating that there are no more processes to be fetched.

  // get_proc() an empty peg_proc_t to fill in, an integer ,
  // and flag: either GET_PROC_BY_INDEX or GET_PROC_BY_PID
  // if GET_PROC_BY_INDEX is set, get_proc takes the integer (pIndex)
  // and locates the (pIndex)th process in the proc filesystem.
  // After the call is completed pIndex is incremented.

  // How this works: get_proc walks through the /proc directory and
  // counts as it steps through process dirs within proc. When it
  // encounters the pIndex'th process dir within /proc it instantiates
  // pInfo with that reference and update pIndex

  // If GET_PROC_BY_PID is set, get_proc will find the process by the pid
  // get_proc(&pList, &pInfo, pid, GET_PROC_BY_PID);

  return get_proc(&pList, &pInfo, pIndex, GET_PROC_BY_INDEX);

  // get_proc returns true if it successfully located a process dir at the
  // pIndex search if we return a false, we either can't open /proc, which
  // will be logger, or we are out of processes
}

/*
================================================================================
NAME              : findProcess
DESCRIPTION       : find the requested process and load its data
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean UNIX_Process::find(String &handle)
{
  // Convert handle to an integer
  int pid = atoi(handle.getCString());
  return get_proc(&pList, &pInfo, pid, GET_PROC_BY_PID);

}


Boolean UNIX_Process::validateKey(CIMKeyBinding &kb) const
{
	/* Keys  */
	//CSCreationClassName
	//CSName
	//OSCreationClassName
	//OSName
	//Handle

	CIMName name = kb.getName();
	if (name.equal(PROPERTY_CS_CREATION_CLASS_NAME) ||
		name.equal(PROPERTY_CS_NAME)  ||
		name.equal(PROPERTY_OS_CREATION_CLASS_NAME) ||
		name.equal(PROPERTY_OS_NAME) ||
		name.equal(PROPERTY_HANDLE))
	{
		return true;
	}
	return false;
}

void UNIX_Process::buildKeyHandle(CIMKeyBinding &kb, String &s) const
{
	//Handle
	if (kb.getName().equal(PROPERTY_HANDLE))
	{
		s.append(kb.getValue());
	}
}


/* ==========================================================================
    Functions.
   ========================================================================== */
/*
================================================================================
NAME              : file2str
DESCRIPTION       : opens a file and turns it into a string ad sends the result
                  : back as char *ret
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/

int file2str(char *directory, const char *myFile, char *ret, int cap) {
    char filename[80];
    int fd, num_read;

    sprintf(filename, "%s/%s", directory, myFile);
    if ( (fd       = open(filename, O_RDONLY, 0)) == -1 ) return -1;
    if ( (num_read = read(fd, ret, cap - 1))      <= 0 ) num_read = -1;
    else ret[num_read] = 0;
    close(fd);
    return num_read;
}

/*
================================================================================
NAME              : parseProcStatm
DESCRIPTION       : parses data from within /proc/X/statm extracting for proc X
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
void parseProcStatm(char* inputFileString, peg_proc_t* P) {
    int num;
    num = sscanf(
        inputFileString, "%ld %ld %ld %ld",
        &P->size, &P->pst_dsize, &P->pst_vshmsize,
        &P->pst_tsize);
}

/*
================================================================================
NAME              : parseProcStat
DESCRIPTION       : parses data from within /proc/X/stat extracting for proc X
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             : returns true upon successful stat read
================================================================================
*/
Boolean  parseProcStat(char* inputFileString, peg_proc_t* P) {
    int num;
    int  int_not_used;
    long long_not_used;
    char* tmp = strrchr(inputFileString, ')');
    if (!tmp)
    {
       // Process must have died on us, rendering the reading of
       // /proc/X/stat impossible. So let's get outta here.
       return false;
    }

    *tmp = '\0';
    memset(P->pst_ucomm, 0, sizeof P->pst_ucomm);
    /* ucomm[16] in kernel */
    sscanf(inputFileString, "%d (%15c", &P->pst_pid, P->pst_ucomm);
    num = sscanf(
       tmp + 2,  /* skip space after ')' too */
       "%c "
       "%d %d %d %d %d "
       "%lu %lu %lu %lu %lu %lu %lu "
       "%ld %ld %ld %ld %ld %ld "
       "%lu %lu ",
       &P->pst_stat,
       &P->pst_ppid, &P->pst_pgrp, &P->pst_sid, &P->pst_tty, &int_not_used,
       &long_not_used, &long_not_used, &long_not_used, &long_not_used,
       &long_not_used,
       &P->pst_utime, &P->pst_stime,
       &P->pst_cutime, &P->pst_cstime, &P->pst_pri, &P->pst_nice,
       &long_not_used,
       &long_not_used,
       &P->pst_start, &P->pst_vmmsize);

    if (P->pst_tty == 0)
        P->pst_tty = -1;
    return true;
}


/*
================================================================================
NAME              : parseProcStatus
DESCRIPTION       : parses data from within /proc/X/status extracting for proc X
ASSUMPTIONS       :
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
void parseProcStatus (char* inputFileString, peg_proc_t* P) {
    /*
     * sets pst_ucomm , pst_stat, pst_pid, pst_ppid, pst_uid,
     * pst_gid, pst_vmmsize, pst_vdsize, pst_vssize
     *
     */

    char* tmp;

    memset(P->pst_ucomm, 0, sizeof P->pst_ucomm);
    sscanf (inputFileString, "Name:\t%15c", P->pst_ucomm);
    tmp = strchr(P->pst_ucomm,'\n');
    if(tmp)
       *tmp='\0';

    tmp = strstr (inputFileString,"State");
    if(tmp)
    sscanf (tmp, "State:\t%c", &P->pst_stat);

    tmp = strstr (inputFileString,"Pid:");
    if(tmp) sscanf (tmp,
        "Pid:\t%d\n"
        "PPid:\t%d\n",
        &P->pst_pid,
        &P->pst_ppid
    );

    tmp = strstr (inputFileString,"Uid:");
    if(tmp) sscanf (tmp,
        "Uid:\t%d",
        &P->pst_uid
    );

    tmp = strstr (inputFileString,"Gid:");
    if(tmp) sscanf (tmp,
        "Gid:\t%d",
        &P->pst_gid
    );

    tmp = strstr (inputFileString,"VmSize:");
    if (tmp)
        sscanf(tmp, "VmSize: %lu kB\n", &P->pst_vmmsize);
    else
        P->pst_vmmsize = 0;

    tmp = strstr (inputFileString,"VmData:");
    if (tmp)
        sscanf(
            tmp,
            "VmData: %lu kB\n"
            "VmStk: %lu kB\n",
            &P->pst_vdsize,
            &P->pst_vssize);
    else /* probably a kernel kernel thread */
    {
        P->pst_vdsize = 0;
        P->pst_vssize = 0;
    }
}

static char *
cmdpart(char *arg0)
{
	char *cp;

	return ((cp = strrchr(arg0, '/')) != NULL ? cp + 1 : arg0);
}

Boolean get_proc(Array<pid_t>* pList, peg_proc_t* p_t, int &pIndex , Boolean find_by_pid)
{
    char errbuf[1024];
    kvm_t *kd = kvm_openfiles(_PATH_DEVNULL, "/dev/null", _PATH_DEVNULL, O_RDONLY, errbuf);
    int num_procs;
    if (!kd) { fprintf(stderr, "kvm_openfiles failed : %s\n", errbuf); return false; }
    struct kinfo_proc *proc_table = kvm_getprocs(kd, KERN_PROC_ALL, 0, &num_procs);
    bool skipped = false;
    bool found = false;
    if (pIndex < num_procs) 
    {
	    for (int i = 0; i < num_procs; i++) {
	    	if (!skipped && i < pIndex) continue;
	    	bool passed = true;
	    	struct kinfo_proc *pproc = &proc_table[i];
	    	if ((pproc->ki_pid > 0) && (pproc->ki_stat == SZOMB))
	    	{
	    		skipped = true;
	      		passed = false;
	  		}
	    	char **proc_argv = kvm_getargv(kd, pproc, 0);
			//char **penv = kvm_getenvv(kd, pproc, 0);
			if (proc_argv == NULL) 
			{
				skipped = true;
				passed = false;
			}
			for (int j = 0, n = pList->size(); j < n; j++)
			{
				if (pList->getData()[j] == pproc->ki_pid) 
				{
					i++;
					skipped = true;
					passed = false;
				}
			}
			if (passed)
			{
				int ie;
			    for(ie = 0; ie < 20; ie++)
			    {
			    	p_t->pst_ucomm[ie] = pproc->ki_comm[ie];
		    	}
		    	p_t->pst_cmd = proc_argv[0];
		    	p_t->pst_stat = pproc->ki_stat;
		    	p_t->pst_pid = pproc->ki_pid;
		    	p_t->pst_ppid = pproc->ki_ppid;
		    	p_t->pst_uid = pproc->ki_uid;
		    	p_t->pst_ruid = pproc->ki_ruid;
		    	p_t->pst_gid = pproc->ki_pgid;
		    	p_t->pst_sid = pproc->ki_sid;
		    	p_t->pst_pgrp = pproc->ki_pgid;
				p_t->pst_tty = pproc->ki_sid;
		    	p_t->pst_vdsize = 0;
		    	p_t->pst_vssize = 0;
				p_t->pst_vmmsize = 0;
		    	p_t->pst_start = pproc->ki_start.tv_sec;
		    	p_t->pst_dsize = 0;
		    	p_t->pst_stime = pproc->ki_rusage.ru_stime.tv_usec;                //kernel time
			    p_t->pst_utime = pproc->ki_rusage.ru_utime.tv_usec;
			    p_t->pst_cutime = pproc->ki_rusage_ch.ru_utime.tv_usec;
			    p_t->pst_cstime = pproc->ki_rusage_ch.ru_stime.tv_usec;
			    p_t->pst_pri = (int)pproc->ki_pri.pri_level;
			    p_t->pst_nice = pproc->ki_nice;
			    p_t->pst_vshmsize = 0;
			    p_t->pst_tsize = 0;
			    p_t->pst_pctcpu = 0;
			    p_t->size = 0;
			    pList->append(pproc->ki_pid);
			    found = true;
			    break;
		    }
	    }
    }
    kvm_close(kd);
    return found;
}

void doPercentCPU(char *inputFileString, peg_proc_t *P)
{
  // Need to get current time/ process uptime, and calc the rest.

   unsigned long seconds_since_boot, seconds;
   unsigned long pcpu  = 0;
   unsigned long total_time;

   if( sscanf(inputFileString, " %lu.", &seconds_since_boot))
   {
     total_time = P->pst_utime + P->pst_stime +P->pst_cutime + P->pst_cstime;
     seconds =seconds_since_boot - P->pst_start;
     if(seconds) pcpu = total_time * 1000 / seconds;
     P->pst_pctcpu = (pcpu > 999)? 999 : pcpu;
   }
   else P->pst_pctcpu = 0;
}

#include <CIM_ManagedSystemElement.cpp>