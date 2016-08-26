#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Mock.h"
#include "TestingMessages.h"

extern "C" {
#include <stdio.h>
#include <assert.h>
    
#include "CallEvents.h"
#include "UserAgentManager.h"
#include "UserAgent.h"
#include "AccountManager.h"
#include "Call.h"
#include "Transporter.h"
#include "TransactionManager.h"
#include "Messages.h"
#include "ViaHeader.h"
}

TEST_GROUP(CallManagerTestGroup)
{
    void setup() 
    {
        UT_PTR_SET(Transporter, &MockTransporter);
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);

        AccountInit();
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

void NotifyClientMock(enum CALL_EVENT event, struct UserAgent *ua)
{
    mock().actualCall("NotifyClient").withParameter("event", event)
        .withParameter("UserAgent", ua);
}

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

TEST(CallManagerTestGroup, ActiveHangupTest)
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

TEST(CallManagerTestGroup, CallEstablishedNotifyClientTest)
{
    char dest[] = "88002";
    char account = 0;

    UT_PTR_SET(NotifyClient, NotifyClientMock);
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "INVITE");
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_200OK_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "ACK");
    
    struct UserAgent *ua = CallOut(account, dest);

    mock().expectOneCall("NotifyClient").withParameter("event", CALL_ESTABLISHED)
        .withParameter("UserAgent", ua);

    ReceiveInMessage();
}


TEST(CallManagerTestGroup, RemoteRingingNotifyClientTest)
{
    char dest[] = "88002";
    char account = 0;

    UT_PTR_SET(NotifyClient, NotifyClientMock);
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "INVITE");
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INVITE_180RINGING_MESSAGE);
    
    struct UserAgent *ua = CallOut(account, dest);

    mock().expectOneCall("NotifyClient").withParameter("event", CALL_REMOTE_RINGING)
        .withParameter("UserAgent", ua);

    ReceiveInMessage();
}
