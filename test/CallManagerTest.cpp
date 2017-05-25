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

void UaMakeCallMock(struct UserAgent *ua, char *dest)
{
    mock().actualCall("UaMakeCall").withParameter("dest", dest);
}

void UaEndCallMock(struct UserAgent *ua)
{
    mock().actualCall("UaEndCall").withParameter("ua", ua);
}

void UaAcceptCallMock(struct UserAgent *ua)
{
    mock().actualCall("UaAcceptCall").withParameter("ua", ua);
}

void UaRingingMock(struct UserAgent *ua)
{
    mock().actualCall("UaRinging").withParameter("ua", ua);
}

TEST_GROUP(CallManagerTestGroup)
{
    void setup() 
    {
        UT_PTR_SET(UaMakeCall, UaMakeCallMock);
        UT_PTR_SET(UaEndCall, UaEndCallMock);
        UT_PTR_SET(UaAcceptCall, UaAcceptCallMock);
        UT_PTR_SET(ClientTransporter, &ClientTransporterMock);
        UT_PTR_SET(UaRinging, UaRingingMock);

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

    mock().expectOneCall("UaMakeCall").withParameter("dest", dest);
    CallOut(account, dest);
        
    CHECK_EQUAL(1, CountUas());
}

TEST(CallManagerTestGroup, CallOutSuccessTest)
{
    char dest[] = "88002";
    char account = 0;

    mock().expectOneCall("UaMakeCall").withParameter("dest", dest);
    struct UserAgent *ua = CallOut(account, dest);
    
    mock().expectOneCall("SendEventToClient").
        withParameter("message","ua=0;event=CALL_ESTABLISHED\r\n");

    NotifyCm(CALL_ESTABLISHED, ua); 
}

TEST(CallManagerTestGroup, ActiveHangupTest)
{
    char dest[] = "88002";
    char account = 0;

    mock().expectOneCall("UaMakeCall").withParameter("dest", dest);
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

    mock().expectOneCall("UaMakeCall").withParameter("dest", dest);
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

TEST(CallManagerTestGroup, CancelCallTest)
{
    struct UserAgent *ua = AddUa(0);

    mock().expectOneCall("SendEventToClient").
        withParameter("message","ua=0;event=CALL_PEER_CANCELED\r\n");    

    NotifyCm(CALL_PEER_CANCELED, ua);
}

TEST(CallManagerTestGroup, ClientAcceptCallMessageTest)
{
    char buf[CLIENT_MESSAGE_MAX_LENGTH] = "ua=0;event=ACCEPT_CALL";
    char buf1[CLIENT_MESSAGE_MAX_LENGTH] = "ua=1;event=ACCEPT_CALL";
    char buf2[CLIENT_MESSAGE_MAX_LENGTH] = "ua=2;event=ACCEPT_CALL";
    
    struct UserAgent *ua = AddUa(0);
    struct UserAgent *ua1 = AddUa(1);
    struct UserAgent *ua2 = AddUa(0);
    
    mock().expectOneCall("UaAcceptCall").withParameter("ua", ua);
    HandleClientMessage(buf, (char *)"192.168.10.62", 5060);

    mock().expectOneCall("UaAcceptCall").withParameter("ua", ua1);
    HandleClientMessage(buf1, (char *)"192.168.10.62", 5060);

    mock().expectOneCall("UaAcceptCall").withParameter("ua", ua2);
    HandleClientMessage(buf2, (char *)"192.168.10.62", 5060);
}

TEST(CallManagerTestGroup, ClientRingingMessageTest)
{
    char buf[CLIENT_MESSAGE_MAX_LENGTH] = "ua=0;event=RINGING";
    char buf1[CLIENT_MESSAGE_MAX_LENGTH] = "ua=1;event=RINGING";
    char buf2[CLIENT_MESSAGE_MAX_LENGTH] = "ua=2;event=RINGING";
    
    struct UserAgent *ua = AddUa(0);
    struct UserAgent *ua1 = AddUa(1);
    struct UserAgent *ua2 = AddUa(0);
    
    mock().expectOneCall("UaRinging").withParameter("ua", ua);
    HandleClientMessage(buf, (char *)"192.168.10.62", 5060);

    mock().expectOneCall("UaRinging").withParameter("ua", ua1);
    HandleClientMessage(buf1, (char *)"192.168.10.62", 5060);

    mock().expectOneCall("UaRinging").withParameter("ua", ua2);
    HandleClientMessage(buf2, (char *)"192.168.10.62", 5060);

}

TEST(CallManagerTestGroup, ClientMakecallMessageTest)
{
    char buf[CLIENT_MESSAGE_MAX_LENGTH] = "ua=0;event=MAKE_CALL;data=88002";
    char buf1[CLIENT_MESSAGE_MAX_LENGTH] = "ua=1;event=MAKE_CALL;data=88002";
    char buf2[CLIENT_MESSAGE_MAX_LENGTH] = "ua=2;event=MAKE_CALL;data=88002";
    
    
    mock().expectOneCall("UaMakeCall").withParameter("dest", "88002");
    HandleClientMessage(buf, (char *)"192.168.10.62", 5060);

    mock().expectOneCall("UaMakeCall").withParameter("dest", "88002");
    HandleClientMessage(buf1, (char *)"192.168.10.62", 5060);

    mock().expectOneCall("UaMakeCall").withParameter("dest", "88002");
    HandleClientMessage(buf2, (char *)"192.168.10.62", 5060);
}
