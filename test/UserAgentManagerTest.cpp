#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Mock.h"
#include "TestingMessages.h"

extern "C" {
#include "UserAgent.h"
#include "UserAgentManager.h"
#include "AccountManager.h"
#include "Transporter.h"
#include "TransactionManager.h"
}

TEST_GROUP(UserAgentManagerTestGroup)
{
    void setup() 
    {
        UT_PTR_SET(Transporter, &MockTransporter);
        AccountInit();
    }
    
    void teardown()
    {
        ClearUserAgentManager();
        ClearAccountManager();

        mock().checkExpectations();
        mock().clear();        
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

TEST(UserAgentManagerTestGroup, CreateUserAgentAfterReceiveInviteTest)
{
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("StatusCode", 100);
    ReceiveInMessage();

    CHECK_EQUAL(1, CountUserAgent());
    ClearTransactionManager();
}
