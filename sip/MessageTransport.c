#include <stdio.h>
#include <string.h>
#include <error.h>
#include <assert.h>

#include "MessageTransport.h"
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
    .sender = DummySend,
    .receiver = DummyReceive,
};

int SendOutMessage(char *message)
{
    return Transporter->sender(message);
}

BOOL ReceiveInMessage()
{
    char received[MAX_MESSAGE_LENGTH] = {0};
    Transporter->receiver(received);
    return ReceiveMessageCallback(received);
}

struct MessageTransporter *Transporter = &DummyTransporter;
MessageHandler ReceiveMessageCallback =  MessageReceived;
