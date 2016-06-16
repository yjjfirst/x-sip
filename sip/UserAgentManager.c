#include "AccountManager.h"
#include "UserAgent.h"
#include "Provision.h"

struct UserAgent *BuildUserAgent(int account)
{
    struct UserAgent *ua = CreateUserAgent(0);

    UserAgentSetUserName(ua, GetUserName(account));
    UserAgentSetRegistrar(ua, GetRegistrar(account));
    UserAgentSetProxy(ua, GetProxy(account));

    return ua;
}

void AddUserAgent(struct UserAgent *ua)
{
}
