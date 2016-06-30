#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "AccountMock.h"

extern "C" {
#include "Session.h"
#include "Dialog.h"
#include "Messages.h"
#include "MessageBuilder.h"
#include "UserAgentManager.h"
#include "UserAgent.h"
#include "TransactionManager.h"
#include "AccountManager.h"
}

TEST_GROUP(SessionTestGroup)
{
    struct Dialog *dialog;
    struct Message *invite;
    struct UserAgent *ua;

    void setup() 
    {
        AccountInitMock();
        ua = CreateUserAgent(0);    
        dialog = CreateDialog(NULL_DIALOG_ID, ua);
        DialogSetToUser(dialog, (char *)"88002");
        invite = BuildInviteMessage(dialog);
    }

    void teardown()
    {
        ClearAccount();
        DestroyUserAgent(&ua);
        
        mock().checkExpectations();
        mock().clear();
    }
};

static struct Session *CreateSessionMock()
{
    mock().actualCall("CreateSession");
    return NULL;
}

void DestroySessionMock(struct Session **session)
{
    mock().actualCall("DestroySession");
}

TEST(SessionTestGroup, CreateSessionStructTest)
{
    struct Session *session = CreateSession();
    CHECK_TRUE(session != NULL);

    DestroySession(&session);
    POINTERS_EQUAL(NULL, session);

    DestroyMessage(&invite);
}

TEST(SessionTestGroup, UACCreateSessionTest)
{
    struct Message *ok = Build200OKMessage(invite);

    UT_PTR_SET(CreateSession, CreateSessionMock);
    mock().expectOneCall("CreateSession");

    DialogAddClientInviteTransaction(dialog, invite);
    DialogClientInviteOkReceived(dialog, ok);
    
    DestroyMessage(&ok);
    RemoveAllTransaction();
}

TEST(SessionTestGroup, UASCreateSessionTest)
{
    UT_PTR_SET(CreateSession, CreateSessionMock);
    mock().expectOneCall("CreateSession");

    DialogAddServerInviteTransaction(dialog, invite);
    DialogSend200OKResponse(dialog);
}

TEST(SessionTestGroup, UACDestroySessionTest)
{
    UT_PTR_SET(CreateSession, CreateSessionMock);
    UT_PTR_SET(DestroySession,DestroySessionMock);

    DialogAddServerInviteTransaction(dialog, invite);
 
    mock().expectOneCall("CreateSession");
    DialogSend200OKResponse(dialog);

    mock().expectOneCall("DestroySession");
    DialogTerminate(dialog);    

    RemoveAllTransaction();
}

TEST(SessionTestGroup, UASDestroySessionTest)
{
    struct Message *ok = Build200OKMessage(invite);

    UT_PTR_SET(CreateSession, CreateSessionMock);
    UT_PTR_SET(DestroySession,DestroySessionMock);

    DialogAddClientInviteTransaction(dialog, invite);

    mock().expectOneCall("CreateSession");
    DialogClientInviteOkReceived(dialog, ok);

    mock().expectOneCall("DestroySession");
    struct Message *bye = BuildByeMessage(dialog);
    DialogReceiveBye(dialog, bye);

    DestroyMessage(&ok);
    RemoveAllTransaction();
}
