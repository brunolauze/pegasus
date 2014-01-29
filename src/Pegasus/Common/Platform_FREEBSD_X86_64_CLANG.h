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
//%/////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
  The platform configuration files in pegasus/src/Pegasus/Common/Platform_*.h
  are intended to normalize platform/compiler differences so that Pegasus code
  can be written to work on all platforms without requiring platform ifdefs.
  Since these files are part of the Pegasus SDK, their contents should be
  the minimum that achieves the stated purpose.

  Examples of differences that are normalized here are support for C++
  namespaces, 64-bit integer type, support for C++ template specialization.

  Examples of things that are not appropriate in these files are the enablement
  of Pegasus features for a specific platform and other build configuration.
******************************************************************************/

#ifndef Pegasus_Platform_FREEBSD_X86_64_CLANG_h
#define Pegasus_Platform_FREEBSD_X86_64_CLANG_h

#include <stddef.h>

#define PEGASUS_OS_TYPE_UNIX

#ifndef PEGASUS_PLATFORM_FREEBSD_GENERIC_GNU
#define PEGASUS_PLATFORM_FREEBSD_GENERIC_GNU
#endif

#ifndef
#define PEGASUS_OS_FREEBSD
#endif

#define PEGASUS_POINTER_64BIT

#define PEGASUS_COMPILER_CLANG

#define PEGASUS_UINT64 unsigned long long

#define PEGASUS_SINT64 long long

#define PEGASUS_HAVE_NAMESPACES

#define PEGASUS_HAVE_FOR_SCOPE

#define PEGASUS_HAVE_TEMPLATE_SPECIALIZATION

//clang still uses the facility provided by gcc and lib,
//So as the following lines
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef _REENTRANT
#define _REENTRANT
#endif
#define _THREAD_SAFE
#ifdef __GLIBC__
#include <features.h>
#else
#include <sys/cdefs.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h> /* For MAXHOSTNAMELEN */
#include <limits.h>    /* _POSIX_HOST_NAME_MAX  */

#if (__clang__ )
#   if (__has_attribute(visibility))
#       define PEGASUS_EXPORT __attribute__ ((visibility("default")))
#       define PEGASUS_IMPORT __attribute__ ((visibility("default")))
#       define PEGASUS_HIDDEN_LINKAGE __attribute__((visibility("hidden")))
#   else
#       define PEGASUS_EXPORT 
#       define PEGASUS_IMPORT 
#       define PEGASUS_HIDDEN_LINKAGE
#   endif
#endif


#define PEGASUS_HAVE_PTHREADS

#define PEGASUS_HAVE_NANOSLEEP

#define PEGASUS_HAS_SIGNALS

#define PEGASUS_INTEGERS_BOUNDARY_ALIGNED

/* getifaddrs() avilable in gcc version >= 3.4 */
#if (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))
# define PEGASUS_HAS_GETIFADDRS
#endif

/* use POSIX read-write locks on this platform */
#define PEGASUS_USE_POSIX_RWLOCK

#endif /* Pegasus_Platform_FREEBSD_X86_64_CLANG_h */
