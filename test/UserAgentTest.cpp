#include "CppUTest/TestHarness.h"

extern "C" {
#include "UserAgent.h"
#include "string.h"
}

TEST_GROUP(UserAgentTestGroup)
{
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
