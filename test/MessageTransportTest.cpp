#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include <string.h>
#include "MessageTransport.h"
}

char Message4ReceivingTest[] = "Receiving test string";

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

TEST_GROUP(MessageTransportTestGroup)
{

    void setup() {
        AddMessageTransporter((char *)"Mock", SendMessageMock, ReceiveMessageMock);
    }
    void teardown() {
        RemoveMessageTransporter((char *)"Mock");
        mock().clear();
    }
};

TEST(MessageTransportTestGroup, ReceiveMessageTest)
{
    char message[64] = {0};

    mock().expectOneCall("ReceiveMessageMock");
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
