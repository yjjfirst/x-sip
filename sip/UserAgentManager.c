#include "UserAgentManager.h"
#include "UserAgent.h"
#include "Provision.h"

struct UserAgent *BuildUserAgent(int account)
{
    struct UserAgent *ua = CreateUserAgent();
    
    UserAgentSetUserName(ua, GetUserName(account));
    UserAgentSetRegistrar(ua, GetRegistrar(account));
    UserAgentSetProxy(ua, GetProxy(account));

    return ua;
}

void AddUserAgent(struct UserAgent *ua)
{
}
