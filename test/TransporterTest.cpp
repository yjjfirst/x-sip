#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Mock.h"

extern "C" {
#include <string.h>
#include "Transporter.h"
#include "TransactionManager.h"
}

TEST_GROUP(MessageTransportTestGroup)
{
    void teardown() {
        mock().clear();
    }
};

TEST(MessageTransportTestGroup, ReceiveMessageTest)
{
    UT_PTR_SET(Transporter, &MockTransporterAndHandle);

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue("Receiving test string");
    mock().expectOneCall("MessageHandleMock");

    ReceiveInMessage();

    mock().checkExpectations();
}

TEST(MessageTransportTestGroup, SendMessageTest)
{
    char message[32] = "Sending test string";

    UT_PTR_SET(Transporter, &MockTransporterAndHandle);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", "");
    SendOutMessage(message);
    mock().checkExpectations();
}

TEST(MessageTransportTestGroup, SetTransporterTest)
{
    struct MessageTransporter *pre = SetTransporter(&MockTransporter);
                   
    POINTERS_EQUAL(&MockTransporter, GetTransporter());
    POINTERS_EQUAL(&MockTransporter, SetTransporter(pre));   
}

struct MessageTransporter NullTransporter = {
    NULL,
    NULL,
    NULL,
    SipMessageHandle
};

TEST(MessageTransportTestGroup, NullReceiveTest)
{
    UT_PTR_SET(Transporter, &NullTransporter);
    CHECK_EQUAL(-1, ReceiveInMessage());
    mock().checkExpectations();
}

TEST(MessageTransportTestGroup, NullSendTest)
{
    char message[32] = "Sending test string";

    UT_PTR_SET(Transporter, &NullTransporter);
    CHECK_EQUAL(-1, SendOutMessage(message));
    mock().checkExpectations();
}

TEST(MessageTransportTestGroup, InitTest)
{
    int result = 102;
    UT_PTR_SET(Transporter, &MockTransporter);

    mock().expectOneCall("InitMock").andReturnValue(result);
    
    CHECK_EQUAL(result, TransporterInit());
    CHECK_EQUAL(result, Transporter->fd);
    
    mock().checkExpectations();
}
