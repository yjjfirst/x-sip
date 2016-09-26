#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "Transporter.h"
#include "SipUdp.h"
#include "Udp.h"
#include "TransactionManager.h"

#define MAXLINE 80
#define SIP_SERV_PORT 5060

int SipUdpReceiveMessage(char *message, int fd)
{
    return UdpReceive(message, fd);
}

int SipUdpSendMessage(char *message, char *destaddr, int destport, int fd)
{
    return UdpSend(message, fd);
}

int SipUdpInit(int port)
{
    return UdpInit(port);
}

        
struct MessageTransporter SipUdpTransporter = {
    .send = SipUdpSendMessage,
    .receive = SipUdpReceiveMessage,
    .init = SipUdpInit,
    .callback = SipMessageHandle,
    .port = SIP_SERV_PORT,
};

