#include <stdio.h>
#include <string.h>
#include <error.h>
#include <assert.h>

#include "Transporter.h"
#include "TransactionManager.h"
#include "Messages.h"
#include "SipUdp.h"
#include "ClientUdp.h"

int ReceiveMessage()
{
    char received[MAX_MESSAGE_LENGTH] = {0};

    if (SipTransporter->receive == NULL) return -1;
    SipTransporter->receive(received, SipTransporter->fd);
    
    return SipTransporter->callback(received);
}

int SendMessage(MESSAGE *message)
{
    if (SipTransporter->send == NULL) return -1;

    char s[MAX_MESSAGE_LENGTH] = {0};
    Message2String(s, message);

    return SipTransporter->send(s,
                                MessageGetDestAddr(message),
                                MessageGetDestPort(message),
                                SipTransporter->fd);
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
