#include "Bool.h"

#define PROXY_MAX_LENGTH 64
#define REGISTRAR_MAX_LENGTH 64
#define AUTH_NAME_MAX_LENGTH 64

struct UserAgent;
struct DialogId;

char *UaGetUsername(struct UserAgent *ua);
char *UaGetPassword(struct UserAgent *ua);
char *UaGetProxy(struct UserAgent *ua);
int UaGetProxyPort(struct UserAgent *ua);
char *UaGetRegistrar(struct UserAgent *ua);
int UaGetRegistrarPort(struct UserAgent *ua);
char *UaGetAuthname(struct UserAgent *ua);


struct Account *UaGetAccount(struct UserAgent *ua);


void UaSetBinded(struct UserAgent *ua);
void UaSetUnbinded(struct UserAgent *ua);
BOOL UaBinded(struct UserAgent *ua);


extern void (*UaMakeCall)(struct UserAgent *ua);
extern void (*UaEndCall)(struct UserAgent *ua);
extern void (*UaAcceptCall)(struct UserAgent *ua);
extern void (*UaRinging)(struct UserAgent *ua);

struct UserAgent *CreateUserAgent(int account);
void DestroyUserAgent(struct UserAgent **ua);

void DumpUa(struct UserAgent *ua);
