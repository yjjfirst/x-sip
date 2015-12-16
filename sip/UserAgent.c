#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "UserAgent.h"
#include "TransactionNotifyInterface.h"
#include "Transaction.h"
#include "ExpiresHeader.h"
#include "Messages.h"
#include "Header.h"
#include "Dialogs.h"
#include "Utils.h"
#include "DialogId.h"
#include "Dialog.h"

struct UserAgent {
    struct TransactionOwnerInterface notifyInterface;
    char userName[USER_NAME_MAX_LENGTH];
    char authName[AUTH_NAME_MAX_LENGTH];
    char proxy[PROXY_MAX_LENGTH];
    char registrar[REGISTRAR_MAX_LENGTH];
    BOOL binded;
    struct Dialogs *dialogs;
};

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

void OnTransactionEvent(struct Transaction *t)
{
    struct Message *m = TransactionGetLatestResponse(t);
    struct UserAgent *ua = (struct UserAgent *) TransactionGetOwner(t);

    if (TransactionGetCurrentEvent(t) == TRANSACTION_EVENT_200OK) {
        if (TransactionGetType(t) == TRANSACTION_TYPE_CLIENT_NON_INVITE) {
            if (MessageGetExpires(m) != 0)
                ua->binded = TRUE;
            else
                ua->binded = FALSE;
        } else {
            struct DialogId *dialogid = CreateDialogIdFromMessage(m);
            struct Dialog *dialog = CreateDialog(dialogid);
            UserAgentAddDialog(ua, dialog);
        } 
    }
}

void UserAgentAddDialog(struct UserAgent *ua, struct Dialog *dialog)
{
    AddDialog(ua->dialogs, dialog);
}

struct Dialog *UserAgentGetDialog(struct UserAgent *ua, struct DialogId *callid)
{
    return GetDialogById(ua->dialogs, callid);
}

struct UserAgent *CreateUserAgent()
{
    struct UserAgent *ua = calloc(1, sizeof(struct UserAgent));
    struct Dialogs *dialogs = CreateDialogs();
    ua->notifyInterface.onEvent = OnTransactionEvent;
    ua->dialogs = dialogs;

    return ua;
}

void DestoryUserAgent(struct UserAgent **ua)
{
    if (*ua != NULL) {
        DestoryDialogs(&(*ua)->dialogs);
        free(*ua);
        *ua = NULL;
    }
}
