#include <stdlib.h>

struct TransactionId {
    char* branch;
    char* method;
};

struct TransactionId *CreateTransactionId()
{
    return calloc(1, sizeof(struct TransactionId));
}

void DestoryTransactionId(struct TransactionId **transactionId)
{
    if (*transactionId != NULL) {
        free(*transactionId);
        *transactionId = NULL;
    }
}

char *TransactionIdGetBranch(struct TransactionId *transactionId)
{
    return transactionId->branch;
}

char *TransactionIdGetMethod(struct TransactionId *transactionId)
{
    return transactionId->method;
}

void TransactionIdSetBranch(struct TransactionId *transactionId, char *branch)
{
    transactionId->branch = branch;
}

void TransactionIdSetMethod(struct TransactionId *transactionId, char *method)
{
    transactionId->method = method;
}
