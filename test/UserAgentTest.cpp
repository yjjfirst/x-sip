#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TransportMock.h"
#include "AccountMock.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "AccountManager.h"
#include "UserAgent.h"
#include "Accounts.h"
#include "UserAgentManager.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "TransactionManager.h"
#include "Transaction.h"
#include "Transporter.h"
#include "TestingMessages.h"
#include "DialogId.h"
#include "Dialog.h"
#include "Provision.h"
}

TEST_GROUP(UserAgentTestGroup)
{    
    struct UserAgent *ua;
    struct Dialog *dialog;
    struct Message *message;

    void BuildTestingMessage()
    {
        ua = CreateUserAgent(0);
        dialog = AddNewDialog(NULL_DIALOG_ID, ua);
        DialogSetToUser(dialog, GetUserName(0));
        message = BuildAddBindingMessage(dialog);
    }

    void DestroyTestingMessage()
    {
        DestroyMessage(&message);
        DestroyUserAgent(&ua);
    }

    void setup()
    {
        UT_PTR_SET(Transporter, &MockTransporter);
        AccountInitMock();
    }

    void teardown()
    {
        ClearAccountManager();
        mock().clear();
    }    
};

TEST(UserAgentTestGroup, CreateUserAgentTest)
{
    struct UserAgent *ua = CreateUserAgent(0);

    CHECK_TRUE(ua != NULL);
    DestroyUserAgent(&ua);
    CHECK(ua == NULL);
}

TEST(UserAgentTestGroup, BindingTest)
{

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));
    BuildTestingMessage();

    struct Transaction *t = AddClientNonInviteTransaction(message, (struct TransactionUserObserver *)dialog);
    struct Account *account = UserAgentGetAccount(ua);

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE);
    ReceiveInMessage();

    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));
    CHECK_EQUAL(TRUE, AccountBinded(account));

    DestroyUserAgent(&ua);
    ClearTransactionManager();    
}

TEST(UserAgentTestGroup, RemoveBindingTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));
    BuildTestingMessage();
    struct Transaction *t = AddClientNonInviteTransaction(message, (struct TransactionUserObserver *)dialog);
    struct Account *account = UserAgentGetAccount(ua);
    
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE);
    ReceiveInMessage();
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));
    CHECK_EQUAL(TRUE, AccountBinded(account));

    ClearTransactionManager();

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method",MethodMap2String(SIP_METHOD_REGISTER));
    message = BuildAddBindingMessage(dialog);
    t = AddClientNonInviteTransaction(message, (struct TransactionUserObserver *)dialog);

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(REMOVE_BINDING_OK_MESSAGE);
    ReceiveInMessage();
    CHECK_EQUAL(FALSE, AccountBinded(account));

    DestroyUserAgent(&ua);
    ClearTransactionManager();
}

TEST(UserAgentTestGroup, AddDialogTest)
{
    ua = CreateUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"1", (char *)"2",(char *)"3");
    dialog = AddNewDialog(dialogid, ua);

    CHECK_TRUE(UserAgentGetDialogById(ua, dialogid) != NULL);
    POINTERS_EQUAL(dialog, UserAgentGetDialogById(ua, dialogid));

    DestroyUserAgent(&ua);
}

TEST(UserAgentTestGroup, CountDialogTest)
{
    ua = AddUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"1", (char *)"2",(char *)"3");
    dialog = AddNewDialog(dialogid, ua);

    CHECK_EQUAL(1, UserAgentCountDialogs(ua));

    ClearUserAgentManager();
    
}

TEST(UserAgentTestGroup, GetDialogManagerTest)
{
    ua = AddUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"1", (char *)"2",(char *)"3");
    dialog = AddNewDialog(dialogid, ua);

    CHECK_FALSE(UserAgentGetDialogManager(ua) == NULL);

    ClearUserAgentManager();
}

TEST(UserAgentTestGroup, GetDialogTest)
{
    ua = AddUserAgent(0);
    dialog = AddNewDialog(NULL_DIALOG_ID, ua);
    
    POINTERS_EQUAL(dialog, UserAgentGetDialog(ua, 0));
    ClearUserAgentManager();
}
