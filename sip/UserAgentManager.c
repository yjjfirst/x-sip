#include "UserAgentManager.h"
#include "UserAgent.h"
#include "Provision.h"

struct UserAgent *BuildUserAgent()
{
    struct UserAgent *ua = CreateUserAgent();
    
    UserAgentSetUserName(ua, GetUserName());
    UserAgentSetRegistrar(ua, GetRegistrar());
    UserAgentSetProxy(ua, GetProxy());
    
    return ua;
}

void AddUserAgent(struct UserAgent *ua)
{
}
