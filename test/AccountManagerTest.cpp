#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "Accounts.h"
#include "AccountManager.h"
}

void AddSecondAccount(struct AccountManager *am)
{
    struct Account *second;
    second = CreateAccount(
                           (char *)"88002", 
                           (char *)"88002", 
                           (char *)"192.168.10.62", 
                           (char *)"192.168.10.72");
    AddAccount(am, second);
}

void AddThirdAccount(struct AccountManager *am)
{
    struct Account *third;
    third = CreateAccount(
                          (char *)"88003", 
                          (char *)"88003", 
                          (char *)"192.168.10.62", 
                          (char *)"192.168.10.72");

    AddAccount(am, third);
}

TEST_GROUP(AccountManagerTestGroup)
{
    struct AccountManager *am;
    struct Account *account;

    void setup()
    {
        am = AccountManagerGet();
        account = CreateAccount(
                                (char *)"88001", 
                                (char *)"88001", 
                                (char *)"192.168.10.62", 
                                (char *)"192.168.10.72");
        AddFirstAccount();

    }

    void teardown()
    {
        ClearAccount(am);
    }

    void AddFirstAccount()
    {
        AddAccount(am, account);
    }

};

TEST(AccountManagerTestGroup, AccountManagerGetTest)
{
    CHECK_TRUE(am != NULL);
}

TEST(AccountManagerTestGroup, AddOneAccountTest)
{
    struct Account *retrived = GetAccount(am, 0);

    STRCMP_EQUAL("88001", AccountGetUserName(retrived));
    STRCMP_EQUAL("88001", AccountGetAuthName(retrived));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxy(retrived));
    STRCMP_EQUAL("192.168.10.72", AccountGetRegistrar(retrived));
    CHECK_EQUAL(1, TotalAccount(am));

}

TEST(AccountManagerTestGroup, AddTwoAccountsTest)
{
    AddSecondAccount(am);
    CHECK_EQUAL(2, TotalAccount(am));
}

TEST(AccountManagerTestGroup, AddThreeAccountsTest)
{
    AddSecondAccount(am);
    AddThirdAccount(am);
    CHECK_EQUAL(3, TotalAccount(am));
}

TEST(AccountManagerTestGroup, ClearAccountTest)
{
    ClearAccount(am);
    CHECK_EQUAL(0, TotalAccount(am));
}

TEST(AccountManagerTestGroup, ClearMulitAccountTest)
{
    AddSecondAccount(am);
    AddThirdAccount(am);
    ClearAccount(am);
    CHECK_EQUAL(0, TotalAccount(am));
}

TEST(AccountManagerTestGroup, RemoveAccountTest)
{
    RemoveAccount(am, 0);
    CHECK_EQUAL(0, TotalAccount(am));
}

TEST(AccountManagerTestGroup, RemoveMultiAccountTest)
{
    AddSecondAccount(am);
    AddThirdAccount(am);

    RemoveAccount(am, 0);
    CHECK_EQUAL(2, TotalAccount(am));
    RemoveAccount(am, 0);
    CHECK_EQUAL(1, TotalAccount(am));
    RemoveAccount(am, 0);
    CHECK_EQUAL(0, TotalAccount(am));
}

void AccountInitMock(struct AccountManager *am)
{
    AddSecondAccount(am);
    AddThirdAccount(am);
}

TEST(AccountManagerTestGroup, InitTest)
{
    UT_PTR_SET(AccountInit, AccountInitMock);

    AccountInit(am);
    CHECK_EQUAL(3, TotalAccount(am));

    struct Account *a = GetAccount(am, 0);
    STRCMP_EQUAL("88001", AccountGetUserName(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxy(a));

    a = GetAccount(am,1);
    STRCMP_EQUAL("88002", AccountGetUserName(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxy(a));

    a = GetAccount(am,2);
    STRCMP_EQUAL("88003", AccountGetUserName(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxy(a));

}

TEST(AccountManagerTestGroup, GetOurOfRangeAccountTest)
{
    UT_PTR_SET(AccountInit, AccountInitMock);

    AccountInit(am);
    CHECK_EQUAL(3, TotalAccount(am));

    struct Account *a = GetAccount(am, -100);
    POINTERS_EQUAL(NULL, a);
    
    a = GetAccount(am, 100);
    POINTERS_EQUAL(NULL, a);
    
}

IGNORE_TEST(AccountManagerTestGroup, AccountRegisterTest)
{
    AddAccount(am, account);
}
