
typedef int (*MessageSender) (char *message);
typedef int (*MessageReceiver) (char *message);
typedef int (*MessageHandler) (char *message);

void AddMessageTransporter(char *name ,MessageSender s, MessageReceiver r);
void InitReceiveMessageCallback(MessageSender h);
void RemoveMessageTransporter(char *name);
void ReceiveMessage(char *message);
void SendMessage(char *message);

















