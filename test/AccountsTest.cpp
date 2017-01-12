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
        account = CreateAccount(
            (char *)"88001", 
            (char *)"88001", 
            (char *)"88001", 
            (char *)"192.168.10.62", 
            (char *)"192.168.10.72");
    }

    void teardown()
    {
    }
};

TEST(AccountsTestGroup, EmptyAccountCreateTest)
{
    CHECK_TRUE(account != NULL); 
    DestroyAccount(&account);
}

TEST(AccountsTestGroup, AccountCreateTest)
{
    struct Account *acc = CreateAccount(
                                        (char *)"88001", 
                                        (char *)"88001", 
                                        (char *)"88001", 
                                        (char *)"192.168.10.62", 
                                        (char *)"192.168.10.72");

    STRCMP_EQUAL("88001", AccountGetUserName(acc));
    STRCMP_EQUAL("88001", AccountGetAuthName(acc));
    STRCMP_EQUAL("88001", AccountGetPassword(acc));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxyAddr(acc));
    STRCMP_EQUAL("192.168.10.72", AccountGetRegistrarAddr(acc));

    DestroyAccount(&acc);
    DestroyAccount(&account);
}

TEST(AccountsTestGroup, AccountDestroyTest)
{
    DestroyAccount(&account);    
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
    DestroyAccount(&account);
}

TEST(AccountsTestGroup, AccountSetAuthNameTest)
{
    char user[] = "Test Auth Name";
    char anotherUser[] = "Another Auth Name";

    AccountSetAuthName(account, user);
    STRCMP_EQUAL(user, AccountGetAuthName(account));

    AccountSetAuthName(account, anotherUser);
    STRCMP_EQUAL(anotherUser, AccountGetAuthName(account));
    DestroyAccount(&account);
}

TEST(AccountsTestGroup, AccountSetProxyTest)
{
    char proxy[] = "Test.Proxy";
    char anotherProxy[] = "Another.Proxy";

    AccountSetProxyAddr(account, proxy);
    STRCMP_EQUAL(proxy, AccountGetProxyAddr(account));

    AccountSetProxyAddr(account, anotherProxy);
    STRCMP_EQUAL(anotherProxy, AccountGetProxyAddr(account));
    DestroyAccount(&account);
}

TEST(AccountsTestGroup, AccountSetRegistrarTest)
{
    char r[] = "Test.r";
    char ar[] = "Another.ar";

    AccountSetRegistrarAddr(account, r);
    STRCMP_EQUAL(r, AccountGetRegistrarAddr(account));

    AccountSetRegistrarAddr(account, ar);
    STRCMP_EQUAL(ar, AccountGetRegistrarAddr(account));
    DestroyAccount(&account);
}


TEST(AccountsTestGroup, SetVeryLongUserNameTest)
{
    char userName[] = "1234567890123456789012345678901234567890123456789012345678901234asdf";

    AccountSetUserName(account, userName);

    STRCMP_CONTAINS(AccountGetUserName(account), userName);
    CHECK_EQUAL(USER_NAME_MAX_LENGTH - 1, strlen(AccountGetUserName(account)));
    DestroyAccount(&account);
}

TEST(AccountsTestGroup, SetVeryLongProxyTest)
{
    char proxy[] = "1234567890.1234567890.1234567890.1234567890123456789012345678901234";
    
    AccountSetProxyAddr(account, (char *)proxy);

    STRCMP_CONTAINS(AccountGetProxyAddr(account), proxy);
    CHECK_EQUAL(PROXY_MAX_LENGTH - 1, strlen(AccountGetProxyAddr(account)));
    DestroyAccount(&account);
}

TEST(AccountsTestGroup, SetVeryLongRegistrarTest)
{
    char registrar[] = "1234567890.1234567890.1234567890.1234567890123456789012345678901234";

    AccountSetRegistrarAddr(account, registrar);
    STRCMP_CONTAINS(AccountGetRegistrarAddr(account), registrar);
    CHECK_EQUAL(REGISTRAR_MAX_LENGTH -1 , strlen(AccountGetRegistrarAddr(account)));

    DestroyAccount(&account);
}

TEST(AccountsTestGroup, SetPasswordTest)
{
    char passwd[] = "asdf";

    AccountSetPassword(account, passwd);
    STRCMP_EQUAL(passwd, AccountGetPassword(account));
    
    DestroyAccount(&account);
}

TEST(AccountsTestGroup, SetRegistrarPortTest)
{
    int port = 1000;

    AccountSetRegistrarPort(account, port);
    CHECK_EQUAL(port, AccountGetRegistrarPort(account));

    port = 1003;
    AccountSetRegistrarPort(account, port);
    CHECK_EQUAL(port, AccountGetRegistrarPort(account));

    DestroyAccount(&account);
}

TEST(AccountsTestGroup, SetProxyPortTest)
{
    int port = 2000;
    
    AccountSetProxyPort(account, port);
    CHECK_EQUAL(port, AccountGetProxyPort(account));

    port = 3000;
    AccountSetProxyPort(account, port);
    CHECK_EQUAL(port, AccountGetProxyPort(account));

    DestroyAccount(&account);

}

TEST(AccountsTestGroup, DefaultProxyPortTest)
{
    CHECK_EQUAL(5060, AccountGetProxyPort(account));
    DestroyAccount(&account);
}

TEST(AccountsTestGroup, DefaultRegistrarPortTest)
{
    CHECK_EQUAL(5060, AccountGetRegistrarPort(account));
    DestroyAccount(&account);
}
