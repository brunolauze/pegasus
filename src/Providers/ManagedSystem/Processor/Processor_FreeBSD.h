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
// Author: Christopher Neufeld <neufeld@linuxcare.com>
//         David Kennedy       <dkennedy@linuxcare.com>
//
// Modified By:
//         David Kennedy       <dkennedy@linuxcare.com>
//         Christopher Neufeld <neufeld@linuxcare.com>
//         Al Stone, Hewlett-Packard Company <ahs3@fc.hp.com>
//         Jim Metcalfe, Hewlett-Packard Company
//         Carlos Bonilla, Hewlett-Packard Company
//         Mike Glantz, Hewlett-Packard Company <michael_glantz@hp.com>
//
//%////////////////////////////////////////////////////////////////////////////

#ifndef PG_PROCESSOR_FREEBSD_H
#define PG_PROCESSOR_FREEBSD_H

/* ==========================================================================
   Includes.
   ========================================================================== */

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMDateTime.h>

#include <sys/dirent.h>   // readdir
#include <sys/param.h>    // pstat
#include <sys/stat.h>     // stat
#include <sys/utsname.h>  // uname
#include <unistd.h>       // gethostname
#include <sys/socket.h>   // gethostbyname
#include <netinet/in.h>   // gethostbyname
#include <netdb.h>        // gethostbyname

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

#define PST_MAX_CPUSTATES 15

struct pst_processor {
		uint64_t        psp_idx;        /* Index of the current spu in the array of processor statistic entries */
        uint64_t        psp_fsreads;    /* # of reads from filesys blocks. */
        uint64_t        psp_fswrites;   /* # of writes to filesys blocks. */
        uint64_t        psp_nfsreads;   /* # of nfs disk blk reads issued. */
        uint64_t        psp_nfswrites;  /* # of nfs disk blk writes issued. */
        uint64_t        psp_bnfsread;   /* # of bytes read from NFS. */
        uint64_t        psp_bnfswrite;  /* # of bytes written to NFS. */
        uint64_t        psp_phread;     /* # of physical reads to raw devs. */
        uint64_t        psp_phwrite;    /* # of physical writes to raw devs. */
        uint64_t        psp_runocc;     /* # of times the processor had
                                         * processes waiting to run.  This
                                         * running total is updated once
                                         * a second.
                                         */
        uint64_t        psp_runque;     /* # of processes the processor had
                                         * waiting to run.  This running total
                                         * is updated once a second.
                                         */
        uint64_t        psp_sysexec;    /* # of exec system calls. */
        uint64_t        psp_sysread;    /* # of read system calls. */
        uint64_t        psp_syswrite;   /* # of write system calls. */
        uint64_t        psp_sysnami;    /* # of calls to sysnami(). */
        uint64_t        psp_sysiget;    /* # of calls to sysiget(). */
        uint64_t        psp_dirblk;     /* # of filesystem blocks read doing
                                         * directory lookup.
                                         */
        uint64_t        psp_semacnt;    /* # of System V semaphore ops. */
        uint64_t        psp_msgcnt;     /* # of System V message ops. */
        uint64_t        psp_muxincnt;   /* # of MUX interrupts received. */
        uint64_t        psp_muxoutcnt;  /* # of MUX interrupts sent. */
        uint64_t        psp_ttyrawcnt;  /* # of raw characters read. */
        uint64_t        psp_ttycanoncnt; /* # of canonical chars processed. */
        uint64_t        psp_ttyoutcnt;  /* # of characters output. */
        uint64_t        psp_iticksperclktick; /* interval timer counts (CR16)
                                               * per clock tick,
                                               * see sysconf(_SC_CLK_TCK)
                                               */
        uint64_t        psp_sysselect;  /* # of select system calls. */
        double          psp_avg_1_min;  /* per-processor run queue lengths */
        double          psp_avg_5_min;
        double          psp_avg_15_min;
                                        /* per-processor cpu time/state */
        int64_t         psp_cpu_time[PST_MAX_CPUSTATES];


        uint64_t        psp_logical_node; /* node the spu is on         */
        int64_t         psp_pset_id;      /* processor set id to which this
                                           * processor is assigned 
                                           */
        int64_t         psp_logical_id;   /* logical id of a processor */
        int64_t         psp_processor_state; /* state of a processor */
        int64_t         psp_flags;        /* processor flags */

};

/* ==========================================================================
   Type Definitions.
   ========================================================================== */

class Processor
{
public:

  Processor();
  ~Processor();

  // CIM_ManagedElement
  Boolean getCaption(String&) const;
  Boolean getDescription(String&) const;

  // CIM_ManagedSystemElement
  Boolean getInstallDate(CIMDateTime&) const;
  Boolean getName(String&) const;
  Boolean getStatus(String&) const;

  // CIM_LogicalElement

  // CIM_LogicalDevice
  String getDeviceID(void) const;
  Boolean getPowerManagementSupported(Boolean&) const;
  Boolean getPowerManagementCapabilities(Array<Uint16>& ui16a) const;
  Boolean getAvailability(Uint16&) const;
  Boolean getStatusInfo(Uint16&) const;
  Boolean getLastErrorCode(Uint32&) const;
  Boolean getErrorDescription(String&) const;
  Boolean getErrorCleared(Boolean&) const;
  Boolean getOtherIdentifyingInfo(Array<String>&) const;
  Boolean getPowerOnHours(Uint64&) const;
  Boolean getTotalPowerOnHours(Uint64&) const;
  Boolean getIdentifyingDescriptions(Array<String>&) const;
  Boolean getAdditionalAvailability(Array<Uint16>&) const;
  Boolean getMaxQuiesceTime(Uint64&) const;
  // Methods
#if 0
  Boolean execSetPowerState(&) const;
  Boolean execReset(&) const;
  Boolean execEnableDevice(&) const;
  Boolean execOnlineDevice(&) const;
  Boolean execQuiesceDevice(&) const;
  Boolean execSaveProperties(&) const;
  Boolean execRestoreProperties(&) const;
#endif

  // CIM_Processor
  Boolean getRole(String&) const;
  Boolean getFamily(Uint16&) const;
  Boolean getOtherFamilyDescription(String&) const;
  Boolean getUpgradeMethod(Uint16&) const;
  Boolean getMaxClockSpeed(Uint32&) const;
  Boolean getCurrentClockSpeed(Uint32&) const;
  Boolean getDataWidth(Uint16&) const;
  Boolean getAddressWidth(Uint16&) const;
  Boolean getLoadPercentage(Uint16&) const;
  Boolean getStepping(String&) const;
  Boolean getUniqueID(String&) const;
  Boolean getCPUStatus(Uint16&) const;

  // PG_Processor
  Boolean getBiosID(String&) const;
  Boolean getFirmwareID(String&) const;

  // Auxiliary routines

  // Loads the internal processor structure with
  // the status data for the indexed processor and, if
  // necessary (on FreeBSD) updates pIndex to skip unused
  // entries so that a simple increment will allow next
  // call to access next entry
  Boolean loadProcessorInfo(int &pIndex);

  // Finds the requested process and loads its info into
  // the internal process status structure
  Boolean findProcessor(const String& deviceID);

  static String getSysName(void);

private:
  int index; // save index of this processor
  struct pst_processor pInfo;
};


#endif  /* #ifndef PG_PROCESSOR_FREEBSD_H */
