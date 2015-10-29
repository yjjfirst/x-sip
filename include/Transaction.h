enum TransactionState {
    TRANSACTION_STATE_TRYING, 
};

struct Transaction *CreateTransaction();
void DestoryTransaction(struct Transaction *t);
enum TransactionState TransactionGetState(struct Transaction *t);
