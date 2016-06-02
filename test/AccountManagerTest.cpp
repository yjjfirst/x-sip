#include "CppUTest/TestHarness.h"

extern "C" {
#include "Accounts.h"
#include "AccountManager.h"
}

TEST_GROUP(AccountManagerTestGroup)
{
    void teardown()
    {
    }
};

TEST(AccountManagerTestGroup, AccountManagerGetTest)
{
    struct AccountManager *am = AccountManagerGet();

    CHECK_TRUE(am != NULL);
}

TEST(AccountManagerTestGroup, AddOneAccountTest)
{
    struct AccountManager *am = AccountManagerGet();
    struct Account *account = CreateAccount(
                                            (char *)"88001", 
                                            (char *)"88001", 
                                            (char *)"192.168.10.62", 
                                            (char *)"192.168.10.72");

    AddAccount(am, account);
    struct Account *retrived = GetAccount(am, 0);

    STRCMP_EQUAL("88001", AccountGetUserName(retrived));
    STRCMP_EQUAL("88001", AccountGetAuthName(retrived));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxy(retrived));
    STRCMP_EQUAL("192.168.10.72", AccountGetRegistrar(retrived));
    CHECK_EQUAL(1, TotalAccount(am));

    ClearAccount(am);
}

TEST(AccountManagerTestGroup, ClearAccountTest)
{
    struct AccountManager *am = AccountManagerGet();
    struct Account *account = CreateAccount(
                                            (char *)"88001", 
                                            (char *)"88001", 
                                            (char *)"192.168.10.62", 
                                            (char *)"192.168.10.72");

    AddAccount(am, account);
    ClearAccount(am);

    CHECK_EQUAL(0, TotalAccount(am));
}

IGNORE_TEST(AccountManagerTestGroup, AccountRegisterTest)
{
    struct AccountManager *am = AccountManagerGet();
    struct Account *account = CreateAccount(
                                            (char *)"88001", 
                                            (char *)"88001", 
                                            (char *)"192.168.10.62", 
                                            (char *)"192.168.10.72");
    
    AddAccount(am, account);
    ClearAccount(am);
}
