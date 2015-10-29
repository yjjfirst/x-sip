#include "CppUTest/TestHarness.h"

extern "C" {
#include "Transaction.h"
}

TEST_GROUP(TransactionTestGroup)
{
};

TEST(TransactionTestGroup, TransactionInitTest)
{
    struct Transaction *t = CreateTransaction();
    enum TransactionState s = TransactionGetState(t);
    
    CHECK_EQUAL(TRANSACTION_STATE_TRYING, s);

    DestoryTransaction(t);
}
