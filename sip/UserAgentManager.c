#include "AccountManager.h"
#include "UserAgent.h"
#include "Provision.h"
#include "utils/list/include/list.h"

struct UserAgentManager {
    t_list *userAgents;
};

int AddUserAgent(struct UserAgentManager *uam, struct UserAgent *ua)
{
    put_in_list(&uam->userAgents, ua);
    return get_list_len(uam->userAgents);
}

int CountUserAgent(struct UserAgentManager *uam)
{
    return get_list_len(uam->userAgents);
}

struct UserAgentManager *CreateUserAgentManager()
{
    return calloc(1, sizeof(struct UserAgentManager));
}

void ClearUserAgentManager(struct UserAgentManager *uam)
{
    int i = 0;
    int len = CountUserAgent(uam);
    
    for (; i < len; i ++) {
        struct UserAgent *ua = get_data_at(uam->userAgents, i);
        DestoryUserAgent(&ua);
    }
    
    destroy_list(&uam->userAgents, NULL);
}

void DestoryUserAgentManager(struct UserAgentManager **uam)
{
    if (*uam == NULL) return;
    
    ClearUserAgentManager(*uam);
    free(*uam);
    *uam = NULL;
}

