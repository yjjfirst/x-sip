#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Mock.h"

extern "C" {
#include <string.h>
#include "Transporter.h"
#include "TransactionManager.h"
#include "SipUdp.h"
}

TEST_GROUP(MessageTransportTestGroup)
{
    void teardown() {
        mock().clear();
    }
};

TEST(MessageTransportTestGroup, ReceiveMessageTest)
{
    UT_PTR_SET(SipTransporter, &MockTransporterAndHandle);

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue("Receiving test string");
    mock().expectOneCall("MessageHandleMock");

    ReceiveMessage();

    mock().checkExpectations();
}


struct MessageTransporter NullTransporter = {
    NULL,
    NULL,
    NULL,
    SipMessageHandle
};

TEST(MessageTransportTestGroup, NullReceiveTest)
{
    UT_PTR_SET(SipTransporter, &NullTransporter);
    CHECK_EQUAL(-1, ReceiveMessage());
    mock().checkExpectations();
}

TEST(MessageTransportTestGroup, InitTest)
{
    int result = 102;
    UT_PTR_SET(SipTransporter, &MockTransporter);

    mock().expectOneCall("InitMock").withParameter("port", SipTransporter->port).andReturnValue(result);
    
    CHECK_EQUAL(result, TransporterInit(SipTransporter->port));
    CHECK_EQUAL(result, SipTransporter->fd);
    
    mock().checkExpectations();
}
