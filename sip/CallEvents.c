#include <stdio.h>
#include <assert.h>

#include "CallEvents.h"
#include "UserAgentManager.h"
#include "ClientUdp.h"
#include "Transporter.h"

void BuildClientMessage(char *msg, int ua, enum CALL_EVENT event)
{
    assert(msg != NULL);
    
    sprintf(msg, "ua=%d:event=%s", ua, "call_incoming");
}

void NotifyClientImpl(enum CALL_EVENT event, struct UserAgent *ua)
{
    char msg[CLIENT_MESSAGE_MAX_LENGTH] = {0};

    BuildClientMessage(msg, GetUserAgentPosition(ua), event);
    ClientTransporter->send(msg, "192.168.10.1", 5556, ClientTransporter->fd);
}

void (*NotifyClient)(enum CALL_EVENT event, struct UserAgent *ua) = NotifyClientImpl;
