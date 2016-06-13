#include "CppUTest/TestHarness.h"

extern "C" {
#include "UserAgent.h"
#include "UserAgentManager.h"
}

TEST_GROUP(UserAgentManagerTestGroup)
{
};

TEST(UserAgentManagerTestGroup, AddUserAgentTest)
{
    struct UserAgent *ua = BuildUserAgent(NULL);
    //    struct UserAgent *uaGetBack = NULL;

    AddUserAgent(ua);
    
    DestoryUserAgent(&ua);
}

//TEST(UserAgentManagerTestGroup, 
