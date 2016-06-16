#include "Bool.h"

#define PROXY_MAX_LENGTH 64
#define REGISTRAR_MAX_LENGTH 64
#define AUTH_NAME_MAX_LENGTH 64

struct UserAgent;
struct DialogId;

void UserAgentSetUserName(struct UserAgent *ua, char *name);
char *UserAgentGetUserName(struct UserAgent *ua);
void UserAgentSetProxy(struct UserAgent *ua, char *proxy);
char *UserAgentGetProxy(struct UserAgent *ua);
void UserAgentSetRegistrar(struct UserAgent *ua, char *registrar);
char *UserAgentGetRegistrar(struct UserAgent *ua);
void UserAgentSetAuthName(struct UserAgent *ua, char *authName);
char *UserAgentGetAuthName(struct UserAgent *ua);
void UserAgentSetBinded(struct UserAgent *ua);
void UserAgentSetUnbinded(struct UserAgent *ua);
struct Account *UserAgentGetAccount(struct UserAgent *ua);


BOOL UserAgentBinded(struct UserAgent *ua);
struct Dialog *UserAgentGetDialog(struct UserAgent *ua, struct DialogId *callid);
void UserAgentAddDialog(struct UserAgent *ua, struct Dialog *dialog);
void UserAgentRemoveDialog(struct UserAgent *ua, struct DialogId *id);

struct UserAgent *CreateUserAgent();
void DestoryUserAgent(struct UserAgent **ua);
