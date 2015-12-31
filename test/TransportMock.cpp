#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include <stdio.h>
#include <string.h>
#include "MessageTransport.h"
#include "Messages.h"
}

int ReceiveInMessageMock(char *message)
{
    strcpy(message, mock().actualCall("ReceiveInMessageMock").returnStringValue());
    return 0;
}

int SendOutMessageMock(char *message)
{
    return mock().actualCall("SendOutMessageMock").returnIntValue();
}

int SendOutAckMessageMock(char *message)
{
    struct Message *m = CreateMessage();
    char *remoteTag = NULL;

    ParseMessage(message, m);
    remoteTag = MessageGetRemoteTag(m);    
    mock().actualCall("SendOutMessageMock").withParameter("RemoteTag", remoteTag).returnIntValue();

    DestoryMessage(&m);
    return 0;
}

struct MessageTransporter MockTransporter = {
    "Mock",
    SendOutMessageMock,
    ReceiveInMessageMock,
};

struct MessageTransporter MockTransporterForAck = {
    "Mock",
    SendOutAckMessageMock,
    ReceiveInMessageMock,
};
