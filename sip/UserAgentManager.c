#include "UserAgentManager.h"
#include "UserAgent.h"
#include "Provision.h"

struct UserAgent *BuildUserAgent()
{
    struct UserAgent *ua = CreateUserAgent();
    
    UserAgentSetUserName(ua, GetUserName(0));
    UserAgentSetRegistrar(ua, GetRegistrar(0));
    UserAgentSetProxy(ua, GetProxy(0));
    
    return ua;
}

void AddUserAgent(struct UserAgent *ua)
{
}
