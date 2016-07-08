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
    AddUserAgent(0);    
    
    CHECK_EQUAL(1, CountUserAgent());
}

TEST(UserAgentManagerTestGroup, AddTwoUserAgentTest)
{
    AddUserAgent(0);    
    AddUserAgent(1);    
    
    CHECK_EQUAL(2, CountUserAgent());
}
