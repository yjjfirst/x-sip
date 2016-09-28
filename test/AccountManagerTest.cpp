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
#include "TransactionManager.h"
}

TEST_GROUP(AccountManagerTestGroup)
{
    void setup()
    {
        UT_PTR_SET(SipTransporter, &MockTransporter);
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);

        ClearTransactionManager();
        ClearAccountManager();
        AddAccount(CreateAccount(
                   (char *)"88001", 
                   (char *)"88001", 
                   (char *)"88001", 
                   (char *)"192.168.10.62", 
                   (char *)"192.168.10.72"));
    }

    void teardown()
    {
        ClearAccountManager();

        mock().checkExpectations();
        mock().clear();
    }
};

TEST(AccountManagerTestGroup, AddOneAccountTest)
{
    struct Account *retrived = GetAccount(0);

    STRCMP_EQUAL("88001", AccountGetUserName(retrived));
    STRCMP_EQUAL("88001", AccountGetAuthName(retrived));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxyAddr(retrived));
    STRCMP_EQUAL("192.168.10.72", AccountGetRegistrar(retrived));
    CHECK_EQUAL(1, TotalAccount());

}

TEST(AccountManagerTestGroup, AddTwoAccountsTest)
{
    AddAccount(CreateAccount(
                   (char *)"88002", 
                   (char *)"88002", 
                   (char *)"88002", 
                   (char *)"192.168.10.62", 
                   (char *)"192.168.10.72"));

    CHECK_EQUAL(2, TotalAccount());
}

TEST(AccountManagerTestGroup, AddThreeAccountsTest)
{
    AddAccount(CreateAccount(
                   (char *)"88002", 
                   (char *)"88002", 
                   (char *)"88002", 
                   (char *)"192.168.10.62", 
                   (char *)"192.168.10.72"));
    AddAccount(CreateAccount(
                   (char *)"88003", 
                   (char *)"88003", 
                   (char *)"88003", 
                   (char *)"192.168.10.62", 
                   (char *)"192.168.10.72"));

    CHECK_EQUAL(3, TotalAccount());
}

TEST(AccountManagerTestGroup, ClearAccountTest)
{
    ClearAccountManager();
    CHECK_EQUAL(0, TotalAccount());
}

TEST(AccountManagerTestGroup, ClearMulitAccountTest)
{
    AddAccount(CreateAccount(
                   (char *)"88002", 
                   (char *)"88002", 
                   (char *)"88002", 
                   (char *)"192.168.10.62", 
                   (char *)"192.168.10.72"));
    AddAccount(CreateAccount(
                   (char *)"88003", 
                   (char *)"88003", 
                   (char *)"88003", 
                   (char *)"192.168.10.62", 
                   (char *)"192.168.10.72"));
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
    AddAccount(CreateAccount(
                   (char *)"88002", 
                   (char *)"88002", 
                   (char *)"88002", 
                   (char *)"192.168.10.62", 
                   (char *)"192.168.10.72"));
    AddAccount(CreateAccount(
                   (char *)"88003", 
                   (char *)"88003", 
                   (char *)"88003", 
                   (char *)"192.168.10.62", 
                   (char *)"192.168.10.72"));

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
    STRCMP_EQUAL("192.168.10.62", AccountGetProxyAddr(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetRegistrar(a));


    a = GetAccount(1);
    STRCMP_EQUAL("88002", AccountGetUserName(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxyAddr(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetRegistrar(a));

    a = GetAccount(2);
    STRCMP_EQUAL("88003", AccountGetUserName(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxyAddr(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetRegistrar(a));
}

TEST(AccountManagerTestGroup, GetOurOfRangeAccountTest)
{
    ClearAccountManager();

    int total = AccountInit();
    CHECK_EQUAL(total, TotalAccount());

    struct Account *a = GetAccount( -100);
    POINTERS_EQUAL(NULL, a);
    
    a = GetAccount(100);
    POINTERS_EQUAL(NULL, a);
    
}

TEST(AccountManagerTestGroup, AddAccountReturnTest)
{
    CHECK_EQUAL(1, AddAccount(CreateAccount(
                                  (char *)"88002", 
                                  (char *)"88002", 
                                  (char *)"88002", 
                                  (char *)"192.168.10.62", 
                                  (char *)"192.168.10.72")));
    
    CHECK_EQUAL(2, AddAccount(CreateAccount(
                                  (char *)"88002", 
                                  (char *)"88002", 
                                  (char *)"88002", 
                                  (char *)"192.168.10.62", 
                                  (char *)"192.168.10.72")));
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

}


TEST(AccountManagerTestGroup, AuthoriaztionNeedTest)
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
}

TEST(AccountManagerTestGroup, BindAllAccountsTest)
{
    AccountInit();

    mock().expectNCalls(TotalAccount(), SEND_OUT_MESSAGE_MOCK).withParameter("Method", "REGISTER");
    BindAllAccounts();

    ClearUserAgentManager();
    ClearTransactionManager();
}

TEST(AccountManagerTestGroup,  FindDestAccountTest)
{
    AccountInit();

    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    CHECK_EQUAL(0, FindMessageDestAccount(invite));

    DestroyMessage(&invite);
}
