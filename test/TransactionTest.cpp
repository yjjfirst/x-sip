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
    transaction = AddClientInviteTransaction(invite,(struct TransactionUser *) dialog);

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
    transaction = AddClientInviteTransaction(invite,(struct TransactionUser *) dialog);
    
    POINTERS_EQUAL(dialog, TransactionGetUser(transaction));

    DestroyUserAgent(&ua);

}

TEST(TransactionTestGroup, MessageDestTest)
{
    UT_PTR_SET(SipTransporter, &MockTransporterAndHandle);    
    mock().expectNCalls(3, SEND_OUT_MESSAGE_MOCK).withStringParameter("Method", "REGISTER").
        withStringParameter("destaddr", (char *)"192.168.10.62").
        withIntParameter("port", 5060);

    AccountInit();
    
    BindAllAccounts();
}
