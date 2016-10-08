#include "Bool.h"

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

int ReceiveInMessage();
int SendOutMessage(char *message, char *destaddr, int port);
int TransporterInit(int port);
struct MessageTransporter *SetTransporter(struct MessageTransporter *t);

extern struct MessageTransporter *SipTransporter;
extern struct MessageTransporter *ClientTransporter;
