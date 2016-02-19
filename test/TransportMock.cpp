#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TransportMock.h"

extern "C" {
#include <stdio.h>
#include <string.h>
#include "MessageTransport.h"
#include "Messages.h"
}

int ReceiveInMessageMock(char *message)
{
    strcpy(message, mock().actualCall(RECEIVE_IN_MESSAGE_MOCK).returnStringValue());
    return 0;
}

int SendOutMessageMock(char *message)
{
    return mock().actualCall(SEND_OUT_MESSAGE_MOCK).returnIntValue();
}

struct MessageTransporter MockTransporter = {
    SendOutMessageMock,
    ReceiveInMessageMock,
};
