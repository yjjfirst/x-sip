#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TransportMock.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "URI.h"
#include "RequestLine.h"
#include "Header.h"
#include "UserAgent.h"
#include "Accounts.h"
#include "UserAgentManager.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "ContactHeader.h"
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
        ua = BuildUserAgent();
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
    }

    void teardown()
    {
        mock().clear();
    }    
};

TEST(UserAgentTestGroup, CreateUserAgentTest)
{
    struct UserAgent *ua = CreateUserAgent();

    CHECK_TRUE(ua != NULL);
    DestoryUserAgent(&ua);
    CHECK(ua == NULL);
}

TEST(UserAgentTestGroup, SetUserNameTest)
{
    struct UserAgent *ua = CreateUserAgent();
    
    UserAgentSetUserName(ua, (char *)"User Name");
    STRCMP_EQUAL("User Name", UserAgentGetUserName(ua));

    DestoryUserAgent(&ua);
}

TEST(UserAgentTestGroup, SetVeryLongUserNameTest)
{
    struct UserAgent *ua = CreateUserAgent();
    char userName[] = "1234567890123456789012345678901234567890123456789012345678901234asdf";

    UserAgentSetUserName(ua, userName);
    STRCMP_CONTAINS(UserAgentGetUserName(ua), userName);
    CHECK_EQUAL(USER_NAME_MAX_LENGTH - 1, strlen(UserAgentGetUserName(ua)));
    DestoryUserAgent(&ua);
}

TEST(UserAgentTestGroup, SetProxyTest)
{
    struct UserAgent *ua = CreateUserAgent();
    
    UserAgentSetProxy(ua, (char *)"proxy.server.domain");

    STRCMP_EQUAL("proxy.server.domain", UserAgentGetProxy(ua));
    DestoryUserAgent(&ua);
}

TEST(UserAgentTestGroup, SetVeryLongProxyTest)
{
    struct UserAgent *ua = CreateUserAgent();
    char proxy[] = "1234567890.1234567890.1234567890.1234567890123456789012345678901234";
    
    UserAgentSetProxy(ua, (char *)proxy);

    STRCMP_CONTAINS(UserAgentGetProxy(ua), proxy);
    CHECK_EQUAL(PROXY_MAX_LENGTH - 1, strlen(UserAgentGetProxy(ua)));
    DestoryUserAgent(&ua);
}

TEST(UserAgentTestGroup, SetRegisterTest)
{
    struct UserAgent *ua = CreateUserAgent();

    UserAgentSetRegistrar(ua, (char *)"registrar.domain");
    STRCMP_EQUAL("registrar.domain", UserAgentGetRegistrar(ua));

    DestoryUserAgent(&ua);
}

TEST(UserAgentTestGroup, SetVeryLongRegistrarTest)
{
    struct UserAgent *ua = CreateUserAgent();
    char registrar[] = "1234567890.1234567890.1234567890.1234567890123456789012345678901234";
    UserAgentSetRegistrar(ua, registrar);
    STRCMP_CONTAINS(UserAgentGetRegistrar(ua), registrar);
    CHECK_EQUAL(REGISTRAR_MAX_LENGTH -1 , strlen(UserAgentGetRegistrar(ua)));

    DestoryUserAgent(&ua);
}

TEST(UserAgentTestGroup, SetAuthNameTest)
{
    struct UserAgent *ua = CreateUserAgent();
   
    UserAgentSetAuthName(ua, (char *)"Auth Name");
    STRCMP_EQUAL("Auth Name", UserAgentGetAuthName(ua));

    DestoryUserAgent(&ua);
}

TEST(UserAgentTestGroup, BindingsRequestLineTest)
{
    BuildTestingMessage();

    struct RequestLine *rl = MessageGetRequestLine(message);
    STRCMP_EQUAL("REGISTER", RequestLineGetMethodName(rl));
    STRCMP_EQUAL("SIP/2.0", RequestLineGetSipVersion(rl));

    struct URI *uri = RequestLineGetUri(rl);
    STRCMP_EQUAL(URI_SCHEME_SIP,  UriGetScheme(uri));
    STRCMP_EQUAL(GetProxy(0), UriGetHost(uri));

    DestoryTestingMessage();
}

TEST(UserAgentTestGroup, BindingsToHeaderTest)
{
    BuildTestingMessage();

    struct ContactHeader *to = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, message); 
    struct URI *uri = ContactHeaderGetUri(to);

    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));

    DestoryTestingMessage();
}

TEST(UserAgentTestGroup, BindingsFromHeaderTest)
{
    BuildTestingMessage();

    struct ContactHeader *from = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, message); 
    struct URI *uri = ContactHeaderGetUri(from);

    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));
    DestoryTestingMessage();
}

TEST(UserAgentTestGroup, BindingsContactHeaderTest)
{
    BuildTestingMessage();

    struct ContactHeader *contact = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_CONTACT, message); 
    struct URI *uri = ContactHeaderGetUri(contact);

    STRCMP_EQUAL(GetUserName(0), UriGetUser(uri));

    DestoryTestingMessage();
}

TEST(UserAgentTestGroup, BindingTest)
{
    char revMessage[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    BuildTestingMessage();
    struct Transaction *t = AddClientNonInviteTransaction(message, (struct TransactionUserNotifiers *)dialog);
    
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE);
    ReceiveInMessage(revMessage);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));
    CHECK_EQUAL(TRUE, UserAgentBinded(ua));

    DestoryUserAgent(&ua);
    EmptyTransactionManager();    
}

TEST(UserAgentTestGroup, RemoveBindingTest)
{
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);

    char revMessage[MAX_MESSAGE_LENGTH] = {0};
    BuildTestingMessage();
    struct Transaction *t = AddClientNonInviteTransaction(message, (struct TransactionUserNotifiers *)dialog);
    
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE);
    ReceiveInMessage(revMessage);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));
    CHECK_EQUAL(TRUE, UserAgentBinded(ua));

    EmptyTransactionManager();

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK);
    message = BuildBindingMessage(dialog);
    t = AddClientNonInviteTransaction(message, (struct TransactionUserNotifiers *)dialog);

    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(REMOVE_BINDING_OK_MESSAGE);
    ReceiveInMessage(revMessage);
    CHECK_EQUAL(FALSE, UserAgentBinded(ua));

    DestoryUserAgent(&ua);
    EmptyTransactionManager();
}

TEST(UserAgentTestGroup, AddDialogTest)
{
    ua = BuildUserAgent();
    struct DialogId *dialogid = CreateDialogId((char *)"1", (char *)"2",(char *)"3");
    dialog = CreateDialog(dialogid, ua);

    CHECK_TRUE(UserAgentGetDialog(ua, dialogid) != NULL);
    POINTERS_EQUAL(dialog, UserAgentGetDialog(ua, dialogid));

    DestoryUserAgent(&ua);
}

