struct Message;

typedef void (*TimerCallback)(void *transaction);
typedef void (*TransactionTimerAdder)(void *transaction, int ms, TimerCallback onTime);

enum TransactionState {
    TRANSACTION_STATE_TRYING, 
    TRANSACTION_STATE_PROCEEDING,
    TRANSACTION_STATE_COMPLETED,
    TRANSACTION_STATE_TERMINATED,
};

struct Transaction *CreateTransaction(struct Message *request);
void DestoryTransaction(struct Transaction **t);
enum TransactionState TransactionGetState(struct Transaction *t);
int TransactionHandleMessage(char *string);
void TransactionSetTimerAdder(TransactionTimerAdder adder);
