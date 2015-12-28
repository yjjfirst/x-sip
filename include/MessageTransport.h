#include "Bool.h"

typedef int (*MessageSender) (char *message);
typedef int (*MessageReceiver) (char *message);
typedef int (*MessageHandler) (char *message);

void AddMessageTransporter(char *name ,MessageSender s, MessageReceiver r);
void RemoveMessageTransporter(char *name);
BOOL ReceiveMessage(char *message);
int SendMessage(char *message);
extern MessageHandler ReceiveMessageCallback;
