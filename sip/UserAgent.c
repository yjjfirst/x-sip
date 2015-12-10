#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "UserAgent.h"
#include "TransactionNotifyInterface.h"
#include "Transaction.h"
#include "ExpiresHeader.h"
#include "Messages.h"
#include "Header.h"

struct UserAgent {
    struct TransactionOwnerInterface notifyInterface;
    char userName[USER_NAME_MAX_LENGTH];
    char authName[AUTH_NAME_MAX_LENGTH];
    char proxy[PROXY_MAX_LENGTH];
    char registrar[REGISTRAR_MAX_LENGTH];
    BOOL binded;
    struct Dialog *dialogs;
};

#define DEFINE_STRING_MEMBER_WRITER(struct, name, field, maxLength)    \
    void name (struct *ua, char *field)                                \
    {                                                                  \
        assert(ua != NULL);                                            \
        strncpy(ua->field, field, sizeof(ua->field) - 1);              \
        ua->field[maxLength - 1] = '\0';                               \
    }                                                                  \
    
#define DEFINE_STRING_MEMBER_READER(struct, name, field) \
    char *name (struct *ua)                              \
    {                                                    \
        assert(ua != NULL);                              \
        return ua->field;                                \
    }                                                    \

DEFINE_STRING_MEMBER_WRITER(struct UserAgent, UserAgentSetUserName, userName, USER_NAME_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct UserAgent, UserAgentGetUserName,userName);

DEFINE_STRING_MEMBER_WRITER(struct UserAgent, UserAgentSetProxy, proxy, PROXY_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct UserAgent, UserAgentGetProxy, proxy);

DEFINE_STRING_MEMBER_WRITER(struct UserAgent, UserAgentSetRegistrar, registrar, REGISTRAR_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct UserAgent, UserAgentGetRegistrar, registrar);

DEFINE_STRING_MEMBER_WRITER(struct UserAgent, UserAgentSetAuthName, authName, AUTH_NAME_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct UserAgent, UserAgentGetAuthName, authName);

BOOL UserAgentBinded(struct UserAgent *ua)
{
    return ua->binded;
}

struct Dialog *UserAgentGetDialog(struct UserAgent *ua)
{
    return ua->dialogs;
}

void OnTransactionEvent(struct Transaction *t)
{
    struct UserAgent *ua = NULL;

    if (TransactionGetCurrentEvent(t) == TRANSACTION_EVENT_200OK) {
        if (TransactionGetType(t) == TRANSACTION_TYPE_CLIENT_NON_INVITE) {
            struct Message *m = TransactionGetLatestResponse(t);
            struct ExpiresHeader *e = (struct ExpiresHeader *)MessageGetHeader(HEADER_NAME_EXPIRES, m); 
            ua = (struct UserAgent *) TransactionGetOwner(t);
            if (ExpiresHeaderGetExpires(e) != 0)
                ua->binded = TRUE;
            else
                ua->binded = FALSE;
        } 
        
    }
}

struct UserAgent *CreateUserAgent()
{
    struct UserAgent *ua = calloc(1, sizeof(struct UserAgent));
    ua->notifyInterface.onEvent = OnTransactionEvent;
    return ua;
}

void DestoryUserAgent(struct UserAgent **ua)
{
    if (*ua != NULL) {
        free(*ua);
        *ua = NULL;
    }
}
