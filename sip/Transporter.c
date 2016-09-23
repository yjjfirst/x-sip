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
    if (SipTransporter->send == NULL) return -1;
    
    return SipTransporter->send(message, SipTransporter->fd);
}

int ReceiveInMessage()
{
    char received[MAX_MESSAGE_LENGTH] = {0};

    if (SipTransporter->receive == NULL) return -1;
    SipTransporter->receive(received, SipTransporter->fd);
    
    return SipTransporter->callback(received);
}

int TransporterInit()
{
    SipTransporter->fd = SipTransporter->init();
    return SipTransporter->fd;
}

struct MessageTransporter *SetTransporter(struct MessageTransporter *t)
{
    struct MessageTransporter *pre = SipTransporter;

    SipTransporter = t;
    return pre;
}

struct MessageTransporter *SipTransporter = &SipUdpTransporter;
