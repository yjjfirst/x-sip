#include <stdio.h>

#include "AccountManager.h"
#include "UserAgent.h"
#include "Provision.h"
#include "utils/list/include/list.h"

struct UserAgentManager {
    t_list *userAgents;
};

struct UserAgentManager UserAgentManager;

struct UserAgent *AddUserAgent(int account)
{
    struct UserAgent *ua = CreateUserAgent(account);
    struct UserAgentManager *uam = &UserAgentManager;
    
    put_in_list(&uam->userAgents, ua);

    return ua;
}

int CountUserAgent()
{
    struct UserAgentManager *uam = &UserAgentManager;
    return get_list_len(uam->userAgents);
}

struct UserAgent *GetUserAgent(int pos)
{
    struct UserAgentManager *uam = &UserAgentManager;
    return get_data_at(uam->userAgents, pos);
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

void UserAgentManagerDump()
{
    int i = 0;
    struct UserAgentManager *uam = &UserAgentManager;   
    int len = CountUserAgent(uam);

    printf("\n");
    for (; i < len; i ++) {
        struct UserAgent *ua = get_data_at(uam->userAgents, i);
        UserAgentDump(ua);
    }
}
