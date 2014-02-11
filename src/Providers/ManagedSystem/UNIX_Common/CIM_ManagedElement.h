
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64
#endif

#define PROPERTY_CAPTION		 						"Caption"
#define PROPERTY_DESCRIPTION	 						"Description"
#define PROPERTY_ELEMENT_NAME		 					"ElementName"
#define PROPERTY_GENERATION		 						"Generation"
#define PROPERTY_INSTANCE_ID		 					"InstanceID"

#define BASE_CLASS_CIM_NAME 			CIMName (BASE_CLASS_NAME)
#define CLASS_IMPLEMENTATION_CIM_NAME 	CIMName (CLASS_IMPLEMENTATION_NAME)

Boolean initialize();
Boolean load(int&);
Boolean finalize();
Boolean find(String&);
String getHandle() const;
Boolean validateKey(CIMKeyBinding&) const;
void buildKeyHandle(CIMKeyBinding &kb, String &s) const;

//ManagedElement
Boolean getCaption(CIMProperty&) const;
Boolean getDescription(CIMProperty&) const;
Boolean getElementName(CIMProperty&) const;
Boolean getGeneration(CIMProperty&) const;
Boolean getInstanceID(CIMProperty&) const;


