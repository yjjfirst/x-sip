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
        ClearUaManager();
    }
};

TEST(CallEventTestGroup, NotifyCmTest)
{
    UT_PTR_SET(NotifyCm, NotifyCallManagerMock);

    AddUa(0);
    AddUa(1);
    AddUa(2);
    AddUa(3);
    
    mock().expectOneCall("NotifyCm").withParameter("event", CALL_INCOMING).
        withParameter("ua", GetUa(0));
    NotifyCm(CALL_INCOMING, GetUa(0));

}

TEST(CallEventTestGroup, BuildClientMessageTest)
{
    char msg[CLIENT_MESSAGE_MAX_LENGTH] = {0};
    int ua = 0;
    enum CALL_EVENT event = CALL_INCOMING;
    
    BuildClientMessage(msg, ua, event);

    STRCMP_EQUAL("ua=0;event=CALL_INCOMING\r\n", msg);
}

TEST(CallEventTestGroup, SendClientMessageTest)
{
    UT_PTR_SET(ClientTransporter, &ClientTransporterMock);
    
    mock().expectOneCall("SendEventToClient").
        withParameter("message","ua=0;event=CALL_INCOMING\r\n");
    
    AccountInit();
    struct UserAgent *ua = AddUa(0);

    NotifyCm(CALL_INCOMING, ua);
}

TEST(CallEventTestGroup, SecondUserAgentClientMessageSendTest)
{
    UT_PTR_SET(ClientTransporter, &ClientTransporterMock);
    
    mock().expectOneCall("SendEventToClient").
        withParameter("message","ua=1;event=CALL_INCOMING\r\n");
    
    AccountInit();
    AddUa(0);
    struct UserAgent *ua = AddUa(0);

    NotifyCm(CALL_INCOMING, ua);
}

TEST(CallEventTestGroup, ParseClientMessage)
{
    char message[] = "ua=1;event=CALL_INCOMING\r\n";
    struct ClientMessage event;
    
    ParseClientMessage(message, &event);
    CHECK_EQUAL(1, event.ua);
    CHECK_EQUAL(CALL_INCOMING, event.event);
}

TEST(CallEventTestGroup, ParseAnotherClientMessage)
{
    char message[] = "ua=2;event=CALL_ESTABLISHED\r\n";
    struct ClientMessage event;
    
    ParseClientMessage(message, &event);
    CHECK_EQUAL(2, event.ua);
    CHECK_EQUAL(CALL_ESTABLISHED, event.event);

}

TEST(CallEventTestGroup, ParseCallCommandMessage)
{
    char message[] = "ua=3;event=ACCEPT_CALL\r\n";
    struct ClientMessage event;
    
    ParseClientMessage(message, &event);

    CHECK_EQUAL(3, event.ua);
    CHECK_EQUAL(ACCEPT_CALL, event.event);
}

TEST(CallEventTestGroup, MakeCallMessageTest)
{
    char message[] = "ua=4;event=MAKE_CALL;data=88002\r\n";
    struct ClientMessage event;

    ParseClientMessage(message, &event);
    
    CHECK_EQUAL(4, event.ua);
    CHECK_EQUAL(MAKE_CALL, event.event);
    STRCMP_EQUAL("88002", event.data);
}

TEST(CallEventTestGroup, LongDataMessageTest)
{
    char message[] = "ua=4;event=MAKE_CALL;data=0123456789012345678901234567890123456789012345678901234567890123456789\r\n";
    struct ClientMessage event;

    ParseClientMessage(message, &event);
    
    CHECK_EQUAL(4, event.ua);
    CHECK_EQUAL(MAKE_CALL, event.event);
    STRCMP_EQUAL("012345678901234567890123456789012345678901234567890123456789012", event.data);

}
