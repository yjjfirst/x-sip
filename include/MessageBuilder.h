#define LOCAL_PORT  5060
#define PROXY_PORT  5060
#define REGISTRAR_PORT 5060

struct Message;
struct UserAgent;
struct Dialog;

struct Message *BuildRemoveBindingMessage(struct Dialog *dialog);
struct Message *BuildAddBindingMessage(struct Dialog *dialog);
struct Message *BuildInviteMessage(struct Dialog *dialog);
struct Message *BuildAckMessage(struct Dialog *dialog);
struct Message *BuildByeMessage(struct Dialog *dialog);
struct Message *BuildAuthorizationMessage(struct Dialog *dialog, struct Message *challenge);

struct Message *BuildTryingMessage(struct Message *invite);
struct Message *BuildRingingMessage(struct Message *invite);
struct Message *Build200OkMessage(struct Message *invite);
struct Message *Build301Message(struct Message *invite);
struct Message *BuildAckMessageWithinClientTransaction(struct Message *invite);
