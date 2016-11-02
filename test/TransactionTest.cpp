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
    
    POINTERS_EQUAL(dialog, GetTransactionUser(transaction));

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

TEST(TransactionTestGroup, TransactionStateToStringTest)
{
    STRCMP_EQUAL("TRANSACTION_STATE_INIT",TransactionState2String(TRANSACTION_STATE_INIT));
    STRCMP_EQUAL("TRANSACTION_STATE_TRYING",TransactionState2String(TRANSACTION_STATE_TRYING));
    STRCMP_EQUAL("TRANSACTION_STATE_CALLING",TransactionState2String(TRANSACTION_STATE_CALLING));
    STRCMP_EQUAL("TRANSACTION_STATE_PROCEEDING",TransactionState2String(TRANSACTION_STATE_PROCEEDING));
    STRCMP_EQUAL("TRANSACTION_STATE_COMPLETED",TransactionState2String(TRANSACTION_STATE_COMPLETED));
    STRCMP_EQUAL("TRANSACTION_STATE_CONFIRMED",TransactionState2String(TRANSACTION_STATE_CONFIRMED));
    STRCMP_EQUAL("TRANSACTION_STATE_TERMINATED",TransactionState2String(TRANSACTION_STATE_TERMINATED));

}

TEST(TransactionTestGroup, TransactionEventToStringTest)
{
    STRCMP_EQUAL("TRANSACTION_EVENT_NEW", TransactionEvent2String(TRANSACTION_EVENT_NEW));
    STRCMP_EQUAL("TRANSACTION_EVENT_200OK",TransactionEvent2String(TRANSACTION_EVENT_200OK));
    STRCMP_EQUAL("TRANSACTION_EVENT_ACK",TransactionEvent2String(TRANSACTION_EVENT_ACK));
    STRCMP_EQUAL("TRANSACTION_EVENT_100TRYING",TransactionEvent2String(TRANSACTION_EVENT_100TRYING));
    STRCMP_EQUAL("TRANSACTION_EVENT_180RINGING",TransactionEvent2String(TRANSACTION_EVENT_180RINGING));
    STRCMP_EQUAL("TRANSACTION_EVENT_401UNAUTHORIZED",TransactionEvent2String(TRANSACTION_EVENT_401UNAUTHORIZED));
    STRCMP_EQUAL("TRANSACTION_EVENT_3XX",TransactionEvent2String(TRANSACTION_EVENT_3XX));
    STRCMP_EQUAL("TRANSACTION_EVENT_INVITE",TransactionEvent2String(TRANSACTION_EVENT_INVITE));
    STRCMP_EQUAL("TRANSACTION_EVENT_BYE",TransactionEvent2String(TRANSACTION_EVENT_BYE));
    
}

TEST(TransactionTestGroup, SetTransactionStateTest)
{
    AccountInit();
    
    ua = CreateUserAgent(0);
    dialog = AddDialog(NULL_DIALOG_ID, ua);
    invite = BuildInviteMessage(dialog, (char *)"88002");     
    struct Transaction *t = CreateTransaction(invite, (struct TransactionUser*)dialog, TRANSACTION_TYPE_SERVER_INVITE);

    SetTransactionState(t, TRANSACTION_STATE_TRYING);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, GetTransactionState(t));
    
    DestroyUserAgent(&ua);
    DestroyTransaction(&t);
}
