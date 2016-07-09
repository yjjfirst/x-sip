#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "AccountMock.h"
#include "TransportMock.h"
#include "TestingMessages.h"

extern "C" {
#include "UserAgentManager.h"
#include "AccountManager.h"
#include "Call.h"
#include "MessageTransport.h"
#include "TransactionManager.h"
#include "Messages.h"
}

TEST_GROUP(CallManagerTestGroup)
{
    void setup() 
    {
        UT_PTR_SET(Transporter, &MockTransporter);
        AccountInitMock();
    }
    
    void teardown()
    {
        ClearAccountManager();
        ClearTransactionManager();
        ClearUserAgentManager();
    }
};

TEST(CallManagerTestGroup, CallOutSendInviteTest)
{
    char dest[] = "88002";
    char account = 0;

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "INVITE");

    CallOut(account, dest);

    mock().checkExpectations();
    mock().clear();
}

TEST(CallManagerTestGroup, CallOutSuccessTest)
{
    char dest[] = "88002";
    char account = 0;

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "INVITE");
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "ACK");

    CallOut(account, dest);
    ReceiveInMessage();

    mock().checkExpectations();
    mock().clear();
    
}
