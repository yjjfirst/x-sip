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
    struct Message *request;
    struct Transaction *transaction;
    void setup() 
    {
        request = CreateMessage();
        ParseMessage(BYE_MESSAGE, request);
        transaction = AddServerNonInviteTransaction(request, NULL);

    }

    void teardown() 
    {
        EmptyTransactionManager();
    }
};

TEST(ServerNonInviteTransactionTestGroup, ServerNonInviteTransactionCreateTest)
{
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, TransactionGetState(transaction));
    CHECK_EQUAL(TRANSACTION_TYPE_SERVER_NON_INVITE, TransactionGetType(transaction));
}

TEST(ServerNonInviteTransactionTestGroup, ServerTransactionRequestMatchTest)
{
    struct Message *newRequest = CreateMessage();
    ParseMessage(BYE_MESSAGE, newRequest);

    CHECK_TRUE(IfRequestMatchTransaction(transaction, newRequest));

    DestoryMessage(&newRequest);
}

TEST(ServerNonInviteTransactionTestGroup, ServerTransactonRequestBranchNotMatchTest)
{
    char branch[] = "1234567890";
    struct Message *newRequest = CreateMessage();

    ParseMessage(BYE_MESSAGE, newRequest);
    MessageSetViaBranch(newRequest, branch); 
    
    CHECK_FALSE(IfRequestMatchTransaction(transaction, newRequest));

    DestoryMessage(&newRequest);
}
