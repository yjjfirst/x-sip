#include "Bool.h"

#define USER_NAME_MAX_LENGTH 64
#define AUTH_NAME_MAX_LENGTH 64
#define PASS_WORD_MAX_LENGTH 64
#define PROXY_MAX_LENGTH 64
#define REGISTRAR_MAX_LENGTH 64

struct Account;

void AccountSetUserName(struct Account *account, char *user);
char *AccountGetUserName(struct Account *account);

void AccountSetPasswd(struct Account *account, char *passwd);
char *AccountGetPasswd(struct Account *account);

void AccountSetAuthName(struct Account *account, char *userName);
char *AccountGetAuthName(struct Account *account);

void AccountSetProxy(struct Account *account, char *proxy);
char *AccountGetProxy(struct Account *account);

void AccountSetRegistrar(struct Account *account, char *registrar);
char *AccountGetRegistrar(struct Account *account);

BOOL AccountBinded(struct Account *account);
void AccountSetBinded(struct Account *account);
void AccountSetUnbinded(struct Account *account);


struct Account *CreateEmptyAccount();
struct Account *CreateAccount(char *username, char *authname, char *passwd, char *proxy, char *registrar);
void DestroyAccount(struct Account **account);

void AccountDump(struct Account *account);
