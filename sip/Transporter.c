#include <stdio.h>
#include <string.h>
#include <error.h>
#include <assert.h>

#include "Transporter.h"
#include "TransactionManager.h"
#include "Messages.h"
#include "SipUdp.h"
#include "ClientUdp.h"

int SendOutMessage(char *message, char *destaddr, int port)
{
    if (SipTransporter->send == NULL) return -1;

    return SipTransporter->send(message, destaddr, port, SipTransporter->fd);
}

int ReceiveInMessage()
{
    char received[MAX_MESSAGE_LENGTH] = {0};

    if (SipTransporter->receive == NULL) return -1;
    SipTransporter->receive(received, SipTransporter->fd);
    
    return SipTransporter->callback(received);
}

int TransporterInit(int port)
{
    SipTransporter->fd = SipTransporter->init(port);
    return SipTransporter->fd;
}

struct MessageTransporter *SetTransporter(struct MessageTransporter *t)
{
    struct MessageTransporter *pre = SipTransporter;

    SipTransporter = t;
    return pre;
}

struct MessageTransporter *SipTransporter = &SipUdpTransporter;
struct MessageTransporter *ClientTransporter = &ClientUdpTransporter;
