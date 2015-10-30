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

int MessageHandleMock(char *message)
{
    mock().actualCall("MessageHandleMock");
    return 0;
}

void InitTransportMock()
{
    AddMessageTransporter((char *)"Mock", SendMessageMock, ReceiveMessageMock);
    InitReceiveMessageCallback(MessageHandleMock);
}

void CleanupTransportMock()
{
    RemoveMessageTransporter((char *)"Mock");
}


TEST_GROUP(MessageTransportTestGroup)
{

    void setup() {
        InitTransportMock();
    }
    void teardown() {
        CleanupTransportMock();
        mock().clear();
    }
};

TEST(MessageTransportTestGroup, ReceiveMessageTest)
{
    char message[64] = {0};

    mock().expectOneCall("ReceiveMessageMock");
    mock().expectOneCall("MessageHandleMock");

    ReceiveMessage(message);

    STRCMP_EQUAL(Message4ReceivingTest, message);
    mock().checkExpectations();
}

TEST(MessageTransportTestGroup, SendMessageTest)
{
    char message[32] = "Sending test string";

    mock().expectOneCall("SendMessageMock").withStringParameter("message", message);
    SendMessage(message);
    mock().checkExpectations();
}
