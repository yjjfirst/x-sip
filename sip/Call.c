#include <stdio.h>

#include "UserAgentManager.h"
#include "UserAgent.h"

struct UserAgent *CallOut(int account, char *dest)
{
    struct UserAgent *ua = AddUserAgent(0);
    UserAgentMakeCall(ua);
    return ua;
}

void EndCall(struct UserAgent *ua)
{
    UserAgentEndCall(ua);
}

void AcceptCall(struct UserAgent *ua)
{
    UserAgentAcceptCall(ua);
}

BOOL ClientMessageHandle(char *string)
{
    struct UserAgent *ua = GetUserAgent(0);
    AcceptCall(ua);
    
    return 1;
}

