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
#include "CallManager.h"
#include "Transporter.h"
}

void UaMakeCallMock(struct UserAgent *ua)
{
    mock().actualCall("UaMakeCall");
}

void UaEndCallMock(struct UserAgent *ua)
{
    mock().actualCall("UaEndCall").withParameter("ua", ua);
}

void UaAcceptCallMock(struct UserAgent *ua)
{
    mock().actualCall("UaAcceptCall").withParameter("ua", ua);
}

TEST_GROUP(CallManagerTestGroup)
{
    void setup() 
    {
        UT_PTR_SET(UaMakeCall, UaMakeCallMock);
        UT_PTR_SET(UaEndCall, UaEndCallMock);
        UT_PTR_SET(UaAcceptCall, UaAcceptCallMock);
        UT_PTR_SET(ClientTransporter, &ClientTransporterMock);

        AccountInit();
    }
    
    void teardown()
    {
        ClearAccountManager();
        ClearUaManager();

        mock().checkExpectations();
        mock().clear();
    }
};

TEST(CallManagerTestGroup, CallOutTest)
{
    char dest[] = "88002";
    char account = 0;

    mock().expectOneCall("UaMakeCall");
    CallOut(account, dest);

    CHECK_EQUAL(1, CountUas());
}

TEST(CallManagerTestGroup, CallOutSuccessTest)
{
    char dest[] = "88002";
    char account = 0;

    mock().expectOneCall("UaMakeCall");
    struct UserAgent *ua = CallOut(account, dest);
    
    mock().expectOneCall("SendEventToClient").
        withParameter("message","ua=0;event=CALL_ESTABLISHED\r\n");

    NotifyCm(CALL_ESTABLISHED, ua); 
}

TEST(CallManagerTestGroup, ActiveHangupTest)
{
    char dest[] = "88002";
    char account = 0;

    mock().expectOneCall("UaMakeCall");
    struct UserAgent *ua = CallOut(account, dest);
    
    mock().expectOneCall("SendEventToClient").
        withParameter("message","ua=0;event=CALL_ESTABLISHED\r\n");
    NotifyCm(CALL_ESTABLISHED, ua); 

    mock().expectOneCall("UaEndCall").withParameter("ua", ua);
    EndCall(ua);

}

TEST(CallManagerTestGroup, RemoteRingingNotifyCmTest)
{
    char dest[] = "88002";
    char account = 0;

    mock().expectOneCall("UaMakeCall");
    struct UserAgent *ua = CallOut(account, dest);
    
    mock().expectOneCall("SendEventToClient").
        withParameter("message","ua=0;event=CALL_REMOTE_RINGING\r\n");
    
    NotifyCm(CALL_REMOTE_RINGING, ua);     
}

TEST(CallManagerTestGroup, IncomingCallTest)
{
    struct UserAgent *ua = AddUa(0);

    mock().expectOneCall("SendEventToClient").
        withParameter("message","ua=0;event=CALL_INCOMING\r\n");    
    NotifyCm(CALL_INCOMING, ua);     

    mock().expectOneCall("UaAcceptCall").withParameter("ua", ua);
    AcceptCall(ua);    
}

