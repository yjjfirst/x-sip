#include <stdio.h>
#include <string.h>
#include <error.h>
#include <assert.h>

#include "Transporter.h"
#include "TransactionManager.h"
#include "Messages.h"

int DummySend(char *message)
{
    return 0;
}

int DummyReceive(char *message)
{
    return 0;
}

struct MessageTransporter DummyTransporter = {
    .send = DummySend,
    .receive = DummyReceive,
};

int SendOutMessage(char *message)
{
    return Transporter->send(message);
}

BOOL ReceiveInMessage()
{
    char received[MAX_MESSAGE_LENGTH] = {0};
    Transporter->receive(received);
    return ReceiveMessageCallback(received);
}

struct MessageTransporter *SetTransporter(struct MessageTransporter *t)
{
    struct MessageTransporter *pre = Transporter;

    Transporter = t;
    return pre;
}

struct MessageTransporter *GetTransporter()
{
    return Transporter;
}

struct MessageTransporter *Transporter = &DummyTransporter;
MessageHandle ReceiveMessageCallback =  MessageReceived;
