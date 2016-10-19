#include "Bool.h"

#define PROXY_MAX_LENGTH 64
#define REGISTRAR_MAX_LENGTH 64
#define AUTH_NAME_MAX_LENGTH 64

struct UserAgent;
struct DialogId;

void UaSetUserName(struct UserAgent *ua, char *name);
char *UaGetUserName(struct UserAgent *ua);
void UaSetProxy(struct UserAgent *ua, char *proxy);
char *UaGetProxy(struct UserAgent *ua);
void UaSetRegistrar(struct UserAgent *ua, char *registrar);
char *UaGetRegistrar(struct UserAgent *ua);
void UaSetAuthName(struct UserAgent *ua, char *authName);
char *UaGetAuthName(struct UserAgent *ua);
void UaSetBinded(struct UserAgent *ua);
void UaSetUnbinded(struct UserAgent *ua);
struct Account *UaGetAccount(struct UserAgent *ua);

BOOL UaBinded(struct UserAgent *ua);


extern void (*UaMakeCall)(struct UserAgent *ua);
extern void (*UaEndCall)(struct UserAgent *ua);
extern void (*UaAcceptCall)(struct UserAgent *ua);


struct UserAgent *CreateUserAgent(int account);
void DestroyUserAgent(struct UserAgent **ua);

void DumpUa(struct UserAgent *ua);
