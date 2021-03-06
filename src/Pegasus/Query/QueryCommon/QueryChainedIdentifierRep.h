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

#ifndef Pegasus_QueryChainedIdentifierRep_h
#define Pegasus_QueryChainedIdentifierRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Query/QueryCommon/Linkage.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Query/QueryCommon/QueryIdentifier.h>
#include <Pegasus/Query/QueryCommon/QueryContext.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_QUERYCOMMON_LINKAGE QueryChainedIdentifierRep
{
public:

    QueryChainedIdentifierRep();

    QueryChainedIdentifierRep(const QueryIdentifier &id);

    QueryChainedIdentifierRep(const QueryChainedIdentifierRep* rep);

    virtual ~QueryChainedIdentifierRep();

    const Array<QueryIdentifier>& getSubIdentifiers()const;

    String toString()const;

    void append(const QueryIdentifier & id);

    Uint32 size()const;

    Boolean prepend(const QueryIdentifier & id);

    QueryIdentifier operator[](Uint32 index)const;

    QueryChainedIdentifierRep& operator=(const QueryChainedIdentifierRep& rhs);

    Boolean isSubChain(const QueryChainedIdentifier & chain)const;

    QueryIdentifier getLastIdentifier()const;

    void applyContext(const QueryContext& inContext);

protected:

    Array<QueryIdentifier> _subIdentifiers;

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_QueryChainedIdentifierRep_h */
