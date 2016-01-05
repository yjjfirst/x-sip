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

struct MessageTransporter MockTransporter = {
    SendOutMessageMock,
    ReceiveInMessageMock,
};
