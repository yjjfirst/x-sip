#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "AccountManager.h"
#include "UserAgent.h"
#include "Transaction.h"
#include "ExpiresHeader.h"
#include "Messages.h"
#include "Header.h"
#include "DialogManager.h"
#include "Utils.h"
#include "DialogId.h"
#include "Dialog.h"
#include "Accounts.h"

struct UserAgent {
    struct Account *account;
    struct DialogManager *dialogs;
};

char *UserAgentGetUserName(struct UserAgent *ua)
{
    return AccountGetUserName(ua->account);
}

char *UserAgentGetAuthName(struct UserAgent *ua)
{
    return AccountGetAuthName(ua->account);
}

char *UserAgentGetProxy(struct UserAgent *ua)
{
    return AccountGetProxy(ua->account);
}

char *UserAgentGetRegistrar(struct UserAgent *ua)
{
    return AccountGetRegistrar(ua->account);
}

struct Account *UserAgentGetAccount(struct UserAgent *ua)
{
    assert(ua != NULL);
    return ua->account;
}

BOOL UserAgentBinded(struct UserAgent *ua)
{
    struct Account *account = ua->account;
    return AccountBinded(account);
}

void UserAgentSetBinded(struct UserAgent *ua)
{    
    assert(ua != NULL);
    struct Account *account = ua->account;
    AccountSetBinded(account);
}

void UserAgentSetUnbinded(struct UserAgent *ua)
{
    assert(ua != NULL);
    struct Account *account = ua->account;
    AccountSetUnbinded(account);
}

void UserAgentAddDialog(struct UserAgent *ua, struct Dialog *dialog)
{
    assert(ua != NULL);
    assert(dialog != NULL);

    AddDialog(ua->dialogs, dialog);
}

void UserAgentRemoveDialog(struct UserAgent *ua, struct DialogId *id)
{
    assert(ua != NULL);
    assert(id != NULL);

    RemoveDialog(ua->dialogs, id);
}

struct DialogManager *UserAgentGetDialogManager(struct UserAgent *ua)
{
    assert(ua != NULL);

    return ua->dialogs;
}

struct Dialog *UserAgentGetDialogById(struct UserAgent *ua, struct DialogId *callid)
{
    assert(ua != NULL);
    assert(callid != NULL);

    return GetDialogById(ua->dialogs, callid);
}

struct Dialog *UserAgentGetDialog(struct UserAgent *ua, int pos)
{
    assert(ua != NULL);

    return GetDialog(ua->dialogs, pos);
}

int UserAgentCountDialogs(struct UserAgent *ua)
{
    assert (ua != NULL);

    return CountDialogs(ua->dialogs);
}

void UserAgentMakeCall(struct UserAgent *ua)
{
    struct Dialog *dialog = AddNewDialog(NULL_DIALOG_ID, ua);
    DialogInvite(dialog);
}

void UserAgentEndCall(struct UserAgent *ua)
{
    struct Dialog *dialog = UserAgentGetDialog(ua, 0);
    DialogBye(dialog);
}

struct UserAgent *CreateUserAgent(int account)
{
    struct UserAgent *ua = calloc(1, sizeof(struct UserAgent));
    struct Account *acc = GetAccount(account);
    ua->dialogs = CreateDialogs();
    ua->account = acc;

    return ua;
}

void DestroyUserAgent(struct UserAgent **ua)
{
    if (*ua != NULL) {
        DestroyDialogs(&(*ua)->dialogs);
        free(*ua);
        *ua = NULL;
    }
}
