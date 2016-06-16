#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include <string.h>
#include "Accounts.h"
};

TEST_GROUP(AccountsTestGroup)
{
    struct Account *account;
    void setup() 
    {
        account = CreateEmptyAccount();
    }

    void teardown()
    {
    }
};

TEST(AccountsTestGroup, EmptyAccountCreateTest)
{
    CHECK_TRUE(account != NULL); 
    DestoryAccount(&account);
}

TEST(AccountsTestGroup, AccountCreateTest)
{
    struct Account *acc = CreateAccount(
                                        (char *)"88001", 
                                        (char *)"88001", 
                                        (char *)"192.168.10.62", 
                                        (char *)"192.168.10.72");

    STRCMP_EQUAL("88001", AccountGetUserName(acc));
    STRCMP_EQUAL("88001", AccountGetAuthName(acc));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxy(acc));
    STRCMP_EQUAL("192.168.10.72", AccountGetRegistrar(acc));

    DestoryAccount(&acc);
    DestoryAccount(&account);
}

TEST(AccountsTestGroup, AccountDestoryTest)
{
    DestoryAccount(&account);    
    CHECK_TRUE(account == NULL);
}

TEST(AccountsTestGroup, AccountSetUserTest)
{
    char user[] = "Test User Name";
    char anotherUser[] = "Another User";

    AccountSetUserName(account, user);
    STRCMP_EQUAL(user, AccountGetUserName(account));

    AccountSetUserName(account, anotherUser);
    STRCMP_EQUAL(anotherUser, AccountGetUserName(account));
    DestoryAccount(&account);
}

TEST(AccountsTestGroup, AccountSetAuthNameTest)
{
    char user[] = "Test Auth Name";
    char anotherUser[] = "Another Auth Name";

    AccountSetAuthName(account, user);
    STRCMP_EQUAL(user, AccountGetAuthName(account));

    AccountSetAuthName(account, anotherUser);
    STRCMP_EQUAL(anotherUser, AccountGetAuthName(account));
    DestoryAccount(&account);
}

TEST(AccountsTestGroup, AccountSetProxyTest)
{
    char proxy[] = "Test.Proxy";
    char anotherProxy[] = "Another.Proxy";

    AccountSetProxy(account, proxy);
    STRCMP_EQUAL(proxy, AccountGetProxy(account));

    AccountSetProxy(account, anotherProxy);
    STRCMP_EQUAL(anotherProxy, AccountGetProxy(account));
    DestoryAccount(&account);
}

TEST(AccountsTestGroup, AccountSetRegistrarTest)
{
    char r[] = "Test.r";
    char ar[] = "Another.ar";

    AccountSetRegistrar(account, r);
    STRCMP_EQUAL(r, AccountGetRegistrar(account));

    AccountSetRegistrar(account, ar);
    STRCMP_EQUAL(ar, AccountGetRegistrar(account));
    DestoryAccount(&account);
}


TEST(AccountsTestGroup, SetVeryLongUserNameTest)
{
    char userName[] = "1234567890123456789012345678901234567890123456789012345678901234asdf";

    AccountSetUserName(account, userName);

    STRCMP_CONTAINS(AccountGetUserName(account), userName);
    CHECK_EQUAL(USER_NAME_MAX_LENGTH - 1, strlen(AccountGetUserName(account)));
    DestoryAccount(&account);
}

TEST(AccountsTestGroup, SetVeryLongProxyTest)
{
    char proxy[] = "1234567890.1234567890.1234567890.1234567890123456789012345678901234";
    
    AccountSetProxy(account, (char *)proxy);

    STRCMP_CONTAINS(AccountGetProxy(account), proxy);
    CHECK_EQUAL(PROXY_MAX_LENGTH - 1, strlen(AccountGetProxy(account)));
    DestoryAccount(&account);
}

TEST(AccountsTestGroup, SetVeryLongRegistrarTest)
{
    char registrar[] = "1234567890.1234567890.1234567890.1234567890123456789012345678901234";

    AccountSetRegistrar(account, registrar);
    STRCMP_CONTAINS(AccountGetRegistrar(account), registrar);
    CHECK_EQUAL(REGISTRAR_MAX_LENGTH -1 , strlen(AccountGetRegistrar(account)));

    DestoryAccount(&account);
}
