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
#include "Transaction.h"
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
        invite = DialogBuildInvite(dialog, (char *)"88002");
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
    MESSAGE *ok = BuildResponse(invite, STATUS_CODE_OK);

    UT_PTR_SET(CreateSession, CreateSessionMock);
    mock().expectOneCall("CreateSession");

    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_CLIENT_INVITE);
    InviteDialogReceiveOk(dialog, ok);
    
    DestroyMessage(&ok);
    ClearTransactionManager();
}

TEST(SessionTestGroup, UASCreateSessionTest)
{
    UT_PTR_SET(CreateSession, CreateSessionMock);
    mock().expectOneCall("CreateSession");

    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_SERVER_INVITE);
    DialogOk(dialog);
}

TEST(SessionTestGroup, UACDestroySessionTest)
{
    UT_PTR_SET(CreateSession, CreateSessionMock);
    UT_PTR_SET(DestroySession,DestroySessionMock);

    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_SERVER_INVITE);
 
    mock().expectOneCall("CreateSession");
    DialogOk(dialog);

    mock().expectOneCall("DestroySession");
    DialogTerminate(dialog);    

    ClearTransactionManager();
}

TEST(SessionTestGroup, UASDestroySessionTest)
{
    MESSAGE *ok = BuildResponse(invite, STATUS_CODE_OK);

    UT_PTR_SET(CreateSession, CreateSessionMock);
    UT_PTR_SET(DestroySession,DestroySessionMock);

    DialogNewTransaction(dialog, invite, TRANSACTION_TYPE_CLIENT_INVITE);
    
    mock().expectOneCall("CreateSession");
    InviteDialogReceiveOk(dialog, ok);
    mock().expectOneCall("DestroySession");
    MESSAGE *bye = BuildByeMessage(dialog);
    DialogReceiveBye(dialog, bye);
    DestroyMessage(&ok);
    ClearTransactionManager();
}
