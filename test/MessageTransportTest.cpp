#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TransportMock.h"

extern "C" {
#include <string.h>
#include "MessageTransport.h"
}

static char Message4ReceivingTest[] = "Receiving test string";


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
