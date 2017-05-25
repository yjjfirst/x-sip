#define CLIENT_MESSAGE_MAX_LENGTH 64

enum CALL_EVENT {
    CALL_INCOMING,
    CALL_ESTABLISHED,
    CALL_REMOTE_RINGING,
    CALL_FINISHED,
    CALL_PEER_CANCELED,
    
    ACCEPT_CALL,
    MAKE_CALL,
    RINGING,
};

struct ClientMessage {
    int ua;
    int event;
    char data[64];
};
struct UserAgent;

void BuildClientMessage(char *msg, int ua, enum CALL_EVENT event, char *data);
void ParseClientMessage(char *msg, struct ClientMessage *event);

extern void (*NotifyCm)(int event, struct UserAgent *ua);
