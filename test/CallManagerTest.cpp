#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "AccountMock.h"
#include "TransportMock.h"
#include "TestingMessages.h"

extern "C" {
#include <stdio.h>

#include "UserAgentManager.h"
#include "UserAgent.h"
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

        mock().checkExpectations();
        mock().clear();
    }
};

TEST(CallManagerTestGroup, CallOutSendInviteTest)
{
    char dest[] = "88002";
    char account = 0;

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "INVITE");

    CallOut(account, dest);
}

TEST(CallManagerTestGroup, CallOutSuccessTest)
{
    char dest[] = "88002";
    char account = 0;

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "INVITE");
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "ACK");

    struct UserAgent *ua = CallOut(account, dest);
    ReceiveInMessage();

    CHECK_EQUAL(1, CountUserAgent());
    CHECK_EQUAL(1, UserAgentCountDialogs(ua));
}

IGNORE_TEST(CallManagerTestGroup, ActiveHangupTest)
{
    char dest[] = "88002";
    char account = 0;

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "INVITE");
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "ACK");

    struct UserAgent *ua = CallOut(account, dest);
    ReceiveInMessage();

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "BYE");
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(BYE_200OK_MESSAGE);
    EndCall(ua);

    ReceiveInMessage();
    
    CHECK_EQUAL(0, UserAgentCountDialogs(ua));
}
