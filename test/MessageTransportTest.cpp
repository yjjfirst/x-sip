#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TransportMock.h"

extern "C" {
#include <string.h>
#include "MessageTransport.h"
}

static char Message4ReceivingTest[] = "Receiving test string";

int MessageHandleMock(char *message)
{
    mock().actualCall("MessageHandleMock");
    return 0;
}

void InitTransportMock()
{
    AddMessageTransporter((char *)"Mock", SendMessageMock, ReceiveMessageMock);
    UT_PTR_SET(ReceiveMessageCallback, MessageHandleMock);
}

void CleanupTransportMock()
{
    RemoveMessageTransporter((char *)"Mock");
}


TEST_GROUP(MessageTransportTestGroup)
{

    void setup() {
        RemoveMessageTransporter((char *)"TRANS");
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

    mock().expectOneCall("ReceiveMessageMock").andReturnValue("Receiving test string");
    mock().expectOneCall("MessageHandleMock");

    ReceiveMessage(message);

    STRCMP_EQUAL(Message4ReceivingTest, message);
    mock().checkExpectations();
}

TEST(MessageTransportTestGroup, SendMessageTest)
{
    char message[32] = "Sending test string";

    mock().expectOneCall("SendMessageMock");
    SendMessage(message);
    mock().checkExpectations();
}
