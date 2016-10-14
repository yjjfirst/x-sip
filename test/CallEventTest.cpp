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

static void NotifyClientMock(enum CALL_EVENT event, struct UserAgent *ua)
{
    mock().actualCall("NotifyClient").withParameter("event", event).
        withParameter("ua", ua);
}

TEST(CallEventTestGroup, NotifyClientTest)
{
    UT_PTR_SET(NotifyClient, NotifyClientMock);

    AddUserAgent(0);
    AddUserAgent(1);
    AddUserAgent(2);
    AddUserAgent(3);
    
    mock().expectOneCall("NotifyClient").withParameter("event", CALL_INCOMING).
        withParameter("ua", GetUserAgent(0));
    NotifyClient(CALL_INCOMING, GetUserAgent(0));

}

TEST(CallEventTestGroup, BuildClientMessageTest)
{
    char msg[CLIENT_MESSAGE_MAX_LENGTH] = {0};
    int ua = 0;
    enum CALL_EVENT event = CALL_INCOMING;
    
    BuildClientMessage(msg, ua, event);

    STRCMP_EQUAL("ua=0;event=call_incoming\r\n", msg);
}

TEST(CallEventTestGroup, ClientMessageSendTest)
{
    UT_PTR_SET(ClientTransporter, &ClientTransporterMock);
    
    mock().expectOneCall("ClientUdpSend").
        withParameter("message","ua=0;event=call_incoming\r\n").
        withParameter("addr", "192.168.10.1").
        withParameter("port", 5556).
        withParameter("fd", ClientTransporter->fd);
    
    AccountInit();
    struct UserAgent *ua = AddUserAgent(0);

    NotifyClient(CALL_INCOMING, ua);

}

TEST(CallEventTestGroup, SecondUserAgentClientMessageSendTest)
{
    UT_PTR_SET(ClientTransporter, &ClientTransporterMock);
    
    mock().expectOneCall("ClientUdpSend").
        withParameter("message","ua=1;event=call_incoming\r\n").
        withParameter("addr", "192.168.10.1").
        withParameter("port", 5556).
        withParameter("fd", ClientTransporter->fd);
    
    AccountInit();
    AddUserAgent(0);
    struct UserAgent *ua = AddUserAgent(0);

    NotifyClient(CALL_INCOMING, ua);
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
