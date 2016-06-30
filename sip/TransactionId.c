#include <stdlib.h>

#include "TransactionId.h"
#include "Messages.h"
#include "Utils.h"

struct TransactionId {
    char* branch;
    char* method;
};

struct TransactionId *CreateTransactionId()
{
    return calloc(1, sizeof(struct TransactionId));
}

void DestroyTransactionId(struct TransactionId **transactionId)
{
    if (*transactionId != NULL) {
        free(*transactionId);
        *transactionId = NULL;
    }
}

DEFINE_STRING_MEMBER_READER(struct TransactionId, TransactionIdGetBranch, branch);
DEFINE_STRING_MEMBER_READER(struct TransactionId, TransactionIdGetMethod, method);

void TransactionIdSetBranch(struct TransactionId *transactionId, char *branch)
{
    transactionId->branch = branch;
}

void TransactionIdSetMethod(struct TransactionId *transactionId, char *method)
{
    transactionId->method = method;
}

void TransactionIdExtract(struct TransactionId *tid, struct Message *message)
{
    tid->branch = MessageGetViaBranch(message);
    tid->method = MessageGetCSeqMethod(message);
}
