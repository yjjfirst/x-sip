#include <stdio.h>
#include <string.h>
#include <error.h>
#include <assert.h>

#include "MessageTransport.h"

int DummySend(char *message)
{
    return 0;
}

int DummyReceive(char *message)
{
    return 0;
}

struct MessageTransporter DummyTransporter = {
    .name = "Dummy",
    .sender = DummySend,
    .receiver = DummyReceive,
};

int SendOutMessage(char *message)
{
    return Transporter->sender(message);
}

BOOL ReceiveInMessage(char *message)
{
    Transporter->receiver(message);
    return ReceiveMessageCallback(message);
}

int ReceiveMessageCallbackImpl (char *message) 
{
    return 0;
}

struct MessageTransporter *Transporter = &DummyTransporter;
MessageHandler ReceiveMessageCallback =  ReceiveMessageCallbackImpl;
