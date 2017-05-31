#define LOCAL_PORT  5060
#define PROXY_PORT  5060
#define REGISTRAR_PORT 5060

struct Message;
struct UserAgent;
struct Dialog;

struct Message *BuildRemoveBindingMessage(char *from, char *to, char *ipaddr, int port);
struct Message *BuildAddBindingMessage(char *from, char *to, char *ipaddr, int port);
struct Message *BuildInviteMessage(char *from, char *to, char *ipaddr, int port);
struct Message *BuildAckMessage(struct Message *invite, char *ipaddr, int port);
struct Message *BuildByeMessage(struct Message *invite, char *ipaddr, int port);
struct Message *BuildAuthorizationMessage(struct Message *challenge, char *user, char *secret, char *ipaddr, int port);
struct Message *BuildAckMessageWithinClientTransaction(struct Message *invite, char *ipaddr, int port);

struct Message *BuildResponse(struct Message *request, int statusCode);
