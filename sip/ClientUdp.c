#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "Transporter.h"
#include "ClientUdp.h"
#include "Udp.h"
#include "TransactionManager.h"

#define MAXLINE 80
#define CLIENT_SERV_PORT 5555

int ClientReceiveMessage(char *message, int fd)
{
    return UdpReceive(message, fd);
}

int ClientSendMessage(char *message, char *destaddr, int destport, int fd)
{
    return UdpSend(message, fd);
}

int ClientInit(int port)
{
    return UdpInit(port);
}

BOOL ClientMessageHandle(char *string)
{
    return 1;
}

struct MessageTransporter ClientTransporter = {
    .send = ClientSendMessage,
    .receive = ClientReceiveMessage,
    .init = ClientInit,
    .callback = ClientMessageHandle,
    .port = CLIENT_SERV_PORT
};
