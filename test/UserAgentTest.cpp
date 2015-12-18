#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "Header.h"
#include "UserAgent.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "ContactHeader.h"
#include "TransactionManager.h"
#include "Transaction.h"
#include "MessageTransport.h"
#include "TestingMessages.h"
#include "DialogId.h"
#include "Dialog.h"
}

static int ReceiveMessageMock(char *message)
{
    strcpy(message, mock().actualCall("ReceiveMessageMock").returnStringValue());
    return 0;
}

static int SendMessageMock(char *message)
{
    return 0;
}


static struct Timer *AddTimer(void *p, int ms, TimerCallback onTime) 
{
    return NULL;
}

static void RemoveTimer(struct Timer *timer)
{
}

TEST_GROUP(UserAgentTestGroup)
{    
    struct UserAgent *BuildUserAgent()
    {
        struct UserAgent *ua = CreateUserAgent();
        struct TimerManager *tm = GetTimerManager(AddTimer, RemoveTimer);

        (void)tm;
        UserAgentSetUserName(ua, (char *)"88001");
        UserAgentSetRegistrar(ua, (char *)"192.168.10.63");
        UserAgentSetProxy(ua, (char *)"192.168.10.63");

        return ua;
    }
    void setup()
    {
        AddMessageTransporter((char *)"TRANS", SendMessageMock, ReceiveMessageMock);
        InitReceiveMessageCallback(MessageReceived);
        TransactionSetTimerManager(AddTimer);
    }

    void teardown()
    {
        RemoveMessageTransporter((char *)"TRANS");
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
    struct UserAgent *ua = BuildUserAgent();
    struct Dialog *dialog = CreateDialog(NULL, ua);
    struct Message *message = NULL;
        
    message = BuildBindingMessage(dialog);

    struct RequestLine *rl = MessageGetRequestLine(message);
    STRCMP_EQUAL("REGISTER", RequestLineGetMethod(rl));
    STRCMP_EQUAL("SIP/2.0", RequestLineGetSipVersion(rl));

    struct URI *uri = RequestLineGetUri(rl);
    STRCMP_EQUAL(URI_SCHEME_SIP,  UriGetScheme(uri));
    STRCMP_EQUAL("192.168.10.63", UriGetHost(uri));

    DestoryMessage(&message);
    DestoryUserAgent(&ua);
    DestoryDialog(&dialog);
}

TEST(UserAgentTestGroup, BindingsToHeaderTest)
{
    struct UserAgent *ua = BuildUserAgent();
    struct Dialog *dialog = CreateDialog(NULL, ua);
    struct Message *message = BuildBindingMessage(dialog);

    struct ContactHeader *to = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, message); 
    struct URI *uri = ContactHeaderGetUri(to);

    STRCMP_EQUAL("88001", UriGetUser(uri));
    DestoryMessage(&message);
    DestoryUserAgent(&ua);
    DestoryDialog(&dialog);
}

TEST(UserAgentTestGroup, BindingsFromHeaderTest)
{
    struct UserAgent *ua = BuildUserAgent();
    struct Dialog *dialog = CreateDialog(NULL, ua);
    struct Message *message = BuildBindingMessage(dialog);

    struct ContactHeader *from = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, message); 
    struct URI *uri = ContactHeaderGetUri(from);

    STRCMP_EQUAL("88001", UriGetUser(uri));
    DestoryMessage(&message);
    DestoryUserAgent(&ua);
    DestoryDialog(&dialog);
}

TEST(UserAgentTestGroup, BindingsContactHeaderTest)
{
    struct UserAgent *ua = BuildUserAgent();
    struct Dialog *dialog = CreateDialog(NULL, ua);
    struct Message *message = BuildBindingMessage(dialog);

    struct ContactHeader *contact = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_CONTACT, message); 
    struct URI *uri = ContactHeaderGetUri(contact);

    STRCMP_EQUAL("88001", UriGetUser(uri));
    DestoryMessage(&message);
    DestoryUserAgent(&ua);
    DestoryDialog(&dialog);
}

TEST(UserAgentTestGroup, BindingTest)
{
    char revMessage[MAX_MESSAGE_LENGTH] = {0};
    struct UserAgent *ua = BuildUserAgent();
    struct Dialog *dialog = CreateDialog(NULL, ua);
    struct Message *message = BuildBindingMessage(dialog);
    struct Transaction *t = CreateTransactionExt(message, (struct TransactionOwnerInterface *)ua);
    
    mock().expectOneCall("ReceiveMessageMock").andReturnValue(ADD_BINDING_MESSAGE);
    ReceiveMessage(revMessage);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));
    CHECK_EQUAL(TRUE, UserAgentBinded(ua));

    DestoryUserAgent(&ua);
    DestoryDialog(&dialog);
    DestoryTransactionManager();
    
}

TEST(UserAgentTestGroup, RemoveBindingTest)
{
    char revMessage[MAX_MESSAGE_LENGTH] = {0};
    struct UserAgent *ua = BuildUserAgent();
    struct Dialog *dialog = CreateDialog(NULL, ua);
    struct Message *message = BuildBindingMessage(dialog);
    struct Transaction *t = CreateTransactionExt(message, (struct TransactionOwnerInterface *)ua);
    
    mock().expectOneCall("ReceiveMessageMock").andReturnValue(ADD_BINDING_MESSAGE);
    ReceiveMessage(revMessage);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));
    CHECK_EQUAL(TRUE, UserAgentBinded(ua));

    DestoryTransactionManager();

    message = BuildBindingMessage(dialog);
    t = CreateTransactionExt(message, (struct TransactionOwnerInterface *)ua);

    mock().expectOneCall("ReceiveMessageMock").andReturnValue(REMOVE_BINDING_MESSAGE);
    ReceiveMessage(revMessage);
    CHECK_EQUAL(FALSE, UserAgentBinded(ua));

    DestoryUserAgent(&ua);
    DestoryDialog(&dialog);
    DestoryTransactionManager();
}

TEST(UserAgentTestGroup, AddDialogTest)
{
    struct UserAgent *ua = BuildUserAgent();
    struct DialogId *dialogid = CreateDialogId((char *)"1", (char *)"2",(char *)"3");
    struct Dialog *dialog = CreateDialog(dialogid, ua);

    UserAgentAddDialog(ua, dialog);
    CHECK_TRUE(UserAgentGetDialog(ua, dialogid) != NULL);
    POINTERS_EQUAL(dialog, UserAgentGetDialog(ua, dialogid));

    DestoryUserAgent(&ua);
}

TEST(UserAgentTestGroup, InviteSucceedTest)
{
    char revMessage[MAX_MESSAGE_LENGTH] = {0};
    struct UserAgent *ua = BuildUserAgent();
    struct Dialog *dialog = CreateDialog(NULL, ua);
    struct Message *message = BuildInviteMessage(dialog);
    struct DialogId *dialogid = CreateDialogId((char *)"97295390",(char *)"1296642367",(char *)"as6151ad25");
    CreateTransactionExt(message, (struct TransactionOwnerInterface *)ua);
    mock().expectOneCall("ReceiveMessageMock").andReturnValue(INVITE_200OK_MESSAGE);    
    
    ReceiveMessage(revMessage);
    CHECK_TRUE(UserAgentGetDialog(ua, dialogid) != NULL);
  
    DestoryDialogId(&dialogid);
    DestoryDialog(&dialog);
    DestoryUserAgent(&ua);
    DestoryTransactionManager();
    mock().checkExpectations();
} 

