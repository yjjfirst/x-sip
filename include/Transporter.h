#include "Bool.h"

typedef int (*MessageSend) (char *message);
typedef int (*MessageReceive) (char *message);
typedef int (*MessageHandle) (char *message);
typedef int (*Init) ();

struct MessageTransporter {        
    MessageSend send;
    MessageReceive receive;
    Init init;
    int fd;
};

BOOL ReceiveInMessage();
int SendOutMessage(char *message);
int TransporterInit();
struct MessageTransporter *SetTransporter(struct MessageTransporter *t);
struct MessageTransporter *GetTransporter();

extern MessageHandle ReceiveMessageCallback;
extern struct MessageTransporter *Transporter;
