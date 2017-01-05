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

DEFINE_INT_MEMBER_WRITER(struct Account, AccountSetProxyPort, proxyPort);
DEFINE_INT_MEMBER_READER(struct Account, AccountGetProxyPort, proxyPort);

DEFINE_STRING_MEMBER_WRITER(struct Account, AccountSetRegistrarAddr, registrar, USER_NAME_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct Account, AccountGetRegistrarAddr, registrar);

DEFINE_INT_MEMBER_WRITER(struct Account, AccountSetRegistrarPort, registrarPort);
DEFINE_INT_MEMBER_READER(struct Account, AccountGetRegistrarPort, registrarPort);

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
    printf("User Name: %s | Auth Name: %s | Pass word:%s | Proxy:%s | Registrar :%s | Binded:%d\n",
           AccountGetUserName(account),
           AccountGetAuthName(account),
           AccountGetPasswd(account),
           AccountGetProxyAddr(account),
           AccountGetRegistrarAddr(account),
           AccountBinded(account));
}

struct Account *CreateAccount(char *username, char *authname, char *passwd, char *proxy, char *registrar)
{
    struct Account *account = calloc(1, sizeof(struct Account));

    AccountSetUserName(account, username);
    AccountSetAuthName(account, authname);
    AccountSetPasswd(account, passwd);
    AccountSetProxyAddr(account, proxy);
    AccountSetRegistrarAddr(account, registrar);

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
