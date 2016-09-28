#include <stdio.h>
#include "Accounts.h"
#include "Utils.h"

struct Account {
    char userName[USER_NAME_MAX_LENGTH];
    char authName[AUTH_NAME_MAX_LENGTH];
    char passwd[PASS_WORD_MAX_LENGTH];
    char proxy[PROXY_MAX_LENGTH];
    int  proxyPort;
    char registrar[REGISTRAR_MAX_LENGTH];
    int  registrarPort;
    BOOL binded;
};

DEFINE_STRING_MEMBER_WRITER(struct Account, AccountSetUserName, userName, USER_NAME_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct Account, AccountGetUserName, userName);

DEFINE_STRING_MEMBER_WRITER(struct Account, AccountSetPasswd, passwd, PASS_WORD_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct Account, AccountGetPasswd, passwd);

DEFINE_STRING_MEMBER_WRITER(struct Account, AccountSetAuthName, authName, USER_NAME_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct Account, AccountGetAuthName, authName);

DEFINE_STRING_MEMBER_WRITER(struct Account, AccountSetProxyAddr, proxy, USER_NAME_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct Account, AccountGetProxyAddr, proxy);

DEFINE_STRING_MEMBER_WRITER(struct Account, AccountSetRegistrar, registrar, USER_NAME_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct Account, AccountGetRegistrar, registrar);

int AccountGetRegistrarPort(struct Account *account)
{
    return account->registrarPort;
}

void AccountSetRegistrarPort(struct Account *account, int port)
{
    account->registrarPort = port;
}

int AccountGetProxyPort(struct Account *account)
{
    return account->proxyPort;
}

void AccountSetProxyPort(struct Account *account, int port)
{
    account->proxyPort = port;
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

void AccountDump(struct Account *account)
{
    printf("%s:%s:%s:%s:%s:%d\n",
           AccountGetUserName(account),
           AccountGetAuthName(account),
           AccountGetPasswd(account),
           AccountGetProxyAddr(account),
           AccountGetRegistrar(account),
           AccountBinded(account));
}

struct Account *CreateAccount(char *username, char *authname, char *passwd, char *proxy, char *registrar)
{
    struct Account *account = calloc(1, sizeof(struct Account));

    AccountSetUserName(account, username);
    AccountSetAuthName(account, authname);
    AccountSetPasswd(account, passwd);
    AccountSetProxyAddr(account, proxy);
    AccountSetRegistrar(account, registrar);

    AccountSetProxyPort(account, 5060);
    AccountSetRegistrarPort(account, 5060);

    return account;
}

void DestroyAccount(struct Account **account)
{
    if (*account != NULL) {
        free(*account);
        *account = NULL;
    }
}
