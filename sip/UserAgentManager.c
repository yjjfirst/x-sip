#include "AccountManager.h"
#include "UserAgent.h"
#include "Provision.h"

struct UserAgent *BuildUserAgent(int account)
{
    struct UserAgent *ua = CreateUserAgent(0);
    return ua;
}

void AddUserAgent(struct UserAgent *ua)
{
}
