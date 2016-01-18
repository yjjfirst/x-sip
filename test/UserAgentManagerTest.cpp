#include "CppUTest/TestHarness.h"
#include "UserAgentTest.h"

extern "C" {
#include "UserAgent.h"
#include "UserAgentManager.h"
}
TEST_GROUP(UserAgentManagerTestGroup)
{
};

TEST(UserAgentManagerTestGroup, AddUserAgentTest)
{
    struct UserAgent *ua = BuildUserAgent();

    AddUserAgent(ua);
    DestoryUserAgent(&ua);
}
