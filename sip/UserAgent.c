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

char *UaGetUsername(struct UserAgent *ua)
{
    return AccountGetUserName(ua->account);
}

char *UaGetAuthname(struct UserAgent *ua)
{
    return AccountGetAuthName(ua->account);
}

char *UaGetPassword(struct UserAgent *ua)
{
    return AccountGetPassword(ua->account);
}

char *UaGetProxy(struct UserAgent *ua)
{
    return AccountGetProxyAddr(ua->account);
}

int UaGetProxyPort(struct UserAgent *ua)
{
    return AccountGetProxyPort(ua->account);
}

char *UaRegistrar(struct UserAgent *ua)
{
    return AccountGetRegistrarAddr(ua->account);
}

struct Account *UaGetAccount(struct UserAgent *ua)
{
    assert(ua != NULL);
    return ua->account;
}

BOOL UaBinded(struct UserAgent *ua)
{
    struct Account *account = ua->account;
    return AccountBinded(account);
}

void UaSetBinded(struct UserAgent *ua)
{    
    assert(ua != NULL);
    struct Account *account = ua->account;
    AccountSetBinded(account);
}

void UaSetUnbinded(struct UserAgent *ua)
{
    assert(ua != NULL);
    struct Account *account = ua->account;
    AccountSetUnbinded(account);
}

void UaMakeCallImpl(struct UserAgent *ua, char *dest)
{
    struct Dialog *dialog = AddDialog(NULL_DIALOG_ID, ua);
    DialogInvite(dialog, dest);
}
void (*UaMakeCall)(struct UserAgent *ua, char *dest) = UaMakeCallImpl;

void UaEndCallImpl(struct UserAgent *ua)
{
    struct Dialog *dialog = GetDialog(0);
    DialogTerminate(dialog);
}
void (*UaEndCall)(struct UserAgent *ua) = UaEndCallImpl;


void UaAcceptCallImpl(struct UserAgent *ua)
{
    struct Dialog *dialog = GetDialogByUa(ua);
    DialogOk(dialog);
}
void (*UaAcceptCall)(struct UserAgent *ua) = UaAcceptCallImpl;

void UaRingingImpl(struct UserAgent *ua)
{
    struct Dialog *d = GetDialogByUa(ua);
    DialogRinging(d);
}
void (*UaRinging)(struct UserAgent *ua) = UaRingingImpl;

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

void DumpUa(struct UserAgent *ua)
{
    printf("User Name: %s\n", AccountGetUserName(ua->account));
}
