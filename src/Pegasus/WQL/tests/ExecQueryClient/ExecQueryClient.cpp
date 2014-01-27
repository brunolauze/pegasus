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

#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/LanguageParser.h>
#include <Pegasus/Client/CIMClientRep.h>

#include <iostream>
#include <fstream>
#include <Pegasus/Common/Network.h>
#include <Pegasus/Common/Logger.h>

PEGASUS_USING_STD;

extern "C"  __declspec(dllexport) const __unused char* __attribute__((cdecl)) CIMExecQuery(char* nameSpace, char* language, char* query)
{
Pegasus::CIMClientRep client;
Pegasus::Buffer buf;

try
{
  client.connectLocal();
  Pegasus::CIMNamespaceName NAMESPACE(nameSpace);

  Pegasus::CIMResponseData response = client.execQuery(NAMESPACE,Pegasus::String(language),
                                    Pegasus::String(query));


  response.encodeXmlResponse(buf);
  printf(buf.getData());
}
catch(Pegasus::Exception& e)
{
  client.disconnect();
  return "";
}

client.disconnect();

return buf.getData();

}

