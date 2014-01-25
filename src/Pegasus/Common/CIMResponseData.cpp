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
// Class CIMResponseData encapsulates the possible types of response data
// representations and supplies conversion methods between these types.
// PEP#348 - The CMPI infrastructure using SCMO (Single Chunk Memory Objects)
// describes its usage in the server flow.
// The design document can be found on the OpenPegasus website openpegasus.org
// at https://collaboration.opengroup.org/pegasus/pp/documents/21210/PEP_348.pdf
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMResponseData.h"
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/SCMOXmlWriter.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/CIMInternalXmlEncoder.h>
#include <Pegasus/Common/SCMOInternalXmlEncoder.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

// C++ objects interface handling

// Instance Names handling
Array<CIMObjectPath>& CIMResponseData::getInstanceNames()
{
    PEGASUS_DEBUG_ASSERT(
    (_dataType==RESP_INSTNAMES || _dataType==RESP_OBJECTPATHS));
    _resolveToCIM();
    PEGASUS_DEBUG_ASSERT(_encoding==RESP_ENC_CIM || _encoding == 0);
    return _instanceNames;
}

// Instance handling
CIMInstance& CIMResponseData::getInstance()
{
    PEGASUS_DEBUG_ASSERT(_dataType == RESP_INSTANCE);
    _resolveToCIM();
    if (0 == _instances.size())
    {
        _instances.append(CIMInstance());
    }
    return _instances[0];
}

// Instances handling
Array<CIMInstance>& CIMResponseData::getInstances()
{
    PEGASUS_DEBUG_ASSERT(_dataType == RESP_INSTANCES);
    _resolveToCIM();
    return _instances;
}

// Objects handling
Array<CIMObject>& CIMResponseData::getObjects()
{
    PEGASUS_DEBUG_ASSERT(_dataType == RESP_OBJECTS);
    _resolveToCIM();
    return _objects;
}

// SCMO representation, single instance stored as one element array
// object paths are represented as SCMOInstance
// Resolve all of the information in the CIMResponseData container to
// SCMO  and return all scmoInstances.
// Note that since the SCMO representation,
// a is single instance stored as one element array and object paths are
// represented as SCMOInstance this returns array of SCMOInstance.
Array<SCMOInstance>& CIMResponseData::getSCMO()
{
    // This function resolves to instances and so cannot handle responses to
    // the associators,etc.requests that return classes (input object path with
    // no keys). That issue is resolved however, since CIMResponseData uses the
    // _isClassOperation variable (set by the request) to determine whether
    // the responses are classpaths or instancepaths and the default is
    // false(instancePaths) so that this should always produce instance paths.
    _resolveToSCMO();
    return _scmoInstances;
}

void CIMResponseData::setSCMO(const Array<SCMOInstance>& x)
{
    _scmoInstances=x;
    _encoding |= RESP_ENC_SCMO;
}


// Binary data is just a data stream
Array<Uint8>& CIMResponseData::getBinary()
{
    PEGASUS_DEBUG_ASSERT(_encoding == RESP_ENC_BINARY || _encoding == 0);
    return _binaryData;
}

bool CIMResponseData::setBinary(CIMBuffer& in)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMResponseData::setBinary");

    // Append all serial data from the CIMBuffer to the local data store.
    // Returns error if input not a serialized Uint8A
    if (!in.getUint8A(_binaryData))
    {
        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "Failed to get binary input data!");
        PEG_METHOD_EXIT();
        return false;
    }
    _encoding |= RESP_ENC_BINARY;
    PEG_METHOD_EXIT();
    return true;
}

bool CIMResponseData::setRemainingBinaryData(CIMBuffer& in)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMResponseData::setRemainingBinaryData");

    // Append any data that has not been deserialized already from
    // the CIMBuffer.
    size_t remainingDataLength = in.remainingDataLength();
    _binaryData.append((Uint8*)in.getPtr(), remainingDataLength);

    _encoding |= RESP_ENC_BINARY;
    PEG_METHOD_EXIT();
    return true;
}

bool CIMResponseData::setXml(CIMBuffer& in)
{
    switch (_dataType)
    {
        case RESP_INSTANCE:
        {
            Array<Sint8> inst;
            Array<Sint8> ref;
            CIMNamespaceName ns;
            String host;
            if (!in.getSint8A(inst))
            {
                PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                    "Failed to get XML instance data!");
                return false;
            }
            _instanceData.insert(0,inst);
            if (!in.getSint8A(ref))
            {
                PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                    "Failed to get XML instance data (reference)!");
                return false;
            }
            _referencesData.insert(0,ref);
            if (!in.getString(host))
            {
                PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                    "Failed to get XML instance data (host)!");
                return false;
            }
            _hostsData.insert(0,host);
            if (!in.getNamespaceName(ns))
            {
                PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                    "Failed to get XML instance data (namespace)!");
                return false;
            }
            _nameSpacesData.insert(0,ns);
            break;
        }
        case RESP_INSTANCES:
        {
            Uint32 count;
            if (!in.getUint32(count))
            {
                PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                    "Failed to get XML instance data (number of instance)!");
                return false;
            }
            for (Uint32 i = 0; i < count; i++)
            {
                Array<Sint8> inst;
                Array<Sint8> ref;
                CIMNamespaceName ns;
                String host;
                if (!in.getSint8A(inst))
                {
                    PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                        "Failed to get XML instance data (instances)!");
                    return false;
                }
                if (!in.getSint8A(ref))
                {
                    PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                        "Failed to get XML instance data (references)!");
                    return false;
                }
                if (!in.getString(host))
                {
                    PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                        "Failed to get XML instance data (host)!");
                    return false;
                }
                if (!in.getNamespaceName(ns))
                {
                    PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                        "Failed to get XML instance data (namespace)!");
                    return false;
                }
                _instanceData.append(inst);
                _referencesData.append(ref);
                _hostsData.append(host);
                _nameSpacesData.append(ns);
            }
            break;
        }
        case RESP_OBJECTS:
        {
            Uint32 count;
            if (!in.getUint32(count))
            {
                PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                    "Failed to get XML object data (number of objects)!");
                return false;
            }
            for (Uint32 i = 0; i < count; i++)
            {
                Array<Sint8> obj;
                Array<Sint8> ref;
                CIMNamespaceName ns;
                String host;
                if (!in.getSint8A(obj))
                {
                    PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                        "Failed to get XML object data (object)!");
                    return false;
                }
                if (!in.getSint8A(ref))
                {
                    PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                        "Failed to get XML object data (reference)!");
                    return false;
                }
                if (!in.getString(host))
                {
                    PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                        "Failed to get XML object data (host)!");
                    return false;
                }
                if (!in.getNamespaceName(ns))
                {
                    PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                        "Failed to get XML object data (namespace)!");
                    return false;
                }
                _instanceData.append(obj);
                _referencesData.append(ref);
                _hostsData.append(host);
                _nameSpacesData.append(ns);
            }
            break;
        }
        // internal xml encoding of instance names and object paths not
        // done today
        case RESP_INSTNAMES:
        case RESP_OBJECTPATHS:
        default:
        {
            PEGASUS_DEBUG_ASSERT(false);
        }
    }
    _encoding |= RESP_ENC_XML;
    return true;
}

// function used by OperationAggregator to aggregate response data in a
// single ResponseData object
void CIMResponseData::appendResponseData(const CIMResponseData & x)
{
    // as the Messages set the data types, this should be impossible
    PEGASUS_DEBUG_ASSERT(_dataType == x._dataType);
    _encoding |= x._encoding;

    // add all binary data
    _binaryData.appendArray(x._binaryData);

    // add all the C++ stuff
    _instanceNames.appendArray(x._instanceNames);
    _instances.appendArray(x._instances);
    _objects.appendArray(x._objects);

    // add the SCMO instances
    _scmoInstances.appendArray(x._scmoInstances);

    // add Xml encodings too
    _referencesData.appendArray(x._referencesData);
    _instanceData.appendArray(x._instanceData);
    _hostsData.appendArray(x._hostsData);
    _nameSpacesData.appendArray(x._nameSpacesData);
}

// Encoding responses into output format
void CIMResponseData::encodeBinaryResponse(CIMBuffer& out)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMResponseData::encodeBinaryResponse");
    // Need to do a complete job here by transferring all contained data
    // into binary format and handing it out in the CIMBuffer
    if (RESP_ENC_BINARY == (_encoding & RESP_ENC_BINARY))
    {
        // Binary does NOT need a marker as it consists of C++ and SCMO
        const Array<Uint8>& data = _binaryData;
        out.putBytes(data.getData(), data.size());
    }
    if (RESP_ENC_CIM == (_encoding & RESP_ENC_CIM))
    {
        out.putTypeMarker(BIN_TYPE_MARKER_CPPD);
        switch (_dataType)
        {
            case RESP_INSTNAMES:
            {
                out.putObjectPathA(_instanceNames);
                break;
            }
            case RESP_INSTANCE:
            {
                if (0 == _instances.size())
                {
                    _instances.append(CIMInstance());
                }
                out.putInstance(_instances[0], true, true);
                break;
            }
            case RESP_INSTANCES:
            {
                out.putInstanceA(_instances);
                break;
            }
            case RESP_OBJECTS:
            {
                out.putObjectA(_objects);
                break;
            }
            case RESP_OBJECTPATHS:
            {
                out.putObjectPathA(_instanceNames);
                break;
            }
            default:
            {
                PEGASUS_DEBUG_ASSERT(false);
            }
        }
    }
    if (RESP_ENC_SCMO == (_encoding & RESP_ENC_SCMO))
    {
        out.putTypeMarker(BIN_TYPE_MARKER_SCMO);
        out.putSCMOInstanceA(_scmoInstances);
    }
    if (RESP_ENC_XML == (_encoding & RESP_ENC_XML))
    {
        // This actually should not happen following general code logic
        PEGASUS_DEBUG_ASSERT(false);
    }

    PEG_METHOD_EXIT();
}

void CIMResponseData::completeNamespace(const SCMOInstance * x)
{
    const char * ns;
    Uint32 len;
    ns = x->getNameSpace_l(len);
    // Both internal XML as well as binary always contain a namespace
    // don't have to do anything for those two encodings
    if ((RESP_ENC_BINARY == (_encoding&RESP_ENC_BINARY)) && (len != 0))
    {
        _defaultNamespace = CIMNamespaceName(ns);
    }
    if (RESP_ENC_CIM == (_encoding & RESP_ENC_CIM))
    {
        CIMNamespaceName nsName(ns);
        switch (_dataType)
        {
            case RESP_INSTANCE:
            {
                if (_instances.size() > 0)
                {
                    const CIMInstance& inst = _instances[0];
                    CIMObjectPath& p =
                        const_cast<CIMObjectPath&>(inst.getPath());
                    if (p.getNameSpace().isNull())
                    {
                        p.setNameSpace(nsName);
                    }
                }
                break;
            }
            case RESP_INSTANCES:
            {
                for (Uint32 j = 0, n = _instances.size(); j < n; j++)
                {
                    const CIMInstance& inst = _instances[j];
                    CIMObjectPath& p =
                        const_cast<CIMObjectPath&>(inst.getPath());
                    if (p.getNameSpace().isNull())
                    {
                        p.setNameSpace(nsName);
                    }
                }
                break;
            }
            case RESP_OBJECTS:
            {
                for (Uint32 j = 0, n = _objects.size(); j < n; j++)
                {
                    const CIMObject& object = _objects[j];
                    CIMObjectPath& p =
                        const_cast<CIMObjectPath&>(object.getPath());
                    if (p.getNameSpace().isNull())
                    {
                        p.setNameSpace(nsName);
                    }
                }
                break;
            }
            case RESP_INSTNAMES:
            case RESP_OBJECTPATHS:
            {
                for (Uint32 j = 0, n = _instanceNames.size(); j < n; j++)
                {
                    CIMObjectPath& p = _instanceNames[j];
                    if (p.getNameSpace().isNull())
                    {
                        p.setNameSpace(nsName);
                    }
                }
                break;
            }
            default:
            {
                PEGASUS_DEBUG_ASSERT(false);
            }
        }
    }
    if (RESP_ENC_SCMO == (_encoding & RESP_ENC_SCMO))
    {
        for (Uint32 j = 0, n = _scmoInstances.size(); j < n; j++)
        {
            SCMOInstance & scmoInst=_scmoInstances[j];
            if (0 == scmoInst.getNameSpace())
            {
                scmoInst.setNameSpace_l(ns,len);
            }
        }
    }
}


void CIMResponseData::completeHostNameAndNamespace(
    const String & hn,
    const CIMNamespaceName & ns)
{
    if (RESP_ENC_BINARY == (_encoding & RESP_ENC_BINARY))
    {
        // On binary need remember hostname and namespace in case someone
        // builds C++ default objects or Xml types from it later on
        // -> usage: See resolveBinary()
        _defaultNamespace=ns;
        _defaultHostname=hn;
    }
    // InternalXml does not support objectPath calls
    if ((RESP_ENC_XML == (_encoding & RESP_ENC_XML)) &&
            (RESP_OBJECTS == _dataType))
    {
        for (Uint32 j = 0, n = _referencesData.size(); j < n; j++)
        {
            if (0 == _hostsData[j].size())
            {
                _hostsData[j]=hn;
            }
            if (_nameSpacesData[j].isNull())
            {
                _nameSpacesData[j]=ns;
            }
        }
    }
    if (RESP_ENC_CIM == (_encoding & RESP_ENC_CIM))
    {
        switch (_dataType)
        {
            case RESP_OBJECTS:
            {
                for (Uint32 j = 0, n = _objects.size(); j < n; j++)
                {
                    const CIMObject& object = _objects[j];
                    CIMObjectPath& p =
                        const_cast<CIMObjectPath&>(object.getPath());
                    if (p.getHost().size()==0)
                    {
                        p.setHost(hn);
                    }
                    if (p.getNameSpace().isNull())
                    {
                        p.setNameSpace(ns);
                    }
                }
                break;
            }
            case RESP_OBJECTPATHS:
            {
                for (Uint32 j = 0, n = _instanceNames.size(); j < n; j++)
                {
                    CIMObjectPath& p = _instanceNames[j];
                    if (p.getHost().size() == 0)
                        p.setHost(hn);
                    if (p.getNameSpace().isNull())
                        p.setNameSpace(ns);
                }
                break;
            }
            default:
            {
                PEGASUS_DEBUG_ASSERT(false);
            }
        }
    }
    if (RESP_ENC_SCMO == (_encoding & RESP_ENC_SCMO))
    {
        CString hnCString=hn.getCString();
        const char* hnChars = hnCString;
        Uint32 hnLen = strlen(hnChars);
        CString nsCString=ns.getString().getCString();
        const char* nsChars=nsCString;
        Uint32 nsLen = strlen(nsChars);
        switch (_dataType)
        {
            case RESP_OBJECTS:
            case RESP_OBJECTPATHS:
            {
                for (Uint32 j = 0, n = _scmoInstances.size(); j < n; j++)
                {
                    SCMOInstance & scmoInst=_scmoInstances[j];
                    scmoInst.completeHostNameAndNamespace(
                        hnChars,
                        hnLen,
                        nsChars,
                        nsLen);
                }
                break;
            }
            default:
            {
                PEGASUS_DEBUG_ASSERT(false);
            }
        }
    }
}

void CIMResponseData::encodeXmlResponse(Buffer& out)
{
    PEG_TRACE((TRC_XML, Tracer::LEVEL3,
        "CIMResponseData::encodeXmlResponse(encoding=%X,content=%X)",
        _encoding,
        _dataType));

    // already existing Internal XML does not need to be encoded further
    // binary input is not actually impossible here, but we have an established
    // fallback
    if (RESP_ENC_BINARY == (_encoding & RESP_ENC_BINARY))
    {
        _resolveBinary();
    }
    if (RESP_ENC_XML == (_encoding & RESP_ENC_XML))
    {
        switch (_dataType)
        {
            case RESP_INSTANCE:
            {
                const Array<ArraySint8>& a = _instanceData;
                out.append((char*)a[0].getData(), a[0].size() - 1);
                break;
            }
            case RESP_INSTANCES:
            {
                const Array<ArraySint8>& a = _instanceData;
                const Array<ArraySint8>& b = _referencesData;

                for (Uint32 i = 0, n = a.size(); i < n; i++)
                {
                    out << STRLIT("<VALUE.NAMEDINSTANCE>\n");
                    out.append((char*)b[i].getData(), b[i].size() - 1);
                    out.append((char*)a[i].getData(), a[i].size() - 1);
                    out << STRLIT("</VALUE.NAMEDINSTANCE>\n");
                }
                break;
            }
            case RESP_OBJECTS:
            {
                const Array<ArraySint8>& a = _instanceData;
                const Array<ArraySint8>& b = _referencesData;
                for (Uint32 i = 0, n = a.size(); i < n; i++)
                {
                    out << STRLIT("<VALUE.OBJECTWITHPATH>\n");
                    out << STRLIT("<INSTANCEPATH>\n");
                    XmlWriter::appendNameSpacePathElement(
                            out,
                            _hostsData[i],
                            _nameSpacesData[i]);
                    // Leave out the surrounding tags "<VALUE.REFERENCE>\n"
                    // and "</VALUE.REFERENCE>\n" which are 18 and 19 characters
                    // long
                    out.append(
                        ((char*)b[i].getData())+18,
                        b[i].size() - 1 - 18 -19);
                    out << STRLIT("</INSTANCEPATH>\n");
                    // append instance body
                    out.append((char*)a[i].getData(), a[i].size() - 1);
                    out << STRLIT("</VALUE.OBJECTWITHPATH>\n");
                }
                break;
            }
            // internal xml encoding of instance names and object paths not
            // done today
            case RESP_INSTNAMES:
            case RESP_OBJECTPATHS:
            default:
            {
                PEGASUS_DEBUG_ASSERT(false);
            }
        }
    }

    if (RESP_ENC_CIM == (_encoding & RESP_ENC_CIM))
    {
        switch (_dataType)
        {
            case RESP_INSTNAMES:
            {
                for (Uint32 i = 0, n = _instanceNames.size(); i < n; i++)
                {
                    XmlWriter::appendInstanceNameElement(out,_instanceNames[i]);
                }
                break;
            }
            case RESP_INSTANCE:
            {
                if (_instances.size() > 0)
                {
                    XmlWriter::appendInstanceElement(
                        out,
                        _instances[0],
                        _includeQualifiers,
                        _includeClassOrigin,
                        _propertyList);
                }
                break;
            }
            case RESP_INSTANCES:
            {
                for (Uint32 i = 0, n = _instances.size(); i < n; i++)
                {
                    XmlWriter::appendValueNamedInstanceElement(
                        out,
                        _instances[i],
                        _includeQualifiers,
                        _includeClassOrigin,
                        _propertyList);
                }
                break;
            }
            case RESP_OBJECTS:
            {
                for (Uint32 i = 0; i < _objects.size(); i++)
                {
                    XmlWriter::appendValueObjectWithPathElement(
                        out,
                        _objects[i],
                        _includeQualifiers,
                        _includeClassOrigin,
                        _isClassOperation,
                        _propertyList);
                }
                break;
            }
            case RESP_OBJECTPATHS:
            {
                for (Uint32 i = 0, n = _instanceNames.size(); i < n; i++)
                {
                    out << "<OBJECTPATH>\n";
                    XmlWriter::appendValueReferenceElement(
                        out,
                        _instanceNames[i],
                        _isClassOperation,
                        false);
                    out << "</OBJECTPATH>\n";
                }
                break;
            }
            default:
            {
                PEGASUS_DEBUG_ASSERT(false);
            }
        }
    }
    if (RESP_ENC_SCMO == (_encoding & RESP_ENC_SCMO))
    {
        switch (_dataType)
        {
            case RESP_INSTNAMES:
            {
                for (Uint32 i = 0, n = _scmoInstances.size(); i < n; i++)
                {
                    SCMOXmlWriter::appendInstanceNameElement(
                        out,
                        _scmoInstances[i]);

                }
                break;
            }
            case RESP_INSTANCE:
            {
                if (_scmoInstances.size() > 0)
                {
                    if(_propertyList.isNull())
                    {
                        Array<Uint32> emptyNodes;
                        SCMOXmlWriter::appendInstanceElement(
                            out,
                            _scmoInstances[0],
                            false,
                            emptyNodes);
                    }
                    else
                    {
                        Array<propertyFilterNodesArray_t> propFilterNodesArrays;
                        // This searches for an already created array of nodes,
                        //if not found, creates it inside propFilterNodesArrays
                        const Array<Uint32> & nodes=
                            SCMOXmlWriter::getFilteredNodesArray(
                                propFilterNodesArrays,
                                _scmoInstances[0],
                                _propertyList);
                        SCMOXmlWriter::appendInstanceElement(
                            out,
                            _scmoInstances[0],
                            true,
                            nodes);
                    }
                }
                break;
            }
            case RESP_INSTANCES:
            {
                SCMOXmlWriter::appendValueSCMOInstanceElements(
                    out,
                    _scmoInstances,
                    _propertyList);
                break;
            }
            case RESP_OBJECTS:
            {
                SCMOXmlWriter::appendValueObjectWithPathElement(
                    out,
                   _scmoInstances,
                  _propertyList);
                break;
            }
            case RESP_OBJECTPATHS:
            {
                for (Uint32 i = 0, n = _scmoInstances.size(); i < n; i++)
                {
                    out << "<OBJECTPATH>\n";
                    SCMOXmlWriter::appendValueReferenceElement(
                        out,
                        _scmoInstances[i],
                        false);
                    out << "</OBJECTPATH>\n";
                }
                break;
            }
            default:
            {
                PEGASUS_DEBUG_ASSERT(false);
            }
        }
    }
}

// contrary to encodeXmlResponse this function encodes the Xml in a format
// not usable by clients
void CIMResponseData::encodeInternalXmlResponse(CIMBuffer& out)
{
    PEG_TRACE((TRC_XML, Tracer::LEVEL3,
        "CIMResponseData::encodeInternalXmlResponse(encoding=%X,content=%X)",
        _encoding,
        _dataType));
    // For mixed (CIM+SCMO) responses, we need to tell the receiver the
    // total number of instances. The totalSize variable is used to keep track
    // of this.
    Uint32 totalSize = 0;

    // already existing Internal XML does not need to be encoded further
    // binary input is not actually impossible here, but we have an established
    // fallback
    if (RESP_ENC_BINARY == (_encoding & RESP_ENC_BINARY))
    {
        _resolveBinary();
    }
    if ((0 == _encoding) ||
        (RESP_ENC_CIM == (_encoding & RESP_ENC_CIM)))
    {
        switch (_dataType)
        {
            case RESP_INSTANCE:
            {
                if (0 == _instances.size())
                {
                    _instances.append(CIMInstance());
                    CIMInternalXmlEncoder::_putXMLInstance(
                        out,
                        _instances[0]);
                    break;
                }
                CIMInternalXmlEncoder::_putXMLInstance(
                    out,
                    _instances[0],
                    _includeQualifiers,
                    _includeClassOrigin,
                    _propertyList);
                break;
            }
            case RESP_INSTANCES:
            {
                Uint32 n = _instances.size();
                totalSize = n + _scmoInstances.size();
                out.putUint32(totalSize);
                for (Uint32 i = 0; i < n; i++)
                {
                    CIMInternalXmlEncoder::_putXMLNamedInstance(
                        out,
                        _instances[i],
                        _includeQualifiers,
                        _includeClassOrigin,
                        _propertyList);
                }
                break;
            }
            case RESP_OBJECTS:
            {
                Uint32 n = _objects.size();
                totalSize = n + _scmoInstances.size();
                out.putUint32(totalSize);
                for (Uint32 i = 0; i < n; i++)
                {
                    CIMInternalXmlEncoder::_putXMLObject(
                        out,
                        _objects[i],
                        _includeQualifiers,
                        _includeClassOrigin,
                        _propertyList);
                }
                break;
            }
            // internal xml encoding of instance names and object paths not
            // done today
            case RESP_INSTNAMES:
            case RESP_OBJECTPATHS:
            default:
            {
                PEGASUS_DEBUG_ASSERT(false);
            }
        }
    }
    if (RESP_ENC_SCMO == (_encoding & RESP_ENC_SCMO))
    {
        switch (_dataType)
        {
            case RESP_INSTANCE:
            {
                if (0 == _scmoInstances.size())
                {
                    _scmoInstances.append(SCMOInstance());
                }
                SCMOInternalXmlEncoder::_putXMLInstance(
                    out,
                    _scmoInstances[0],
                    _propertyList);
                break;
            }
            case RESP_INSTANCES:
            {
                Uint32 n = _scmoInstances.size();
                // Only put the size when not already done above
                if (0==totalSize)
                {
                    out.putUint32(n);
                }
                SCMOInternalXmlEncoder::_putXMLNamedInstance(
                    out,
                    _scmoInstances,
                    _propertyList);
                break;
            }
            case RESP_OBJECTS:
            {
                Uint32 n = _scmoInstances.size();
                // Only put the size when not already done above
                if (0==totalSize)
                {
                    out.putUint32(n);
                }
                SCMOInternalXmlEncoder::_putXMLObject(
                    out,
                    _scmoInstances,
                    _propertyList);
                break;
            }
            // internal xml encoding of instance names and object paths not
            // done today
            case RESP_INSTNAMES:
            case RESP_OBJECTPATHS:
            default:
            {
                PEGASUS_DEBUG_ASSERT(false);
            }
        }
    }

}

void CIMResponseData::_resolveToCIM()
{
    PEG_TRACE((TRC_XML, Tracer::LEVEL3,
        "CIMResponseData::_resolveToCIM(encoding=%X,content=%X)",
        _encoding,
        _dataType));

    if (RESP_ENC_XML == (_encoding & RESP_ENC_XML))
    {
        _resolveXmlToCIM();
    }
    if (RESP_ENC_BINARY == (_encoding & RESP_ENC_BINARY))
    {
        _resolveBinary();
    }
    if (RESP_ENC_SCMO == (_encoding & RESP_ENC_SCMO))
    {
        _resolveSCMOToCIM();
    }

    PEGASUS_DEBUG_ASSERT(_encoding == RESP_ENC_CIM || _encoding == 0);
}

void CIMResponseData::_resolveToSCMO()
{
    PEG_TRACE((TRC_XML, Tracer::LEVEL3,
        "CIMResponseData::_resolveToSCMO(encoding=%X,content=%X)",
        _encoding,
        _dataType));

    if (RESP_ENC_XML == (_encoding & RESP_ENC_XML))
    {
        _resolveXmlToSCMO();
    }
    if (RESP_ENC_BINARY == (_encoding & RESP_ENC_BINARY))
    {
        _resolveBinary();
    }
    if (RESP_ENC_CIM == (_encoding & RESP_ENC_CIM))
    {
        _resolveCIMToSCMO();
    }
    PEGASUS_DEBUG_ASSERT(_encoding == RESP_ENC_SCMO || _encoding == 0);
}

// helper functions to transform different formats into one-another
// functions work on the internal data and calling of them should be
// avoided whenever possible
void CIMResponseData::_resolveBinary()
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMResponseData::_resolveBinary");

    CIMBuffer in((char*)_binaryData.getData(), _binaryData.size());

    while (in.more())
    {
        Uint32 binaryTypeMarker=0;
        if(!in.getTypeMarker(binaryTypeMarker))
        {
            PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "Failed to get type marker for binary objects!");
            PEG_METHOD_EXIT();
            in.release();
            return;
        }

        if (BIN_TYPE_MARKER_SCMO==binaryTypeMarker)
        {
            if (!in.getSCMOInstanceA(_scmoInstances))
            {
                _encoding &=(~RESP_ENC_BINARY);
                in.release();
                PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                    "Failed to resolve binary SCMOInstances!");
                PEG_METHOD_EXIT();
                return;
            }

            _encoding |= RESP_ENC_SCMO;
        }
        else
        {
            switch (_dataType)
            {
                case RESP_INSTNAMES:
                case RESP_OBJECTPATHS:
                {
                    if (!in.getObjectPathA(_instanceNames))
                    {
                        _encoding &=(~RESP_ENC_BINARY);
                        in.release();
                        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                            "Failed to resolve binary CIMObjectPaths!");
                        PEG_METHOD_EXIT();
                        return;
                    }
                    break;
                }
                case RESP_INSTANCE:
                {
                    CIMInstance instance;
                    if (!in.getInstance(instance))
                    {
                        _encoding &=(~RESP_ENC_BINARY);
                        _encoding |= RESP_ENC_CIM;
                        _instances.append(instance);
                        in.release();
                        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                            "Failed to resolve binary instance!");
                        PEG_METHOD_EXIT();
                        return;
                    }

                    _instances.append(instance);
                    break;
                }
                case RESP_INSTANCES:
                {
                    if (!in.getInstanceA(_instances))
                    {
                        _encoding &=(~RESP_ENC_BINARY);
                        in.release();
                        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                            "Failed to resolve binary CIMInstances!");
                        PEG_METHOD_EXIT();
                        return;
                    }
                    break;
                }
                case RESP_OBJECTS:
                {
                    if (!in.getObjectA(_objects))
                    {
                        in.release();
                        _encoding &=(~RESP_ENC_BINARY);
                        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                            "Failed to resolve binary CIMObjects!");
                        PEG_METHOD_EXIT();
                        return;
                    }
                    break;
                }
                default:
                {
                    PEGASUS_DEBUG_ASSERT(false);
                }
            } // switch
            _encoding |= RESP_ENC_CIM;
        } // else SCMO
    }
    _encoding &=(~RESP_ENC_BINARY);
    // fix up the hostname and namespace for objects if defaults
    // were set
    if (_defaultHostname.size() > 0 && !_defaultNamespace.isNull())
    {
        completeHostNameAndNamespace(_defaultHostname, _defaultNamespace);
    }
    in.release();
    PEG_METHOD_EXIT();
}


void CIMResponseData::_deserializeObject(Uint32 idx,CIMObject& cimObject)
{
    // Only start the parser when instance data is present.
    if (0 != _instanceData[idx].size())
    {
        CIMInstance cimInstance;
        CIMClass cimClass;

        XmlParser parser((char*)_instanceData[idx].getData());

        if (XmlReader::getInstanceElement(parser, cimInstance))
        {
            cimObject = CIMObject(cimInstance);
            return;
        }

        if (XmlReader::getClassElement(parser, cimClass))
        {
            cimObject = CIMObject(cimClass);
            return;
        }
        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "Failed to resolve XML object data, parser error!");
    }
}

void CIMResponseData::_deserializeInstance(Uint32 idx,CIMInstance& cimInstance)
{
    // Only start the parser when instance data is present.
    if (0 != _instanceData[idx].size())
    {
        XmlParser parser((char*)_instanceData[idx].getData());
        if (XmlReader::getInstanceElement(parser, cimInstance))
        {
            return;
        }
        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "Failed to resolve XML instance, parser error!");
    }
    // reset instance when parsing may not be successfull or
    // no instance is present.
    cimInstance = CIMInstance();
}

Boolean CIMResponseData::_deserializeReference(
    Uint32 idx,
    CIMObjectPath& cimObjectPath)
{
    // Only start the parser when reference data is present.
    if (0 != _referencesData[idx].size())
    {
        XmlParser parser((char*)_referencesData[idx].getData());
        if (XmlReader::getValueReferenceElement(parser, cimObjectPath))
        {
            if (_hostsData[idx].size())
            {
                cimObjectPath.setHost(_hostsData[idx]);
            }
            if (!_nameSpacesData[idx].isNull())
            {
                cimObjectPath.setNameSpace(_nameSpacesData[idx]);
            }
            return true;
        }
        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "Failed to resolve XML reference, parser error!");

    }
    return false;
}

Boolean CIMResponseData::_deserializeInstanceName(
    Uint32 idx,
    CIMObjectPath& cimObjectPath)
{
    // Only start the parser when instance name data is present.
    if (0 != _referencesData[idx].size())
    {
        XmlParser parser((char*)_referencesData[idx].getData());
        if (XmlReader::getInstanceNameElement(parser, cimObjectPath))
        {
            if (_hostsData[idx].size())
            {
                cimObjectPath.setHost(_hostsData[idx]);
            }
            if (!_nameSpacesData[idx].isNull())
            {
                cimObjectPath.setNameSpace(_nameSpacesData[idx]);
            }
            return true;
        }
        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "Failed to resolve XML instance name, parser error!");

    }
    return false;
}

void CIMResponseData::_resolveXmlToCIM()
{
    switch (_dataType)
    {
        // Xml encoding for instance names and object paths not used
        case RESP_OBJECTPATHS:
        case RESP_INSTNAMES:
        {
            break;
        }
        case RESP_INSTANCE:
        {
            CIMInstance cimInstance;
            CIMObjectPath cimObjectPath;

            _deserializeInstance(0,cimInstance);
            if (_deserializeReference(0,cimObjectPath))
            {
                cimInstance.setPath(cimObjectPath);
                // A single CIMInstance has to have an objectpath.
                // So only add it when an objectpath exists.
                _instances.append(cimInstance);
            }
            break;
        }
        case RESP_INSTANCES:
        {
            for (Uint32 i = 0; i < _instanceData.size(); i++)
            {
                CIMInstance cimInstance;
                CIMObjectPath cimObjectPath;

                _deserializeInstance(i,cimInstance);
                if (_deserializeInstanceName(i,cimObjectPath))
                {
                    cimInstance.setPath(cimObjectPath);
                }
                // enumarate instances can be without name
                _instances.append(cimInstance);
            }
            break;
        }
        case RESP_OBJECTS:
        {
            for (Uint32 i=0, n=_instanceData.size(); i<n; i++)
            {
                CIMObject cimObject;
                CIMObjectPath cimObjectPath;

                _deserializeObject(i,cimObject);
                if (_deserializeReference(i,cimObjectPath))
                {
                    cimObject.setPath(cimObjectPath);
                }
                _objects.append(cimObject);
            }
            break;
        }
        default:
        {
            PEGASUS_DEBUG_ASSERT(false);
        }
    }
    // Xml was resolved, release Xml content now
    _referencesData.clear();
    _hostsData.clear();
    _nameSpacesData.clear();
    _instanceData.clear();
    // remove Xml Encoding flag
    _encoding &=(~RESP_ENC_XML);
    // add CIM Encoding flag
    _encoding |=RESP_ENC_CIM;
}

void CIMResponseData::_resolveXmlToSCMO()
{
    // Not optimal, can probably be improved
    // but on the other hand, since using the binary format this case should
    // actually not ever happen.
    _resolveXmlToCIM();
    _resolveCIMToSCMO();
}

void CIMResponseData::_resolveSCMOToCIM()
{
    switch(_dataType)
    {
        case RESP_INSTNAMES:
        case RESP_OBJECTPATHS:
        {
            for (Uint32 x=0, n=_scmoInstances.size(); x < n; x++)
            {
                CIMObjectPath newObjectPath;
                _scmoInstances[x].getCIMObjectPath(newObjectPath);
                _instanceNames.append(newObjectPath);
            }
            break;
        }
        case RESP_INSTANCE:
        {
            if (_scmoInstances.size() > 0)
            {
                CIMInstance newInstance;
                _scmoInstances[0].getCIMInstance(newInstance);
                _instances.append(newInstance);
            }
            break;
        }
        case RESP_INSTANCES:
        {
            for (Uint32 x=0, n=_scmoInstances.size(); x < n; x++)
            {
                CIMInstance newInstance;
                _scmoInstances[x].getCIMInstance(newInstance);
                _instances.append(newInstance);
            }
            break;
        }
        case RESP_OBJECTS:
        {
            for (Uint32 x=0, n=_scmoInstances.size(); x < n; x++)
            {
                CIMInstance newInstance;
                _scmoInstances[x].getCIMInstance(newInstance);
                _objects.append(CIMObject(newInstance));
            }
            break;
        }
        default:
        {
            PEGASUS_DEBUG_ASSERT(false);
        }
    }
    _scmoInstances.clear();
    // remove CIM Encoding flag
    _encoding &=(~RESP_ENC_SCMO);
    // add SCMO Encoding flag
    _encoding |=RESP_ENC_CIM;
}

void CIMResponseData::_resolveCIMToSCMO()
{
    CString nsCString=_defaultNamespace.getString().getCString();
    const char* _defNamespace = nsCString;
    Uint32 _defNamespaceLen;
    if (_defaultNamespace.isNull())
    {
        _defNamespaceLen=0;
    }
    else
    {
        _defNamespaceLen=strlen(_defNamespace);
    }
    switch (_dataType)
    {
        case RESP_INSTNAMES:
        {
            for (Uint32 i=0,n=_instanceNames.size();i<n;i++)
            {
                SCMOInstance addme(
                    _instanceNames[i],
                    _defNamespace,
                    _defNamespaceLen);
                _scmoInstances.append(addme);
            }
            _instanceNames.clear();
            break;
        }
        case RESP_INSTANCE:
        {
            if (_instances.size() > 0)
            {
                SCMOInstance addme(
                    _instances[0],
                    _defNamespace,
                    _defNamespaceLen);
                _scmoInstances.clear();
                _scmoInstances.append(addme);
                _instances.clear();
            }
            break;
        }
        case RESP_INSTANCES:
        {
            for (Uint32 i=0,n=_instances.size();i<n;i++)
            {
                SCMOInstance addme(
                    _instances[i],
                    _defNamespace,
                    _defNamespaceLen);
                _scmoInstances.append(addme);
            }
            _instances.clear();
            break;
        }
        case RESP_OBJECTS:
        {
            for (Uint32 i=0,n=_objects.size();i<n;i++)
            {
                SCMOInstance addme(
                    _objects[i],
                    _defNamespace,
                    _defNamespaceLen);
                _scmoInstances.append(addme);
            }
            _objects.clear();
            break;
        }
        case RESP_OBJECTPATHS:
        {
            for (Uint32 i=0,n=_instanceNames.size();i<n;i++)
            {
                SCMOInstance addme(
                    _instanceNames[i],
                    _defNamespace,
                    _defNamespaceLen);
                if (_isClassOperation)
                {
                    addme.setIsClassOnly(true);
                }
                _scmoInstances.append(addme);
            }
            _instanceNames.clear();
            break;
        }
        default:
        {
            PEGASUS_DEBUG_ASSERT(false);
        }
    }

    // remove CIM Encoding flag
    _encoding &=(~RESP_ENC_CIM);
    // add SCMO Encoding flag
    _encoding |=RESP_ENC_SCMO;
}

void CIMResponseData::setRequestProperties(
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    _includeQualifiers = includeQualifiers;
    _includeClassOrigin = includeClassOrigin;
    _propertyList = propertyList;
}

void CIMResponseData::setIsClassOperation(Boolean b)
{
    _isClassOperation = b;
}

PEGASUS_NAMESPACE_END
