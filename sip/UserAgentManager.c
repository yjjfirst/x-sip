#include "UserAgentManager.h"
#include "UserAgent.h"
#include "Provision.h"

struct UserAgent *BuildUserAgent(struct Account *account)
{
    struct UserAgent *ua = CreateUserAgent();
    
    if (account == NULL) {
        UserAgentSetUserName(ua, GetUserName(0));
        UserAgentSetRegistrar(ua, GetRegistrar(0));
        UserAgentSetProxy(ua, GetProxy(0));
    } else {
    }

    return ua;
}

void AddUserAgent(struct UserAgent *ua)
{
}
