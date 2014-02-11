
	#include <CIM_ManagedSystemElement.h>

	//EnabledLogicalElement
	Boolean getEnabledState(CIMProperty&) const;
	Boolean getOtherEnabledState(CIMProperty&) const;
	Boolean getRequestedState(CIMProperty&) const;
	Boolean getEnabledDefault(CIMProperty&) const;
	Boolean getTimeOfLastStateChange(CIMProperty&) const;
	Boolean getAvailableRequestedStates(CIMProperty&) const;
	Boolean getTransitioningToState(CIMProperty&) const;