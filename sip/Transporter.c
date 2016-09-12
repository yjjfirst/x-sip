#include <stdio.h>
#include <string.h>
#include <error.h>
#include <assert.h>

#include "Transporter.h"
#include "TransactionManager.h"
#include "Messages.h"
#include "Udp.h"

int SendOutMessage(char *message)
{
    if (Transporter->send == NULL) return -1;
    
    return Transporter->send(message);
}

BOOL ReceiveInMessage()
{
    char received[MAX_MESSAGE_LENGTH] = {0};
    if (Transporter->receive == NULL) return FALSE;

    Transporter->receive(received);
    return ReceiveMessageCallback(received);
}

void TransporterInit()
{
    Transporter->init();
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

struct MessageTransporter *Transporter = &UdpTransporter;
MessageHandle ReceiveMessageCallback =  MessageReceived;
