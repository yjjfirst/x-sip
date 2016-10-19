#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Mock.h"

extern "C" {
#include <assert.h>
    
#include "AccountManager.h"
#include "UserAgentManager.h"
#include "CallEvents.h"
#include "AccountManager.h"
#include "UserAgentManager.h"
#include "Transporter.h"
}

TEST_GROUP(CallEventTestGroup)
{
    void setup()
    {
        AccountInit();
    }
    void teardown()
    {
        mock().checkExpectations();
        mock().clear();

        ClearAccountManager();
        ClearUserAgentManager();
    }
};

TEST(CallEventTestGroup, NotifyClientTest)
{
    UT_PTR_SET(NotifyCallManager, NotifyCallManagerMock);

    AddUserAgent(0);
    AddUserAgent(1);
    AddUserAgent(2);
    AddUserAgent(3);
    
    mock().expectOneCall("NotifyClient").withParameter("event", CALL_INCOMING).
        withParameter("ua", GetUserAgent(0));
    NotifyCallManager(CALL_INCOMING, GetUserAgent(0));

}

TEST(CallEventTestGroup, BuildClientMessageTest)
{
    char msg[CLIENT_MESSAGE_MAX_LENGTH] = {0};
    int ua = 0;
    enum CALL_EVENT event = CALL_INCOMING;
    
    BuildClientMessage(msg, ua, event);

    STRCMP_EQUAL("ua=0;event=call_incoming\r\n", msg);
}

TEST(CallEventTestGroup, SendClientMessageTest)
{
    UT_PTR_SET(ClientTransporter, &ClientTransporterMock);
    
    mock().expectOneCall("SendEventToClient").
        withParameter("message","ua=0;event=call_incoming\r\n");
    
    AccountInit();
    struct UserAgent *ua = AddUserAgent(0);

    NotifyCallManager(CALL_INCOMING, ua);
}

TEST(CallEventTestGroup, SecondUserAgentClientMessageSendTest)
{
    UT_PTR_SET(ClientTransporter, &ClientTransporterMock);
    
    mock().expectOneCall("SendEventToClient").
        withParameter("message","ua=1;event=call_incoming\r\n");
    
    AccountInit();
    AddUserAgent(0);
    struct UserAgent *ua = AddUserAgent(0);

    NotifyCallManager(CALL_INCOMING, ua);
}

TEST(CallEventTestGroup, ParseClientMessage)
{
    char message[] = "ua=1;event=call_incoming\r\n";
    struct ClientEvent event;
    
    ParseClientMessage(message, &event);
    CHECK_EQUAL(1, event.ua);
    CHECK_EQUAL(CALL_INCOMING, event.event);
}

TEST(CallEventTestGroup, ParseAnotherClientMessage)
{
    char message[] = "ua=2;event=call_established\r\n";
    struct ClientEvent event;
    
    ParseClientMessage(message, &event);
    CHECK_EQUAL(2, event.ua);
    CHECK_EQUAL(CALL_ESTABLISHED, event.event);

}

TEST(CallEventTestGroup, ParseCallCommandMessage)
{
    char message[] = "ua=3;event=accept_call\r\n";
    struct ClientEvent event;
    
    ParseClientMessage(message, &event);

    CHECK_EQUAL(3, event.ua);
    CHECK_EQUAL(ACCEPT_CALL, event.event);
}
