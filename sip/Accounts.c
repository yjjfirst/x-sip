#include <stdio.h>
#include "Accounts.h"
#include "Utils.h"

struct Account {
    char userName[USER_NAME_MAX_LENGTH];
    char authName[AUTH_NAME_MAX_LENGTH];
    char proxy[PROXY_MAX_LENGTH];
    char registrar[REGISTRAR_MAX_LENGTH];
    BOOL binded;
};

DEFINE_STRING_MEMBER_WRITER(struct Account, AccountSetUserName, userName, USER_NAME_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct Account, AccountGetUserName, userName);

DEFINE_STRING_MEMBER_WRITER(struct Account, AccountSetAuthName, authName, USER_NAME_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct Account, AccountGetAuthName, authName);

DEFINE_STRING_MEMBER_WRITER(struct Account, AccountSetProxy, proxy, USER_NAME_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct Account, AccountGetProxy, proxy);

DEFINE_STRING_MEMBER_WRITER(struct Account, AccountSetRegistrar, registrar, USER_NAME_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct Account, AccountGetRegistrar, registrar);

struct Account *CreateEmptyAccount()
{
    struct Account *account = calloc(1, sizeof(struct Account));
    return account;
}

BOOL AccountBinded(struct Account *account)
{
    assert(account != NULL);
    return account->binded;
}

void AccountSetBinded(struct Account *account)
{
    assert(account != NULL);
    account->binded = TRUE;
}

void AccountSetUnbinded(struct Account *account)
{
    assert(account != NULL);
    account->binded = FALSE;
}

struct Account *CreateAccount(char *username, char *authname, char *proxy, char *registrar)
{
    struct Account *account = calloc(1, sizeof(struct Account));

    AccountSetUserName(account, username);
    AccountSetAuthName(account, authname);
    AccountSetProxy(account, proxy);
    AccountSetRegistrar(account, registrar);

    return account;
}

void DestroyAccount(struct Account **account)
{
    if (*account != NULL) {
        free(*account);
        *account = NULL;
    }
}
