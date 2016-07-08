#include "CppUTest/TestHarness.h"
#include "AccountMock.h"

extern "C" {
#include "AccountManager.h"
#include "Call.h"
}

TEST_GROUP(CallManagerTestGroup)
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

TEST(CallManagerTestGroup, CallOutTest)
{
    char dest[] = "88002";
    char account = 0;

    CallOut(account, dest);
}
