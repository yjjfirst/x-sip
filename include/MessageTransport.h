#include "Bool.h"

typedef int (*MessageSender) (char *message);
typedef int (*MessageReceiver) (char *message);
typedef int (*MessageHandler) (char *message);

struct MessageTransporter {
    char name[8];
    MessageSender sender;
    MessageReceiver receiver;
};

void AddMessageTransporter(char *name ,MessageSender s, MessageReceiver r);
void RemoveMessageTransporter(char *name);
BOOL ReceiveMessage(char *message);
BOOL ReceiveInMessage(char *message);
int SendOutMessage(char *message);
int SendMessage(char *message);

extern MessageHandler ReceiveMessageCallback;
extern struct MessageTransporter *Transporter;
