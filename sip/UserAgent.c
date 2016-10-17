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
#include "TransactionManager.h"
#include "UserAgentManager.h"

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
    return AccountGetProxyAddr(ua->account);
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

void UaMakeCall(struct UserAgent *ua)
{
    struct Dialog *dialog = AddDialog(NULL_DIALOG_ID, ua);
    DialogInvite(dialog);
}

void UaEndCall(struct UserAgent *ua)
{
    struct Dialog *dialog = GetDialog(0);
    DialogBye(dialog);
}

void UaAcceptCall(struct UserAgent *ua)
{
    struct Dialog *dialog = GetDialogByUserAgent(ua);

    DialogOk(dialog);
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
    printf("%s\n", AccountGetUserName(ua->account));
}
