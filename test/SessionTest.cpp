#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "Session.h"
#include "Dialog.h"
#include "Messages.h"
#include "MessageBuilder.h"
#include "UserAgentManager.h"
#include "UserAgent.h"
#include "TransactionManager.h"
}

TEST_GROUP(SessionTestGroup)
{
    struct Dialog *dialog;
    struct Message *invite;
    struct UserAgent *ua;

    void setup() 
    {
        ua = BuildUserAgent();    
        dialog = CreateDialog(NULL_DIALOG_ID, ua);
        DialogSetToUser(dialog, (char *)"88002");
        invite = BuildInviteMessage(dialog);
    }

    void teardown()
    {
        DestoryUserAgent(&ua);

        mock().checkExpectations();
        mock().clear();
    }
};

static struct Session *CreateSessionMock()
{
    mock().actualCall("CreateSession");
    return NULL;
}

void DestorySessionMock(struct Session **session)
{
    mock().actualCall("DestorySession");
}

TEST(SessionTestGroup, CreateSessionStructTest)
{
    struct Session *session = CreateSession();
    CHECK_TRUE(session != NULL);

    DestorySession(&session);
    POINTERS_EQUAL(NULL, session);

    DestoryMessage(&invite);
}

TEST(SessionTestGroup, UACCreateSessionTest)
{
    struct Message *ok = Build200OKMessage(invite);

    UT_PTR_SET(CreateSession, CreateSessionMock);
    mock().expectOneCall("CreateSession");

    DialogAddClientInviteTransaction(dialog, invite);
    DialogClientInviteOkReceived(dialog, ok);
    
    DestoryMessage(&ok);
    RemoveAllTransaction();
}

TEST(SessionTestGroup, UASCreateSessionTest)
{
    UT_PTR_SET(CreateSession, CreateSessionMock);
    mock().expectOneCall("CreateSession");

    DialogAddServerInviteTransaction(dialog, invite);
    DialogSend200OKResponse(dialog);
}

TEST(SessionTestGroup, UACDestorySessionTest)
{
    UT_PTR_SET(CreateSession, CreateSessionMock);
    UT_PTR_SET(DestorySession,DestorySessionMock);

    DialogAddServerInviteTransaction(dialog, invite);
 
    mock().expectOneCall("CreateSession");
    DialogSend200OKResponse(dialog);

    mock().expectOneCall("DestorySession");
    DialogTerminate(dialog);    

    RemoveAllTransaction();
}

TEST(SessionTestGroup, UASDestorySessionTest)
{
    struct Message *ok = Build200OKMessage(invite);

    UT_PTR_SET(CreateSession, CreateSessionMock);
    UT_PTR_SET(DestorySession,DestorySessionMock);

    DialogAddClientInviteTransaction(dialog, invite);

    mock().expectOneCall("CreateSession");
    DialogClientInviteOkReceived(dialog, ok);

    mock().expectOneCall("DestorySession");
    struct Message *bye = BuildByeMessage(dialog);
    DialogReceiveBye(dialog, bye);

    DestoryMessage(&ok);
    RemoveAllTransaction();
}
