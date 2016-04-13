#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TransportMock.h"

extern "C" {
#include <string.h>
#include "MessageTransport.h"
}

int MessageHandleMock(char *message)
{
    mock().actualCall("MessageHandleMock");
    return 0;
}

TEST_GROUP(MessageTransportTestGroup)
{

    void setup() {
        UT_PTR_SET(Transporter, &MockTransporter);
        UT_PTR_SET(ReceiveMessageCallback, MessageHandleMock);
    }
    void teardown() {
        mock().clear();
    }
};

TEST(MessageTransportTestGroup, ReceiveMessageTest)
{
    char message[64] = {0};
    char Message4ReceivingTest[] = "Receiving test string";


    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue("Receiving test string");
    mock().expectOneCall("MessageHandleMock");

    ReceiveInMessage(message);

    STRCMP_EQUAL(Message4ReceivingTest, message);
    mock().checkExpectations();
}

TEST(MessageTransportTestGroup, SendMessageTest)
{
    char message[32] = "Sending test string";

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", "");
    SendOutMessage(message);
    mock().checkExpectations();
}
