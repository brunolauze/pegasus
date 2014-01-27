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
// Author: Amit K Arora, IBM (amita@in.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Client/CIMClientRep.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
#define MAX_LEN 4096

int main(int argc, char** argv)
{
  CIMClientRep client;

  try
  {
    client.connectLocal();
    CIMNamespaceName NAMESPACE(argv[1]);
    
    unsigned int i;
    size_t len = 0;
    char *_all_args, *all_args;

    for(i=2; i<argc; i++) {
        len += strlen(argv[i]);
    }

    _all_args = all_args = (char *)malloc(len+argc-1);

    for(i=2; i<argc; i++) {
        memcpy(_all_args, argv[i], strlen(argv[i]));
        _all_args += strlen(argv[i])+1;
        *(_all_args-1) = ' ';
    }
    *(_all_args-1) = 0;

    CIMResponseData response = client.execQuery(NAMESPACE,String("WQL"),
                                      String(all_args));

    
    Buffer buf;
    response.encodeXmlResponse(buf);
    printf(buf.getData());
  }
  catch(Exception& e)
  {
    printf("ERROR!");
    cout << argv[0] << ": "<< e.getMessage() << endl;
    client.disconnect();
    return 1;
  }

  client.disconnect();

  return 0;
}


