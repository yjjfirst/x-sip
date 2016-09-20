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

int ReceiveInMessage()
{
    char received[MAX_MESSAGE_LENGTH] = {0};

    if (Transporter->receive == NULL) return -1;
    Transporter->receive(received);
    
    return Transporter->callback(received);
}

int TransporterInit()
{
    Transporter->fd = Transporter->init();
    return Transporter->fd;
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
