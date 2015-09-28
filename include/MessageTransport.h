
typedef int (*MessageSender) (char *message);
typedef int (*MessageReceiver) (char *message);

void AddMessageTransporter(char *name ,MessageSender sender, MessageReceiver receaiver);
void RemoveMessageTransporter(char *name);
void ReceiveMessage(char *message);
void SendMessage(char *message);










