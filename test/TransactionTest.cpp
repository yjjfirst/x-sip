#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Mock.h"

extern "C" {
#include <string.h>
    
#include "Transporter.h"
#include "AccountManager.h"
#include "UserAgent.h"
#include "Dialog.h"
#include "MessageBuilder.h"
#include "TransactionManager.h"
#include "Transaction.h"
#include "Messages.h"
#include "DialogManager.h"
#include "UserAgentManager.h"
#include "Accounts.h"
}

TEST_GROUP(TransactionTestGroup)
{
    struct UserAgent *ua;
    MESSAGE *invite;
    struct Transaction *transaction;
    struct Dialog *dialog;

    void teardown()
    {
        ClearTransactionManager();
        ClearAccountManager();
        ClearDialogManager();
        ClearUserAgentManager();

        mock().checkExpectations();
        mock().clear();
    }
};

TEST(TransactionTestGroup, MessageOwnerTest)
{
    UT_PTR_SET(SipTransporter, &DummyTransporter);        
    AccountInit();
    
    ua = CreateUserAgent(0);
    dialog = AddDialog(NULL_DIALOG_ID, ua);
    invite = BuildInviteMessage(dialog, (char *)"88002"); 
    transaction = AddTransaction(invite,(struct TransactionUser *) dialog, TRANSACTION_TYPE_CLIENT_INVITE);

    POINTERS_EQUAL(transaction, MessageBelongTo(invite));
    
    DestroyUserAgent(&ua);
}

TEST(TransactionTestGroup, TransactionOwnerTest)
{
    UT_PTR_SET(SipTransporter, &DummyTransporter);        
    AccountInit();
    
    ua = CreateUserAgent(0);
    dialog = AddDialog(NULL_DIALOG_ID, ua);
    invite = BuildInviteMessage(dialog, (char *)"88002"); 
    transaction = AddTransaction(invite,(struct TransactionUser *) dialog, TRANSACTION_TYPE_CLIENT_INVITE);
    
    POINTERS_EQUAL(dialog, TransactionGetUser(transaction));

    DestroyUserAgent(&ua);

}

TEST(TransactionTestGroup, MessageDestTest)
{
    UT_PTR_SET(SipTransporter, &MockTransporterAndHandle);

    AccountInit();    

    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", "REGISTER").
        withStringParameter("destaddr", (char *)AccountGetRegistrar(GetAccount(0))).
        withIntParameter("port", AccountGetRegistrarPort(GetAccount(0)));
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", "REGISTER").
        withStringParameter("destaddr", (char *)AccountGetRegistrar(GetAccount(1))).
        withIntParameter("port", AccountGetRegistrarPort(GetAccount(1)));
    mock().expectOneCall(SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", "REGISTER").
        withStringParameter("destaddr", (char *)AccountGetRegistrar(GetAccount(2))).
        withIntParameter("port", AccountGetRegistrarPort(GetAccount(2)));

    BindAllAccounts();
}
