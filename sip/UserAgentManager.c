#include <stdio.h>

#include "AccountManager.h"
#include "UserAgent.h"
#include "Provision.h"
#include "utils/list/include/list.h"

struct UserAgentManager {
    t_list *userAgents;
};

struct UserAgentManager UserAgentManager;

struct UserAgent *AddUserAgent(int ua)
{
    struct UserAgent *userAgent = CreateUserAgent(ua);
    struct UserAgentManager *uam = &UserAgentManager;
    
    put_in_list(&uam->userAgents, userAgent);

    return userAgent;
}

int CountUserAgent()
{
    struct UserAgentManager *uam = &UserAgentManager;
    return get_list_len(uam->userAgents);
}

void ClearUserAgentManager()
{
    int i = 0;
    struct UserAgentManager *uam = &UserAgentManager;   
    int len = CountUserAgent(uam);
    
    for (; i < len; i ++) {
        struct UserAgent *ua = get_data_at(uam->userAgents, i);
        DestroyUserAgent(&ua);
    }
    
    destroy_list(&uam->userAgents, NULL);
    uam->userAgents = NULL;
}

