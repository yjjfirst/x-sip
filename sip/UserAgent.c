#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "UserAgent.h"
#include "Transaction.h"
#include "ExpiresHeader.h"
#include "Messages.h"
#include "Header.h"
#include "Dialogs.h"
#include "Utils.h"
#include "DialogId.h"
#include "Dialog.h"

struct UserAgent {
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

void UserAgentSetBinded(struct UserAgent *ua)
{
    ua->binded = TRUE;
}

void UserAgentSetUnbinded(struct UserAgent *ua)
{
    ua->binded = FALSE;
}

void UserAgentAddDialog(struct UserAgent *ua, struct Dialog *dialog)
{
    assert(ua != NULL);
    assert(dialog != NULL);

    AddDialog(ua->dialogs, dialog);
}

struct Dialog *UserAgentGetDialog(struct UserAgent *ua, struct DialogId *callid)
{
    assert(ua != NULL);
    assert(callid != NULL);
    return GetDialogById(ua->dialogs, callid);
}

struct UserAgent *CreateUserAgent()
{
    struct UserAgent *ua = calloc(1, sizeof(struct UserAgent));
    struct Dialogs *dialogs = CreateDialogs();
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
