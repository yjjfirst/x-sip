#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TestingMessages.h"

extern "C" {
#include "Messages.h"
#include "TransactionManager.h"
#include "Transaction.h"
}

TEST_GROUP(ServerNonInviteTransactionTestGroup)
{
};

TEST(ServerNonInviteTransactionTestGroup, ServerNonInviteTransactionCreateTest)
{
    struct Message *request = CreateMessage();    
    ParseMessage((char *)BYE_MESSAGE,request);

    struct Transaction *t = AddServerNonInviteTransaction(request, NULL);
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, TransactionGetState(t));

    EmptyTransactionManager();
}

TEST(ServerNonInviteTransactionTestGroup, ServerTransactionRequestMatchTest)
{
}








