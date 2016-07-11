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
        ClearAccountManager();
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

TEST(UserAgentManagerTestGroup, GetUserAgentTest)
{
    struct UserAgent *ua = AddUserAgent(0);
    POINTERS_EQUAL(ua, GetUserAgent(0));

    ua = AddUserAgent(1);
    POINTERS_EQUAL(ua, GetUserAgent(1));

    ua = AddUserAgent(2);
    POINTERS_EQUAL(ua, GetUserAgent(2));
}

TEST(UserAgentManagerTestGroup, GetUserAgentOutOfRangeTest)
{
    AddUserAgent(0);
    AddUserAgent(1);
    AddUserAgent(2);

    POINTERS_EQUAL(NULL, GetUserAgent(-1));
    POINTERS_EQUAL(NULL, GetUserAgent(-100));
    POINTERS_EQUAL(NULL, GetUserAgent(3));
    POINTERS_EQUAL(NULL, GetUserAgent(100));
}
