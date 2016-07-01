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
#include "MessageTransport.h"
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
        dialog = CreateDialog(NULL_DIALOG_ID, ua);
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
        ClearAccount();
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
    char revMessage[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));
    BuildTestingMessage();

    struct Transaction *t = AddClientNonInviteTransaction(message, (struct TransactionUserObserver *)dialog);
    struct Account *account = UserAgentGetAccount(ua);

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE);
    ReceiveInMessage(revMessage);

    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));
    CHECK_EQUAL(TRUE, AccountBinded(account));

    DestroyUserAgent(&ua);
    RemoveAllTransaction();    
}

TEST(UserAgentTestGroup, RemoveBindingTest)
{
    char revMessage[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", MethodMap2String(SIP_METHOD_REGISTER));
    BuildTestingMessage();
    struct Transaction *t = AddClientNonInviteTransaction(message, (struct TransactionUserObserver *)dialog);
    struct Account *account = UserAgentGetAccount(ua);
    
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE);
    ReceiveInMessage(revMessage);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));
    CHECK_EQUAL(TRUE, AccountBinded(account));

    RemoveAllTransaction();

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method",MethodMap2String(SIP_METHOD_REGISTER));
    message = BuildAddBindingMessage(dialog);
    t = AddClientNonInviteTransaction(message, (struct TransactionUserObserver *)dialog);

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(REMOVE_BINDING_OK_MESSAGE);
    ReceiveInMessage(revMessage);
    CHECK_EQUAL(FALSE, AccountBinded(account));

    DestroyUserAgent(&ua);
    RemoveAllTransaction();
}

TEST(UserAgentTestGroup, AddDialogTest)
{
    ua = CreateUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"1", (char *)"2",(char *)"3");
    dialog = CreateDialog(dialogid, ua);

    CHECK_TRUE(UserAgentGetDialog(ua, dialogid) != NULL);
    POINTERS_EQUAL(dialog, UserAgentGetDialog(ua, dialogid));

    DestroyUserAgent(&ua);
}

