#include "CppUTest/TestHarness.h"
#include "AccountMock.h"

extern "C" {
#include "UserAgent.h"
#include "UserAgentManager.h"
#include "AccountManager.h"
}

TEST_GROUP(UserAgentManagerTestGroup)
{
    struct UserAgentManager *uam;
    void setup() 
    {
        uam = CreateUserAgentManager();
        AccountInitMock();
    }
    
    void teardown()
    {
        DestroyUserAgentManager(&uam);
        ClearAccount();
    }
};

TEST(UserAgentManagerTestGroup, AddUserAgentTest)
{
    struct UserAgent *ua = CreateUserAgent(0);
    AddUserAgent(uam, ua);    
    
    CHECK_EQUAL(1, CountUserAgent(uam));
}

TEST(UserAgentManagerTestGroup, AddTwoUserAgentTest)
{
    struct UserAgent *ua1 = CreateUserAgent(0);
    struct UserAgent *ua2 = CreateUserAgent(1);
    
    AddUserAgent(uam, ua1);    
    AddUserAgent(uam, ua2);    
    
    CHECK_EQUAL(2, CountUserAgent(uam));
}
