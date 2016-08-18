enum CALL_EVENT {
    CALL_ESTABLISHED,
};

struct UserAgent;

extern void (*NotifyClient)(enum CALL_EVENT event, struct UserAgent *ua);
