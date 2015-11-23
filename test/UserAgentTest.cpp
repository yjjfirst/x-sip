#include "CppUTest/TestHarness.h"

extern "C" {
#include "UserAgent.h"
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
