#include "Bool.h"

#define USER_NAME_MAX_LENGTH 64
#define AUTH_NAME_MAX_LENGTH 64
#define PASS_WORD_MAX_LENGTH 64
#define PROXY_MAX_LENGTH 64
#define REGISTRAR_MAX_LENGTH 64

struct Account;

void AccountSetUserName(struct Account *account, char *user);
char *AccountGetUserName(struct Account *account);

void AccountSetPassword(struct Account *account, char *passwd);
char *AccountGetPassword(struct Account *account);

void AccountSetAuthName(struct Account *account, char *userName);
char *AccountGetAuthName(struct Account *account);

void AccountSetProxyAddr(struct Account *account, char *proxy);
char *AccountGetProxyAddr(struct Account *account);

void AccountSetRegistrarAddr(struct Account *account, char *registrar);
char *AccountGetRegistrarAddr(struct Account *account);

int AccountGetRegistrarPort(struct Account *account);
void AccountSetRegistrarPort(struct Account *account, int port);

int AccountGetProxyPort(struct Account *account);
void AccountSetProxyPort(struct Account *account, int port);

BOOL AccountBinded(struct Account *account);
void AccountSetBinded(struct Account *account);
void AccountSetUnbinded(struct Account *account);

struct Account *CreateAccount(char *username, char *authname, char *passwd, char *proxy, char *registrar);
void DestroyAccount(struct Account **account);

void AccountDump(struct Account *account);
