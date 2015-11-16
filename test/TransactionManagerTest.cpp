#include "CppUTest/TestHarness.h"

extern "C" {
#include <stdio.h>
#include "TransactionManager.h"
#include "MessageBuilder.h"
#include "Transaction.h"
}

TEST_GROUP(TransactionManager)
{
};

TEST(TransactionManager, Init)
{
    struct TransactionManager *manager = GetTransactionManager();
    CHECK_FALSE(manager == NULL);

    DestoryTransactionManager(&manager);
    CHECK_TRUE(manager == NULL);    
}

TEST(TransactionManager, Signleton)
{
    struct TransactionManager *m1 = GetTransactionManager();
    struct TransactionManager *m2 = GetTransactionManager();

    CHECK_EQUAL(m1, m2);

    DestoryTransactionManager(&m1);
}

TEST(TransactionManager, NewTransaction)
{
    struct Message *message = BuildRegisterMessage();
    struct TransactionManager *manager = GetTransactionManager();
    struct Transaction *transaction;

    transaction = manager->CreateTransaction(message);
    CHECK_EQUAL(1, manager->CountTransaction());

    message = BuildRegisterMessage();
    transaction = manager->CreateTransaction(message);
    CHECK_EQUAL(2, manager->CountTransaction());

    message = BuildRegisterMessage();
    transaction = manager->CreateTransaction(message);
    CHECK_EQUAL(3, manager->CountTransaction());


    CHECK_FALSE(0 == transaction)
    DestoryTransactionManager(&manager);
}

