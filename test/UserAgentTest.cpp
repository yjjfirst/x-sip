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
        ua = BuildUserAgent(0);
        dialog = CreateDialog(NULL_DIALOG_ID, ua);
        DialogSetToUser(dialog, GetUserName(0));
        message = BuildBindingMessage(dialog);
    }

    void DestoryTestingMessage()
    {
        DestoryMessage(&message);
        DestoryUserAgent(&ua);
    }

    void setup()
    {
        UT_PTR_SET(Transporter, &MockTransporter);
        UT_PTR_SET(ReceiveMessageCallback, MessageReceived);
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
    DestoryUserAgent(&ua);
    CHECK(ua == NULL);
}

TEST(UserAgentTestGroup, SetUserNameTest)
{
    struct UserAgent *ua = CreateUserAgent(0);
    
    UserAgentSetUserName(ua, (char *)"User Name");
    STRCMP_EQUAL("User Name", UserAgentGetUserName(ua));

    DestoryUserAgent(&ua);
}

TEST(UserAgentTestGroup, SetVeryLongUserNameTest)
{
    struct UserAgent *ua = CreateUserAgent(0);
    char userName[] = "1234567890123456789012345678901234567890123456789012345678901234asdf";

    UserAgentSetUserName(ua, userName);
    STRCMP_CONTAINS(UserAgentGetUserName(ua), userName);
    CHECK_EQUAL(USER_NAME_MAX_LENGTH - 1, strlen(UserAgentGetUserName(ua)));
    DestoryUserAgent(&ua);
}

TEST(UserAgentTestGroup, SetProxyTest)
{
    struct UserAgent *ua = CreateUserAgent(0);
    
    UserAgentSetProxy(ua, (char *)"proxy.server.domain");

    STRCMP_EQUAL("proxy.server.domain", UserAgentGetProxy(ua));
    DestoryUserAgent(&ua);
}

TEST(UserAgentTestGroup, SetVeryLongProxyTest)
{
    struct UserAgent *ua = CreateUserAgent(0);
    char proxy[] = "1234567890.1234567890.1234567890.1234567890123456789012345678901234";
    
    UserAgentSetProxy(ua, (char *)proxy);

    STRCMP_CONTAINS(UserAgentGetProxy(ua), proxy);
    CHECK_EQUAL(PROXY_MAX_LENGTH - 1, strlen(UserAgentGetProxy(ua)));
    DestoryUserAgent(&ua);
}

TEST(UserAgentTestGroup, SetRegisterTest)
{
    struct UserAgent *ua = CreateUserAgent(0);

    UserAgentSetRegistrar(ua, (char *)"registrar.domain");
    STRCMP_EQUAL("registrar.domain", UserAgentGetRegistrar(ua));

    DestoryUserAgent(&ua);
}

TEST(UserAgentTestGroup, SetVeryLongRegistrarTest)
{
    struct UserAgent *ua = CreateUserAgent(0);
    char registrar[] = "1234567890.1234567890.1234567890.1234567890123456789012345678901234";
    UserAgentSetRegistrar(ua, registrar);
    STRCMP_CONTAINS(UserAgentGetRegistrar(ua), registrar);
    CHECK_EQUAL(REGISTRAR_MAX_LENGTH -1 , strlen(UserAgentGetRegistrar(ua)));

    DestoryUserAgent(&ua);
}

TEST(UserAgentTestGroup, SetAuthNameTest)
{
    struct UserAgent *ua = CreateUserAgent(0);
   
    UserAgentSetAuthName(ua, (char *)"Auth Name");
    STRCMP_EQUAL("Auth Name", UserAgentGetAuthName(ua));

    DestoryUserAgent(&ua);
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

    DestoryUserAgent(&ua);
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
    message = BuildBindingMessage(dialog);
    t = AddClientNonInviteTransaction(message, (struct TransactionUserObserver *)dialog);

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(REMOVE_BINDING_OK_MESSAGE);
    ReceiveInMessage(revMessage);
    CHECK_EQUAL(FALSE, AccountBinded(account));

    DestoryUserAgent(&ua);
    RemoveAllTransaction();
}

TEST(UserAgentTestGroup, AddDialogTest)
{
    ua = BuildUserAgent(0);
    struct DialogId *dialogid = CreateDialogId((char *)"1", (char *)"2",(char *)"3");
    dialog = CreateDialog(dialogid, ua);

    CHECK_TRUE(UserAgentGetDialog(ua, dialogid) != NULL);
    POINTERS_EQUAL(dialog, UserAgentGetDialog(ua, dialogid));

    DestoryUserAgent(&ua);
}

