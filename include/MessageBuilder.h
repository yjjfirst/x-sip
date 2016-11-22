#define LOCAL_PORT  5060
#define PROXY_PORT  5060
#define REGISTRAR_PORT 5060

struct Message;
struct UserAgent;
struct Dialog;

struct Message *BuildRemoveBindingMessage(struct Dialog *dialog);
struct Message *BuildAddBindingMessage(struct Dialog *dialog);
struct Message *BuildInviteMessage(struct Dialog *dialog, char *to);
struct Message *BuildAckMessage(struct Dialog *dialog);
struct Message *BuildByeMessage(struct Dialog *dialog);
struct Message *BuildAuthorizationMessage(struct Dialog *dialog, struct Message *challenge);
struct Message *BuildAckMessageWithinClientTransaction(struct Message *invite);

struct Message *BuildResponse(struct Message *request, int statusCode);
