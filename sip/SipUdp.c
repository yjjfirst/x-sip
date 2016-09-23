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
#define SERV_PORT 5060
#define CLIENT_SERV_PORT 5555

int SipUdpReceiveMessage(char *message, int fd)
{
    return UdpReceive(message, fd);
}

int SipUdpSendMessage(char *message, int fd)
{
    return UdpSend(message, fd);
}

int SipUdpInit()
{
    return UdpInit(SERV_PORT);
}

        
struct MessageTransporter SipUdpTransporter = {
    .send = SipUdpSendMessage,
    .receive = SipUdpReceiveMessage,
    .init = SipUdpInit,
    .callback = SipMessageHandle
};

