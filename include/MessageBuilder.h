
#define PROXY_IPADDR  "192.168.10.62"
#define REGISTRAR_IPADDR "192.168.10.62"
#define LOCAL_IPADDR  "192.168.10.1"
#define LOCAL_PORT  5060
#define PROXY_PORT  5060
#define REGISTRAR_PORT 5060
#define USER_NAME "88001"

struct Message;
struct UserAgent;

void MessageAddViaParameter(struct Message *message, char *name, char *value);
void MessageSetCSeqMethod (struct Message *message, char *method);

struct Message *BuildBindingMessage(struct UserAgent *ua);
struct Message *BuildInviteMessage(struct UserAgent *ua, char *to);
