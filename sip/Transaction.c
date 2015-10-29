#include "Transaction.h"
#include "MessageBuilder.h"

struct Transaction {
    enum TransactionState state;
};

enum TransactionState TransactionGetState(struct Transaction *t)
{
    return t->state;
}

struct Transaction *CreateTransaction()
{
    struct Transaction *t;

    t = calloc(1, sizeof (struct Transaction));
    t->state = TRANSACTION_STATE_TRYING;
    return t;
}

void DestoryTransaction(struct Transaction *t)
{
    free(t);
}
