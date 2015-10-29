#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include <string.h>
#include "MessageTransport.h"
}

static char Message4ReceivingTest[] = "Receiving test string";

int ReceiveMessageMock(char *message)
{
    strcpy(message, Message4ReceivingTest);
    mock().actualCall("ReceiveMessageMock");
    return 0;
}

int SendMessageMock(char *message)
{
    mock().actualCall("SendMessageMock").withStringParameter("message",message);
    return 0;
}

void InitTransportMock()
{
    AddMessageTransporter((char *)"Mock", SendMessageMock, ReceiveMessageMock);
}

void CleanupTransportMock()
{
    RemoveMessageTransporter((char *)"Mock");
}
