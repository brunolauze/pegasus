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

#ifndef UNIX_PROCESS_FREEBSD_H
#define UNIX_PROCESS_FREEBSD_H

/* ==========================================================================
   Includes.
   ========================================================================== */

#define CLASS_IMPLEMENTATION_NAME			"UNIX_Process"
#define CLASS_IMPLEMENTATION				UNIX_Process
#define BASE_CLASS_NAME 					"CIM_Process"
#define ELEMENT_NAME						"Process"

#include <dirent.h>
#include <fcntl.h>    // for O_RDONLY
#include <stdio.h>    // for sprintf
#include <unistd.h>   // for close, read and getpagesize
#include <string.h>   // for strchr
#include <pthread.h>  // for pthreads and spinlocks
#include <UNIX_Common.h>


// Properties in CIM_Process
#define PROPERTY_HANDLE                      "Handle"
#define PROPERTY_PRIORITY                    "Priority"
#define PROPERTY_EXECUTION_STATE             "ExecutionState"
#define PROPERTY_OTHER_EXECUTION_DESCRIPTION "OtherExecutionDescription"
#define PROPERTY_CREATION_DATE               "CreationDate"
#define PROPERTY_TERMINATION_DATE            "TerminationDate"
#define PROPERTY_KERNEL_MODE_TIME            "KernelModeTime"
#define PROPERTY_USER_MODE_TIME              "UserModeTime"
#define PROPERTY_WORKING_SET_SIZE            "WorkingSetSize"

// Properties in UnixProcess

#define PROPERTY_PARENT_PROCESS_ID           "ParentProcessID"
#define PROPERTY_REAL_USER_ID                "RealUserID"
#define PROPERTY_PROCESS_GROUP_ID            "ProcessGroupID"
#define PROPERTY_PROCESS_SESSION_ID          "ProcessSessionID"
#define PROPERTY_PROCESS_TTY                 "ProcessTTY"
#define PROPERTY_MODULE_PATH                 "ModulePath"
#define PROPERTY_PARAMETERS                  "Parameters"
#define PROPERTY_PROCESS_NICE_VALUE          "ProcessNiceValue"
#define PROPERTY_PROCESS_WAITING_FOR_EVENT   "ProcessWaitingForEvent"

//STATS PROPERTIES
// Keys
#define PROPERTY_CS_CREATION_CLASS_NAME      "CSCreationClassName"
#define PROPERTY_CS_NAME                     "CSName"
#define PROPERTY_OS_CREATION_CLASS_NAME      "OSCreationClassName"
#define PROPERTY_OS_NAME                     "OSName"
#define PROPERTY_PROCESS_CREATION_CLASS_NAME "ProcessCreationClassName"

// Local properties
#define PROPERTY_CPU_TIME                    "CPUTime"
#define PROPERTY_REAL_TEXT                   "RealText"
#define PROPERTY_REAL_DATA                   "RealData"
#define PROPERTY_REAL_STACK                  "RealStack"
#define PROPERTY_VIRTUAL_TEXT                "VirtualText"
#define PROPERTY_VIRTUAL_DATA                "VirtualData"
#define PROPERTY_VIRTUAL_STACK               "VirtualStack"
#define PROPERTY_VIRTUAL_MEMORY_MAPPED_FILESIZE  "VirtualMemoryMappedFileSize"
#define PROPERTY_VIRTUAL_SHARED_MEMORY       "VirtualSharedMemory"
#define PROPERTY_CPU_TIME_DEAD_CHILDREN      "CpuTimeDeadChildren"
#define PROPERTY_SYSTEM_TIME_DEAD_CHILDREN   "SystemTimeDeadChildren"
#define PROPERTY_REAL_SPACE                  "RealSpace"



// Some get_proc defines
#define GET_PROC_BY_PID true
#define GET_PROC_BY_INDEX false

/* ==========================================================================
   Type Definitions.
   ========================================================================== */
typedef struct peg_proc_status {
    char pst_ucomm[20];
    String pst_cmd;
    char   pst_stat;
    int    pst_pid;
    int   pst_ppid;
    int   pst_uid;
    int   pst_ruid;
    int   pst_gid;
    int   pst_sid;
    int   pst_pgrp;
    int   pst_tty;
    unsigned long   pst_vdsize;
    unsigned long   pst_vssize;
    unsigned long   pst_vmmsize;
    unsigned long   pst_start;
    unsigned long   pst_dsize;
    long pst_stime;                //kernel time
    long pst_utime;
    long pst_cutime;
    long pst_cstime;
    long  pst_pri;
    long  pst_nice;
    long  pst_vshmsize;
    long  pst_tsize;
    unsigned int  pst_pctcpu;
    long size;

    struct peg_proc_status *l, *r; // linked list pointers
} peg_proc_t;

int file2str(char *directory, char *myFile, char *ret, int cap);
void parseProcStatm(char* inputFileString, peg_proc_t* P);
Boolean parseProcStat(char* inputFileString, peg_proc_t* P);
void parseProcStatus (char* inputFileString, peg_proc_t* P);
Boolean get_proc(Array<int>* pList, peg_proc_t* P, int &pIndex , Boolean find_by_pid);
void doPercentCPU(char *inputFileString, peg_proc_t *P);

class UNIX_Process
{
public:

  UNIX_Process();
  ~UNIX_Process();

  #include <CIM_ManagedSystemElement.h>

  Boolean getPriority(CIMProperty&) const;
  Boolean getExecutionState(CIMProperty&) const;
  Boolean getOtherExecutionDescription(CIMProperty&) const;
  Boolean getCreationDate(CIMProperty&) const;
  Boolean getTerminationDate(CIMProperty&) const;
  Boolean getKernelModeTime(CIMProperty&) const;
  Boolean getUserModeTime(CIMProperty&) const;
  Boolean getWorkingSetSize(CIMProperty&) const;
  Boolean getRealUserID(CIMProperty&) const;
  Boolean getProcessGroupID(CIMProperty&) const;
  Boolean getProcessSessionID(CIMProperty&) const;
  Boolean getProcessTTY(CIMProperty&) const;
  Boolean getModulePath(CIMProperty&) const;
  Boolean getParameters(CIMProperty&) const;
  Boolean getProcessNiceValue(CIMProperty&) const;
  Boolean getProcessWaitingForEvent(CIMProperty&) const;
  Boolean getCPUTime(CIMProperty&) const;
  Boolean getRealText(CIMProperty&) const;
  Boolean getRealData(CIMProperty&) const;
  Boolean getRealStack(CIMProperty&) const;
  Boolean getVirtualText(CIMProperty&) const;
  Boolean getVirtualData(CIMProperty&) const;
  Boolean getVirtualStack(CIMProperty&) const;
  Boolean getVirtualMemoryMappedFileSize(CIMProperty&) const;
  Boolean getVirtualSharedMemory(CIMProperty&) const;
  Boolean getCpuTimeDeadChildren(CIMProperty&) const;
  Boolean getSystemTimeDeadChildren(CIMProperty&) const;
  Boolean getParentProcessID(CIMProperty&) const;
  Boolean getRealSpace(CIMProperty&) const;

  // Loads the internal process status structure with
  // the status data for the indexed process and, if
  // necessary (on HP-UX) updates pIndex to skip unused
  // entries so that a simple increment will allow next
  // call to access next entry
  Boolean loadProcessInfo(int &pIndex);

  // Finds the requested process and loads its info into
  // the internal process status structure
  Boolean findProcess(const String& handle);

private:
  peg_proc_t pInfo;
  Array<int> pList;
};

#endif  /* #ifndef UNIX_PROCESS_FREEBSD_H */