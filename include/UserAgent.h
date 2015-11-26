#define USER_NAME_MAX_LENGTH 64
#define PROXY_MAX_LENGTH 64
#define REGISTRAR_MAX_LENGTH 64
#define AUTH_NAME_MAX_LENGTH 64

struct UserAgent;

void UserAgentSetUserName(struct UserAgent *ua, char *name);
char *UserAgentGetUserName(struct UserAgent *ua);
void UserAgentSetProxy(struct UserAgent *ua, char *proxy);
char *UserAgentGetProxy(struct UserAgent *ua);
void UserAgentSetRegistrar(struct UserAgent *ua, char *registrar);
char *UserAgentGetRegistrar(struct UserAgent *ua);
void UserAgentSetAuthName(struct UserAgent *ua, char *authName);
char *UserAgentGetAuthName(struct UserAgent *ua);

void UserAgentAddBindings(struct UserAgent *ua);

struct UserAgent *CreateUserAgent();
void DestoryUserAgent(struct UserAgent **ua);
