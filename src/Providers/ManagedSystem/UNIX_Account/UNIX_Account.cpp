#include "UNIX_Account.h"
#include <fcntl.h>
#include <kvm.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/vmmeter.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utmpx.h>
#include <time.h>
#include <grp.h>
#include <utmpx.h>
#include <login_cap.h>


static bool hasPasswordMinimumLength(const passwd* pw)
{
    login_cap_t *lc;
	int min_length = 1;
	if ((lc = login_getpwclass(pw)) != NULL) {
		/* minpasswordlen capablity */
		return ((int)login_getcapnum(lc, "minpasswordlen",
				min_length, min_length)) > 1;
	}
	return false;
}

static bool hasPasswordForceMixCase(const passwd* pw)
{
	login_cap_t *lc;
	if ((lc = login_getpwclass(pw)) != NULL) {
		return login_getcapbool(lc, "mixpasswordcase", 1) == 1;
	}
	return false;
}

static int getLoginRetries(const passwd* pw)
{
	login_cap_t *lc;
	if ((lc = login_getpwclass(pw)) != NULL) {
		return login_getcapnum(lc, "login-retries", 1, 1);
	}
	return 10;
}

static time_t getIdleTimeout(const passwd* pw)
{
	login_cap_t *lc;
	if ((lc = login_getpwclass(pw)) != NULL) {
		/* idle timeout capablity */
		return ((time_t)login_getcaptime(lc, "idletime",
				0, 0));
	}
	return (time_t)0;
}

static time_t getLastLoginValue(const passwd* pw)
{
	struct utmpx *u;
	if (setutxdb(UTXDB_LASTLOGIN, NULL) != 0)
				return (time_t)0;
	if ((u = getutxuser(pw->pw_name)) == NULL) {
		return (time_t)0;
	}
	time_t t = u->ut_tv.tv_sec;
	return t;
}


UNIX_Account::UNIX_Account()
{
}

UNIX_Account::~UNIX_Account()
{
}

//ManagedElement
Boolean UNIX_Account::getCaption(CIMProperty& p) const
{
	p = CIMProperty(
        PROPERTY_CAPTION,
        String(user->pw_name));
    return true;
}

Boolean UNIX_Account::getDescription(CIMProperty& p) const
{
    /*
    char* fullname;
    int max = 1024;

    int n = strcspn(user->pw_gecos, ",");
    if (n > max -1)
    	n = max - 1;
	memcpy(fullname,user->pw_gecos, n); 
	*/
	char* fullname = user->pw_gecos;
	p = CIMProperty(
        PROPERTY_DESCRIPTION,
        String(fullname));
    return true;
}

Boolean UNIX_Account::getInstanceID(CIMProperty &p) const
{
    p = CIMProperty(
        PROPERTY_INSTANCE_ID,
        String(user->pw_name));
    return true;
}

//ManagedSystemElement
Boolean UNIX_Account::getInstallDate(CIMProperty& p) const
{
    if (sizeof(user->pw_dir) == 0) return false;
    /// We could at least check the user profile creationdate
    struct tm* clock;			// create a time structure
	struct stat attrib;			// create a file attribute structure
	if (stat(user->pw_dir, &attrib) == 0)
	{		// get the attributes mnt
		clock = gmtime(&(attrib.st_birthtime));	// Get the last modified time and put it into the time structure

		p = CIMProperty(PROPERTY_INSTALL_DATE,CIMDateTime(
				clock->tm_year + 1900, 
				clock->tm_mon + 1, 
				clock->tm_mday,
				clock->tm_hour,
				clock->tm_min,
				clock->tm_sec,
				0,0,
				clock->tm_gmtoff
				));
	    return true;
    }
    return false;
}

Boolean UNIX_Account::getName(CIMProperty& p) const
{
    // can vary, depending on class
    p = CIMProperty(
        PROPERTY_NAME,
        String(user->pw_name));
    return true;
}


//EnabledLogicalElement
Boolean UNIX_Account::getEnabledState(CIMProperty& p) const
{
    p = CIMProperty(
        PROPERTY_ENABLED_STATE, 1);
    return true;
}

Boolean UNIX_Account::getOtherEnabledState(CIMProperty& p) const
{
    p = CIMProperty(
        PROPERTY_OTHER_ENABLED_STATE,
        String(""));
    return true;
}

Boolean UNIX_Account::getRequestedState(CIMProperty& p) const
{
    p = CIMProperty(
        PROPERTY_REQUESTED_STATE, Uint16(0));
    return true;
}

Boolean UNIX_Account::getEnabledDefault(CIMProperty& p) const
{
    p = CIMProperty(
        PROPERTY_ENABLED_DEFAULT, Uint16(0));
    return true;
}

Boolean UNIX_Account::getTimeOfLastStateChange(CIMProperty& p) const
{
    /* TODO: LAST LOGIN for now */
    time_t ll = getLastLoginValue(user);
    if (ll == 0) return false;
	struct tm* clock;			// create a time structure
	clock = gmtime(&(ll));	// Get the last modified time and put it into the time structure
	p = CIMProperty(PROPERTY_TIME_OF_LAST_STATE_CHANGE,CIMDateTime(
			clock->tm_year + 1900, 
			clock->tm_mon + 1, 
			clock->tm_mday,
			clock->tm_hour,
			clock->tm_min,
			clock->tm_sec,
			0,0,
			clock->tm_gmtoff
			));
	return true;
}

Boolean UNIX_Account::getAvailableRequestedStates(CIMProperty& p) const
{
	p = CIMProperty(
        PROPERTY_AVAILABLE_REQUESTED_STATES, Uint16(0));
    return true;
}

Boolean UNIX_Account::getTransitioningToState(CIMProperty& p) const
{
    p = CIMProperty(
        PROPERTY_TRANSITIONING_TO_STATE, Uint16(0));
    return true;
}


	//CIM_Account
Boolean UNIX_Account::getUserID(CIMProperty &p) const
{
	p = CIMProperty(
        PROPERTY_USERID, Uint16(user->pw_uid));
    return true;
}

Boolean UNIX_Account::getObjectClass(CIMProperty &p) const
{
    Array<String> val;
    val.append(String("POSIXAccount"));
	p = CIMProperty(
        PROPERTY_OBJECT_CLASS, val);
    return true;
}

Boolean UNIX_Account::getDescriptions(CIMProperty &p) const
{
    Array<String> val;
	p = CIMProperty(
        PROPERTY_DESCRIPTIONS, val);
    return true;
}

Boolean UNIX_Account::getHost(CIMProperty &p) const
{
    Array<String> val;
    String csName;
    if (getHostName(csName))
    {
    	val.append(csName);
	}
	p = CIMProperty(
        PROPERTY_HOST, val);
    return true;
}

Boolean UNIX_Account::getLocalityName(CIMProperty &p) const
{
    Array<String> val;
	p = CIMProperty(
        PROPERTY_LOCALITY_NAME, val);
    return true;
}

Boolean UNIX_Account::getOrganizationName(CIMProperty &p) const
{
    Array<String> val;
	p = CIMProperty(
        PROPERTY_ORGANIZATION_NAME, val);
    return true;
}

Boolean UNIX_Account::getOU(CIMProperty &p) const
{
    Array<String> val;
	p = CIMProperty(
        PROPERTY_OU, val);
    return true;
}

Boolean UNIX_Account::getSeeAlso(CIMProperty &p) const
{
    Array<String> val;
    /* TODO: */
    gid_t *groups;
    int ngroups = 1022;
    groups = (gid_t*)calloc(ngroups, sizeof(gid_t));
    if (getgrouplist(user->pw_name, user->pw_gid, groups, &ngroups) == 0)
    {
    	for(int i = 0; i < ngroups;i++)
    	{
    		if (groups[i])
    		{
	    	  	val.append(String(group_from_gid(groups[i], 0)));
    	  	}
    	}
    }
	p = CIMProperty(
        PROPERTY_SEEALSO, val);
    return true;
}

Boolean UNIX_Account::getUserCertificate(CIMProperty &p) const
{
    Array<String> val;
	p = CIMProperty(
        PROPERTY_USER_CERTIFICATE, val);
    return true;
}

Boolean UNIX_Account::getUserPassword(CIMProperty &p) const
{
    Array<String> val;
    val.append(String(user->pw_passwd));
	p = CIMProperty(
        PROPERTY_USER_PASSWORD, val);
    return true;
}

Boolean UNIX_Account::getUserPasswordEncryptionAlgorithm(CIMProperty &p) const
{
	p = CIMProperty(
        PROPERTY_USER_PASSWORD_ENCRYPTION_ALGORITHM, Uint16(1));
    return true;
}

Boolean UNIX_Account::getOtherUserPasswordEncryptionAlgorithm(CIMProperty &p) const
{
	p = CIMProperty(
        PROPERTY_OTHER_USER_PASSWORD_ENCRYPTION_ALGORITHM, String("SHA512")); /* CHECK login.conf */
    return true;
}

Boolean UNIX_Account::getComplexPasswordRulesEnforced(CIMProperty &p) const
{
    Array<Uint16> val;
    if (hasPasswordMinimumLength(user))
    {
    	val.append(2);
    }
    if (hasPasswordForceMixCase(user))
    {
    	val.append(5);
    	val.append(6);
    }
	p = CIMProperty(
        PROPERTY_COMPLEX_PASSWORD_RULES_ENFORCED, val);
    return true;
}

Boolean UNIX_Account::getInactivityTimeout(CIMProperty &p) const
{
    time_t idle = getIdleTimeout(user);
    if (idle == 0) return false;
	p = CIMProperty(PROPERTY_INACTIVITY_TIMEOUT,CIMDateTime(
			idle
			, true));
    return true;
}

Boolean UNIX_Account::getLastLogin(CIMProperty &p) const
{
	time_t ll = getLastLoginValue(user);
    if (ll == 0) return false;
    struct tm* clock;			// create a time structure
	clock = gmtime(&(ll));	// Get the last modified time and put it into the time structure
	p = CIMProperty(PROPERTY_LAST_LOGIN,CIMDateTime(
			clock->tm_year + 1900, 
			clock->tm_mon + 1, 
			clock->tm_mday,
			clock->tm_hour,
			clock->tm_min,
			clock->tm_sec,
			0,0,
			clock->tm_gmtoff
			));
    return true;
}

Boolean UNIX_Account::getMaximumSuccessiveLoginFailures(CIMProperty &p) const
{
    int count = getLoginRetries(user);
	p = CIMProperty(
        PROPERTY_MAXIMUM_SUCCESSIVE_LOGIN_FAILURES, Uint16(count));
    return true;
}

Boolean UNIX_Account::getPasswordExpiration(CIMProperty &p) const
{
    if (user->pw_change == 0) return false;
    struct tm* clock;			// create a time structure
	clock = gmtime(&(user->pw_change));	// Get the last modified time and put it into the time structure
	p = CIMProperty(PROPERTY_PASSWORD_EXPIRATION,CIMDateTime(
			clock->tm_year + 1900, 
			clock->tm_mon + 1, 
			clock->tm_mday,
			clock->tm_hour,
			clock->tm_min,
			clock->tm_sec,
			0,0,
			clock->tm_gmtoff));
    return true;
}

Boolean UNIX_Account::getPasswordHistoryDepth(CIMProperty &p) const
{
    /* TODO: Or return false ?? */
	p = CIMProperty(
        PROPERTY_PASSWORD_HISTORY_DEPTH, Uint16(0));
    return true;
}

Boolean UNIX_Account::getUserPasswordEncoding(CIMProperty &p) const
{
	p = CIMProperty(
        PROPERTY_USER_PASSWORD_ENCODING, Uint16(5));
    return true;
}

String UNIX_Account::getHandle() const
{
	return String(user->pw_name);
}

Boolean UNIX_Account::find(String &handle)
{
	return false;
}

Boolean UNIX_Account::load(int &pIndex)
{
	if ((user = getpwent()) != NULL)
	{
		return true;
	}
	return false;
}

Boolean UNIX_Account::validateKey(CIMKeyBinding &kb) const
{
	/* Keys  */
	//CreationClassName
	//SystemCreationClassName
	//Name
	//SystemName

	CIMName name = kb.getName();
	if (name.equal(PROPERTY_CREATION_CLASS_NAME) ||
		name.equal(PROPERTY_SYSTEM_NAME) ||
		name.equal(PROPERTY_SYSTEM_CREATION_CLASS_NAME) ||
		name.equal(PROPERTY_NAME))
	{
		return true;
	}
	return false;
}

void UNIX_Account::buildKeyHandle(CIMKeyBinding &kb, String &s) const
{
	//Name
	if (kb.getName().equal(PROPERTY_NAME))
	{
		s.append(kb.getValue());
	}
}


#include <CIM_ManagedSystemElement.cpp>