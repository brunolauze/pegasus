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

#ifndef UNIX_PROCESS_STAT_PROVIDER_H
#define UNIX_PROCESS_STAT_PROVIDER_H

/* ==========================================================================
   Includes.
   ========================================================================== */

#include "UNIX_ProcessPlatform.h"
#undef UNIX_PROVIDER_NAME
#undef UNIX_PROVIDER
#undef CLASS_IMPLEMENTATION_NAME
#undef BASE_CLASS_NAME
#undef ELEMENT_NAME
#undef NUMKEYS_CLASS_IMPLEMENTATION
#define UNIX_PROVIDER_NAME	"UNIX_ProcessStatProvider"
#define UNIX_PROVIDER		UNIX_ProcessStatProvider
#define CLASS_IMPLEMENTATION_NAME			"UNIX_ProcessStatisticalInformation"
#define BASE_CLASS_NAME 					"CIM_ProcessStatisticalInformation"
#define ELEMENT_NAME						"ProcessStatistics"
#define NUMKEYS_CLASS_IMPLEMENTATION		6

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;


class UNIX_ProcessStatProvider : 
    public CIMInstanceQueryProvider,
    public CIMMethodProvider
{
public:

    UNIX_ProcessStatProvider();

    ~UNIX_ProcessStatProvider();


     #include <CIM_ManagedSystemElementProvider.h>
   
private:

    #include <CIM_ManagedSystemElementProviderPrivate.h>

};


#endif  /* #ifndef UNIX_PROCESS_STAT_PROVIDER_H */
