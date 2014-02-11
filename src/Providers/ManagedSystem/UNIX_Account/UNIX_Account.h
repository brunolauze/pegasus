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

#ifndef UNIX_ACCOUNT_FREEBSD_H
#define UNIX_ACCOUNT_FREEBSD_H

/* ==========================================================================
   Includes.
   ========================================================================== */
#include <pwd.h>
#include <UNIX_Common.h>

#define CLASS_IMPLEMENTATION_NAME			"UNIX_Account"
#define CLASS_IMPLEMENTATION				UNIX_Account
#define BASE_CLASS_NAME 					"CIM_Account"
#define ELEMENT_NAME						"Account"
#define NUMKEYS_CLASS_IMPLEMENTATION		4

//EnabledLogicalElement
#define PROPERTY_ENABLED_STATE				"EnabledState"
#define PROPERTY_OTHER_ENABLED_STATE		"OtherEnabledState"
#define PROPERTY_REQUESTED_STATE			"RequestedState"
#define PROPERTY_ENABLED_DEFAULT			"EnabledDefault"
#define PROPERTY_TIME_OF_LAST_STATE_CHANGE	"TimeOfLastStateChange"
#define PROPERTY_AVAILABLE_REQUESTED_STATES	"AvailableRequestedStates"
#define PROPERTY_TRANSITIONING_TO_STATE		"TransitioningToState"

#define PROPERTY_USERID									"UserID"
#define PROPERTY_OBJECT_CLASS							"ObjectClass"
#define PROPERTY_DESCRIPTIONS							"Descriptions"
#define PROPERTY_HOST									"Host"
#define PROPERTY_LOCALITY_NAME							"LocalityName"
#define PROPERTY_ORGANIZATION_NAME						"OrganizationName"
#define PROPERTY_OU										"OU"
#define PROPERTY_SEEALSO								"SeeAlso"
#define PROPERTY_USER_CERTIFICATE						"UserCertificate"
#define PROPERTY_USER_PASSWORD							"UserPassword"
#define PROPERTY_USER_PASSWORD_ENCRYPTION_ALGORITHM		"UserPasswordEncryptionAlgorithm"
#define PROPERTY_OTHER_USER_PASSWORD_ENCRYPTION_ALGORITHM		"OtherUserPasswordEncryptionAlgorithm"
#define PROPERTY_COMPLEX_PASSWORD_RULES_ENFORCED		"ComplexPasswordRulesEnforced"
#define PROPERTY_INACTIVITY_TIMEOUT						"InactivityTimeout"
#define PROPERTY_LAST_LOGIN								"LastLogin"
#define PROPERTY_MAXIMUM_SUCCESSIVE_LOGIN_FAILURES		"MaximumSuccessiveLoginFailures"
#define PROPERTY_PASSWORD_EXPIRATION					"PasswordExpiration"
#define PROPERTY_PASSWORD_HISTORY_DEPTH					"PasswordHistoryDepth"
#define PROPERTY_USER_PASSWORD_ENCODING					"UserPasswordEncoding"

class UNIX_Account 
{
public:
   	UNIX_Account();
   	~UNIX_Account();

   	#include <CIM_EnabledLogicalElement.h>

	//CIM_Account
	Boolean getUserID(CIMProperty&) const;
	Boolean getObjectClass(CIMProperty&) const;
	Boolean getDescriptions(CIMProperty&) const;
	Boolean getHost(CIMProperty&) const;
	Boolean getLocalityName(CIMProperty&) const;
	Boolean getOrganizationName(CIMProperty&) const;
	Boolean getOU(CIMProperty&) const;
	Boolean getSeeAlso(CIMProperty&) const;
	Boolean getUserCertificate(CIMProperty&) const;
	Boolean getUserPassword(CIMProperty&) const;
	Boolean getUserPasswordEncryptionAlgorithm(CIMProperty&) const;
	Boolean getOtherUserPasswordEncryptionAlgorithm(CIMProperty&) const;
	Boolean getComplexPasswordRulesEnforced(CIMProperty&) const;
	Boolean getInactivityTimeout(CIMProperty&) const;
	Boolean getLastLogin(CIMProperty&) const;
	Boolean getMaximumSuccessiveLoginFailures(CIMProperty&) const;
	Boolean getPasswordExpiration(CIMProperty&) const;
	Boolean getPasswordHistoryDepth(CIMProperty&) const;
	Boolean getUserPasswordEncoding(CIMProperty&) const;

private:
   	passwd* user;
};
#endif /* UNIX_ACCOUNT_FREEBSD_H */