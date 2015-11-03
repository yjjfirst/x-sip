#include <stdio.h>

#include "Transaction.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "MessageTransport.h"
#include "utils/list/include/list.h"

struct Transaction {
    enum TransactionState state;
};

struct Transaction  *Transaction;

enum TransactionState TransactionGetState(struct Transaction *t)
{
    return t->state;
}

int TransactionHandleMessage(char *message)
{
    Transaction->state = TRANSACTION_STATE_COMPLETED;
    return 0;
}

struct Transaction *CreateTransaction(struct Message *request)
{
    struct Transaction *t;
    char s[MAX_MESSAGE_LENGTH] = {0};

    t = calloc(1, sizeof (struct Transaction));
    t->state = TRANSACTION_STATE_TRYING;
    Message2String(s, request);
    SendMessage(s);
    Transaction = t;

    return t;
}

void DestoryTransaction(struct Transaction **t)
{
    free(*t);
    *t = NULL;
}

void InitTransactionLayer()
{
    InitReceiveMessageCallback(TransactionHandleMessage);
}
