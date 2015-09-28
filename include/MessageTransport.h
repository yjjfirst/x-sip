
typedef int (*MessageSender) (char *message);
typedef int (*MessageReceiver) (char *message);

void AddMessageTransport(char *name ,MessageSender sender, MessageReceiver receaiver); 
void ReceiveMessage(char *message);










