enum CALL_EVENT {
    CALL_ESTABLISHED,
    CALL_REMOTE_RINGING,
};

struct UserAgent;

extern void (*NotifyClient)(enum CALL_EVENT event, struct UserAgent *ua);
