#include <stdio.h>
#include <assert.h>

#include "UserAgentManager.h"
#include "UserAgent.h"
#include "CallEvents.h"
#include "UserAgentManager.h"
#include "ClientUdp.h"
#include "Transporter.h"
#include "Maps.h"
#include "Parameter.h"
#include "utils/StringExt.h"

struct IntStringMap CallEventMap[] = {
    {CALL_INCOMING, "call_incoming"},
    {CALL_ESTABLISHED, "call_established"},
    {CALL_REMOTE_RINGING,"call_remote_ringing"},

    {ACCEPT_CALL, "accept_call"},
    {-1, ""},
};

struct UserAgent *CallOut(int account, char *dest)
{
    struct UserAgent *ua = AddUserAgent(account);
    UaMakeCall(ua);

    return ua;
}

void EndCall(struct UserAgent *ua)
{
    UaEndCall(ua);
}

void AcceptCall(struct UserAgent *ua)
{
    UaAcceptCall(ua);
}

BOOL HandleClientMessage(char *string)
{
    struct UserAgent *ua = GetUserAgent(3);
    AcceptCall(ua);
    
    return 1;
}

void BuildClientMessage(char *msg, int ua, enum CALL_EVENT event)
{
    assert(msg != NULL);
    
    sprintf(msg, "ua=%d;event=%s\r\n", ua, IntMap2String(event, CallEventMap));
}

void ParseClientMessage(char *msg, struct ClientEvent *event)
{
    assert (msg != NULL);
    assert (event != NULL);

    RemoveTailingCRLF(msg);
    struct Parameters *ps = CreateParameters();
    ParseParametersExt(msg, ps);
    

    event->event = StringMap2Int(GetParameter(ps, "event"), CallEventMap);
    event->ua = atoi(GetParameter(ps, "ua"));    

    DestroyParameters(&ps);
}

void NotifyClientImpl(int event, struct UserAgent *ua)
{
    char msg[CLIENT_MESSAGE_MAX_LENGTH] = {0};

    BuildClientMessage(msg, GetUaPosition(ua), event);
    ClientTransporter->send(msg, "192.168.10.1", 5556, ClientTransporter->fd);
}

void (*NotifyCallManager)(int event, struct UserAgent *ua) = NotifyClientImpl;
