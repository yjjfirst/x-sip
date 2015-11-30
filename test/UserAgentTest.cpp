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
}

int ReceiveMessageAddBindings(char *message)
{
    strcpy(message, "SIP/2.0 200 OK\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5060;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5060\r\n\
From: <sip:88001@192.168.10.62>;tag=1225432999\r\n\
To: <sip:88001@192.168.10.62>;tag=as1d07559a\r\n\
Call-ID: 1222971951\r\n\
CSeq: 1 REGISTER\r\n\
Expires: 3600\r\n\
Contact: <sip:88001@192.168.10.1;line=f2fd53ebfa7728f>;expires=3600\r\n\
Content-Length: 0\r\n");

    return 0;
}

int SendMessageAddBindings(char *message)
{
    return 0;
}

TEST_GROUP(UserAgentTestGroup)
{
    struct UserAgent *BuildUserAgent()
    {
        struct UserAgent *ua = CreateUserAgent();

        UserAgentSetUserName(ua, (char *)"88002");
        UserAgentSetRegistrar(ua, (char *)"192.168.10.63");
        UserAgentSetProxy(ua, (char *)"192.168.10.63");

        return ua;
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
    struct Message *message = NULL;
        
    message = BuildRegisterMessage(ua);

    struct RequestLine *rl = MessageGetRequest(message);
    STRCMP_EQUAL("REGISTER", RequestLineGetMethod(rl));
    STRCMP_EQUAL("SIP/2.0", RequestLineGetSipVersion(rl));

    struct URI *uri = RequestLineGetUri(rl);
    STRCMP_EQUAL(URI_SCHEME_SIP,  UriGetScheme(uri));
    STRCMP_EQUAL("192.168.10.63", UriGetHost(uri));

    DestoryMessage(&message);
    DestoryUserAgent(&ua);
}

TEST(UserAgentTestGroup, BindingsToHeaderTest)
{
    struct UserAgent *ua = BuildUserAgent();
    struct Message *message = BuildRegisterMessage(ua);

    struct ContactHeader *to = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_TO, message); 
    struct URI *uri = ContactHeaderGetUri(to);

    STRCMP_EQUAL("88002", UriGetUser(uri));
    DestoryMessage(&message);
    DestoryUserAgent(&ua);
}

TEST(UserAgentTestGroup, BindingsFromHeaderTest)
{
    struct UserAgent *ua = BuildUserAgent();
    struct Message *message = BuildRegisterMessage(ua);

    struct ContactHeader *from = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_FROM, message); 
    struct URI *uri = ContactHeaderGetUri(from);

    STRCMP_EQUAL("88002", UriGetUser(uri));
    DestoryMessage(&message);
    DestoryUserAgent(&ua);
}

TEST(UserAgentTestGroup, BindingsContactHeaderTest)
{
    struct UserAgent *ua = BuildUserAgent();
    struct Message *message = BuildRegisterMessage(ua);

    struct ContactHeader *contact = (struct ContactHeader *)MessageGetHeader(HEADER_NAME_CONTACT, message); 
    struct URI *uri = ContactHeaderGetUri(contact);

    STRCMP_EQUAL("88002", UriGetUser(uri));
    DestoryMessage(&message);
    DestoryUserAgent(&ua);
}

TEST(UserAgentTestGroup, BindingTest)
{
    char revMessage[MAX_MESSAGE_LENGTH] = {0};

    AddMessageTransporter((char *)"TRANS", SendMessageAddBindings, ReceiveMessageAddBindings);
    InitReceiveMessageCallback(MessageReceived);

    struct UserAgent *ua = BuildUserAgent();
    struct Message *message = BuildRegisterMessage(ua);
    struct Transaction *t = CreateTransactionExt(message, (struct TransactionOwnerInterface *)ua);
    
    ReceiveMessage(revMessage);
    CHECK_EQUAL(TRANSACTION_STATE_COMPLETED, TransactionGetState(t));
    CHECK_EQUAL(TRUE, UserAgentBinded(ua));

    DestoryUserAgent(&ua);
    DestoryTransactionManager();
    
    RemoveMessageTransporter((char *)"TRANS");

}
