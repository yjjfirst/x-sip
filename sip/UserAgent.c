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

    AddDialog(dialog);
}

void UserAgentRemoveDialog(struct UserAgent *ua, struct DialogId *id)
{
    assert(ua != NULL);
    assert(id != NULL);

    RemoveDialog(id);
}

struct Dialog *UserAgentGetDialogById(struct UserAgent *ua, struct DialogId *callid)
{
    assert(ua != NULL);
    assert(callid != NULL);

    return GetDialogById(callid);
}

struct Dialog *UserAgentGetDialog(struct UserAgent *ua, int pos)
{
    assert(ua != NULL);

    return GetDialog(pos);
}

int UserAgentCountDialogs(struct UserAgent *ua)
{
    assert (ua != NULL);

    return CountDialogs();
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
    ua->account = acc;

    return ua;
}

void DestroyUserAgent(struct UserAgent **ua)
{
    if (*ua != NULL) {
        DestroyDialogList();
        free(*ua);
        *ua = NULL;
    }
}

void UserAgentDump(struct UserAgent *ua)
{
    printf("%s:%d\n", AccountGetUserName(ua->account), CountDialogs());
}
