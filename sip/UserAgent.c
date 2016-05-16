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
#include "Accounts.h"

struct UserAgent {
    struct Account *account;
    BOOL binded;
    struct DialogManager *dialogs;
};

void UserAgentSetUserName(struct UserAgent *ua, char *user)
{
    AccountSetUserName(ua->account, user);
}

char *UserAgentGetUserName(struct UserAgent *ua)
{
    return AccountGetUserName(ua->account);
}

void UserAgentSetAuthName(struct UserAgent *ua, char *authName)
{
    AccountSetAuthName(ua->account, authName);
}

char *UserAgentGetAuthName(struct UserAgent *ua)
{
    return AccountGetAuthName(ua->account);
}

void UserAgentSetProxy(struct UserAgent *ua, char *proxy)
{
    AccountSetProxy(ua->account, proxy);
}

char *UserAgentGetProxy(struct UserAgent *ua)
{
    return AccountGetProxy(ua->account);
}

void UserAgentSetRegistrar(struct UserAgent *ua, char *registrar)
{
    AccountSetRegistrar(ua->account, registrar);
}

char *UserAgentGetRegistrar(struct UserAgent *ua)
{
    return AccountGetRegistrar(ua->account);
}

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
    ua->dialogs = CreateDialogs();
    ua->account = CreateAccount();
    return ua;
}

void DestoryUserAgent(struct UserAgent **ua)
{
    if (*ua != NULL) {
        DestoryAccount(&(*ua)->account);
        DestoryDialogs(&(*ua)->dialogs);
        free(*ua);
        *ua = NULL;
    }
}
