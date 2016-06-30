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
        ClearUserAgentManager();
        ClearAccount();
    }
};

TEST(UserAgentManagerTestGroup, AddUserAgentTest)
{
    struct UserAgent *ua = CreateUserAgent(0);
    AddUserAgent(ua);    
    
    CHECK_EQUAL(1, CountUserAgent());
}

TEST(UserAgentManagerTestGroup, AddTwoUserAgentTest)
{
    struct UserAgent *ua1 = CreateUserAgent(0);
    struct UserAgent *ua2 = CreateUserAgent(1);
    
    AddUserAgent(ua1);    
    AddUserAgent(ua2);    
    
    CHECK_EQUAL(2, CountUserAgent());
}
