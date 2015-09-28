#include <string.h>
#include "MessageTransport.h"
#include "utils/list/include/list.h"

struct MessageTransporter {
    char name[8];
    MessageSender sender;
    MessageReceiver receiver;
};

t_list *MessageTransports;

struct MessageTransporter *CreateMessageTransport(char *name, MessageSender sender, MessageReceiver receiver)
{
    struct MessageTransporter *t = (struct MessageTransporter *)calloc (1, sizeof (struct MessageTransporter));
    strncpy(t->name, name, sizeof(t->name - 1));
    t->sender = sender;
    t->receiver = receiver;
    
    return t;
}

void AddMessageTransport(char *name ,MessageSender sender, MessageReceiver receiver)
{
    struct MessageTransporter *t = CreateMessageTransport(name,sender,receiver);    
    
    put_in_list(&MessageTransports, t);

}

void ReceiveMessage(char *message)
{
    struct MessageTransporter *t = (struct MessageTransporter *)get_data_at(MessageTransports, 0);
    t->receiver(message);

    free(t);
}
