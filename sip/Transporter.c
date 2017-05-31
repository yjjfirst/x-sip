#include <stdio.h>
#include <string.h>
#include <error.h>
#include <assert.h>

#include "Transporter.h"
#include "TransactionManager.h"
#include "Messages.h"
#include "SipUdp.h"
#include "ClientUdp.h"

struct ReceivedInfo {
    char buf[MAX_MESSAGE_LENGTH];
    char peerIpAddr[16];
    int peerPort;
};

int ReceiveMessage()
{
    char received[MAX_MESSAGE_LENGTH] = {0};
    struct ReceivedInfo info;
    bzero(&info, sizeof(struct ReceivedInfo));
    
    if (SipTransporter->receive == NULL) return -1;
    SipTransporter->receive(received, SipTransporter->fd);
    
    return SipTransporter->callback(received,"192.168.10.62", 5060);
}

int SendMessageImpl(MESSAGE *message)
{
    if (SipTransporter->send == NULL) return -1;

    char s[MAX_MESSAGE_LENGTH] = {0};
    Message2String(s, message);

    return SipTransporter->send(s,
                                GetMessageAddr(message),
                                GetMessagePort(message),
                                SipTransporter->fd);
}
int (*SendMessage)(MESSAGE *message) = SendMessageImpl;

int InitTransporter(int port)
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
