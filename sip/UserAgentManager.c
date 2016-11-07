#include <stdio.h>

#include "AccountManager.h"
#include "UserAgentManager.h"
#include "UserAgent.h"
#include "Provision.h"
#include "utils/list/include/list.h"

struct UserAgentManager {
    t_list *userAgents;
};

struct UserAgentManager UserAgentManager;

struct UserAgent *AddUa(int account)
{
    struct UserAgent *ua = CreateUserAgent(account);
    struct UserAgentManager *uam = &UserAgentManager;
    
    put_in_list(&uam->userAgents, ua);

    return ua;
}

void RemoveUa(struct UserAgent *ua)
{
    int i = 0;
    struct UserAgentManager *uam = &UserAgentManager;

    for (; i < CountUas(); i ++) {
        if (get_data_at(uam->userAgents, i) == ua) {
            del_node_at(&uam->userAgents, i);
            DestroyUserAgent(&ua);
        }
    }
}

int CountUas()
{
    struct UserAgentManager *uam = &UserAgentManager;
    return get_list_len(uam->userAgents);
}

struct UserAgent *GetUa(int pos)
{
    struct UserAgentManager *uam = &UserAgentManager;
    return get_data_at(uam->userAgents, pos);
}

int GetUaPosition(struct UserAgent *ua)
{
    int i = 0;
    struct UserAgentManager *uam = &UserAgentManager;   
    int len = CountUas(uam);

    for (; i < len; i ++) {
        struct UserAgent *ua1 = get_data_at(uam->userAgents, i);
        if (ua1 == ua) return i;
    }

    return -1;
}

void ClearUaManager()
{
    int i = 0;
    struct UserAgentManager *uam = &UserAgentManager;   
    int len = CountUas(uam);
    
    for (; i < len; i ++) {
        struct UserAgent *ua = get_data_at(uam->userAgents, i);
        DestroyUserAgent(&ua);
    }
    
    destroy_list(&uam->userAgents, NULL);
    uam->userAgents = NULL;
}

void DumpUaManager()
{
    int i = 0;
    struct UserAgentManager *uam = &UserAgentManager;   
    int len = CountUas(uam);

    printf("\n");
    for (; i < len; i ++) {
        struct UserAgent *ua = get_data_at(uam->userAgents, i);
        DumpUa(ua);
    }
}
