#include <stdio.h>
#include <string.h>
#include "MessageTransport.h"
#include "utils/list/include/list.h"

struct MessageTransporter {
    char name[8];
    MessageSender sender;
    MessageReceiver receiver;
};

t_list *MessageTransports;
MessageHandler ReceiveMessageCallback;

void InitReceiveMessageCallback(MessageHandler h)
{
    ReceiveMessageCallback = h;
}

struct MessageTransporter *CreateMessageTransport(char *name, MessageSender s, MessageReceiver r)
{
    struct MessageTransporter *t = calloc (1, sizeof (struct MessageTransporter));
    strncpy(t->name, name, sizeof(t->name - 1));
    t->sender = s;
    t->receiver = r;
    
    return t;
}

void AddMessageTransporter(char *name ,MessageSender s, MessageReceiver r)
{
    struct MessageTransporter *t = CreateMessageTransport(name, s, r);    
    
    put_in_list(&MessageTransports, t);

}

void RemoveMessageTransporter(char *name)
{
    struct MessageTransporter *t =get_data_at(MessageTransports, 0);
    free(t);
    del_node_at(&MessageTransports, 0);
}

struct MessageTransporter *GetTransporterAt(int pos)
{
    return  (struct MessageTransporter *)get_data_at(MessageTransports, pos);
}

void ReceiveMessage(char *message)
{
    GetTransporterAt(0)->receiver(message);
    ReceiveMessageCallback(message);
}

void SendMessage(char *message)
{
    GetTransporterAt(0)->sender(message);
}
