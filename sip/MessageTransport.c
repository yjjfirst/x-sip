#include <stdio.h>
#include <string.h>
#include <error.h>
#include <assert.h>

#include "MessageTransport.h"
#include "TransactionManager.h"

int DummySend(char *message)
{
    return 0;
}

int DummyReceive(char *message)
{
    return 0;
}

struct MessageTransporter DummyTransporter = {
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

struct MessageTransporter *Transporter = &DummyTransporter;
MessageHandler ReceiveMessageCallback =  MessageReceived;
