#include <CIM_ManagedElement.h>

#define PROPERTY_SYSTEM_CREATION_CLASS_NAME 			"SystemCreationClassName"
#define PROPERTY_SYSTEM_NAME				 			"SystemName"
#define PROPERTY_CS_NAME					 			"CSName"
#define PROPERTY_CREATION_CLASS_NAME 					"CreationClassName"
#define PROPERTY_CS_CREATION_CLASS_NAME					"CSCreationClassName"
#define PROPERTY_NAME									"Name"
#define PROPERTY_INSTALL_DATE	 						"InstallDate"
#define PROPERTY_STATUS									"Status"
#define PROPERTY_COMMUNICATION_STATUS					"CommunicationStatus"
#define PROPERTY_DETAILED_STATUS 						"DetailedStatus"
#define PROPERTY_HEALTH_STATUS 							"HealthStatus"
#define PROPERTY_OPERATING_STATUS 						"OperatingStatus"
#define PROPERTY_OPERATIONAL_STATUS 					"OperationalStatus"
#define PROPERTY_PRIMARY_STATUS 						"PrimaryStatus"
#define PROPERTY_STATUS_DESCRIPTIONS					"StatusDescriptions"


//ManagedSystemElement
Boolean getInstallDate(CIMProperty&) const;
Boolean getCreationClassName(CIMProperty&) const;
Boolean getSystemCreationClassName(CIMProperty&) const;
Boolean getName(CIMProperty&) const;
Boolean getStatus(CIMProperty&) const;
Boolean getCommunicationStatus(CIMProperty&) const;
Boolean getDetailedStatus(CIMProperty&) const;
Boolean getHealthStatus(CIMProperty&) const;
Boolean getOperatingStatus(CIMProperty&) const;
Boolean getPrimaryStatus(CIMProperty&) const;
Boolean getOperationalStatus(CIMProperty&) const;
Boolean getStatusDescriptions(CIMProperty&) const;