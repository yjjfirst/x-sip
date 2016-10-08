#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Mock.h"

extern "C" {
#include <stdio.h>
    
#include "Session.h"
#include "Dialog.h"
#include "Messages.h"
#include "MessageBuilder.h"
#include "UserAgentManager.h"
#include "UserAgent.h"
#include "TransactionManager.h"
#include "AccountManager.h"
#include "DialogManager.h"
#include "Transporter.h"
}

TEST_GROUP(SessionTestGroup)
{
    struct Dialog *dialog;
    MESSAGE *invite;
    struct UserAgent *ua;

    void setup() 
    {
        UT_PTR_SET(SipTransporter, &DummyTransporter);

        AccountInit();
        ua = CreateUserAgent(0);    
        dialog = AddDialog(NULL_DIALOG_ID, ua);
        invite = BuildInviteMessage(dialog, (char *)"88002");
    }

    void teardown()
    {
        ClearAccountManager();
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
    MESSAGE *ok = Build200OkMessage(NULL, invite);

    UT_PTR_SET(CreateSession, CreateSessionMock);
    mock().expectOneCall("CreateSession");

    DialogAddClientInviteTransaction(dialog, invite);
    ClientReceiveOk(dialog, ok);
    
    DestroyMessage(&ok);
    ClearTransactionManager();
}

TEST(SessionTestGroup, UASCreateSessionTest)
{
    UT_PTR_SET(CreateSession, CreateSessionMock);
    mock().expectOneCall("CreateSession");

    DialogAddServerInviteTransaction(dialog, invite);
    DialogOk(dialog);
}

TEST(SessionTestGroup, UACDestroySessionTest)
{
    UT_PTR_SET(CreateSession, CreateSessionMock);
    UT_PTR_SET(DestroySession,DestroySessionMock);

    DialogAddServerInviteTransaction(dialog, invite);
 
    mock().expectOneCall("CreateSession");
    DialogOk(dialog);

    mock().expectOneCall("DestroySession");
    DialogTerminate(dialog);    

    ClearTransactionManager();
}

TEST(SessionTestGroup, UASDestroySessionTest)
{
    MESSAGE *ok = Build200OkMessage(NULL, invite);

    UT_PTR_SET(CreateSession, CreateSessionMock);
    UT_PTR_SET(DestroySession,DestroySessionMock);

    DialogAddClientInviteTransaction(dialog, invite);
    
    mock().expectOneCall("CreateSession");
    ClientReceiveOk(dialog, ok);
    mock().expectOneCall("DestroySession");
    MESSAGE *bye = BuildByeMessage(dialog);
    DialogReceiveBye(dialog, bye);
    DestroyMessage(&ok);
    ClearTransactionManager();
}
