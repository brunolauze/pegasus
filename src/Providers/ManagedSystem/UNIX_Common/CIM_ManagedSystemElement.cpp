#include <CIM_ManagedElement.cpp>

#ifndef __getStatus_H
Boolean CLASS_IMPLEMENTATION::getStatus(CIMProperty& p) const
{
	p = CIMProperty(
        PROPERTY_STATUS,
        String("OK"));
    return true;
}
#endif

#ifndef __getCommunicationStatus_H
Boolean CLASS_IMPLEMENTATION::getCommunicationStatus(CIMProperty& p) const
{
	p = CIMProperty(
        PROPERTY_COMMUNICATION_STATUS,
        Uint16(2));
    return true;
}
#endif

#ifndef __getDetailedStatus_H
Boolean CLASS_IMPLEMENTATION::getDetailedStatus(CIMProperty& p) const
{
	p = CIMProperty(
        PROPERTY_DETAILED_STATUS,
        Uint16(0));
    return true;
}
#endif

#ifndef __getHealthStatus_H
Boolean CLASS_IMPLEMENTATION::getHealthStatus(CIMProperty& p) const
{
	p = CIMProperty(
        PROPERTY_HEALTH_STATUS,
        Uint16(0));
    return true;
}
#endif

#ifndef __getOperatingStatus_H
Boolean CLASS_IMPLEMENTATION::getOperatingStatus(CIMProperty& p) const
{
	p = CIMProperty(
        PROPERTY_OPERATING_STATUS,
        Uint16(1));
    return true;
}
#endif

#ifndef __getOperationalStatus_H
Boolean CLASS_IMPLEMENTATION::getOperationalStatus(CIMProperty& p) const
{
	p = CIMProperty(
        PROPERTY_OPERATIONAL_STATUS,
        Uint16(1));
    return true;
}
#endif

#ifndef __getStatusDescriptions_H
Boolean CLASS_IMPLEMENTATION::getStatusDescriptions(CIMProperty &p) const
{
	return false;
}
#endif
