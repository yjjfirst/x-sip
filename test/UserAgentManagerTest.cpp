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
        UT_PTR_SET(SipTransporter, &MockTransporter);
        AccountInit();
    }
    
    void teardown()
    {
        ClearUaManager();
        ClearAccountManager();

        mock().checkExpectations();
        mock().clear();        
    }
};

TEST(UserAgentManagerTestGroup, AddUaTest)
{
    AddUa(0);    
    
    CHECK_EQUAL(1, CountUas());
}

TEST(UserAgentManagerTestGroup, AddTwoUserAgentTest)
{
    AddUa(0);    
    AddUa(1);    
    
    CHECK_EQUAL(2, CountUas());
}

TEST(UserAgentManagerTestGroup, GetUaTest)
{
    struct UserAgent *ua = AddUa(0);
    POINTERS_EQUAL(ua, GetUa(0));

    ua = AddUa(1);
    POINTERS_EQUAL(ua, GetUa(1));

    ua = AddUa(2);
    POINTERS_EQUAL(ua, GetUa(2));
}

TEST(UserAgentManagerTestGroup, GetUaOutOfRangeTest)
{
    AddUa(0);
    AddUa(1);
    AddUa(2);

    POINTERS_EQUAL(NULL, GetUa(-1));
    POINTERS_EQUAL(NULL, GetUa(-100));
    POINTERS_EQUAL(NULL, GetUa(3));
    POINTERS_EQUAL(NULL, GetUa(100));
}

TEST(UserAgentManagerTestGroup, CreateUserAgentAfterReceiveInviteTest)
{
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(INCOMMING_INVITE_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("StatusCode", 100);
    ReceiveMessage();

    CHECK_EQUAL(1, CountUas());
    ClearTransactionManager();
}

TEST(UserAgentManagerTestGroup, GetUaPositionTest)
{
    struct UserAgent *u1 = AddUa(0);
    struct UserAgent *u2 = AddUa(1);
    struct UserAgent *u3 = AddUa(2);

    CHECK_EQUAL(0, GetUaPosition(u1));
    CHECK_EQUAL(1, GetUaPosition(u2));
    CHECK_EQUAL(2, GetUaPosition(u3));
}

TEST(UserAgentManagerTestGroup, GetNonExistUserAgentPositionTest)
{
    AddUa(0);
    AddUa(1);
    AddUa(2);

    struct UserAgent *ua = CreateUserAgent(0);
    CHECK_EQUAL(-1, GetUaPosition(ua));

    DestroyUserAgent(&ua);
}

TEST(UserAgentManagerTestGroup, RemoveFirstUserAgentTest)
{
    struct UserAgent *ua1 = AddUa(0);
    struct UserAgent *ua2 = AddUa(1);
    struct UserAgent *ua3 = AddUa(2);

    RemoveUa(ua1);

    POINTERS_EQUAL(ua2, GetUa(0));
    POINTERS_EQUAL(ua3, GetUa(1));
}

TEST(UserAgentManagerTestGroup, RemoveSecondUserAgentTest)
{
    struct UserAgent *ua1 = AddUa(0);
    struct UserAgent *ua2 = AddUa(1);
    struct UserAgent *ua3 = AddUa(2);

    RemoveUa(ua2);

    POINTERS_EQUAL(ua1, GetUa(0));
    POINTERS_EQUAL(ua3, GetUa(1));
}

TEST(UserAgentManagerTestGroup, RemoveLastUserAgentTest)
{
    struct UserAgent *ua1 = AddUa(0);
    struct UserAgent *ua2 = AddUa(1);
    struct UserAgent *ua3 = AddUa(2);

    RemoveUa(ua3);

    POINTERS_EQUAL(ua1, GetUa(0));
    POINTERS_EQUAL(ua2, GetUa(1));
}
