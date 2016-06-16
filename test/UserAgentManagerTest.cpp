#include "CppUTest/TestHarness.h"
#include "AccountMock.h"

extern "C" {
#include "UserAgent.h"
#include "UserAgentManager.h"
#include "AccountManager.h"
}

TEST_GROUP(UserAgentManagerTestGroup)
{
    void setup() 
    {
        AccountInitMock();
    }
    
    void teardown()
    {
        ClearAccount();
    }
};

TEST(UserAgentManagerTestGroup, AddUserAgentTest)
{
    struct UserAgent *ua = BuildUserAgent(0);
    AddUserAgent(ua);    
    DestoryUserAgent(&ua);
}










