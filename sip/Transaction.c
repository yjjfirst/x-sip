#include "Transaction.h"

struct Transaction *CreateTransaction()
{
    return NULL;
}

void DestoryTransaction(struct Transaction *t)
{
}

enum TransactionState TransactionGetState(struct Transaction *t)
{
    return TRANSACTION_STATE_TRYING;
}
