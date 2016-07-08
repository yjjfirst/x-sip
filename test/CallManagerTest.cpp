#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "AccountMock.h"
#include "TransportMock.h"

extern "C" {
#include "UserAgentManager.h"
#include "AccountManager.h"
#include "Call.h"
#include "MessageTransport.h"
#include "TransactionManager.h"
}

TEST_GROUP(CallManagerTestGroup)
{
    void setup() 
    {
        UT_PTR_SET(Transporter, &MockTransporter);
        AccountInitMock();
    }
    
    void teardown()
    {
        ClearAccount();
        ClearUserAgentManager();
    }
};

IGNORE_TEST(CallManagerTestGroup, CallOutSendInviteTest)
{
    char dest[] = "88002";
    char account = 0;

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "INVITE");

    CallOut(account, dest);

    mock().checkExpectations();
    mock().clear();
}
