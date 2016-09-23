#include "Bool.h"

typedef int (*MessageSend) (char *message, int fd);
typedef int (*MessageReceive) (char *message, int fd);
typedef int (*MessageHandle) (char *message);
typedef int (*Init) ();

struct MessageTransporter {        
    MessageSend send;
    MessageReceive receive;
    Init init;
    MessageHandle callback;
    int fd;
};

int ReceiveInMessage();
int SendOutMessage(char *message);
int TransporterInit();
struct MessageTransporter *SetTransporter(struct MessageTransporter *t);
extern struct MessageTransporter *SipTransporter;
