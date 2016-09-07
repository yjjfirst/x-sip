#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Mock.h"
#include "TestingMessages.h"

extern "C" {
#include <stdio.h>
#include "Accounts.h"
#include "AccountManager.h"
#include "Messages.h"
#include "Transporter.h"
#include "UserAgentManager.h"
#include "ViaHeader.h"
extern void ClearTransactionManager();
}

TEST_GROUP(AccountManagerTestGroup)
{
    void setup()
    {
        UT_PTR_SET(Transporter, &MockTransporter);
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);

        ClearAccountManager();
        ::AddFirstAccount();
    }

    void teardown()
    {
        ClearAccountManager();
    }
};

TEST(AccountManagerTestGroup, AddOneAccountTest)
{
    struct Account *retrived = GetAccount(0);

    STRCMP_EQUAL("88001", AccountGetUserName(retrived));
    STRCMP_EQUAL("88001", AccountGetAuthName(retrived));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxy(retrived));
    STRCMP_EQUAL("192.168.10.72", AccountGetRegistrar(retrived));
    CHECK_EQUAL(1, TotalAccount());

}

TEST(AccountManagerTestGroup, AddTwoAccountsTest)
{
    AddSecondAccount();
    CHECK_EQUAL(2, TotalAccount());
}

TEST(AccountManagerTestGroup, AddThreeAccountsTest)
{
    AddSecondAccount();
    AddThirdAccount();
    CHECK_EQUAL(3, TotalAccount());
}

TEST(AccountManagerTestGroup, ClearAccountTest)
{
    ClearAccountManager();
    CHECK_EQUAL(0, TotalAccount());
}

TEST(AccountManagerTestGroup, ClearMulitAccountTest)
{
    AddSecondAccount();
    AddThirdAccount();
    ClearAccountManager();
    CHECK_EQUAL(0, TotalAccount());
}

TEST(AccountManagerTestGroup, RemoveAccountTest)
{
    RemoveAccount(0);
    CHECK_EQUAL(0, TotalAccount());
}

TEST(AccountManagerTestGroup, RemoveMultiAccountTest)
{
    AddSecondAccount();
    AddThirdAccount();

    RemoveAccount(0);
    CHECK_EQUAL(2, TotalAccount());
    RemoveAccount(0);
    CHECK_EQUAL(1, TotalAccount());
    RemoveAccount(0);
    CHECK_EQUAL(0, TotalAccount());
}

TEST(AccountManagerTestGroup, InitTest)
{
    ClearAccountManager();

    AccountInit();
    CHECK_EQUAL(3, TotalAccount());

    struct Account *a = GetAccount( 0);
    STRCMP_EQUAL("88001", AccountGetUserName(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxy(a));
    STRCMP_EQUAL("192.168.10.72", AccountGetRegistrar(a));


    a = GetAccount(1);
    STRCMP_EQUAL("88002", AccountGetUserName(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxy(a));
    STRCMP_EQUAL("192.168.10.72", AccountGetRegistrar(a));

    a = GetAccount(2);
    STRCMP_EQUAL("88003", AccountGetUserName(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxy(a));
    STRCMP_EQUAL("192.168.10.72", AccountGetRegistrar(a));
}

TEST(AccountManagerTestGroup, GetOurOfRangeAccountTest)
{
    ClearAccountManager();

    AccountInit();
    CHECK_EQUAL(3, TotalAccount());

    struct Account *a = GetAccount( -100);
    POINTERS_EQUAL(NULL, a);
    
    a = GetAccount(100);
    POINTERS_EQUAL(NULL, a);
    
}

TEST(AccountManagerTestGroup, AddAccountReturnTest)
{
    CHECK_EQUAL(1,AddSecondAccount());
    CHECK_EQUAL(2,AddThirdAccount());
}

TEST(AccountManagerTestGroup, AccountAddBindingTest)
{
    struct Account *account = GetAccount(0);

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "REGISTER");
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE);

    AccountAddBinding(0);    
    ReceiveInMessage();
    CHECK_EQUAL(TRUE, AccountBinded(account));

    ClearUserAgentManager();
    ClearTransactionManager();

    mock().checkExpectations();
    mock().clear();
}

TEST(AccountManagerTestGroup, AccountRemoveBindingTest)
{
    struct Account *account = GetAccount(0);

    AccountSetBinded(account);
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "REGISTER");
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(REMOVE_BINDING_OK_MESSAGE);

    AccountRemoveBinding(0);    
    ReceiveInMessage();

    CHECK_EQUAL(FALSE, AccountBinded(account));

    ClearUserAgentManager();
    ClearTransactionManager();

    mock().checkExpectations();
    mock().clear();
}


IGNORE_TEST(AccountManagerTestGroup, AuthoriaztionTest)
{
    struct Account *account = GetAccount(0);
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "REGISTER");
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(UNAUTHORIZED_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "REGISTER");
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE_FOR_AUTHORIZATION);

    AccountAddBinding(0);    

    UT_PTR_SET(GenerateBranch, GenerateBranchMockForAuthorization);
    ReceiveInMessage();
    CHECK_EQUAL(FALSE, AccountBinded(account));

    ReceiveInMessage();
    CHECK_EQUAL(TRUE, AccountBinded(account));
    
    ClearUserAgentManager();
    ClearTransactionManager();

    mock().checkExpectations();
    mock().clear();

}

TEST(AccountManagerTestGroup, BindAllAccountsTest)
{
    ClearAccountManager();
    AccountInit();

    mock().expectNCalls(TotalAccount(), SEND_OUT_MESSAGE_MOCK).withParameter("Method", "REGISTER");
    BindAllAccounts();

    ClearUserAgentManager();
    ClearTransactionManager();

    mock().checkExpectations();
    mock().clear();
}
