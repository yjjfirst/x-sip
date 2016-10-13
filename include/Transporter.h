#include "Bool.h"

struct Message;

typedef int (*MessageSend) (char *message, char *destaddr, int destport, int fd);
typedef int (*MessageReceive) (char *message, int fd);
typedef int (*MessageHandle) (char *message);
typedef int (*Init) (int port);

struct MessageTransporter {        
    MessageSend send;
    MessageReceive receive;
    Init init;
    MessageHandle callback;
    int fd;
    int port;
};

int ReceiveMessage();
int  SendMessage(struct Message *message);
int InitTransporter(int port);
struct MessageTransporter *SetTransporter(struct MessageTransporter *t);

extern struct MessageTransporter *SipTransporter;
extern struct MessageTransporter *ClientTransporter;
