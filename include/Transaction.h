struct Message;

enum TransactionState {
    TRANSACTION_STATE_TRYING, 
    TRANSACTION_STATE_COMPLETED,
};

struct Transaction *CreateTransaction(struct Message *request);
void DestoryTransaction(struct Transaction **t);
enum TransactionState TransactionGetState(struct Transaction *t);
void InitTransactionLayer();
