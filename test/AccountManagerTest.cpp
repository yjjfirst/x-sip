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
    STRCMP_EQUAL("192.168.10.72", AccountGetRegistrarAddr(retrived));
    CHECK_EQUAL(1, CountAccounts());

}

TEST(AccountManagerTestGroup, AddTwoAccountsTest)
{
    AddAccount(CreateAccount(
                   (char *)"88002", 
                   (char *)"88002", 
                   (char *)"88002", 
                   (char *)"192.168.10.62", 
                   (char *)"192.168.10.72"));

    CHECK_EQUAL(2, CountAccounts());
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

    CHECK_EQUAL(3, CountAccounts());
}

TEST(AccountManagerTestGroup, ClearAccountTest)
{
    ClearAccountManager();
    CHECK_EQUAL(0, CountAccounts());
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
    CHECK_EQUAL(0, CountAccounts());
}

TEST(AccountManagerTestGroup, RemoveAccountTest)
{
    RemoveAccount(0);
    CHECK_EQUAL(0, CountAccounts());
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
    CHECK_EQUAL(2, CountAccounts());
    RemoveAccount(0);
    CHECK_EQUAL(1, CountAccounts());
    RemoveAccount(0);
    CHECK_EQUAL(0, CountAccounts());
}

TEST(AccountManagerTestGroup, InitTest)
{
    ClearAccountManager();

    AccountInit();
    CHECK_EQUAL(3, CountAccounts());

    struct Account *a = GetAccount( 0);
    STRCMP_EQUAL("88001", AccountGetUserName(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxyAddr(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetRegistrarAddr(a));


    a = GetAccount(1);
    STRCMP_EQUAL("88002", AccountGetUserName(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxyAddr(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetRegistrarAddr(a));

    a = GetAccount(2);
    STRCMP_EQUAL("88003", AccountGetUserName(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetProxyAddr(a));
    STRCMP_EQUAL("192.168.10.62", AccountGetRegistrarAddr(a));
}

TEST(AccountManagerTestGroup, GetOurOfRangeAccountTest)
{
    ClearAccountManager();

    int total = AccountInit();
    CHECK_EQUAL(total, CountAccounts());

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
    ReceiveMessage();
    CHECK_EQUAL(TRUE, AccountBinded(account));

    ClearUaManager();
    ClearTransactionManager();
}
#define REMOVE_BINDING_OK_MESSAGE (char *)"\
SIP/2.0 200 OK\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5060;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5060\r\n\
From: <sip:88001@192.168.10.62>;tag=1225432999\r\n\
To: <sip:88001@192.168.10.62>;tag=as1d07559a\r\n\
Call-ID: 1222971951\r\n\
CSeq: 1 REGISTER\r\n\
Expires: 0\r\n\
Contact: <sip:88001@192.168.10.1;line=f2fd53ebfa7728f>;expires=3600\r\n\
Content-Length: 0\r\n"

TEST(AccountManagerTestGroup, AccountRemoveBindingTest)
{
    struct Account *account = GetAccount(0);

    AccountSetBinded(account);
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "REGISTER");
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(REMOVE_BINDING_OK_MESSAGE);
    
    AccountRemoveBinding(0);    
    ReceiveMessage();

    CHECK_EQUAL(FALSE, AccountBinded(account));

    ClearUaManager();
    ClearTransactionManager();

}

#define ADD_BINDING_OK_MESSAGE_FOR_AUTHORIZATION (char *)"\
SIP/2.0 200 OK\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5060;branch=z9hG4bK1491280927;received=192.168.10.1;rport=5060\r\n\
From: <sip:88001@192.168.10.62>;tag=1225432999\r\n\
To: <sip:88001@192.168.10.62>;tag=as1d07559a\r\n\
Call-ID: 1222971951\r\n\
CSeq: 1 REGISTER\r\n\
Expires: 3600\r\n\
Contact: <sip:88001@192.168.10.1;line=f2fd53ebfa7728f>;expires=3600\r\n\
Content-Length: 0\r\n"

TEST(AccountManagerTestGroup, AuthoriaztionNeedTest)
{
    struct Account *account = GetAccount(0);
    
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "REGISTER");
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(UNAUTHORIZED_MESSAGE);
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withParameter("Method", "REGISTER");
    mock().expectOneCall(RECEIVE_IN_MESSAGE_MOCK).andReturnValue(ADD_BINDING_OK_MESSAGE_FOR_AUTHORIZATION);

    AccountAddBinding(0);    

    UT_PTR_SET(GenerateBranch, GenerateBranchMockForAuthorization);
    ReceiveMessage();
    CHECK_EQUAL(FALSE, AccountBinded(account));

    ReceiveMessage();
    CHECK_EQUAL(TRUE, AccountBinded(account));
    
    ClearUaManager();
    ClearTransactionManager();
}

TEST(AccountManagerTestGroup, BindAllAccountsTest)
{
    AccountInit();

    mock().expectNCalls(CountAccounts(), SEND_OUT_MESSAGE_MOCK).withParameter("Method", "REGISTER");
    BindAllAccounts();

    ClearUaManager();
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
