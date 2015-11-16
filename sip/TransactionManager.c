#include <stdlib.h>
#include <stdio.h>
#include "TransactionManager.h"
#include "Transaction.h"
#include "Messages.h"

static struct TransactionManager *SingletonTransactionManager;

struct Transaction *CreateTransactionExt(struct Message *message)
{
    struct Transaction *t = CreateTransaction(message);
    put_in_list(&SingletonTransactionManager->transactions, t);
    
    return t;
}

int CountTransaction()
{
    return get_list_len(GetTransactionManager()->transactions);
}

struct TransactionManager *GetTransactionManager()
{
    if (SingletonTransactionManager == NULL) {
        SingletonTransactionManager = calloc(1, sizeof(struct TransactionManager));
        SingletonTransactionManager->CreateTransaction = CreateTransactionExt;
        SingletonTransactionManager->CountTransaction = CountTransaction;
    }

    return SingletonTransactionManager;
}

void DestoryTransactions(struct TransactionManager *manager)
{
    int i = 0;
    int length = CountTransaction();
    
    for ( ; i < length; i++) {
        struct Transaction *t = get_data_at(manager->transactions, i);
        DestoryTransaction((struct Transaction **)&t);
    }
}

void DestoryTransactionManager(struct TransactionManager **manager)
{
    if (*manager != NULL) {
        DestoryTransactions(*manager);
        free(*manager);
        SingletonTransactionManager = NULL;
        *manager = NULL;
    }
}
