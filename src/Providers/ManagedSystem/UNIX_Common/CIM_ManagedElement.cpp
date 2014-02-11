
Boolean getHostName(String& csName)
{
     char    hostName[PEGASUS_MAXHOSTNAMELEN + 1];

     if (gethostname(hostName, sizeof(hostName)) != 0)
     {
         return false;
     }
     hostName[sizeof(hostName)-1] = 0;

     // Now get the official hostname.  If this call fails then return
     // the value from gethostname().
     /*
     struct  hostent *he;
     if ((he=gethostbyname(hostName)))
     {
         csName.assign(he->h_name);
     }
     else
     {
         csName.assign(hostName);
     }
     */
     csName.assign(hostName);
     return true;
}


/*
================================================================================
NAME              : getOSName
DESCRIPTION       : Platform-specific method to get OSname.
ASSUMPTIONS       : None
PRE-CONDITIONS    :
POST-CONDITIONS   :
NOTES             :
================================================================================
*/
Boolean getOSName(String& osName)
{
    struct utsname unameInfo;
    // Call uname and check for any errors.
    if (uname(&unameInfo) < 0)
    {
       osName.assign("");
       return false;
    }
    osName.assign(unameInfo.sysname);
    return true;
}

String getCurrentTime()
{
  time_t t = time(0);
  /*
  char buffer[40];
  return String(ctime_r(&t, buffer));
  */

  struct tm* clock;			// create a time structure
  clock = gmtime(&(t));	// Get the last modified time and put it into the time structure
  return CIMDateTime(
  	clock->tm_year + 1900, 
	clock->tm_mon + 1, 
	clock->tm_mday,
	clock->tm_hour,
	clock->tm_min,
	clock->tm_sec,
	0,0,
	clock->tm_gmtoff	
  ).toString();
}

#ifndef __initialize_H
Boolean CLASS_IMPLEMENTATION::initialize()
{
	return true;
}
#endif

#ifndef __finalize_H
Boolean CLASS_IMPLEMENTATION::finalize()
{
	return true;
}
#endif


#ifndef __getCreationClassName_H
Boolean CLASS_IMPLEMENTATION::getCreationClassName(CIMProperty& p) const
{
    // can vary, depending on class
    p = CIMProperty(
        PROPERTY_CREATION_CLASS_NAME,
        String(CLASS_IMPLEMENTATION_NAME));
    return true;
}
#endif

#ifndef __getCreationClassName_H
Boolean CLASS_IMPLEMENTATION::getSystemCreationClassName(CIMProperty& p) const
{
    // can vary, depending on class
    p = CIMProperty(
        PROPERTY_SYSTEM_CREATION_CLASS_NAME,
        String(BASE_CLASS_NAME));
    return true;
}
#endif


#ifndef __getElementName_H
Boolean CLASS_IMPLEMENTATION::getElementName(CIMProperty &p) const
{
    p = CIMProperty(
        PROPERTY_ELEMENT_NAME,
        String(ELEMENT_NAME));
    return true;
}
#endif


#ifndef __getPrimaryStatus_H
Boolean CLASS_IMPLEMENTATION::getPrimaryStatus(CIMProperty &p) const
{
    return false;
}
#endif 

#ifndef __getGeneration_H
Boolean CLASS_IMPLEMENTATION::getGeneration(CIMProperty &p) const
{
	return false;
}
#endif