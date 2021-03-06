struct TransactionId *CreateTransactionId();
void DestroyTransactionId(struct TransactionId **transactionId);

char *TransactionIdGetBranch(struct TransactionId *transactionId);
char *TransactionIdGetMethod(struct TransactionId *transactionId);
void TransactionIdSetBranch(struct TransactionId *transactionId, char *branch);
void TransactionIdSetMethod(struct TransactionId *transactionId, char *method);
