#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Provider/CIMInstanceQueryProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>
#include <Pegasus/Common/String.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

Boolean getHostName(String& csName);
Boolean getOSName(String& osName);
String getCurrentTime();
