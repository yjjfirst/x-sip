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

struct Session *CreateSessionMock()
{
    mock().actualCall("CreateSession");
    return NULL;
}

IGNORE_TEST(SessionTestGroup, CreateSessionStructTest)
{
    struct Session *session = CreateSession();
    CHECK_TRUE(session != NULL);

    DestorySession(&session);
    POINTERS_EQUAL(NULL, session);

    DestoryMessage(&invite);
}

TEST(SessionTestGroup, UACCreateSessionTest)
{
    DialogAddClientInviteTransaction(dialog, invite);

    struct Message *ok = Build200OKMessage(invite);

    UT_PTR_SET(CreateSession, CreateSessionMock);
    mock().expectOneCall("CreateSession");
    DialogClientInviteOkReceived(dialog, ok);
    
    DestoryMessage(&ok);
    RemoveAllTransaction();
}

TEST(SessionTestGroup, UASCreateSessionTest)
{
    DialogAddServerInviteTransaction(dialog, invite);

    UT_PTR_SET(CreateSession, CreateSessionMock);
    mock().expectOneCall("CreateSession");
    DialogSend200OKResponse(dialog);
}







