#define CLIENT_MESSAGE_MAX_LENGTH 64

enum CALL_EVENT {
    CALL_INCOMING,
    CALL_ESTABLISHED,
    CALL_REMOTE_RINGING,
};

struct UserAgent;

void BuildClientMessage(char *msg, int ua, enum CALL_EVENT event);

extern void (*NotifyClient)(enum CALL_EVENT event, struct UserAgent *ua);
