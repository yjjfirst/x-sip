#define LOCAL_PORT  5060
#define PROXY_PORT  5060
#define REGISTRAR_PORT 5060

struct Message;
struct UserAgent;
struct Dialog;

struct Message *BuildRemoveBindingMessage(char *ipaddr, int port);
struct Message *BuildAddBindingMessage(char *from, char *to, char *ipaddr, int port);
struct Message *BuildInviteMessage(char *to);
struct Message *BuildAckMessage();
struct Message *BuildByeMessage();
struct Message *BuildAuthorizationMessage(struct Message *challenge, char *user, char *secret);
struct Message *BuildAckMessageWithinClientTransaction(struct Message *invite);

struct Message *BuildResponse(struct Message *request, int statusCode);
