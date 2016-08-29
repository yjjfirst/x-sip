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
}

TEST_GROUP(TransactionTestGroup)
{
    struct UserAgent *ua;
    struct Message *invite;
    struct Transaction *transaction;
    struct Dialog *dialog;
    char branch[32];

    void setup()
    {
        UT_PTR_SET(Transporter, &DummyTransporter);        
        AccountInit();
        
        ua = CreateUserAgent(0);
        dialog = AddNewDialog(NULL_DIALOG_ID, ua);
        invite = BuildInviteMessage(dialog); 
        transaction = AddClientInviteTransaction(invite,(struct TransactionUser *) dialog);

        strcpy(branch, MessageGetViaBranch(invite));
    }
    void teardown()
    {
        DestroyUserAgent(&ua);
        ClearTransactionManager();
        ClearAccountManager();
    }
};

TEST(TransactionTestGroup, MessageOwnerTest)
{
    POINTERS_EQUAL(transaction, MessageBelongTo(invite));
}

TEST(TransactionTestGroup, TransactionOwnerTest)
{
    POINTERS_EQUAL(dialog, TransactionGetUser(transaction));
}
