#include "Bool.h"

typedef int (*MessageSend) (char *message);
typedef int (*MessageReceive) (char *message);
typedef int (*MessageHandle) (char *message);

struct MessageTransporter {
    MessageSend send;
    MessageReceive receive;
};

BOOL ReceiveInMessage();
int SendOutMessage(char *message);
struct MessageTransporter *SetTransporter(struct MessageTransporter *t);
struct MessageTransporter *GetTransporter();

extern MessageHandle ReceiveMessageCallback;
extern struct MessageTransporter *Transporter;
