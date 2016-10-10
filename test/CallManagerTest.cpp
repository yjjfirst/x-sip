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
#include "DialogManager.h"
}

TEST_GROUP(CallManagerTestGroup)
{
    void setup() 
    {
        UT_PTR_SET(SipTransporter, &MockTransporter);
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

static void NotifyClientMock(enum CALL_EVENT event, struct UserAgent *ua)
{
    mock().actualCall("NotifyClient").withParameter("event", event);
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
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(OK_MESSAGE_RECEIVED);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "ACK");

    CallOut(account, dest);
    ReceiveInMessage();

    CHECK_EQUAL(1, CountUserAgent());
    CHECK_EQUAL(1, CountDialogs());
}
#define BYE_200OK_MESSAGE (char *)"\
SIP/2.0 200 OK\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5061;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5061\r\n\
From: <sip:88001@192.168.10.62>;tag=1069855717\r\n\
To: <sip:88002@192.168.10.62>;tag=as6151ad25\r\n\
Call-ID: 97295390\r\n\
CSeq: 20 BYE\r\n\
Contact: <sip:88002@192.168.10.62:5060>\r\n\
Content-Type: application/sdp\r\n\
Content-Length: 289\r\n"

TEST(CallManagerTestGroup, ActiveHangupTest)
{
    char dest[] = "88002";
    char account = 0;

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "INVITE");
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(OK_MESSAGE_RECEIVED);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "ACK");

    struct UserAgent *ua = CallOut(account, dest);
    ReceiveInMessage();

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "BYE");
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(BYE_200OK_MESSAGE);
    EndCall(ua);

    ReceiveInMessage();
    
    CHECK_EQUAL(0, CountDialogs());
}

TEST(CallManagerTestGroup, CallEstablishedNotifyClientTest)
{
    char dest[] = "88002";
    char account = 0;

    UT_PTR_SET(NotifyClient, NotifyClientMock);
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "INVITE");
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(OK_MESSAGE_RECEIVED);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "ACK");
    
    CallOut(account, dest);

    mock().expectOneCall("NotifyClient").withParameter("event", CALL_ESTABLISHED);

    ReceiveInMessage();
}


TEST(CallManagerTestGroup, RemoteRingingNotifyClientTest)
{
    char dest[] = "88002";
    char account = 0;

    UT_PTR_SET(NotifyClient, NotifyClientMock);
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "INVITE");
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(RINGING_MESSAGE_RECEIVED);
    
    CallOut(account, dest);

    mock().expectOneCall("NotifyClient").withParameter("event", CALL_REMOTE_RINGING);
    
    ReceiveInMessage();
}

TEST(CallManagerTestGroup, IncomingCallTest)
{
    UT_PTR_SET(NotifyClient, NotifyClientMock);
    
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall("NotifyClient").
        withParameter("event", CALL_INCOMING);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("StatusCode", 100);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("StatusCode", 200);

    
    ReceiveInMessage();
    struct UserAgent *ua = GetUserAgent(0);
    AcceptCall(ua);
    
    ClearUserAgentManager();    
}

